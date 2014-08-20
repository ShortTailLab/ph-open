//
//  ButtonSprite.cpp
//  Zuma_V4
//
//  Created by user on 13-1-28.
//
//

#include "ButtonSprite.h"
#include "Board/Utils.h"
#include "extensions/utils.h"
#include "Common.h"
#include "CCCallLambda.h"
#include <boost/move/move.hpp>

/**********************************************
 构造函数和析构函数
 **********************************************/
ButtonSprite::ButtonSprite():m_isDown(false)
{
    m_touchPriority = 0;
}
ButtonSprite::~ButtonSprite(){};

bool ButtonSprite::init(const char* normalFile, const char* pressFile, bool clickAction, bool isSwallowTouch)
{
    if(!CCSprite::init())
        return false;
    CCSprite* press = strlen(pressFile) == 0 ? NULL : PH::GetSprite(pressFile);
    return this->initWithSprite(PH::GetSprite(normalFile),
                                press,
                                clickAction,
                                isSwallowTouch);
}

bool ButtonSprite::initWithSprite(CCSprite* normalSprite,
                                  CCSprite* pressedSprite,
                                  bool clickAction,
                                  bool isSwallowTouch)
{
    if(!CCSprite::init())
        return false;
    
    useClickAction = clickAction;
    this->isSwallow = isSwallowTouch;
    banDoubleClick = false;
    
    normal = normalSprite;
    CCPoint pos = ccp(normal->getContentSize().width*0.5f,
                      normal->getContentSize().height*0.5f);
    normal->setPosition(pos);
    addChild(normal);
    
    pressd = pressedSprite;
    if(pressd)
    {
        pressd->setPosition(pos);
        addChild(pressd);
    }
    
    
    setContentSize(normal->getContentSize());
    setTouchEnabled(true);
    
    onClickCall = [](){};
    onPressCall = [](){};
    onReleaseCall = [](){};
    
    onRelease();
    
    return true;
}

void ButtonSprite::setTouchEnabled(bool value)
{
    if (isTouchable != value)
    {
        isTouchable = value;
        if (m_bRunning)
        {
            if (value)
            {
                if(isSwallow)
                    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,m_touchPriority, true);
                else
                    CCDirector::sharedDirector()->getTouchDispatcher()->addStandardDelegate(this, m_touchPriority);
            }
            else
            {
                // have problems?
                CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
            }
        }
    }
}

void ButtonSprite::onEnter()
{
    CCSprite::onEnter();
    if(isTouchable && isSwallow)
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,m_touchPriority, true);
    else if(isTouchable && !isSwallow)
        CCDirector::sharedDirector()->getTouchDispatcher()->addStandardDelegate(this, m_touchPriority);
}

void ButtonSprite::onExit()
{
    CCSprite::onExit();
    if(isTouchable)
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}


bool ButtonSprite::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint touchPoint = getParent()->convertTouchToNodeSpace(pTouch);
    if (boundingBox().containsPoint(touchPoint))
    {
        m_isDown = true;
        if(useClickAction)
            runAction(CCScaleTo::create(0.06, 0.8));
        
        onPressCall();
        
        return true;
    }
    
    return false;
}
// optional

void ButtonSprite::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint currPoint = getParent()->convertTouchToNodeSpace(pTouch);
    CCPoint prevPoint = getParent()->convertToNodeSpace(pTouch->getPreviousLocation());
    
    CCPoint delta = currPoint - prevPoint;
    
    if(PH::GameConfig::shared()->useTouchSmooth)
    {
        delta = delta * 0.5f;
        currPoint = prevPoint + delta;
    }
    
    float deltaLen = delta.getLength();
    
    if (!boundingBox().containsPoint(currPoint) || !m_isDown || deltaLen > 6)
    {
        m_isDown = false;
        if(useClickAction)
            runAction(CCScaleTo::create(0.06, 1));
        
        onReleaseCall();
        return;
    }
}

void ButtonSprite::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    if (m_isDown)
    {
        if(useClickAction)
        {
            CCActionInterval* sAction = CCScaleTo::create(0.06, 1);
            CCCallFunc*  callFun = CCCallFunc::create(this,
                                                      callfunc_selector(ButtonSprite::clicked));
            stopAllActions();
            runAction(CCSequence::create(sAction, callFun, NULL));
        }
        else
            clicked();
        
        onReleaseCall();

        m_isDown = false;
    }
}

