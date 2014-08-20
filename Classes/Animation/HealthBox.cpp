//
//  HealthBox.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 5/15/13.
//
//

#include "HealthBox.h"
#include "CCCallLambda.h"
#include "AnimPreset.h"
#include <extensions/utils.h>

namespace PH
{
    //--------------------
    // Enemy Health Box
    //--------------------
    HealthBox::HealthBox(GemUtils::GemColor c, bool _isLong)
    :
    scale(1.0),
    isLong(_isLong),
    healthBox(NULL),
    healthFill(NULL)
    {
        this->reset(c);
    }
    
    HealthBox::~HealthBox()
    {
        this->release();
    }
    
    void HealthBox::reset(GemUtils::GemColor c)
    {
        release();
        healthBox = GetSprite(GemUtils::healthBox(c, isLong));
        healthBox->retain();
        healthBox->setAnchorPoint(ccp(0.5, 0));
        
        healthFill = GetSprite(GemUtils::healthFill(c, isLong));
        healthFill->setAnchorPoint(ccp(0, 0));
        healthFill->setPosition(ccp(33, 3));
        
        healthBox->addChild(healthFill);
        healthBox->setPosition(ccp(0,10));
        
        healthFill->setScaleX(scale);
    }
    
    void HealthBox::release()
    {
        if(healthFill)
            healthFill->removeFromParentAndCleanup(true);
        
        if(healthBox)
        {
            if(healthBox->getParent())
                healthBox->removeFromParentAndCleanup(true);
            healthBox->release();
        }
    }
    
    TaskSequencePtr HealthBox::scaleTo(float f)
    {
        //float from = scale;
        float to = f;
        //float from = scale;
        //float time = std::abs(to - from) * 0.5f;
        float time = 0.45;
        TaskSequencePtr seq = TaskSequence::make();
        seq << TaskLambda::make([=]
        {
            this->healthFill->stopAllActions();
            //this->healthFill->setScaleX(from);
        });
        seq << TaskAnim::make(healthFill, CCScaleTo::create(time, to, 1.0), true);
        
        scale = to;
        
        return seq;
    }
    
    TaskBatchPtr HealthBox::fadeTo(float t, int val)
    {
        TaskBatchPtr batch = TaskBatch::make();
        batch->add(TaskAnim::make(healthBox, CCFadeTo::create(t, val)));
        batch->add(TaskAnim::make(healthFill, CCFadeTo::create(t, val)));
        return batch;
    }
    
    
    //-------------------
    // Player Health Box
    //-------------------
    PlayerHealthBox::PlayerHealthBox(float h, Style style) : maxHealth(h), currHealth(h)
    {
        // maxHealth is at least 1 to avoid devive-by-zero
        maxHealth = std::max(1.0f, h);
        
        std::string boxStyle = "";
        std::string fillStyle = "";
        if(style == HEART)
        {
            boxStyle = "ui/health_box.jpg";
            fillStyle = "health_fill.png";
        }
        else if(style == ORANGE)
        {
            boxStyle = "pvp_healthbox_theirs.png";
            fillStyle = "pvp_health_fill_theirs.png";
        }
        else if(style == PINK)
        {
            boxStyle = "pvp_health_box_ours.png";
            fillStyle = "health_fill.png";
        }
        
        // health box
        mHealthBox = GetSprite(boxStyle.c_str());
        mHealthBox->setAnchorPoint(ccp(0, 0));
        mHealthBox->setPosition(ccp(0, 530));
        mHealthBox->retain();
        
        // health bar
        mHealthBar = Get9SpriteFromFrame(fillStyle.c_str(), 5, 5, 576, 5);
        mHealthBar->setPosition(44, 7);
        mHealthBar->setAnchorPoint(ccp(0, 0.0));
        mHealthBox->addChild(mHealthBar);
        
        // health label
        mHealthLabel = CCLabelBMFont::create("", "bmfont/FGDC_28_darkblue.fnt");
        mHealthLabel->setAnchorPoint(ccp(1.0f, 0.0f));
        mHealthLabel->setScale(contentScale());
        mHealthLabel->setPosition(ccp(630, -7));
        mHealthBox->addChild(mHealthLabel);
        
        // set UI states
        resetHealth(maxHealth, maxHealth);
    }
    
    PlayerHealthBox::~PlayerHealthBox()
    {
        mHealthBox->removeFromParentAndCleanup(true);
        mHealthBox->release();
    }
    
    TaskPtr PlayerHealthBox::setHealthWithAnim(float h, float maxHp)
    {
        if(h == currHealth)
            return TaskPtr(new TaskEmpty);
        
        TaskSequencePtr seq = TaskSequence::make();
        seq << TaskLambda::make([=]()
        {
            this->mHealthBar->stopAllActions();
            this->mHealthLabel->stopAllActions();
        });
        
        auto batch = TaskBatch::make();
        if(maxHp == 0.f)
        {
            float scale = h / maxHealth;
            float time = std::abs((currHealth / maxHealth) - scale) * 1.2f;
            seq << TaskLambda::make([=]()
            {
                this->mHealthBar->runAction(CCScaleTo::create(time, scale, 1));
            });
        }
        else maxHealth = maxHp;

        float tCH = currHealth, tH = h;
        seq << TaskLambda::make([=]()
        {
            std::string maxHealhtStr = std::string("/") + toStr(this->maxHealth);
            CCCountBMTextNumber* countNumber = CCCountBMTextNumber::create(tCH,
                                                                           tH,
                                                                           0.5,
                                                                           maxHealhtStr);
            this->mHealthLabel->runAction(countNumber);
        });
        seq << createDelay(this->mHealthBox, 0.4f);
        currHealth = h;
        
        return seq;
    }
    
    void PlayerHealthBox::resetHealth(float health, float maxHP)
    {
        maxHealth = maxHP;
        currHealth = health;
        if(maxHealth != 0.0f)
            mHealthBar->setScaleX(currHealth / maxHealth);
        else
            LOGD("maxHP is 0, divide by 0");
        
        std::string healthStr = toStr(health) + "/" + toStr(maxHealth);
        mHealthLabel->setString(healthStr.c_str());
    }
}