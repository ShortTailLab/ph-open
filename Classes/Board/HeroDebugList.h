//
//  HeroDebugList.h
//  HelloCpp
//
//  Created by Zeyang Li on 2/28/13.
//
//

#pragma once

#include "cocos2d.h"
#include "HeroTable.h"
#include "ZqlScrollView.h"
#include <vector>

USING_NS_CC;

namespace PH
{
    class HeroDebugList : public CCLayer
    {
    public:
        
        static HeroDebugList* create()
        {
            HeroDebugList* o = new HeroDebugList();
            if(o && o->init())
            {
                o->autorelease();
                return o;
            }
            
            CC_SAFE_DELETE(o);
            return o;
        }
    
        virtual bool init();
        void setOnClick(boost::function<void(HeroProfile*)> f) { func = f; }
    
    private:
        void onClick(CCObject* o);
    
    private:
        ZqlScrollView* scroll;
        boost::function<void(HeroProfile*)> func;
    };
}