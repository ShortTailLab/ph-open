//
//  PassiveSkill.h
//  HelloCpp
//
//  Created by Zeyang Li on 5/20/13.
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

    class PassiveSkill
    {
    public:

        static void SetReflectFactor(PlayerControlPtr player,
                                     HeroControl * hc,
                                     float factor,
                                     float probality);

        static void SetAttackFactor(PlayerControlPtr player,
                                    HeroControl* hc,
                                    float factor,
                                    GemUtils::GemColor color);
        
        static void SetRandAttackFactor(PlayerControlPtr player,
                                        HeroControl* hc,
                                        float factor,
                                        float maxFactor,
                                        float ratio,
                                        GemUtils::GemColor color);
        
        static void SetAttackFactorByTitle(PlayerControlPtr player,
                                           HeroControl* hc,
                                           float factor,
                                           const std::string& title);

        static void SetRegenFactor(PlayerControlPtr player,
                                   HeroControl * hc,
                                   float facotr,
                                   GemUtils::GemColor color);
        
        static void SetHealByFactor(PlayerControlPtr player,
                                    HeroControl * hc,
                                    float factor);
        
        static void SetDamageReduceFactor(PlayerControlPtr player,
                                          HeroControl* hc,
                                          float factor,
                                          GemUtils::GemColor color);
        
        static void SetFollowUpAttack(PlayerControlPtr player,
                                      HeroControl* hc,
                                      float factor,
                                      bool isArea);
        
        static void SetSurviveCriticalHitFactor(PlayerControlPtr player,
                                                HeroControl* hc,
                                                float factor);
        
        static void SetBonusHealthByFactor(PlayerControlPtr player,
                                           HeroControl* hc,
                                           float factor,
                                           GemUtils::GemColor c);
        
        static void SetBonusHealthByFactorByTitle(PlayerControlPtr player,
                                                  HeroControl* hc,
                                                  float factor,
                                                  const std::string& title);
        
        static void SetBonusDragTime(PlayerControlPtr player,
                                     float time);
        
        
        static void SetReduceOpponentDragTime(PlayerControlPtr defender,
                                              HeroControl* hc,
                                              float secs);
        
        static void SetBonusEnemyStartTurn(PlayerControlPtr player,
                                           HeroControl * hc,
                                           int bonus);
        
        static void SetEnergyFactor(PlayerControlPtr player,
                                    float factor,
                                    GemUtils::GemColor c);
        
        static void SetMinEnergy(PlayerControlPtr player,
                                 float minCharge,
                                 GemUtils::GemColor c);
        
        static void SetComboFactor(PlayerControlPtr,
                                   float factor);
        
        static void SetReduceDamageOnHealth(PlayerControlPtr player,
                                            HeroControl* hc,
                                            float healthFactor,
                                            float damageFactor);
        
        static void SetBoostAttackOnHealth(PlayerControlPtr player,
                                           HeroControl* hc,
                                           float healthFactor,
                                           float attackFactor);
        
        static void SetBleedingByFactor(PlayerControlPtr player,
                                        HeroControl* hc,
                                        float bleedFactor);
        
        static void SetForceAreaAttackAndFactor(PlayerControlPtr player,
                                                HeroControl* hc,
                                                float factor);
        
        static void SetGemWeight(PlayerControlPtr player,
                                 HeroControl* hc,
                                 GemUtils::GemColor color,
                                 float factor);
        
        static void SetMinComboCount(PlayerControlPtr player,
                                     int count);
        
        static void SetEnergyBonusByGemCount(PlayerControlPtr player,
                                             HeroControl* hc,
                                             int count,
                                             float energy,
                                             GemUtils::GemColor color);
        
        static void SetDamageToHealthByFactor(PlayerControlPtr player,
                                              HeroControl* hc,
                                              float percent);
        
        static void SetBonusCoins(PlayerControlPtr player,
                                  HeroControl * hc,
                                  float percent);
        
        static void SetBonusExp(PlayerControlPtr player,
                                HeroControl * hc,
                                float percent);
        
        static void SetBonusLuck(PlayerControlPtr player,
                                 HeroControl* hc,
                                 float percent);
        
        static void SetArmorByHealthRatio(PlayerControlPtr player,
                                          HeroControl* hc,
                                          float factor1,
                                          float healthRatio);
        
        static void SetExtraWaitForEnemy(PlayerControlPtr player,
                                         HeroControl* hc,
                                         int round);
        
        static void SetBonusGemInAreaAttackCheck(PlayerControlPtr player,
                                                 HeroControl* hc,
                                                 int count);
        
        static void SetFollowUpAttackWithoutResistence(PlayerControlPtr player,
                                                       HeroControl* hc,
                                                       float damage,
                                                       bool isArea);
        
        static void SetInvicible(PlayerControlPtr player,
                                 HeroControl* hc,
                                 float factor);
        
        static void SetShieldAndReflect(PlayerControlPtr player,
                                        HeroControl* hc,
                                        int last, int gap,
                                        int comboCount, int sheildCount,
                                        float reflectFactor);
    };
}
