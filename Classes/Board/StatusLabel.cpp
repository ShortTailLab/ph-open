//
//  StatusLabel.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 11/30/12.
//
//

#include "StatusLabel.h"
#include "Fonts.h"
#include "Utils.h"
#include "Gem.h"
#include "CCCallLambda.h"
#include <extensions/utils.h>

namespace PH
{
    StatusLabel::StatusLabel() : root(NULL), bg(NULL), textLabel(NULL)
    {
        root = CCNode::create();
        root->setAnchorPoint(ccp(0.5f, 0.5f));
        root->retain();
    }
    
    StatusLabel::~StatusLabel()
    {
        root->removeFromParentAndCleanup(true);
        root->release();
    }
    
    TaskPtr StatusLabel::show()
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        auto* action = CCSequence::create(CCScaleTo::create(0.3f, 1.45f),
                                          CCScaleTo::create(0.1f, 1.0f),
                                          NULL);
        batch << TaskAnim::make(root, action);
        
        return batch;
    }
    
    TaskPtr StatusLabel::hide()
    {
        TaskBatchPtr batch = TaskBatch::make();
        float fadeTime = 0.5;
        batch << TaskAnim::make(bg,        CCFadeOut::create(fadeTime))
              << TaskAnim::make(textLabel, CCFadeOut::create(fadeTime));
        
        auto self = this->shared_from_this();
        bg->runAction(CCSequence::create(CCFadeOut::create(fadeTime),
                                         CCCallLambda::create([=]()
                                         {
                                             this->root->removeFromParent();
                                             auto delay = self;
                                         }),
                                         NULL));
        textLabel->runAction(CCFadeOut::create(fadeTime));
        return TaskIgnore::make();
    }
    
    void StatusLabel::showRaw()
    {
        auto* action = CCSequence::create(CCScaleTo::create(0.3f, 1.45f),
                                          CCScaleTo::create(0.1f, 1.0f),
                                          NULL);
        root->stopAllActions();
        root->runAction(action);
    }
    void StatusLabel::hideAndRemoveRaw()
    {
        float fadeTime = 0.5f;
        bg->stopAllActions();
        textLabel->stopAllActions();
        bg->runAction(CCSequence::create(
                      CCFadeOut::create(fadeTime+0.1f),
                      CCCallLambda::create([=]()
                                           {
                                               this->root->removeFromParent();
                                           }), NULL));
        textLabel->runAction(CCFadeOut::create(fadeTime));
    }
    
    static const char* ColorToStatusBG(GemUtils::GemColor c)
    {
        switch(c)
        {
            case GemUtils::Fire:
                return "ui/hong_fangbian.png";
            case GemUtils::Water:
                return "ui/lan_fangbian.png";
            case GemUtils::Wood:
                return "ui/lv_fangbian.png";
            case GemUtils::Dark:
                return "ui/zi_fangbian.png";
            case GemUtils::Light:
                return "ui/huang_fangbian.png";
            case GemUtils::AllColor:
            default:
                return "ui/bai_fangbian.png";
        }
    }
    
    static const char* ColorToStatusFont(GemUtils::GemColor c)
    {
        switch (c)
        {
            case GemUtils::Fire:
                return "bmfont/KTL_Red_28_Status.fnt";
            case GemUtils::Water:
                return "bmfont/KTL_Blue_28_Status.fnt";
            case GemUtils::Wood:
                return "bmfont/KTL_Green_28_Status.fnt";
            case GemUtils::Light:
                return "bmfont/KTL_Yellow_28_Status.fnt";
            case GemUtils::Dark:
                return "bmfont/KTL_Purple_28_Status.fnt";
            default:
                return "bmfont/KTL_White_28_Status.fnt";
        }
    }
    
    void StatusLabel::setTextAndColor(std::string text, GemUtils::GemColor c)
    {
        if(bg)
            bg->removeFromParentAndCleanup(true);
        bg = GetSprite(ColorToStatusBG(c));
        bg->setAnchorPoint(ccp(0.5f, 0.5f));
        root->addChild(bg); // set zorder explicitly below text
        
        CCLabelBMFont* old = textLabel;
        
        textLabel = CCLabelBMFont::create(text.c_str(), ColorToStatusFont(c));
        textLabel->setAnchorPoint(ccp(0.5f, 0.5f));
        textLabel->setPosition(ccp(-3.0f, -1.5f));
        textLabel->setScale(contentScale()*0.9f);
        root->addChild(textLabel);
        
        root->setScale(0.0f);
        
        // COCOS2DX BUG: remove after new sprite batch is created
        // THIS is remedied by changing CC_ENABLE_GL_STATE_CACHE to 0
        // in ccConfig.h
        if(old)
            old->removeFromParentAndCleanup(true);
    }
    
    static inline bool isInSlots(std::vector<StatusLabelPtr>& slots, StatusLabelPtr label)
    {
        return std::find(slots.begin(), slots.end(), label) != slots.end();
    }
    
    static int findSlotForLabel(std::vector<StatusLabelPtr>& slots, StatusLabelPtr label)
    {
        for(int i=0; i<slots.size(); i++)
            if(slots[i] == NULL || slots[i] == label)
                return i;
        return slots.size();
    }
    
    //----------------------
    // Status Label Group
    //----------------------
    TaskPtr StatusLabelGroup::setLabelWithAnim(CCLayer* layer,
                                               StatusLabelPtr label,
                                               std::string text,
                                               GemUtils::GemColor color,
                                               int zorder)
    {
        TaskSequencePtr seq = TaskSequence::make();
        
        /*
         ISSUES:
            StatusLabel::show will fail if setTextAndColor is not called.
            adding status label must be a delayed operation since 
            status labels could be removed in previous updates in the same frame.
         */

        
        // This must be delayed since label could be already scheduled
        // to be removed frame parent in the same frame.
        seq << TaskLambda::make([=]()
        {
            int i = findSlotForLabel(statusSlots, label);
            
            CCPoint p = evalPosition(i);
            label->setPosition(p);
            
            // not in slots
            if(statusSlots.size() == i)
            {
                statusSlots.push_back(label);
                if(layer)
                    layer->addChild(label->getRoot(), zorder);
            }
            // check if already in slots
            else if(!isInSlots(statusSlots, label))
            {
                statusSlots[i] = label;
                if(layer)
                    layer->addChild(label->getRoot(), zorder);
            }
            
            label->setTextAndColor(text, color);
        });
    
        seq << label->show();
        
        return seq;
    }
    
    TaskPtr StatusLabelGroup::clearLabelWithAnim(StatusLabelPtr label, bool occupy)
    {
        // Do not ever erase from status slots, only reset its pointers.
        // Each slot's index is use to compute its position
        auto x = std::find(statusSlots.begin(),
                           statusSlots.end(),
                           label);
        if(x != statusSlots.end())
        {
            x->reset();
            
            TaskSequencePtr seq = TaskSequence::make();
            
            if(!occupy){
                seq << TaskLambda::make([=]()
                {
                    label->hideAndRemoveRaw();
                });
            }else{
                seq << label->hide();
                seq << TaskLambda::make([=]()
                {
                    label->getRoot()->removeFromParent();
                });
            }
            
            return seq;
        }
        return TaskIgnore::make();
    }
}