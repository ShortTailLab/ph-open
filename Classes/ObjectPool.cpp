//
//  ObjectPool.cpp
//  HelloCpp
//
//  Created by zql on 13-4-11.
//
//

#include "ObjectPool.h"
#include "friend.h"

#include <functional>

using namespace std;

namespace PH
{
    
    
template<> JiangHuMenuItem* ObjectPool<JiangHuMenuItem>::getObject(Stage& stage, bool progressive)
{
    std::vector<JiangHuMenuItem*>::iterator it = find_if(freeObjs.begin(), freeObjs.end(), [=](JiangHuMenuItem* item){
        return item->stage.name == stage.name;
    });
    
    if(it != freeObjs.end())
    {
        auto item = *it;
        freeObjs.erase(it);
        item->updateStage(stage);
        item->stopAllSchedule(false);
        inUseObjs.push_back(item);
        return item;
    }
    else
    {
        JiangHuMenuItem* item = JiangHuMenuItem::create(stage,progressive);
        item->retain();
        inUseObjs.push_back(item);
        return item;
    }
    
}
    
template<> void ObjectPool<JiangHuMenuItem>::collectObj(JiangHuMenuItem* obj)
{
    std::vector<JiangHuMenuItem*>::iterator it = find_if(inUseObjs.begin(), inUseObjs.end(), [=](JiangHuMenuItem* item){
        return item->stage.name == obj->stage.name;
    });
    
    if(it != inUseObjs.end())
    {
        if((*it)->getParent())
            (*it)->removeFromParentAndCleanup(true);
        freeObjs.push_back(*it);
        inUseObjs.erase(it);
    }
}
    
template<> void ObjectPool<JiangHuMenuItem>::clear()
{
    for(JiangHuMenuItem* item : freeObjs)
        item->release();
    freeObjs.clear();
    
    for(JiangHuMenuItem* item : inUseObjs)
        item->release();
    inUseObjs.clear();
}
    

}