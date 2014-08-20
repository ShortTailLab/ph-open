//
//  HealthBox.h
//  HelloCpp
//
//  Created by Zeyang Li on 5/15/13.
//
//

#pragma once

#include <cocos2d.h>
#include "CCScale9Sprite.h"

#include "Board/Utils.h"
#include "Board/Gem.h"
#include "Board/Task.h"

namespace PH
{
    USING_NS_CC;
    USING_NS_CC_EXT;
    
    //-----------------------
    // Enemy Health Box
    //-----------------------
    class HealthBox
    {
    private:
        CCSprite* healthBox;
        CCSprite* healthFill;
        float scale;
        bool isLong;
        
    public:
        HealthBox(GemUtils::GemColor c, bool isLong=false);
        ~HealthBox();
        
        void setPosition(CCPoint p)    { healthBox->setPosition(p); }
        void setAnchorPoint(CCPoint p) { healthBox->setAnchorPoint(p); }
        
        void setOpacity(int val)
        {
            healthBox->setOpacity(val);
            healthFill->setOpacity(val);
        }
        
        void reset(GemUtils::GemColor c);
        void release();
        
        CCSprite* get(){ return healthBox; }
        
        TaskSequencePtr scaleTo(float f);
        TaskBatchPtr    fadeTo(float t, int val);
    };
    
    
    //----------------------------
    // Player Health Box
    //----------------------------
    class PlayerHealthBox
    {
    private:
        CCSprite* mHealthBox;
        CCScale9Sprite* mHealthBar;
        CCLabelBMFont* mHealthLabel;
        
        float maxHealth;
        float currHealth;
        
    public:
        enum Style
        {
            PINK,
            ORANGE,
            HEART,
        };
        
        PlayerHealthBox(float h = 1.0f, Style style = PINK);
        virtual ~PlayerHealthBox();
        
        void    setPosition(CCPoint p)  { mHealthBox->setPosition(p); }
        CCPoint getPosition() const     { return mHealthBox->getPosition(); }
        
        CCSprite* get() const
        {
            return mHealthBox;
        }
        
        CCPoint center()
        {
            //return mHealthBox->convertToWorldSpace(boundCenter(mHealthBar->boundingBox()));
            return mHealthBox->getParent()->convertToWorldSpace(boundCenter(mHealthBox->boundingBox()));
        }
        
        TaskPtr setHealthWithAnim(float health, float maxHp= 0.f);
        void resetHealth(float health, float maxHP);
    };
}

