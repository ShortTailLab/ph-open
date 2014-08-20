//
//  Task.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 12/20/12.
//
//

#include "Task.h"
#include "Common.h"
#include "extensions/utils.h"

// execute at most 5 jobs in a sequence in one update,
// this avoids slowing down current frame by too much.
#define MAX_CURRENT_JOBS  5

namespace PH
{
    void TaskSound::launch()
    {
        ring(file.c_str());
        done = true;
    }
    
    //---------------------------
    // TaskQueue entry point
    //---------------------------
    void TaskQueue::update(float dt)
    {
        // The real job queue code starts here
        if(this->size() == 0)
            return;
        
        // execute at most 5 jobs in an update loop,
        // this avoids slowing down this frame too much.
        for(int jcount = 0; jcount < MAX_CURRENT_JOBS && this->size(); jcount++)
        {
            TaskPtr task = this->front();
            
            if( process(dt, task) )
            {
                if(this->size())
                    this->dequeue();
            }
            else
                break;
        }
    }
    
    bool TaskQueue::process(float dt, TaskPtr task)
    {
        if(task->isDone())
            return true;
        
        if(task->getType() == Task::BATCH)
        {
            TaskBatchPtr t = boost::static_pointer_cast<TaskBatch>(task);
            for(std::list<TaskPtr>::iterator i=t->list.begin(); i != t->list.end();)
            {
                if(process(dt, *i))
                    i = t->list.erase(i);
                else
                    i++;
            }
            
            return t->isDone();
        }
        else if(task->getType() == Task::SEQUENCE)
        {
            TaskSequencePtr t = boost::static_pointer_cast<TaskSequence>(task);
            for(int j=0; j< MAX_CURRENT_JOBS && !t->isDone(); j++)
            {
                TaskPtr head = t->list.front();
                if(process(dt, head))
                {
                    if(t->list.size())
                        t->list.pop_front();
                }
                else
                    break;
            }
            
            return t->isDone();
        }
        else if(task->getType() == Task::ANIM)
        {
            TaskAnimPtr anim = boost::static_pointer_cast<TaskAnim>(task);
            
            if(!anim->node->isRunning())
                return true;
            
            if(!anim->hasStarted())
            {
                anim->launch();
            }
            else if(anim->wait)
            {
                // insidious hack
                int ref = (int)anim->node->getUserData();
                if(ref != anim->runningCount)
                {
                    LOGD("overwriting previous anmation %d %d\n", ref, anim->runningCount);
                    return true;
                }
            }
            
            return anim->isDone();
        }
        else if(task->getType() == Task::SOUND)
        {
            TaskSoundPtr sound = boost::static_pointer_cast<TaskSound>(task);
            
            if(!sound->isDone())
                sound->launch();
            return true;
        }
        else if(task->getType() == Task::LAMBDA)
        {
            TaskLambdaPtr t = boost::static_pointer_cast<TaskLambda>(task);
            t->func();
            
            return true;
        }
        else if(task->getType() == Task::EMPTY || task->getType() == Task::IGNORE)
        {
            return true;
        }
        
        LOGD("unknown task %s\n", task->getTag().c_str());
        phassert(false && "Unknown task occured");
        
        return true;
    }

}