//
//  TouchMaskLayer.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 1/3/13.
//
//

#include "TouchMaskLayer.h"
#include "Utils.h"
#include "extensions/utils.h"
#include "CCCallLambda.h"

namespace PH
{
    USING_NS_CC;

    CCSprite* createTutorialArrow(const CCPoint& p,
                                  ArrowSize type,
                                  ArrowDirection direction,
                                  bool useMoveAnim)
    {
        
        CCSprite* s = GetSprite("ui/xinshou_shouzhi.png");
        s->setAnchorPoint(ccp(0.5, 0.5));
        s->setPosition(p);
        
        if(type == ArrowSize::Small)
            s->setScale(0.8f);
        else if(type == ArrowSize::Medium)
            s->setScale(1.0f);
        else if(type == ArrowSize::Big)
            s->setScale(1.2f);
        
        CCPoint moveBy = ccp(0, 0);
        CCPoint lightPos = ccp(0, 0);
        int angle = 0;
        switch(direction)
        {
            case ArrowDirection::Up:
                angle = 0;
                lightPos = ccp(0, 55);
                moveBy = ccp(0, -10);
                break;
                
            case ArrowDirection::Right:
                angle = 90;
                lightPos = ccp(65, 0);
                moveBy = ccp(10, 0);
                break;
                
            case ArrowDirection::Down:
                angle = 180;
                lightPos = ccp(0, -65);
                moveBy = ccp(0, 10);
                break;
                
            case ArrowDirection::Left:
                angle = -90;
                lightPos = ccp(-65, 0);
                moveBy = ccp(-10, 0);
                break;
                
            default:
                break;
        }
        s->setRotation(angle);
        
        if(useMoveAnim)
        {
            CCFiniteTimeAction* anim = AnimPair(CCMoveBy::create(0.4f, moveBy));
            s->runAction(CCRepeatForever::create((CCActionInterval*)anim));
        }
        else
        {
            CCFiniteTimeAction* anim = AnimPair(CCScaleBy::create(0.4f, s->getScale() * 1.2));
            s->runAction(CCRepeatForever::create((CCActionInterval*)anim));
        }
        
        return s;
    }
    
    static CCSprite* createMaskSprite(CCLayerColor* base,
                                      const std::vector<CCRect>& cutoutList)
    {
        CCSize winSize = CCDirector::sharedDirector()->getWinSize();
        
//        CCRenderTexture* rt = CCRenderTexture::create(winSize.width, winSize.height);
        
        // use a low-precision format
        CCRenderTexture* rt = CCRenderTexture::create(winSize.width,
                                                      winSize.height,
                                                      kTexture2DPixelFormat_RGBA8888);
        rt->setAnchorPoint(ccp(0, 0));
        rt->setPosition(ccp(0, 0));
        
        ccBlendFunc cutoutBlend = {GL_DST_ALPHA, GL_ZERO};
        
        // render to texture
        rt->begin();
        base->visit();

        CCSprite* s = CCSprite::create("ui/1x1_b.png");
        s->setAnchorPoint(ccp(0, 0));
        s->setOpacity(0);
        
        for(const CCRect& cutout : cutoutList)
        {
            s->setPosition(cutout.origin);
            s->setScaleX(cutout.size.width * contentScale());
            s->setScaleY(cutout.size.height * contentScale());

            s->setBlendFunc(cutoutBlend);
            s->visit();
        }
        rt->end();
        // render to texture done
        
        std::string randName = std::string("mask_image_") + boost::lexical_cast<std::string>(rand());

        CCImage* img = rt->newCCImage(false);
        img->autorelease();
        
        CCTexture2D* tex = CCTextureCache::sharedTextureCache()->addUIImage(img,
                                                                            randName.c_str());
        CCSprite *mask = CCSprite::createWithTexture(tex);
        mask->setFlipY(true);
        mask->setAnchorPoint(ccp(0, 0));
        mask->setPosition(ccp(0, 0));
        
        return mask;
    }
    
    //------------------------
    // Arrow Mask stuff
    //------------------------
    bool ArrowMask::init()
    {
        if(!CCLayer::init())
            return false;
        
        this->setTouchEnabled(true);
        
        mMask = NULL;
        mArrow = NULL;
        mArrow2 = NULL;
        mArrows = NULL;
        light = NULL;
        
        return true;
    }
    
