//
//  ButtonSprite.h
//  Zuma_V4
//
//  Created by user on 13-1-28.
//
//

#pragma once

#include <cocos2d.h>
#include <boost/function.hpp>

USING_NS_CC;

//---------------------------------------------------------
// An issue with ButtonSprite
// ButtonSprite is not suitable to use inside a ScrollView,
// because it consumes the touch event. Unless ScrollView
// receives touch event at a higher priority, and passes it
// on to its children, users will not be able to drag scrolview
// on top of a buttonsprite.
//---------------------------------------------------------
class ButtonSprite : public CCSprite, public CCTouchDelegate
{
    // touch 事件优先级
    int m_touchPriority;
    bool m_isDown;

public:
    ButtonSprite();
    virtual ~ButtonSprite();
    
    virtual bool init(const char* normalFile,
                      const char* pressFile,
                      bool clickAction,
                      bool isSwallowTouch);
    
    virtual bool initWithSprite(CCSprite* normalSprite,
                                CCSprite* pressedSprite,
                                bool clickAction,
                                bool isSwallowTouch = true);
    
    
    static ButtonSprite* create(const char* normalFile,
                                int tPriority = kCCMenuHandlerPriority,
                                bool clickAction = true);
    
    static ButtonSprite* create(const char* normalFile,
                                const char* pressFile,
                                int tPriority = kCCMenuHandlerPriority,
                                bool clickAction = true,
                                bool isSwallowTouch = true);
    
    static ButtonSprite* create(CCSprite* n,
                                CCSprite* p,
                                int tPriority = kCCMenuHandlerPriority,
                                bool clickAction = true,
                                bool isSwallowTouch = true);
    static ButtonSprite* create(CCSprite* n,
                                int tPriority = kCCMenuHandlerPriority,
                                bool clickAction = true,
                                bool isSwallowTouch = true);
    
    
    static ButtonSprite* createButtonSpriteWithSpriteFrame(CCSpriteFrame *pSpriteFrame,int tPriority = kCCMenuHandlerPriority);
    
    void setTouchEnabled(bool value);
    /* 触摸相关 */
    virtual void onEnter();
    virtual void onExit();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    // optional
    
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
    // default implements are used to call script callback if exist
	virtual void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    virtual void ccTouchesCancelled(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    
    //update  2013-02-17
    CCSprite* getSprite();
    CCSprite* getPressedSprite();
    
    void setSprite(CCSprite* normal);
    void setPressedSprite(CCSprite* normal);
    
    void clicked();
    void onRelease();
    void onPressed();
    
    void setOnClickListener(boost::function<void ()> call);
    void setOnPressListener(boost::function<void ()> call);
    void setOnReleaseListener(boost::function<void ()> call);
    
private:
    CCSprite* normal;
    CCSprite* pressd;
    bool isTouchable;
    bool isSwallow;
    bool useClickAction;
    bool banDoubleClick;
    boost::function<void ()> onClickCall;
    boost::function<void ()> onPressCall;
    boost::function<void ()> onReleaseCall;
};
