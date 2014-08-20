//
//  PassiveSkill.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 5/20/13.
//
//

#include "PassiveSkill.h"
#include "Board/Board.h"
#include "Board/Utils.h"
#include "CCCallLambda.h"
#include "EnemyControl.h"

namespace PH
{
    // **********************************************************
    //                    Passive Skills
    // **********************************************************
    
    void PassiveSkill::SetReflectFactor(PlayerControlPtr player,
                                        HeroControl * hc,
                                        float factor,
                                        float probality)
    {
        player->passiveReflectFactor(hc, std::make_pair(std::make_pair(factor, probality), hc->getColor()));
    }

    void PassiveSkill::SetAttackFactor(PlayerControlPtr player,
                                       HeroControl* hc,
                                       float factor,
                                       GemUtils::GemColor color)
    {
        player->passiveAttackFactor[color](hc, factor);
    }
    
    void PassiveSkill::SetRandAttackFactor(PlayerControlPtr player,
                                           PH::HeroControl *hc,
                                           float factor,
                                           float maxFactor,
                                           float ratio,
                                           GemUtils::GemColor color)
    {
        player->passiveRandAttackFactor[color](hc, std::make_pair(std::make_pair(factor, maxFactor), ratio));
    }
    
    void PassiveSkill::SetAttackFactorByTitle(PlayerControlPtr player,
                                              HeroControl* hc,
                                              float factor,
                                              const std::string& title)
    {
        player->passiveTitleAttackFactorMap[title](hc, factor);
    }

    void PassiveSkill::SetRegenFactor(PlayerControlPtr player,
                                      PH::HeroControl *hc,
                                      float factor,
                                      GemUtils::GemColor color)
    {
        player->passiveRegenFactor[color](hc, factor);
    }
    
    void PassiveSkill::SetHealByFactor(PlayerControlPtr player,
                                       PH::HeroControl *hc,
                                       float factor)
    {
        player->passiveHealByFactor(hc, factor);
    }
    
    void PassiveSkill::SetDamageReduceFactor(PlayerControlPtr player,
                                             HeroControl* hc,
                                             float factor,
                                             GemUtils::GemColor color)
    {
        player->passiveDamageReduceFactor[color](hc, factor);
    }
    
    void PassiveSkill::SetFollowUpAttack(PlayerControlPtr player,
                                         HeroControl* hc,
                                         float factor,
                                         bool isArea)
    {
        player->passiveFollowUpAttacks(hc, std::make_pair(isArea,factor));
    }

    void PassiveSkill::SetSurviveCriticalHitFactor(PlayerControlPtr player,
                                                   HeroControl* hc,
                                                   float factor)
    {
        player->passiveSurviveHitFactor(hc, factor);
    }
    
    //--------------------------
    // Bonus health skills
    //--------------------------
    // By color
    void PassiveSkill::SetBonusHealthByFactor(PlayerControlPtr player,
                                              PH::HeroControl *hc,
                                              float factor,
                                              GemUtils::GemColor c)
    {
        // Multiple factors are multiplied
        float sum = 0.0f;
        for(HeroControl* hc : player->team)
        {
            if(hc->getColor() == c || c == GemUtils::AllColor)
            {
                hc->health = hc->health * factor;
            }
            sum += hc->health;
        }
        player->setMaxHealthDirectly(sum, true);
    }
    
    // By faction
    void PassiveSkill::SetBonusHealthByFactorByTitle(PlayerControlPtr player,
                                                       PH::HeroControl *hc,
                                                       float factor,
                                                       const std::string& title)
    {
        float sum = 0.0f;
        for(HeroControl* hc : player->team)
        {
            if(hc->info.profile->title == title)
            {
                hc->health = hc->health * factor;
            }
            sum += hc->health;
        }
        player->setMaxHealthDirectly(sum, true);
    }
    //----------------------------
 
    void PassiveSkill::SetBonusDragTime(PlayerControlPtr player, float time)
    {
        player->moveTimer.maxTime = std::max(player->moveTimer.maxTime,
                                             player->moveTimer.baseMaxTime + time);
        player->moveTimer.time = player->moveTimer.maxTime;
    }
    
    void PassiveSkill::SetReduceOpponentDragTime(PlayerControlPtr defender,
                                                 PH::HeroControl *hc,
                                                 float secs)
    {
        defender->moveTimer.time -= secs;
        defender->moveTimer.time = std::max(defender->moveTimer.time, 1.f);
    }
    
    
    void PassiveSkill::SetEnergyFactor(PlayerControlPtr player, float factor, GemUtils::GemColor c)
    {
        player->mEnergyFactor[c] *= factor;
    }
    
    void PassiveSkill::SetMinEnergy(PlayerControlPtr player, float minEnergy, GemUtils::GemColor c)
    {
        player->mMinEnergy[c] += minEnergy;
    }
    
