//
//  PvPSkill.h
//  HelloCpp
//
//  Created by Zeyang Li on 5/17/13.
//
//

#pragma once

#include <cocos2d.h>

#include "Gem.h"
#include "Grid2D.h"
#include "PlayerControl.h"
#include "HeroControl.h"
#include "Task.h"

namespace PH
{
    USING_NS_CC;
    USING_NS_CC_EXT;
    
    class PVPBoardLayer;
    
    class PvPActive
    {
    public:
        
        static TaskPtr DamageByValue(CCLayer *layer,
                                     HeroControl* hc,
                                     PlayerControlPtr defender,
                                     float damage,
                                     bool isArea);
        
        static TaskPtr DamageByFactor(CCLayer* layer,
                                      HeroControl* hc,
                                      PlayerControlPtr attacker,
                                      PlayerControlPtr defender,
                                      float factor,
                                      bool isArea);
 
        static TaskPtr DamageByFactorWithoutResistence(CCLayer * layer,
                                                       HeroControl * hc,
                                                       PlayerControlPtr attacker,
                                                       PlayerControlPtr defender,
                                                       GemUtils::GemColor color,
                                                       float factor);           
        
        static TaskPtr DamageByFactorFromColor(CCLayer* layer,
                                               HeroControl* hc,
                                               PlayerControlPtr attacker,
                                               PlayerControlPtr defender,
                                               float factor,
                                               GemUtils::GemColor color,
                                               bool isArea);
        
        static TaskPtr DamageByEnemyHealthFactor(CCLayer* layer,
                                                 HeroControl* hc,
                                                 PlayerControlPtr attacker,
                                                 PlayerControlPtr defender,
                                                 float factor,
                                                 bool isArea);
        
        static TaskPtr DamageByPlayerHealthFactor(CCLayer* layer,
                                                  HeroControl* hc,
                                                  PlayerControlPtr attacker,
                                                  PlayerControlPtr defender,
                                                  float factor,
                                                  bool isArea);
        
        static TaskPtr DamageByFactorAndEnemyHealth(CCLayer* layer,
                                                    HeroControl* hc,
                                                    PlayerControlPtr attacker,
                                                    PlayerControlPtr defender,
                                                    float factor,
                                                    float healthRatio,
                                                    float damageFactor);
        
        static TaskPtr DamageBySelfDamageFactor(CCLayer* layer,
                                                HeroControl* hc,
                                                PlayerControlPtr attacker,
                                                PlayerControlPtr defender,
                                                float healthFactor,
                                                float attackFactor,
                                                bool isArea);
        
        static TaskPtr DamageFromHealthGem(CCLayer* layer,
                                           GemGrid& grid,
                                           CCLayer* gemLayer,
                                           boost::function<GemUtils::GemColor()> randGen,
                                           HeroControl* hc,
                                           PlayerControlPtr attacker,
                                           PlayerControlPtr defender,
                                           float factor,
                                           bool isArea);
        
        static TaskPtr TransformAllGem(PVPBoardLayer* layer);
        
        static TaskPtr MultipleAttacksByFactor(CCLayer* layer,
                                               HeroControl* hc,
                                               PlayerControlPtr attacker,
                                               PlayerControlPtr defender,
                                               int count,
                                               float factor,
                                               bool isArea);
        
        static TaskPtr PoisonByValue(CCLayer* layer,
                                     HeroControl* hc,
                                     PlayerControlPtr defender,
                                     int duration,
                                     float val);
        
        static TaskPtr PoisonByFactor(CCLayer *layer,
                                      HeroControl *hc,
                                      PlayerControlPtr defender,
                                      int duration,
                                      float factor);
        
        static TaskPtr BlindHeroRandom(CCLayer* layer,
                                       HeroControl* caster,
                                       PlayerControlPtr defender,
                                       int count,
                                       int turn);
        
        static TaskPtr BlindHeroByColor(CCLayer *layer,
                                        HeroControl* caster,
                                        PlayerControlPtr defender,
                                        GemUtils::GemColor color,
                                        int turn);
        
        // To be implemented.
        static TaskPtr ReduceAttackByFactor(CCLayer* layer,
                                            HeroControl* caster,
                                            PlayerControlPtr defender,
                                            float factor,
                                            GemUtils::GemColor color,
                                            int turn);

        static TaskPtr ReduceResistance(CCLayer* layer,
                                        HeroControl* caster,
                                        PlayerControlPtr defender,
                                        GemUtils::GemColor color,
                                        float value,
                                        int turn);
        
        static TaskPtr ReduceOpponentDragTime(CCLayer* layer,
                                              HeroControl* caster,
                                              PlayerControlPtr defender,
                                              float secs,
                                              int turn); // turn, secs
        
        static TaskPtr ExplodeGem(PVPBoardLayer* layer,
                                  HeroControl* caster,
                                  PlayerControlPtr attacker,
                                  PlayerControlPtr defender,
                                  float factor,
                                  GemUtils::GemColor gemColor,
                                  GemUtils::GemColor atkColor);  // gem color,  float factor
        
        static TaskPtr SlowByValue(CCLayer* layer,
                                   HeroControl* caster,
                                   PlayerControlPtr defender,
                                   float val,
                                   int duration);
        
        static TaskPtr AmplifyDamageByFactor(CCLayer *layer,
                                             PH::HeroControl *hc,
                                             PlayerControlPtr defender,
                                             int duration, float factor, GemUtils::GemColor color);
    };
}
