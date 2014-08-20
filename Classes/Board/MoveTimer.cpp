//
//  MoveTimer.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 4/17/13.
//
//

#include "MoveTimer.h"
#include "extensions/utils.h"

namespace PH
{
    USING_NS_CC;
    
    MoveTimer::MoveTimer()
    {
        box = GetSprite("ui/daoshu_bg.png");
        box->setAnchorPoint(ccp(-0.3, -1));
        box->retain();
        
        fill = GetSprite("ui/daoshu_xue.png");
        fill->setAnchorPoint(ccp(0.0f, 0.0f));
        fill->setPosition(ccp(6, 6));
        
        hide();
        
        box->addChild(fill);
        
        baseMaxTime = maxTime = time = 5.0f;
        threshold = 5.0f;
    }
    
    MoveTimer::~MoveTimer()
    {
        box->removeFromParentAndCleanup(true);
        box->release();
    }
    
    void MoveTimer::show()
    {
        visible = true;
        box->setVisible(true);
        fill->setVisible(true);
    }
    
    void MoveTimer::hide()
    {
        visible = false;
        box->setVisible(false);
        fill->setVisible(false);
    }
}