    void PassiveSkill::SetComboFactor(PlayerControlPtr player, float factor)
    {
        player->comboFactor += factor;
    }
    
    void PassiveSkill::SetReduceDamageOnHealth(PlayerControlPtr player,
                                               HeroControl* hc,
                                               float healthFactor,
                                               float reduceFactor)
    {
        player->passiveReduceDamageFactorOnHealth(hc,
                                                  std::pair<float, float>(healthFactor,
                                                                          reduceFactor));
    }
    
    void PassiveSkill::SetBoostAttackOnHealth(PlayerControlPtr player,
                                              HeroControl* hc,
                                              float healthFactor,
                                              float attackFactor)
    {
        player->passiveAttackFactorOnHealth(hc,
                                            std::pair<float, float>(healthFactor,
                                                                    attackFactor));
    }
    
    void PassiveSkill::SetBleedingByFactor(PlayerControlPtr player,
                                           HeroControl* hc,
                                           float bleedFactor)
    {
        player->passiveBleeding(hc, hc->health * bleedFactor);
    }
    
    void PassiveSkill::SetForceAreaAttackAndFactor(PlayerControlPtr player,
                                                   HeroControl* hc,
                                                   float factor)
    {
        player->passiveForceAreaAttackAndFactor(hc, factor);
    }
    
    void PassiveSkill::SetGemWeight(PlayerControlPtr player,
                                    HeroControl *hc,
                                    GemUtils::GemColor color,
                                    float factor)
    {
        player->mGemWeights[color] *= factor;
    }
    
    void PassiveSkill::SetMinComboCount(PlayerControlPtr player, int count)
    {
        player->minComboCount += count;
    }
    
    void PassiveSkill::SetEnergyBonusByGemCount(PlayerControlPtr player,
                                                HeroControl* hc,
                                                int count,
                                                float energy,
                                                GemUtils::GemColor color)
    {
        player->passiveEnergyBonusByGemCount[color](hc,
                                                    std::pair<float, float>((float)count,
                                                                            energy));
    }
    
    void PassiveSkill::SetDamageToHealthByFactor(PlayerControlPtr player,
                                                 HeroControl* hc,
                                                 float factor)
    {
        player->passiveDamageToHealthFactor(hc, factor);
    }
    
    void PassiveSkill::SetBonusCoins(PlayerControlPtr player,
                                     HeroControl * hc,
                                     float percent)
    {
        player->passiveBonusCoins(hc, percent);
    }
    
    void PassiveSkill::SetBonusExp(PlayerControlPtr player,
                                   HeroControl * hc,
                                   float percent)
    {
        player->passiveBonusExp(hc, percent);
    }
    
    void PassiveSkill::SetBonusLuck(PlayerControlPtr player,
                                    PH::HeroControl *hc,
                                    float percent)
    {
        player->passiveBonusLuck(hc, percent);
    }
    
    void PassiveSkill::SetBonusEnemyStartTurn(PlayerControlPtr player,
                                              HeroControl * hc,
                                              int bonus)
    {
        player->passiveBonusEnemyStartTurn(hc, bonus);
    }
    
    void PassiveSkill::SetArmorByHealthRatio(PlayerControlPtr player,
                                             PH::HeroControl *hc,
                                             float factor1, float healthRatio)
    {
        player->passiveArmorByHealthRatio(hc, std::make_pair(factor1, healthRatio));
    }
    
    void PassiveSkill::SetExtraWaitForEnemy(PlayerControlPtr player,
                                            PH::HeroControl *hc,
                                            int round)
    {
        player->passiveExtraWaitForEnemy(hc, round);
    }
    
    void PassiveSkill::SetBonusGemInAreaAttackCheck(PlayerControlPtr player,
                                                    PH::HeroControl *hc,
                                                    int count)
    {
        player->passiveBonusGemInAreaAttackCheck(hc,  count);
    }
    
    void PassiveSkill::SetFollowUpAttackWithoutResistence(PlayerControlPtr player,
                                                          PH::HeroControl *hc,
                                                          float damage, bool isArea)
    {
        player->passiveFollowUpAttackWithoutResistence(hc, std::make_pair(isArea,damage));
    }
    
    void PassiveSkill::SetInvicible(PlayerControlPtr player,
                                    PH::HeroControl *hc,
                                    float factor)
    {
        player->passiveInvincible(hc, factor);
    }
    
    void PassiveSkill::SetShieldAndReflect(PlayerControlPtr player,
                                           PH::HeroControl *hc,
                                           int last, int gap,
                                           int comboCount, int shieldCount,
                                           float reflectFactor)
    {
        auto shield = PPShield();
        shield.lastRound = last;
        shield.gapRound = gap;
        shield.comboCount = comboCount;
        shield.shieldCount = shieldCount;
        shield.reflectFactor = reflectFactor;
        
        player->passiveShiled(hc, shield);
    }
    
}
