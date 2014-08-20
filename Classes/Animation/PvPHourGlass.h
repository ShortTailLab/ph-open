//
//  PvPHourGlass.h
//  HelloCpp
//
//  Created by Zeyang Li on 5/15/13.
//
//

#pragma once

#include <cocos2d.h>
#include <boost/function.hpp>

namespace PH
{
    USING_NS_CC;
    
    class PvPHourGlass : public CCNode
    {
    private:
        CCSprite* mHourGlass;
        CCSprite* mHourGlassBG;
        CCLabelBMFont* mText;
        
        bool mStarted;
        bool mRunning;
        int seconds;
        
        boost::function<void()> onTimeout;
        
    public:
        virtual bool init();
        void start(int secs, boost::function<void()> ontimeout);
        void stop();
        
        void pause();
        void resume();
        
        bool isRunning() const
        {
            return mRunning;
        }
        
        void countdown(CCObject* o);
        
        CREATE_FUNC(PvPHourGlass);
    };
}