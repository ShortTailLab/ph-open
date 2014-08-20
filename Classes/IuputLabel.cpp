//
//  IuputLabel.cpp
//  HelloCpp
//
//  Created by Wu Zhen on 13-1-25.
//
//

#include "IuputLabel.h"
#include <boost/move/move.hpp>


bool InputLabel::init(const char *placeholder, const char *fontName, float fontSize)
{
    if(!CCTextFieldTTF::initWithPlaceHolder(placeholder, fontName, fontSize))
        return false;
    
    keyboardHideCall = [](){};
    canDetachWithIME();

    //CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    
    return true;
}

void InputLabel::onEnter()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCTextFieldTTF::onEnter();
    
}

void InputLabel::onExit()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    CCTextFieldTTF::onExit();
    
}

bool InputLabel::ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event)
{
    return true;
}

void InputLabel::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
    
}

void InputLabel::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    CCLog("------------------------inputlabel touched");
    attachWithIME();
}

void InputLabel::keyboardDidHide(CCIMEKeyboardNotificationInfo& info)
{
    keyboardHideCall();
}

void InputLabel::setKeyBoardHideCall(boost::function<void()> call)
{
    keyboardHideCall = boost::move(call);
}
