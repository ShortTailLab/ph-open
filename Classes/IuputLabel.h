//
//  IuputLabel.h
//  HelloCpp
//
//  Created by Wu Zhen on 13-1-25.
//
//

#pragma once

#include "cocos2d.h"
#include "boost/function.hpp"

USING_NS_CC;

class InputLabel : public cocos2d::CCTextFieldTTF, public CCTouchDelegate
{
public:
    InputLabel(){}
    
    bool init(const char *placeholder, const char *fontName, float fontSize);
    virtual void keyboardDidHide(CCIMEKeyboardNotificationInfo& info);
    
    void setKeyBoardHideCall(boost::function<void()> call);
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
    
    static InputLabel* create(const char *text, const char *placeholder, const char *fontName, float fontSize)
    {
        InputLabel *pRet = new InputLabel();
        if(pRet && pRet->init("", fontName, fontSize))
        {
            pRet->autorelease();
            if (placeholder)
            {
                pRet->setPlaceHolder(placeholder);
            }
            if (text)
            {
                pRet->setString(text);
            }
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
    
private:
    boost::function<void ()> keyboardHideCall;
    
};
