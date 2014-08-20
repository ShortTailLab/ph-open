//
//  MoveTimer.h
//  HelloCpp
//
//  Created by Zeyang Li on 4/17/13.
//
//

#pragma once

#include <cocos2d.h>
#include <boost/function.hpp>

namespace PH
{
    USING_NS_CC;
    
    struct MoveTimer
    {
        CCSprite* box;
        CCSprite* fill;
        float baseMaxTime;
        float maxTime;
        float threshold;
        float time;
        bool visible;
        
        boost::function<void()> timeoutCall;
        
        MoveTimer();
        ~MoveTimer();
        
        void show();
        void hide();
        
        void startTicking(boost::function<void()> onTimeout);
        void stopTicking();
        
        void setPosition(CCPoint p)
        {
            box->setPosition(p);
        }
        
        void checkTimer(CCObject* o);
    };
}