    void ArrowMask::setMask(const CCRect& rect, bool useDarkBG)
    {
        // Safe guard against repeated creation of mask
        if(mMask)
        {
            mMask->removeFromParentAndCleanup(true);
            mMask = NULL;
        }
        
        mCutout = rect;
        
        if(useDarkBG)
//        if(false)
        {
            ccColor4B color = useDarkBG ? ccc4(0, 0, 0, 180) : ccc4(0, 0, 0, 0);
            CCLayerColor* baseMask = CCLayerColor::create(color);
            
            std::vector<CCRect> cutouts;
            cutouts.push_back(rect);
            mMask = createMaskSprite(baseMask, cutouts);
            this->addChild(mMask);
        }
        else
        {
            mMask = NULL;
        }
        
        if(getParent() == NULL)
            runningScene()->addChild(this, 1000);
    }
    
    
    // Arrow 1
    void ArrowMask::setArrow(const CCPoint& p,
                             ArrowSize type,
                             ArrowDirection dir)
    {
        clearArrow();
        
        mArrow = createTutorialArrow(p, type, dir, true);
        this->addChild(mArrow, 1);
        
        CCPoint lightPos = ccp(0, 0);
        int angle = 0;
        switch(dir)
        {
            case ArrowDirection::Up:
                angle = 0;
                lightPos = ccp(0, 55);
                break;
                
            case ArrowDirection::Right:
                angle = 90;
                lightPos = ccp(55, 0);
                break;
                
            case ArrowDirection::Down:
                angle = 180;
                lightPos = ccp(0, -55);
                break;
                
            case ArrowDirection::Left:
                angle = -90;
                lightPos = ccp(-55, 0);
                break;
                
            default:
                break;
        }

        light = GetSprite("gameui/xinshou_shouzhi_guang.png");
        light->setRotation(angle);
        light->setPosition(p+lightPos);
        light->setAnchorPoint(ccp(0.5, 0.5));
        this->addChild(light);
        
        light->runAction(CCRepeatForever::create(CCSequence::create(
                                                    CCCallLambda::create([=](){light->setScale(1);}),
                                                    CCSpawn::create(CCFadeOut::create(0.6),
                                                                     CCScaleTo::create(0.6, 2),
                                                                     NULL),
                                                NULL)));
        
        if(getParent() == NULL)
            runningScene()->addChild(this, 1000);
    }
    
    void ArrowMask::setArrow(const CCPoint& p, ArrowDirection dir)
    {
        setArrow(p, ArrowSize::Medium, dir);
    }
    
    void ArrowMask::addArrow(const CCPoint& p,ArrowDirection dir)
    {
        if(!mArrows)
        {
            mArrows = CCLayer::create();
            this->addChild(mArrows);
        }
        
        mArrows->addChild(createTutorialArrow(p, ArrowSize::Medium, dir, true));
        
        if(getParent() == NULL)
            runningScene()->addChild(this, 1000);
    }
    
    void ArrowMask::clearArrow()
    {
        if(mArrow)
        {
            mArrow->removeFromParentAndCleanup(true);
            mArrow = NULL;
        }
        if(light)
        {
            light->removeFromParentAndCleanup(true);
            light = NULL;
        }
        
        if(mArrows)
        {
            mArrows->removeFromParentAndCleanup(true);
            mArrows = NULL;
        }
    }
    
    // Arrow 2
    void ArrowMask::setArrow2(const CCPoint& p,
                              ArrowSize type,
                              ArrowDirection dir)
    {
        clearArrow2();
        
        mArrow2 = createTutorialArrow(p, type, dir, true);
        this->addChild(mArrow2, 1);
        
        if(getParent() == NULL)
            runningScene()->addChild(this, 1000);
    }
    
    void ArrowMask::setArrow2(const CCPoint& p, ArrowDirection dir)
    {
        setArrow2(p, ArrowSize::Medium, dir);
    }
    
    void ArrowMask::clearArrow2()
    {
        if(mArrow2)
        {
            mArrow2->removeFromParentAndCleanup(true);
            mArrow2 = NULL;
        }
    }
    
    void ArrowMask::clear()
    {
        if(mMask)
        {
            mMask->removeFromParentAndCleanup(true);
            mMask = NULL;
            
            mCutout = CCRect(0, 0, 640, 960);
        }
        
        clearArrow();
        clearArrow2();
        
        this->removeFromParentAndCleanup(true);
    }
    
    void ArrowMask::registerWithTouchDispatcher()
    {
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -1000, true);
    }
    
    bool ArrowMask::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
    {
        CCPoint p = this->convertTouchToNodeSpace(pTouch);
        
        CCRect rect = mCutout;
        
        CCPoint lower = rect.origin;
        CCPoint upper = ccp(rect.origin.x + rect.size.width,
                            rect.origin.y + rect.size.height);
        
        // pass on events inside cutout
        if(p.x > lower.x && p.y > lower.y &&
           p.x < upper.x && p.y < upper.y)
        {
            return false;
        }

        return true;
    }
    
    void ArrowMask::fadeInBackground(float time)
    {
        if(mMask)
        {
            mMask->setOpacity(0);
            mMask->runAction(CCFadeIn::create(time));
        }
    }
}