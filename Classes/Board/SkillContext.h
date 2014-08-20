//
//  SkillContext.h
//  HelloCpp
//
//  Created by Zeyang Li on 6/12/13.
//
//

#pragma once

#include <cocos2d.h>
#include "HeroControl.h"
#include "PlayerControl.h"

namespace PH
{
    USING_NS_CC;

    class BoardLayer;
    class PVPBoardLayer;

    namespace SkillContext
    {
        void BeginPvE(BoardLayer* board,
                      HeroControl* hc,
                      TaskSequencePtr seq);
        void EndPvE();
        TaskPtr invokePvE(BoardLayer* layer,
                          HeroControl* hero,
                          int currTurn);


        void BeginPvP(PVPBoardLayer* pvpboard,
                      HeroControl* hc,
                      PlayerControlPtr attacker,
                      PlayerControlPtr defender,
                      TaskSequencePtr seq);
        void EndPvP();
        TaskPtr invokePvP(PVPBoardLayer* layer,
                          HeroControl* hc,
                          int currTurn);

        // pve passive
        void BeginPassive(PlayerControlPtr player,
                          HeroControl* hc);
        void EndPassive();
        void invokePassive(CCLayer* layer,
                           PlayerControlPtr player,
                           HeroControl* hc);

        // pvp passive
        void BeginPvPPassive(PlayerControlPtr attacker,
                             PlayerControlPtr defender,
                             HeroControl* hc);
        void EndPvPPassive();
        void invokePvPPassive(CCLayer* layer,
                              PlayerControlPtr player,
                              PlayerControlPtr defender,
                              HeroControl* hc);
    };
}

//-----------------------
// Lua skill wrappers
//-----------------------

// tolua_begin
void ActiveTransformGemColor(std::string from, std::string to);
void ActiveTransformAllGem();
void ActiveRandomizeGem();
void ActiveStopMoveTimer();
void ActiveExtendMoveTimer(float secs);
void ActiveDisableSweep(float secs);
void ActiveExtendEnemyWaitTurn(int turn, std::string color);
void ActiveSortGem();
void ActiveMoveAgain(int count);
void ActiveInstantEnergy(float factor, std::string color);
void ActiveDispelPassiveBuff();

void ActiveDamage(std::string color, float damage, bool isArea, bool ignoreArmor);
void ActiveDamageByValue(float damage, bool isArea);
void ActiveDamageByPlayerHealthFactor(float factor, bool isArea);
void ActiveDamageByHeroHealthFactor(float factor, bool isArea);
void ActiveDamageByFactorFromColor(float factor, bool isArea);
void ActiveDamageByFactorFromTeam(float factor, bool isArea);
void ActiveDamageByFactor(float factor, bool isArea);
void ActiveDamageByFactorWithoutArmor(std::string color, float factor, bool isArea);
void ActiveDamageByEnemyHealthFactor(float percent, bool isArea);
void ActiveDamageAndHealByFactor(float factor, bool isArea);
void ActiveDamageByFactorAndEnemyHealth(float factor,
                                        float healthRatio, float damageFactor,
                                        bool isArea);
void ActiveDamageBySelfDamageFactor(float healthFactor,
                                    float attackFactor,
                                    bool isArea);
void ActiveDamageByGems(float factor, std::string gemColor, std::string attColor, bool isArea);
void ActiveMultipleAttacksByFactor(int count, float factor, bool isArea);
void ActiveLeechByFactor(float attackFactor, float leechFactor);

void ActiveHeal(float val);
void ActiveHealByFactor(float val);
void ActiveHealFull();

void ActiveBoostAttackByColor(int duration, float val, std::string color);
void ActiveBoostAttackAll(int duration, float val);
void ActiveBoostComboFactor(int duration, float factor);
void ActiveBoostMaxHealth(int duration, float factor);
void ActiveBoostEnergyByColor(int duration, int val, std::string color);

