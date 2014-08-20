//
//  AnimPreset.h
//  HelloCpp
//
//  Created by Zeyang Li on 12/4/12.
//
//

#pragma once

#include <cocos2d.h>

#include "Board/Task.h"
#include "Board/StatusLabel.h"

namespace PH
{
    // helper functions for creating animation presets.
    boost::function<void()> removeGen(CCNode* n);
    
    float calOrbDelay(const CCPoint& src, const CCPoint& dst);
    TaskPtr createOrb(CCNode* parent, CCPoint src, CCPoint dst, bool delay = true);
    
    TaskPtr createFloatWordText(CCNode* parent,
                                const std::string & str,
                                CCPoint src,
                                float delayLaunch);
    
    TaskPtr createFloatText(CCNode* parent,
                            int val,
                            CCPoint src,
                            GemUtils::GemColor color,
                            float delayLaunch);
    
    TaskPtr createTwoPhaseFloatText(CCNode* parent,
                                    const int phase1Val,
                                    const int phase2Val,
                                    const CCPoint src,
                                    const GemUtils::GemColor color,
                                    const float delay);
    
    TaskPtr createEnergyText(CCNode* parent, int val, CCPoint src);
    
    CCSprite* createComboTextLabel(const int val, const CCPoint src);
    
    //CCFiniteTimeAction* createFadeThenRemove(GemPtr gem, float time);
    CCFiniteTimeAction* createIconJumpSequence(CCPoint p, float delayLaunch);
    
    CCSprite* createShadowGem(GemUtils::GemColor c);
    
    TaskPtr createAttackAnim(CCLayer* parent, CCPoint p, GemUtils::GemColor c);
    
    CCFiniteTimeAction* createNumberAnimSeq(float from,
                                            float to,
                                            float duration);
    
    TaskPtr createDelay(CCNode* parent, float t);
    
    TaskPtr createPropertyAnim(StatusLabelPtr propertyLabel);
    
    TaskPtr createComboTextFadeRemove(CCSprite* comboText, int index);
    
    TaskPtr moveGemAnim(CCSprite* gem, Vec2i gridP);
    TaskPtr moveGemAnimSlow(CCSprite* gem, Vec2i gridP);
    
    CCFiniteTimeAction* createShake(float t, CCPoint p, float rmin, float rmax);
    
    CCAnimation* createLoadingCharacter();

    TaskPtr createVictoryAnim(CCNode* parent, const CCPoint& center);
#define DEFEAT_ANIM_TAG   0x1008
    TaskPtr createDefeatAnim(CCNode* parent, const CCPoint& center);
    TaskPtr createFlyin(CCNode* node, float duration);
}