void ButtonSprite::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    m_isDown = false;
    setScale(1);
}

// default implements are used to call script callback if exist
void ButtonSprite::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    this->ccTouchBegan((CCTouch*)pTouches->anyObject(), pEvent);
}
void ButtonSprite::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    this->ccTouchMoved((CCTouch*)pTouches->anyObject(), pEvent);
}
void ButtonSprite::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    this->ccTouchEnded((CCTouch*)pTouches->anyObject(), pEvent);
}
void ButtonSprite::ccTouchesCancelled(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    this->ccTouchCancelled((CCTouch*)pTouches->anyObject(), pEvent);
}

ButtonSprite* ButtonSprite::create(const char* normalFile, const char* pressFile, int tPriority, bool clickAction, bool isSwallowTouch)
{
    ButtonSprite* button  = new ButtonSprite();
    button->m_touchPriority = tPriority;
    if (button && button->init(normalFile, pressFile, clickAction, isSwallowTouch)) {
        button->autorelease();
        return button;
    }
    CC_SAFE_DELETE(button);
    return button;
}


ButtonSprite* ButtonSprite::create(const char* normalFile, int tPriority, bool clickAction)
{
    return create(normalFile, normalFile, tPriority, clickAction);
}

ButtonSprite* ButtonSprite::create(CCSprite* n, CCSprite* p, int tPriority,
                                   bool clickAction, bool isSwallowTouch)
{
    ButtonSprite* button  = new ButtonSprite();
    button->m_touchPriority = tPriority;
    if (button && button->initWithSprite(n, p, clickAction, isSwallowTouch)) {
        button->autorelease();
        return button;
    }
    CC_SAFE_DELETE(button);
    return button;
}

ButtonSprite* ButtonSprite::create(CCSprite* n,
                            int tPriority,
                            bool clickAction,
                            bool isSwallowTouch)
{
    ButtonSprite* button  = new ButtonSprite();
    button->m_touchPriority = tPriority;
    if (button && button->initWithSprite(n, NULL, clickAction, isSwallowTouch)) {
        button->autorelease();
        return button;
    }
    CC_SAFE_DELETE(button);
    return button;
}

ButtonSprite* ButtonSprite::createButtonSpriteWithSpriteFrame(CCSpriteFrame *pSpriteFrame,int tPriority)
{
    ButtonSprite *pobSprite = new ButtonSprite();
    pobSprite->m_touchPriority = tPriority;
    if (pSpriteFrame && pobSprite && pobSprite->initWithSpriteFrame(pSpriteFrame))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    CC_SAFE_DELETE(pobSprite);
    return NULL;
}

//
// update
void ButtonSprite::setOnClickListener(boost::function<void ()> call)
{
    onClickCall = boost::move(call);
}

void ButtonSprite::setOnPressListener(boost::function<void ()> call)
{
    onPressCall = boost::move(call);
}
void ButtonSprite::setOnReleaseListener(boost::function<void ()> call)
{
    onReleaseCall = boost::move(call);
}

void ButtonSprite::clicked()
{
    onClickCall();
}

CCSprite* ButtonSprite::getSprite()
{
    return normal;
}

CCSprite* ButtonSprite::getPressedSprite()
{
    return pressd;
}

void ButtonSprite::setSprite(CCSprite* nor)
{
    normal->removeFromParentAndCleanup(true);
    normal = nor;
    CCPoint pos = ccp(normal->getContentSize().width*0.5f,
                      normal->getContentSize().height*0.5f);
    normal->setPosition(pos);
    addChild(normal);
    setContentSize(normal->getContentSize());
}
void ButtonSprite::setPressedSprite(CCSprite* normal)
{
    
}

void ButtonSprite::onRelease()
{
    normal->setVisible(true);
    if(pressd)
        pressd->setVisible(false);
}
void ButtonSprite::onPressed()
{
    normal->setVisible(false);
    if(pressd)
        pressd->setVisible(true);
}

