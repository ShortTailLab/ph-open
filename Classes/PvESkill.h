//
//  PvESkill.h
//  HelloCpp
//
//  Created by Zeyang Li on 5/20/13.
//
//

#pragma once

#include "ActiveSkill.h"

namespace PH
{
    class BoardLayer;

    // All PvE active skills are listed here
    class PvEActive
    {
    public:
        static TaskPtr Damage(BoardLayer* layer,
                              HeroControl* hc,
                              GemUtils::GemColor color,
                              float damage,
                              bool isArea,
                              bool ignoreArmor);
        
        static TaskPtr DamageByValue(BoardLayer* layer,
                                     HeroControl* hc,
                                     float damage,
                                     bool isArea);

        static TaskPtr DamageByFactor(BoardLayer* layer,
                                      HeroControl* hc,
                                      float factor,
                                      bool isArea);

        static TaskPtr DamageByFactorWithoutArmor(BoardLayer * layer,
                                                  HeroControl * hc,
                                                  GemUtils::GemColor color,
                                                  float factor,
                                                  bool isArea);

        static TaskPtr DamageByFactorFromColor(BoardLayer* layer,
                                               HeroControl* hc,
                                               float factor,
                                               GemUtils::GemColor a,
                                               bool isArea);

        static TaskPtr DamageByEnemyHealthFactor(BoardLayer* layer,
                                                 HeroControl* hc,
                                                 float factor,
                                                 bool isArea);

        static TaskPtr DamageByPlayerHealthFactor(BoardLayer* layer,
                                                  HeroControl* hc,
                                                  float factor,
                                                  bool isArea);

        static TaskPtr DamageBySelfDamageFactor(BoardLayer* layer,
                                                HeroControl* hc,
                                                float healthFactor,
                                                float attackFactor,
                                                bool isArea);

        static TaskPtr DamageFromHealthGem(BoardLayer* layer,
                                           HeroControl* hc,
                                           float factor,
                                           bool isArea);

        static TaskPtr DamageByGems(BoardLayer * layer,
                                    HeroControl * hc,
                                    float factor,
                                    GemUtils::GemColor gemColor,
                                    GemUtils::GemColor attColor,
                                    bool isArea);
        
        static TaskPtr DamageAndHealByFactor(BoardLayer * layer,
                                             HeroControl * hc,
                                             float factor,
                                             bool isArea);
        
        static TaskPtr DamageByFactorAndEnemyHealth(BoardLayer * layer,
                                                    HeroControl * hc,
                                                    float factor,
                                                    float healthRatio,
                                                    float damageFactor,
                                                    bool isArea);
        
        static TaskPtr AmplifyDamageByFactor(BoardLayer * layer,
                                             HeroControl * hc,
                                             int duration,
                                             float factor,
                                             GemUtils::GemColor color,
                                             bool isArea);

        static TaskPtr MultipleAttacksByFactor(BoardLayer* layer,
                                               HeroControl* hc,
                                               int count,
                                               float factor,
                                               bool isArea);

        static TaskPtr LeechByFactor(BoardLayer* layer,
                                     HeroControl* hc,
                                     float attackFactor,
                                     float leechFactor);

        static TaskPtr PoisonByValue(BoardLayer* layer,
                                     HeroControl* hc,
                                     int duration,
                                     float val);

        static TaskPtr PoisonByFactor(BoardLayer* layer,
                                      HeroControl* hc,
                                      int duration,
                                      float factor);

        static TaskPtr Blind(BoardLayer* layer,
                             HeroControl* hc,
                             int duration,
                             bool toAll);

        static TaskPtr Silence(BoardLayer* layer,
                               HeroControl* hc,
                               int duration,
                               bool toAll);

        static TaskPtr ReduceEnemyArmorByFactor(BoardLayer* layer,
                                                HeroControl* hc,
                                                int duration,
                                                float factor);

        static TaskPtr WeakenEnemyAttackByFactor(BoardLayer * layer,
                                                 HeroControl * hc,
                                                 int duration,
                                                 float factor,
                                                 bool isArea);
        
        static TaskPtr ReduceAttackFactorByColor(BoardLayer* layer,
                                                 HeroControl* hc,
                                                 int duration,
                                                 float factor,
                                                 GemUtils::GemColor color);
        
        static TaskPtr ExtendEnemyWaitTurn(BoardLayer * layer,
                                           HeroControl * hc,
                                           int val,
                                           GemUtils::GemColor color);
        
        static TaskPtr TransformAllGem(BoardLayer* layer);
        
        static TaskPtr BloodForBlood(BoardLayer* layer,
                                     HeroControl* hc,
                                     float ratio,
                                     float factor,
                                     bool isArea);
        
        static TaskPtr EyeForEye(BoardLayer* layer,
                                 HeroControl* hc,
                                 float attacker,
                                 float defender,
                                 bool isArea);
    };
}
