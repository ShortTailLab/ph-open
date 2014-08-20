//
//  CCCallLambda.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 11/6/12.
//
//

#include "CCCallLambda.h"
#include "cocos2dx_extensions.h"
#include "CCScale9Sprite.h"

USING_NS_CC_EXT;

CCCallLambda * CCCallLambda::create(boost::function<void ()> lambda)
{
    CCCallLambda *pRet = new CCCallLambda();
    
    if (pRet && pRet->initWithTarget(lambda))
    {
        pRet->autorelease();
        return pRet;
    }
    
    CC_SAFE_DELETE(pRet);
    return NULL;
}

bool CCCallLambda::initWithTarget(boost::function<void ()> lambda)
{
    mLambda = lambda;
    return true;
}

CCObject * CCCallLambda::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CCCallLambda* pRet = NULL;
    
    if (pZone && pZone->m_pCopyObject) {
        //in case of being called at sub class
        pRet = (CCCallLambda*) (pZone->m_pCopyObject);
    } else {
        pRet = new CCCallLambda();
        pZone = pNewZone = new CCZone(pRet);
    }
    
    CCActionInstant::copyWithZone(pZone);
    pRet->initWithTarget(mLambda);
    CC_SAFE_DELETE(pNewZone);
    return pRet;
}

void CCCallLambda::update(float time) {
    CC_UNUSED_PARAM(time);
    this->execute();
}

void CCCallLambda::execute()
{
    if(mLambda)  mLambda();
}

// ------------------------------------
// CCSet9SpriteSize
// ------------------------------------

bool CCAnimate9SpriteProgress::init(float d, float min, float max, float dis, bool cap)
{
    assert(d != 0 && "duration cannot be 0");
    
    CCActionInterval::initWithDuration(d);
    
    duration = d;
    minWidth = min;
    maxWidth = max;
    distance = dis;
    capAtMax = cap;
    
    return true;
}

CCObject* CCAnimate9SpriteProgress::copyWithZone(cocos2d::CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CCAnimate9SpriteProgress* pCopy = NULL;
    
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (CCAnimate9SpriteProgress*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCAnimate9SpriteProgress();
        pZone = pNewZone = new CCZone(pCopy);
    }
    
    CCActionInterval::copyWithZone(pZone);
    
    pCopy->init(duration, minWidth, maxWidth, distance, capAtMax);
    
    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void CCAnimate9SpriteProgress::startWithTarget(CCNode* target)
{
    CCActionInterval::startWithTarget(target);
    
    CCScale9Sprite* sprite = (CCScale9Sprite*)target;
    beginSize = sprite->getContentSize();
}

void CCAnimate9SpriteProgress::update(float fraction)
{
    CCNode* target = getTarget();
    CCScale9Sprite* sprite = (CCScale9Sprite*)target;
    
    float width = beginSize.width + fraction * distance;

    if (capAtMax)
        width = std::min(width, maxWidth);
    else
        while (width - maxWidth > 1e-3) width -= maxWidth;
    
    if (width - minWidth < 1e-3)
        width = minWidth;
    
    sprite->setVisible(width > minWidth);
    sprite->setContentSize(CCSize(width, beginSize.height));
}