void ActiveAmplifyDamageByFactor(int duration, float factor, std::string color, bool isArea);
void ActivePoisonByValue(int duration, float damage);
void ActivePoisonByFactor(int duration, float factor);
void ActiveBlind(int duration);
void ActiveBlindHeroRandom(int count, int duration);
void ActiveBlindHeroByColor(std::string color, int duration);
void ActiveSilence(int duration, bool isArea);
void ActiveReduceEnemyArmorByFactor(int duration, float factor);
void ActiveWeakenEnemyAttackByFactor(int duration, float factor, bool isArea);

void ActiveReflectByFactor(int duration, int count, float factor, float probality);
void ActiveReduceDamageByFactor(int duration, float factor);
void ActiveLeechByAttack(int duration, float leechFactor);
void ActiveShield(int duration, float factor, std::string color);
void ActiveInvincible(int duration, float probality, std::string color);
void ActiveReduceEnergyCost(int count, float factor);
void ActiveReduceCooldownTime(int times, std::string color);
void ActiveAbsorbDamage(int duration, int count);
void ActiveTriggerCriticalAttack(int duration, float factor, float probality);
void ActiveSuckBloodFromEnemyByRegen(int duration, float factor, bool isArea);
void ActiveHealByColor(int duration, std::string color, bool still);
void ActiveHealByMaxHealth(int duration, float factor);

void ActiveReduceAttackByFactor(float factor, std::string color, int turn);
void ActiveReduceResistance(float value, std::string color, int turn);
void ActiveReduceDamageTakenByFactor(float factor, std::string color, int turn);
void ActiveReduceOpponentDragTime(float secs, int turn);

void ActiveZombine(int count);
void ActiveFightBack(int duration, float factor, float probality);
void ActiveForceAreaAttack(int duration);
void ActiveBloodForBlood(float ratio, float factor, bool isArea);
void ActiveEyeForEye(float attacker, float defender, bool isArea);
void ActiveBonusGemWeight(int duration, float factor, std::string color);
void ActiveBoostEnergyByFactorByColor(int duration, float factor, std::string color);

// tolua_end

// tolua_begin
void PassiveReflectByFactor(float factor, float probality);
void PassiveAttackFactor(float factor, std::string color);
void PassiveAttackFactorByTitle(float factor, std::string title);
void PassiveRandomAttackFactor(float factor, float alterFactor, float probality, std::string color);
void PassiveRegenFactor(float factor, std::string color);
void PassiveDamageReduceFactor(float factor, std::string color);
void PassiveFollowUpAttack(float factor, bool isArea);

void PassiveSurviveCriticalHitFactor(float factor);
void PassiveHealByFactor(float factor);
void PassiveSetBonusHealthByFactor(float factor, std::string color);
void PassiveSetBonusHealthByFactorByTitle(float factor, std::string title);
void PassiveBonusDragTime(float factor);
void PassiveSetEnergyFactor(float factor, std::string color);
void PassiveSetMinEnergy(float minEnergy, std::string color);
void PassiveSetComboFactor(float factor);
void PassiveReduceDamageOnHealth(float healthFactor, float reduceFactor);
void PassiveBoostAttackOnHealth(float healthFactor, float attackFactor);
void PassiveBleedingByFactor(float factor);

void PassiveBonusCoins(float percent);
void PassiveBonusExp(float percent);
void PassiveBonusLuck(float percent);
void PassiveBonusEnemyStartTurn(int bonus);

void PassiveSetAreaAttackAndFactor(float factor);
void PassiveSetGemWeight(std::string color, float factor);
void PassiveSetMinComboCount(int count);
void PassiveDamageToHealthByFactor(float factor);
void PassiveSetEnergyBonusByGemCount(int count, float bonus, std::string color);

void PassiveNothingTodo(void);

void PassiveSetArmorByHealthRatio(float factor, float healthRatio);
void PassiveSetExtraWaitForEnemy(int round);
void PassiveSetBonusGemInAreaAttackCheck(int count);
void PassiveFollowUpAttackWithoutResistence(float damage, bool isArea);
void PassiveSetInvicible(float factor);
void PassiveShield(int last, int gap, int comboCount, int shieldCount, float reflectFactor);

void PassiveReduceOpponentDragTime(float secs);
// tolua_end
