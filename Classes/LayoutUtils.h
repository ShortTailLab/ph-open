//
//  LayoutUtils.h
//  HelloCpp
//
//  Created by Zeyang Li on 6/8/13.
//
//

#pragma once

#include <cocos2d.h>

namespace PH
{
    USING_NS_CC;
    
#define ANCHOR_TOP_LEFT (ccp(0.0f, 1.0f))
#define ANCHOR_TOP_CENTER (ccp(0.5f, 1.0f))
#define ANCHOR_TOP_RIGHT (ccp(1.0f, 1.0f))
    
#define ANCHOR_BOTTOM_LEFT (ccp(0.0f, 0.0f))
#define ANCHOR_BOTTOM_CENTER (ccp(0.5f, 0.0f))
#define ANCHOR_BOTTOM_RIGHT (ccp(1.0f, 0.0f))

#define ANCHOR_CENTER (ccp(0.5f, 0.5f))
    
    // Not exactly safe.
    // T must be a pointer type where is base type derives from CCNode.
    template <typename T>
    static void LayoutColumn(const CCPoint& rectPosition,
                             const CCPoint& elementAnchor,
                             float spacing,
                             bool isTopDown,
                             const std::vector<T>& items)
    {
        for(int i=0; i<items.size(); i++)
        {
            CCNode* node = items[i];
            
            node->setAnchorPoint(elementAnchor);
            CCPoint p = rectPosition;
            float offset = isTopDown ? spacing * i * -1.0f : spacing * i;
            p.y += offset;
            node->setPosition(p);
        }
    }
    
    template <typename T>
    static void LayoutRow(const CCPoint& rectPosition,
                          const CCPoint& elementAnchor,
                          float spacing,
                          bool isTopDown,
                          const std::vector<T>& items)
    {
        for(int i=0; i<items.size(); i++)
        {
            CCNode* node = items[i];
            
            node->setAnchorPoint(elementAnchor);
            CCPoint p = rectPosition;
            float offset = isTopDown ? spacing * i * -1.0f : spacing * i;
            p.x += offset;
            node->setPosition(p);
        }
    }
    
    template <typename T>
    static void LayoutApply(const std::vector<T>& items,
                            boost::function<void(T)> f)
    {
        for(T x : items)
            f(x);
    }
    
    template <typename T>
    static std::vector<T> VA(T first, ...)
    {
        va_list args;
        
        std::vector<T> list;
        list.push_back(first);
        
        if(first == NULL)
            return list;
        
        va_start(args, first);
        for (;;)
        {
            T p = va_arg(args, T);
            if(p == NULL)
                break;
            else
                list.push_back(p);
        }
        va_end(args);
        
        return list;
    }
    
    static void getChildRecursive(CCNode * par, std::vector<CCNode*> & seq)
    {
        if(NULL == par) return;
        
        CCArray * kids = par->getChildren();
        CCObject * obj;
        CCARRAY_FOREACH(kids, obj)
        {
            seq.push_back((CCNode*)obj);
            getChildRecursive((CCNode*)obj, seq);
        }
    }
}