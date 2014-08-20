
//create by ZQL

#pragma once

#include "cocos2d.h"

USING_NS_CC;

class ContentLayer : public CCLayer
{
public:
    typedef ContentLayer* (*ContentCreatorFunc)();
    
public:
	cocos2d::CCSize winSize;
    bool isShowTop;
    bool isShowBottom;

public:
	virtual bool init();
    virtual void setTouchEnabled(bool value);
    
    // default implements are used to call script callback if exist
	virtual void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    
    // default implements are used to call script callback if exist
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    virtual void onSingleClick(CCTouch* touch){}
    virtual void onLongClick(CCTouch* touch){}

    
    // Can be used to block clicks to tab buttons(bottom)
    // if it returns false, tab buttons will not be triggered
    //msg is a hack to solve the biandui setTeamFormation probleam
    virtual bool tryExit(int msg = -1) { return true; }
    
    // Always called when pushing a layer.
    virtual void onEnterAnimationDone(){}

	void addChildWithAnim(ContentLayer *layer,
                          bool isAnimate = false,
                          CCFiniteTimeAction *action = NULL);

	CREATE_FUNC(ContentLayer);

private:
    CCTouch* currentTouch;
    bool isSingleClick;

    void onPressTimer(float dt);
};

