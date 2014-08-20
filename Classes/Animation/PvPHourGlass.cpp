//
//  PvPHourGlass.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 5/15/13.
//
//

#include "PvPHourGlass.h"
#include "extensions/utils.h"
#include "Board/Utils.h"

namespace PH
{
    bool PvPHourGlass::init()
    {
        mHourGlassBG = GetSprite("pvp_hourglass_bg.png");
        this->addChild(mHourGlassBG);
        
        mHourGlass = GetSprite("pvp_hourglass.png");
        this->addChild(mHourGlass);
    
        mText = CCLabelBMFont::create("0", "bmfont/Molot_48_red.fnt");
        mText->setScale(contentScale());
        mText->setAnchorPoint(ccp(0.5f, 0.5f));
        mText->setPosition(ccp(0, -20));
        this->addChild(mText);
        
        seconds = 0;
        
        onTimeout = [](){};
        stop();
        
        return true;
    }
    
    void PvPHourGlass::start(int secs, boost::function<void()> ontimeout)
    {
        mHourGlass->setColor(ccWHITE);
        mHourGlassBG->setColor(ccWHITE);
        
        seconds = secs;
        mText->setString(toStr(seconds).c_str());
        mText->setVisible(true);
        
        onTimeout = ontimeout;
        mRunning = true;
        mStarted = true;
        
        this->schedule(schedule_selector(PvPHourGlass::countdown), 1.0f);
    }
    
    void PvPHourGlass::stop()
    {
        this->unschedule(schedule_selector(PvPHourGlass::countdown));
        
        mHourGlass->setColor(ccGRAY);
        mHourGlassBG->setColor(ccGRAY);
        mText->setVisible(false);
        
        mRunning = false;
        mStarted = false;
    }
    
    void PvPHourGlass::pause()
    {
        if(mStarted)
        {
            this->unschedule(schedule_selector(PvPHourGlass::countdown));
            mRunning = false;
        }
    }
    
    void PvPHourGlass::resume()
    {
        if(mStarted)
        {
            this->schedule(schedule_selector(PvPHourGlass::countdown), 1.0f);
            mRunning = true;
        }
    }
    
    void PvPHourGlass::countdown(CCObject *o)
    {
        seconds = std::max(0, seconds - 1);
        mText->setString(toStr(seconds).c_str());
        
        if(seconds == 0)
        {
            stop();
            onTimeout();
            return;
        }
        else if(seconds < 5)
        {
            // animation must end with 1 sec
            mText->runAction(CCSequence::create(CCScaleTo::create(0.2, 1.6),
                                                CCScaleTo::create(0.2, 1.0),
                                                NULL));
        }
    }
}