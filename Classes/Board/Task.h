//
//  Task.h
//  HelloCpp
//
//  Created by Zeyang Li on 10/29/12.
//
//
#pragma once

#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <cocos2d.h>

namespace PH
{
    USING_NS_CC;
    
    class Task
    {
    public:
        enum Type
        {
            ANIM,
            SOUND,
            SEQUENCE,
            BATCH,
            LAMBDA,
            EMPTY,
            IGNORE,
        };
        
        Task(Type t, const std::string& tg) : type(t), tag(tg), done(false) {}
        virtual ~Task() {}

        virtual bool isDone() const { return done; }
        Type getType() const { return type; }
        std::string getTag() const { return tag; }
        
        void setLocation(std::string l ) { location = l; }
        
        virtual void debugPrint(const std::string & prefix){
            std::string pattern = prefix+"%s\n";
            printf(pattern.c_str(), tag.c_str());
        }
        
    protected:
        Type type;
        const std::string tag;
        bool done;
        
        std::string location;
    };
    
    typedef boost::shared_ptr<Task> TaskPtr;

    
    class TaskEmpty;
    typedef boost::shared_ptr<TaskEmpty> TaskEmptyPtr;
    
    struct TaskEmpty : public Task
    {
        TaskEmpty() : Task(EMPTY, "Empty") {}
        static TaskEmptyPtr make()
        {
            return TaskEmptyPtr(new TaskEmpty());
        }
    };
    
    
    class TaskIgnore;
    typedef boost::shared_ptr<TaskIgnore> TaskIgnorePtr;
    
    struct TaskIgnore : public Task
    {
        TaskIgnore() : Task(IGNORE, "Ignore") {}
        static TaskIgnorePtr make()
        {
            return TaskIgnorePtr(new TaskIgnore());
        }
    };
    
    
    class TaskLambda;
    typedef boost::shared_ptr<TaskLambda> TaskLambdaPtr;
    
    struct TaskLambda : public Task
    {
        boost::function<void ()> func;
        TaskLambda(boost::function<void ()> f) : Task(LAMBDA, "Lambda"), func(f) {}
        
        static TaskLambdaPtr make(boost::function<void()> f)
        {
            return TaskLambdaPtr(new TaskLambda(f));
        }
    };
    
    
    class TaskAnim;
    typedef boost::shared_ptr<TaskAnim> TaskAnimPtr;
    
    struct TaskAnim : public Task
    {
    public:
        CCNode* node;
        CCFiniteTimeAction* action;
        bool wait;
        bool started;
        int runningCount;
        
        // do not use this directly
        TaskAnim(CCNode* n, CCFiniteTimeAction* a, bool w = true) : Task(ANIM, "Anim"), node(n), action(a), wait(w), started(false), runningCount(0)
        {
            node->retain();
            action->retain();
        }
        
        TaskAnim(CCNode *n, CCFiniteTimeAction *a, std::string l, bool w = true) :
            Task(ANIM, "Anim"), node(n), action(a), wait(w), started(false), runningCount(0)
        {
            node->retain();
            action->retain();
            location = l;
        }
        
        ~TaskAnim()
        {
            CC_SAFE_RELEASE_NULL(node);
            CC_SAFE_RELEASE_NULL(action);
        }
        
        void launch()
        {
            runningCount = (int)node->getUserData();
            runningCount++;
            node->setUserData((void*)runningCount);
            
            node->runAction(action);
            started = true;
        }
        
        virtual bool isDone() const
        {
            return started ? (wait ? action->isDone() : true) : false;
        }
        
        virtual bool hasStarted() const
        {
            return started;
        }
        
        static TaskAnimPtr make(CCNode* n, CCFiniteTimeAction* a, bool w = true)
        {
            return TaskAnimPtr(new TaskAnim(n, a, w));
        }
        
