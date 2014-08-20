//
//  ObjectPool.h
//  HelloCpp
//
//  Created by zql on 13-4-11.
//
//

#pragma once

#include <cocos2d.h>
#include "BattleScene.h"
#include "JiangHu.h"

USING_NS_CC;

namespace PH
{
    
template<class T>
class ObjectPool
{
public:
    //T hint a CCNode or its children
    T* getObject()
    {
        T* t = NULL;
        if(freeObjs.size() > 0)
        {
            t = *(freeObjs.erase(freeObjs.end()-1));
        }
        else
        {
            t = T::create();
            t->retain();
        }
        inUseObjs.push_back(t);
        return t;
    }
    
    T* getObject(Stage& stage, bool progressive);
    
    void collectObj(T* obj);
    void collect()
    {
        for(T* t : inUseObjs)
        {
            if(t->getParent())
                t->removeFromParentAndCleanup(true);
            freeObjs.push_back(t);
        }
        //freeObjs.insert(freeObjs.end(), inUseObjs.begin(), inUseObjs.end());
        inUseObjs.clear();
    }
    void clear()
    {
        for(T* t : inUseObjs)
            t->release();
        for(T* t : freeObjs)
            t->release();
        inUseObjs.clear();
        freeObjs.clear();
    }
    
    static ObjectPool *getInstance()
    {
		static ObjectPool *o = NULL;
		if(!o)
		{
			o = new ObjectPool();
		}
        
		return o;
	}
    
public:
    std::vector<T*> freeObjs;
    std::vector<T*> inUseObjs;
    
};

}