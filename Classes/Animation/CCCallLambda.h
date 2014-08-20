//
//  CCCallLambda.h
//  HelloCpp
//
//  Created by Zeyang Li on 11/6/12.
//
//

#pragma once

#include "cocos2d.h"

#include "boost/function.hpp"
#include "boost/lexical_cast.hpp"
#include <string>

USING_NS_CC;

class CCCallLambda : public CCActionInstant //<NSCopying>
{
private:
    boost::function<void ()> mLambda;
    
public:
    CCCallLambda() {}
    virtual ~CCCallLambda() {}
    
	static CCCallLambda * create(boost::function<void ()> lambda);

    virtual bool initWithTarget(boost::function<void()> lambda);

    virtual void execute();
    //super methods
    virtual void update(float time);
    CCObject * copyWithZone(CCZone *pZone);
};

class CCCountBMTextNumber : public CCActionInterval
{
private:
    float from;
    float to;
    float duration;
    float accTime;
    
    CCNode* label;
    CCLabelProtocol* labelAsProtocol;
    
    std::string postStr;
    
public:
    CCCountBMTextNumber() : label(NULL) {}
    virtual ~CCCountBMTextNumber()
    {
        if(label)
            label->release();
    }
    
    static CCCountBMTextNumber* create(float f, float t, float d, std::string post = "")
    {
        CCCountBMTextNumber* o = new CCCountBMTextNumber();
        if(o != NULL && o->init(f, t, d, post))
        {
            o->autorelease();
            return o;
        }
        
        CC_SAFE_DELETE(o);
        return o;
    }
    
    bool init(float f, float t, float d, std::string post)
    {
        assert(d != 0 && "duration cannot be NULL");
        
        CCActionInterval::initWithDuration(d);
        
        from = f;
        to = t;
        duration = d;
        postStr = post;
        
        return true;
    }
    
    virtual CCObject* copyWithZone (CCZone *pZone)
    {
        CCZone* pNewZone = NULL;
        CCCountBMTextNumber* pCopy = NULL;
        
        if(pZone && pZone->m_pCopyObject)
        {
            //in case of being called at sub class
            pCopy = (CCCountBMTextNumber*)(pZone->m_pCopyObject);
        }
        else
        {
            pCopy = new CCCountBMTextNumber();
            pZone = pNewZone = new CCZone(pCopy);
        }
        
        CCActionInterval::copyWithZone(pZone);
        
        pCopy->init(from, to, duration, postStr);
        
        CC_SAFE_DELETE(pNewZone);
        return pCopy;
    }
    
    virtual void startWithTarget (CCNode *target)
    {
        CCActionInterval::startWithTarget(target);

        label = target;
        label->retain();
        
        labelAsProtocol = dynamic_cast<CCLabelProtocol*>(label);
    }
    
    virtual void update(float fraction)
    {
        int number = from + (to - from) * fraction;
        std::string text = boost::lexical_cast<std::string>(number) + postStr;
        labelAsProtocol->setString(text.c_str());
    }
};

typedef CCCountBMTextNumber CCCountTextNumber;


class CCAnimate9SpriteProgress : public CCActionInterval
{
private:
    float duration;
    float minWidth;
    float maxWidth;
    float distance;
    
    CCSize beginSize;
    bool capAtMax;
    
public:
    CCAnimate9SpriteProgress() {}
    virtual ~CCAnimate9SpriteProgress() {}
    
    static CCAnimate9SpriteProgress* create(float d, float min, float max, float dis, bool capAtMax=false)
    {
        CCAnimate9SpriteProgress* o = new CCAnimate9SpriteProgress();
        if(o != NULL && o->init(d, min, max, dis, capAtMax))
        {
            o->autorelease();
            return o;
        }
        
        CC_SAFE_DELETE(o);
        return o;
    }
    
    bool init(float d, float min, float max, float dis, bool capAtMax);
    
    virtual CCObject* copyWithZone (CCZone *pZone);

    virtual void startWithTarget(CCNode *target);
    virtual void update(float fraction);
};