        static TaskAnimPtr make(CCNode *n, CCFiniteTimeAction *a, std::string l, bool w = true)
        {
            return TaskAnimPtr(new TaskAnim(n,a,l,w));
        }
    };
    
    
    class TaskSound;
    typedef boost::shared_ptr<TaskSound> TaskSoundPtr;
    
    struct TaskSound : public Task
    {
        std::string file;
        
        TaskSound(const std::string& s) : Task(SOUND, "Sound"), file(s){}
        
        void launch();

        static TaskSoundPtr make(const std::string& s)
        {
            return TaskSoundPtr(new TaskSound(s));
        }
    };
    
    class TaskBatch;
    typedef boost::shared_ptr<TaskBatch> TaskBatchPtr;
    
    class TaskBatch : public Task
    {
    public:
        std::list<TaskPtr> list;
        
        TaskBatch() : Task(BATCH, "Batch") {}
        
        void add(TaskPtr p)
        {
            assert(p != NULL && "task pointer is NULL");
            
            if(p->getType() == Task::IGNORE)
                return;

            list.push_back(p);
        }
        
        virtual bool isDone() const
        {
            for(TaskPtr p : list)
                if(!p->isDone())
                    return false;
            return true;
        }
        
        int size() const { return list.size(); }
        
        virtual void debugPrint(const std::string & prefix)
        {
            Task::debugPrint(prefix);
            auto newPrefix= std::string("\t")+prefix;
            for(auto iter : list)
            {
                iter->debugPrint(newPrefix);
            }
        }
        
        static TaskBatchPtr make()
        {
            return TaskBatchPtr(new TaskBatch);
        }
        
        static TaskBatchPtr make(TaskPtr a, TaskPtr b)
        {
            TaskBatchPtr p = make();
            p->add(a);
            p->add(b);
            return p;
        }
    };

    
    class TaskSequence;
    typedef boost::shared_ptr<TaskSequence> TaskSequencePtr;
    
    class TaskSequence : public Task
    {
    public:
        std::list<TaskPtr> list;
        
        TaskSequence() : Task(SEQUENCE, "Sequence") {}
        
        void add(TaskPtr p)
        {
            assert(p != NULL && "task pointer is NULL");
            
            if(p->getType() == Task::IGNORE)
                return;
            
            list.push_back(p);
        }
        
        virtual bool isDone() const
        {
            if(list.size() == 0)
                return true;
            
            for(TaskPtr t : list)
                if(!t->isDone())
                    return false;
            
            return true;
        }
        
        int size() const { return list.size(); }
        
        virtual void debugPrint(const std::string & prefix)
        {
            Task::debugPrint(prefix);
            auto newPrefix= std::string("\t")+prefix;
            for(auto iter : list)
            {
                iter->debugPrint(newPrefix);
            }
        }
        
        static TaskSequencePtr make()
        {
            return TaskSequencePtr(new TaskSequence);
        }
        
        static TaskSequencePtr make(TaskPtr a, TaskPtr b)
        {
            TaskSequencePtr p = make();
            p->add(a);
            p->add(b);
            return p;
        }
    };
    
    inline TaskBatchPtr operator <<(TaskBatchPtr batch, TaskPtr t)
    {
        batch->add(t);
        return batch;
    }
    
    inline TaskSequencePtr operator <<(TaskSequencePtr seq, TaskPtr t)
    {
        seq->add(t);
        return seq;
    }
    
    
    class TaskQueue
    {
    public:
        void enqueue(TaskPtr t)
        {
            mQueue.push_back(t);
        }
        
        TaskPtr front()
        {
            return mQueue.front();
        }
        
        TaskPtr dequeue()
        {
            TaskPtr f = mQueue.front();
            mQueue.pop_front();
            return f;
        }
        
        int size() const { return mQueue.size(); }
        void clear() { mQueue.clear(); }
        
        void update(float dt);

    protected:
        bool process(float dt, TaskPtr task);
        
    protected:
        std::list<TaskPtr> mQueue;
    };
}
