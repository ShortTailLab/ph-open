//
//  SkillContext.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 6/12/13.
//
//

#include "SkillContext.h"
#include "Board.h"
#include "PVPBoard.h"
#include "PvESkill.h"
#include "PvPSkill.h"
#include "PassiveSkill.h"

namespace PH
{
    namespace SkillContext
    {
        static BoardLayer* mBoard = NULL;
        static PVPBoardLayer* mPvPBoard = NULL;
        static HeroControl* mHC = NULL;
        
        static PlayerControlPtr mAttacker;
        static PlayerControlPtr mDefender;
        
        static TaskSequencePtr mSeq;
        static bool isPvE = true;
        
        static PlayerControlPtr mPlayer;
        
        static void InvokeSkillFromScript(const SkillInfo& skill)
        {
            CCLuaEngine* engine = CCLuaEngine::defaultEngine();
            
            LOGD("exec skill script %s\n", skill.skillScript.c_str());
            std::string errorMessage = "";
            if(engine->getLuaStack()->executeString(skill.skillScript.c_str(), errorMessage) != 0)
            {
                std::string error = skill.name + "(" + toStr(skill.sid) + ")\n" + skill.skillScript + ":\n" + errorMessage;
                SystemInform::alertMessage(runningScene(),
                                           error.c_str(),
                                           "Skill Error",
                                           [](){});
            }
        }
        
        //-----------
        // PvE
        //-----------
        void BeginPvE(BoardLayer* board, HeroControl* hc, TaskSequencePtr seq)
        {
            mBoard = board;
            mHC = hc;
            mSeq = seq;
            isPvE = true;
        }
        
        void EndPvE()
        {
            mBoard = NULL;
            mHC = NULL;
            mSeq.reset();
        }
        
        TaskPtr invokePvE(BoardLayer* layer, HeroControl* hc, int currTurn)
        {
            // no skill
            if(hc->info.profile->activeSkillId == -1)
                return TaskEmpty::make();
            
            const SkillInfo& skill = AST(hc->info.profile->activeSkillId);
            
            LOGD("* Hero %s, skill %s, id %d, call %s\n",
                 hc->info.profile->nameCN.c_str(),
                 skill.name.c_str(),
                 skill.sid,
                 skill.skillScript.c_str());
            
            if(skill.skillScript.size() == 0)
                return TaskIgnore::make();
            
            TaskSequencePtr seq = TaskSequence::make();
            
            BeginPvE(layer, hc, seq);
            InvokeSkillFromScript(skill);
            EndPvE();
            
            return seq;
        }

        //---------
        // PvP
        //---------
        void BeginPvP(PVPBoardLayer* pvpboard,
                      HeroControl* hc,
                      PlayerControlPtr attacker,
                      PlayerControlPtr defender,
                      TaskSequencePtr seq)
        {
            mPvPBoard = pvpboard;
            mHC = hc;
            mSeq = seq;
            mAttacker = attacker;
            mDefender = defender;
            isPvE = false;
        }
        
        void EndPvP()
        {
            mPvPBoard = NULL;
            mHC = NULL;
            mAttacker.reset();
            mDefender.reset();
            mSeq.reset();
        }
        
        TaskPtr invokePvP(PVPBoardLayer* layer, HeroControl* hc, int currTurn)
        {
            if(hc->info.profile->pvpActiveSkillId == -1)
                return TaskEmpty::make();
            
            const SkillInfo& skill = AST(hc->info.profile->pvpActiveSkillId);
            
            LOGD("* Hero %s, skill %s, id %d, call %s\n",
                 hc->info.profile->nameCN.c_str(),
                 skill.name.c_str(),
                 skill.sid,
                 skill.skillScript.c_str());
            
            if(skill.skillScript.size() == 0)
                return TaskIgnore::make();
            
            TaskSequencePtr seq = TaskSequence::make();
            
            BeginPvP(layer, hc, layer->getAttacker(), layer->getDefender(), seq);
            InvokeSkillFromScript(skill);
            EndPvP();
            
            return seq;
        }
        
        //----------
        // Passive
        //----------
        void BeginPassive(PlayerControlPtr player,
                          HeroControl* hc)
        {
            mPlayer = player;
            mHC = hc;
        }
        
        void EndPassive()
        {
            mPlayer.reset();
            mHC = NULL;
        }
        
        void invokePassive(CCLayer* layer,
                           PlayerControlPtr player,
                           HeroControl* hc)
        {
            int passiveSkillId = hc->info.profile->passiveSkillId;
            // no skill
            if(passiveSkillId == -1)
                return;
            
            const SkillInfo& skill = PST(passiveSkillId);
            
            LOGD("* Hero %s, skill %s, id %d, call %s\n",
                 hc->info.profile->nameCN.c_str(),
                 skill.name.c_str(),
                 skill.sid,
                 skill.skillScript.c_str());
            
            if(skill.skillScript.size() == 0)
            {
                LOGD("!!! skill script is empty\n");
                return;
            }
            
            BeginPassive(player, hc);
            InvokeSkillFromScript(skill);
            EndPassive();
        }
        
        // pvp passive
        void BeginPvPPassive(PlayerControlPtr player,
                             PlayerControlPtr defender,
                             HeroControl* hc)
        {
            mAttacker = mPlayer = player;
            mDefender = defender;
            mHC = hc;
        }
        
        void EndPvPPassive()
        {
            mPlayer.reset();
            mAttacker.reset();
            mDefender.reset();
            mHC = NULL;
        }
        
        void invokePvPPassive(CCLayer* layer,
                              PlayerControlPtr player,
                              PlayerControlPtr defender,
                              HeroControl* hc)
        {
            int sid = hc->info.profile->pvpPassiveSkillId;
            // no skill
            if(sid == -1)
                return;
            
            const SkillInfo& skill = PST(sid);
            
            LOGD("* Hero %s, skill %s, id %d, call %s\n",
                 hc->info.profile->nameCN.c_str(),
                 skill.name.c_str(),
                 skill.sid,
                 skill.skillScript.c_str());
            
            if(skill.skillScript.size() == 0)
            {
                LOGD("!!! skill script is empty\n");
                return;
            }
            
            BeginPvPPassive(player, defender, hc);
            InvokeSkillFromScript(skill);
            EndPvPPassive();
        }
    }
}

//--------------------------
// Lua interface section
//--------------------------

using namespace PH;
using namespace SkillContext;

//------------------
// Active Skills
//------------------
void ActiveTransformGemColor(std::string from, std::string to)
{
    if(isPvE)
    {
        mSeq << ActiveSkill::TransformGemColor(mBoard,
                                               mBoard->mBoardControl->getGrid(),
                                               GemUtils::strToColor(from),
                                               GemUtils::strToColor(to));
    }
    else
    {
        mSeq << ActiveSkill::TransformGemColor(mPvPBoard,
                                               mPvPBoard->mBoardControl->getGrid(),
                                               GemUtils::strToColor(from),
                                               GemUtils::strToColor(to));
    }
}

void ActiveTransformAllGem()
{
    if(isPvE)
    {
        mSeq << PvEActive::TransformAllGem(mBoard);
    }
    else
    {
        mSeq << PvPActive::TransformAllGem(mPvPBoard);
    }
}

void ActiveRandomizeGem()
{
    if(isPvE)
    {
        mSeq << ActiveSkill::RandomizeGem(mBoard,
                                          mBoard->mBoardControl->root,
                                          mBoard->mBoardControl->getGrid(),
                                          [=](){ return mBoard->randGemWeighted();} );
    }
    else
    {
        mSeq << ActiveSkill::RandomizeGem(mPvPBoard,
                                          mPvPBoard->mBoardControl->root,
                                          mPvPBoard->mBoardControl->getGrid(),
                                          [=](){ return mPvPBoard->randGemWeighted();} );
    }
}

void ActiveStopMoveTimer()
{
    if(isPvE)
    {
        mSeq << ActiveSkill::StopMoveTimer(mBoard->mPlayer);
    }
    else
    {
        mSeq << ActiveSkill::StopMoveTimer(mAttacker);
    }
}

void ActiveExtendMoveTimer(float secs)
{
    if(isPvE)
    {
        mSeq << ActiveSkill::ExtendMoveTimer(mBoard->mPlayer, secs);
    }
    else
    {
        mSeq << ActiveSkill::ExtendMoveTimer(mAttacker, secs);
    }
}

void ActiveDisableSweep(float secs)
{
    if(isPvE)
        mSeq << ActiveSkill::DisableSweep(mBoard, mBoard->mPlayer, secs);
    else
        mSeq << ActiveSkill::DisableSweep(mPvPBoard, mAttacker, secs);
}

void ActiveExtendEnemyWaitTurn(int turn, std::string color)
{
    if(isPvE)
        mSeq << PvEActive::ExtendEnemyWaitTurn(mBoard, mHC, turn, GemUtils::strToColor(color));
    else
        SystemInform::alertMessage(runningScene(),
                                   "ActiveExtendEnemyWaitTurn not supported in PvP",
                                   "Skill Error",
                                   [](){});
}

void ActiveSortGem()
{
    if(isPvE)
        mSeq << ActiveSkill::SortGem(mBoard, mBoard->mBoardControl->getGrid());
    else
        mSeq << ActiveSkill::SortGem(mPvPBoard, mPvPBoard->mBoardControl->getGrid());
}

void ActiveDamage(std::string color, float damage, bool isArea, bool ignoreArmor)
{
    if(isPvE)
        mSeq << PvEActive::Damage(mBoard, mHC, GemUtils::strToColor(color), damage, isArea, ignoreArmor);
    else
        SystemInform::alertMessage(runningScene(),
                                   "ActiveDamage not supported in PvP",
                                   "Skill Error",
                                   [](){});
}

void ActiveDamageByValue(float damage, bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::DamageByValue(mBoard, mHC, damage, isArea);
    else
        mSeq << PvPActive::DamageByValue(mPvPBoard, mHC, mDefender, damage, isArea);
}

void ActiveDamageByPlayerHealthFactor(float factor, bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::DamageByPlayerHealthFactor(mBoard, mHC, factor, isArea);
    else
        mSeq << PvPActive::DamageByPlayerHealthFactor(mPvPBoard,
                                                      mHC,
                                                      mAttacker,
                                                      mDefender,
                                                      factor,
                                                      isArea);
}

void ActiveDamageByHeroHealthFactor(float factor, bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::DamageByValue(mBoard,
                                         mHC,
                                         mHC->health * factor,
                                         isArea);
    else
        mSeq << PvPActive::DamageByValue(mPvPBoard,
                                         mHC,
                                         mDefender,
                                         mHC->health * factor,
                                         isArea);
}


void ActiveDamageByFactorFromColor(float factor, bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::DamageByFactorFromColor(mBoard,
                                                   mHC,
                                                   factor,
                                                   mHC->getColor(),
                                                   isArea);
    else
        mSeq << PvPActive::DamageByFactorFromColor(mPvPBoard,
                                                   mHC,
                                                   mAttacker,
                                                   mDefender,
                                                   factor,
                                                   mHC->getColor(),
                                                   isArea);
}

void ActiveDamageByFactorFromTeam(float factor, bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::DamageByFactorFromColor(mBoard,
                                                   mHC,
                                                   factor,
                                                   GemUtils::AllColor,
                                                   isArea);
    else
        mSeq << PvPActive::DamageByFactorFromColor(mPvPBoard,
                                                   mHC,
                                                   mAttacker,
                                                   mDefender,
                                                   factor,
                                                   GemUtils::AllColor,
                                                   isArea);
}

void ActiveDamageByFactor(float factor, bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::DamageByFactor(mBoard,
                                          mHC,
                                          factor,
                                          isArea);
    else
        mSeq << PvPActive::DamageByFactor(mPvPBoard,
                                          mHC,
                                          mAttacker,
                                          mDefender,
                                          factor,
                                          isArea);
}

void ActiveDamageByFactorWithoutArmor(std::string color,
                                      float factor,
                                      bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::DamageByFactorWithoutArmor(mBoard,
                                                      mHC,
                                                      GemUtils::strToColor(color),
                                                      factor,
                                                      isArea);
    else
        mSeq << PvPActive::DamageByFactorWithoutResistence(mBoard,
                                                           mHC,
                                                           mAttacker,
                                                           mDefender,
                                                           GemUtils::strToColor(color),
                                                           factor);
}

void ActiveDamageByEnemyHealthFactor(float factor, bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::DamageByEnemyHealthFactor(mBoard,
                                                     mHC,
                                                     factor,
                                                     isArea);
    else
        mSeq << PvPActive::DamageByEnemyHealthFactor(mPvPBoard,
                                                     mHC,
                                                     mAttacker,
                                                     mDefender,
                                                     factor,
                                                     isArea);
}

void ActiveDamageByFactorAndEnemyHealth(float factor,
                                        float healthRatio, float damageFactor,
                                        bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::DamageByFactorAndEnemyHealth(mBoard,
                                                        mHC,
                                                        factor,
                                                        healthRatio,
                                                        damageFactor,
                                                        isArea);
    else
        mSeq << PvPActive::DamageByFactorAndEnemyHealth(mPvPBoard, mHC,
                                                        mAttacker,
                                                        mDefender,
                                                        factor,
                                                        healthRatio, damageFactor);
}

void ActiveAmplifyDamageByFactor(int duration, float factor, std::string color, bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::AmplifyDamageByFactor(mBoard,
                                                 mHC,
                                                 duration,
                                                 factor,
                                                 GemUtils::strToColor(color),
                                                 isArea);
    else
        mSeq << PvPActive::AmplifyDamageByFactor(mPvPBoard,
                                                 mHC,
                                                 mDefender,
                                                 duration,
                                                 factor,
                                                 GemUtils::strToColor(color));
}

void ActiveHeal(float val)
{
    if(isPvE)
        mSeq << ActiveSkill::HealByValue(mBoard,
                                         mHC,
                                         mBoard->mPlayer,
                                         val);
    else
        mSeq << ActiveSkill::HealByValue(mPvPBoard,
                                         mHC,
                                         mAttacker,
                                         val);
}

void ActiveHealByFactor(float val)
{
    if(isPvE)
        mSeq << ActiveSkill::HealByFactor(mBoard, mHC, mBoard->mPlayer, val);
    else
        mSeq << ActiveSkill::HealByFactor(mPvPBoard, mHC, mAttacker, val);
}

void ActiveHealFull()
{
    if(isPvE)
        mSeq << ActiveSkill::HealFull(mBoard, mHC, mBoard->mPlayer);
    else
        mSeq << ActiveSkill::HealFull(mPvPBoard, mHC, mAttacker);
}

void ActiveHealByColor(int duration,
                       std::string color,
                       bool still)
{
    if(isPvE)
        mSeq << ActiveSkill::HealByColor(mBoard,
                                         mBoard->mPlayer,
                                         duration,
                                         GemUtils::strToColor(color),
                                         still);
    else
        mSeq << ActiveSkill::HealByColor(mPvPBoard,
                                         mAttacker,
                                         duration,
                                         GemUtils::strToColor(color),
                                         still);
}

void ActiveHealByMaxHealth(int duration, float factor)
{
    if(isPvE)
        mSeq << ActiveSkill::HealByMaxHealth(mBoard, mBoard->mPlayer, duration, factor);
    else
        mSeq << ActiveSkill::HealByMaxHealth(mPvPBoard, mAttacker, duration, factor);
}

void ActiveBoostAttackByColor(int duration, float val, std::string color)
{
    if(isPvE)
        mSeq << ActiveSkill::BoostAttackByColor(mBoard,
                                                mBoard->mPlayer,
                                                duration,
                                                val,
                                                GemUtils::strToColor(color));
    else
        mSeq << ActiveSkill::BoostAttackByColor(mPvPBoard,
                                                mAttacker,
                                                duration,
                                                val,
                                                GemUtils::strToColor(color));
}

void ActiveBoostAttackAll(int duration, float val)
{
    if(isPvE)
        mSeq << ActiveSkill::BoostAttackAll(mBoard,
                                            mBoard->mPlayer,
                                            duration,
                                            val);
    else
        mSeq << ActiveSkill::BoostAttackAll(mPvPBoard,
                                            mAttacker,
                                            duration,
                                            val);
}

void ActiveBoostComboFactor(int duration, float factor)
{
    if(isPvE)
        mSeq << ActiveSkill::BoostComboFactor(mBoard, mBoard->mPlayer, duration, factor);
    else
        mSeq << ActiveSkill::BoostComboFactor(mPvPBoard, mAttacker, duration, factor);
}

void ActiveBoostMaxHealth(int duration, float factor)
{
    if(isPvE)
        mSeq << ActiveSkill::BoostMaxHealth(mBoard, mBoard->mPlayer, duration, factor);
    else
        mSeq << ActiveSkill::BoostMaxHealth(mPvPBoard, mAttacker, duration, factor);
}

void ActiveBoostEnergyByColor(int duration, int val, std::string color)
{
    if(isPvE)
        mSeq << ActiveSkill::BoostEnergyByColor(mBoard, mBoard->mPlayer,
                                                duration, val,
                                                GemUtils::strToColor(color));
    else
        mSeq << ActiveSkill::BoostEnergyByColor(mPvPBoard,
                                                mAttacker,
                                                duration, val,
                                                GemUtils::strToColor(color));
}

void ActivePoisonByValue(int duration, float damage)
{
    if(isPvE)
        mSeq << PvEActive::PoisonByValue(mBoard,
                                         mHC,
                                         duration,
                                         damage);
    else
        mSeq << PvPActive::PoisonByValue(mPvPBoard,
                                         mHC,
                                         mDefender,
                                         duration,
                                         damage);
}

void ActivePoisonByFactor(int duration, float factor)
{
    if(isPvE)
        mSeq << PvEActive::PoisonByFactor(mBoard, mHC, duration, factor);
    else
        mSeq << PvPActive::PoisonByFactor(mPvPBoard, mHC, mDefender, duration, factor);
}

void ActiveBlind(int duration)
{
    if(isPvE)
        mSeq << PvEActive::Blind(mBoard, mHC, duration, true);
    else
        SystemInform::alertMessage(runningScene(),
                                   "ActiveBlind not supported in PvP",
                                   "Skill Error",
                                   [](){});
}

void ActiveBlindHeroRandom(int count, int duration)
{
    if(isPvE)
        SystemInform::alertMessage(runningScene(),
                                   "ActiveBlind not supported in PvE",
                                   "Skill Error",
                                   [](){});
    else
        mSeq << PvPActive::BlindHeroRandom(mPvPBoard,
                                           mHC,
                                           mDefender,
                                           count,
                                           duration);
}

void ActiveBlindHeroByColor(std::string color, int duration)
{
    if(isPvE)
        SystemInform::alertMessage(runningScene(),
                                   "ActiveBlind not supported in PvE",
                                   "Skill Error",
                                   [](){});
    else
        mSeq << PvPActive::BlindHeroByColor(mPvPBoard,
                                            mHC,
                                            mDefender,
                                            GemUtils::strToColor(color),
                                            duration);
}

void ActiveSilence(int duration, bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::Silence(mBoard, mHC, duration, isArea);
    else
        SystemInform::alertMessage(runningScene(),
                                   "ActiveSilence not supported in PvP",
                                   "Skill Error",
                                   [](){});
}

void ActiveReflectByFactor(int duration, int count, float factor, float probality)
{
    if(isPvE)
        mSeq << ActiveSkill::ReflectByFactor(mBoard,
                                             mHC,
                                             mBoard->mPlayer,
                                             duration,
                                             count,
                                             factor,
                                             probality);
    else
        mSeq << ActiveSkill::ReflectByFactor(mPvPBoard,
                                             mHC,
                                             mAttacker,
                                             duration,
                                             count,
                                             factor,
                                             probality);
}

void ActiveReduceEnemyArmorByFactor(int duration, float factor)
{
    if(isPvE)
        mSeq << PvEActive::ReduceEnemyArmorByFactor(mBoard,
                                                    mHC,
                                                    duration,
                                                    factor);
    else
        SystemInform::alertMessage(runningScene(),
                                   "ActiveReduceEnemyArmorByFactor not supported in PvP",
                                   "Skill Error",
                                   [](){});
}

void ActiveReduceDamageByFactor(int duration, float factor)
{
    if(isPvE)
        mSeq << ActiveSkill::ReduceDamageByFactor(mBoard,
                                                  mHC,
                                                  mBoard->mPlayer,
                                                  duration,
                                                  factor);
    else
        mSeq << ActiveSkill::ReduceDamageByFactor(mPvPBoard,
                                                  mHC,
                                                  mAttacker,
                                                  duration,
                                                  factor);
}

void ActiveMoveAgain(int count)
{
    if(isPvE)
        mSeq << ActiveSkill::MoveAgain(mBoard, mBoard->mPlayer, count);
    else
        mSeq << ActiveSkill::MoveAgain(mPvPBoard, mAttacker, count);
}

void ActiveLeechByFactor(float attackFactor, float leechFactor)
{
    if(isPvE)
        mSeq << PvEActive::LeechByFactor(mBoard, mHC, attackFactor, leechFactor);
    else
        SystemInform::alertMessage(runningScene(),
                                   "ActiveLeechByFactor not supported in PvP",
                                   "Skill Error",
                                   [](){});
}

void ActiveLeechByAttack(int duration, float leechFactor)
{
    if(isPvE)
        mSeq << ActiveSkill::LeechByAttack(mBoard, mBoard->mPlayer,
                                           duration, leechFactor);
    else
        mSeq << ActiveSkill::LeechByAttack(mPvPBoard, mAttacker,
                                           duration, leechFactor);
}

void ActiveDamageBySelfDamageFactor(float healthFactor,
                                    float attackFactor,
                                    bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::DamageBySelfDamageFactor(mBoard,
                                                    mHC,
                                                    healthFactor,
                                                    attackFactor,
                                                    isArea);
    else
        mSeq << PvPActive::DamageBySelfDamageFactor(mPvPBoard,
                                                    mHC,
                                                    mAttacker,
                                                    mDefender,
                                                    healthFactor,
                                                    attackFactor,
                                                    isArea);
}

void ActiveDamageFromHealthGem(float factor, bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::DamageFromHealthGem(mBoard, mHC, factor, isArea);
    else
    {
        SystemInform::alertMessage(runningScene(),
                                   "Skill not supported in PvP",
                                   "Skill Error",
                                   [](){});
    }
        /*
        mSeq << PvPActive::DamageFromHealthGem(mPvPBoard,
                                               mPvPBoard->mBoardControl->getGrid(),
                                               mPvPBoard->mBoardControl->root,
                                               [](){ return mPvPBoard->randGemWeighted(); },
                                               mHC,
                                               mAttacker,
                                               mDefender,
                                               factor,
                                               isArea);
         */
}

void ActiveMultipleAttacksByFactor(int count, float factor, bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::MultipleAttacksByFactor(mBoard,
                                                   mHC,
                                                   count,
                                                   factor,
                                                   isArea);
    else
        mSeq << PvPActive::MultipleAttacksByFactor(mPvPBoard,
                                                   mHC,
                                                   mAttacker,
                                                   mDefender,
                                                   count,
                                                   factor,
                                                   isArea);
}

void ActiveInstantEnergy(float factor, std::string color)
{
    if(isPvE)
        mSeq << ActiveSkill::InstantEnergy(mBoard,
                                           mHC,
                                           mBoard->mPlayer,
                                           factor,
                                           GemUtils::strToColor(color));
    else
        mSeq << ActiveSkill::InstantEnergy(mPvPBoard,
                                           mHC,
                                           mAttacker,
                                           factor,
                                           GemUtils::strToColor(color));
}

void ActiveShield(int duration, float factor, std::string color)
{
    if(isPvE)
        mSeq << ActiveSkill::Shield(mBoard,
                                    mHC,
                                    mBoard->mPlayer,
                                    duration,
                                    factor,
                                    GemUtils::strToColor(color));
    else
        mSeq << ActiveSkill::Shield(mPvPBoard,
                                    mHC,
                                    mAttacker,
                                    duration,
                                    factor,
                                    GemUtils::strToColor(color));
}

void ActiveSuckBloodFromEnemyByRegen(int duration, float factor, bool isArea)
{
    if(isPvE)
        mSeq << ActiveSkill::SuckBloodFromEnemyByRegen(mBoard,
                                                       mHC,
                                                       mBoard->mPlayer,
                                                       duration,
                                                       factor,
                                                       isArea);
    else
        mSeq << ActiveSkill::SuckBloodFromEnemyByRegen(mPvPBoard,
                                                       mHC,
                                                       mAttacker,
                                                       duration,
                                                       factor,
                                                       isArea);
}

void ActiveInvincible(int duration, float probality, std::string color)
{
    if(isPvE)
    {
        mSeq << ActiveSkill::Invincible(mBoard,
                                        mHC,
                                        mBoard->mPlayer,
                                        duration,
                                        probality,
                                        GemUtils::strToColor(color));
    }
    else
        mSeq << ActiveSkill::Invincible(mPvPBoard,
                                        mHC,
                                        mAttacker,
                                        duration,
                                        probality,
                                        GemUtils::strToColor(color));
}

void ActiveWeakenEnemyAttackByFactor(int duration, float factor, bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::WeakenEnemyAttackByFactor(mBoard,
                                                     mHC,
                                                     duration,
                                                     factor,
                                                     isArea);
    else
        SystemInform::alertMessage(runningScene(),
                                   "ActiveWeakenEnemyAttackByFactor not supported in PvP",
                                   "Skill Error",
                                   [](){});
}

void ActiveDamageByGems(float factor,
                        std::string gemColor,
                        std::string attColor,
                        bool isArea)
{
    if(isPvE)
    {
        mSeq << PvEActive::DamageByGems(mBoard,
                                        mHC,
                                        factor,
                                        GemUtils::strToColor(gemColor),
                                        GemUtils::strToColor(attColor),
                                        isArea);
    }
    else
        mSeq << PvPActive::ExplodeGem(mPvPBoard,
                                      mHC,
                                      mAttacker,
                                      mDefender,
                                      factor,
                                      GemUtils::strToColor(gemColor),
                                      GemUtils::strToColor(attColor));
}

void ActiveDamageAndHealByFactor(float factor, bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::DamageAndHealByFactor(mBoard, mHC, factor, isArea);
    else
        SystemInform::alertMessage(runningScene(),
                                   "ActiveDamageByGems not supported in PvP",
                                   "Skill Error",
                                   [](){});
}

void ActiveReduceEnergyCost(int count, float factor)
{
    if(isPvE)
        mSeq << ActiveSkill::ReduceEnergyCost(mBoard, mBoard->mPlayer,
                                              count, factor);
    else
        mSeq << ActiveSkill::ReduceEnergyCost(mPvPBoard,
                                              mAttacker,
                                              count, factor);
}

void ActiveReduceCooldownTime(int times, std::string color)
{
    if(isPvE)
        mSeq << ActiveSkill::ReduceCooldownTime(mBoard,
                                                mBoard->mPlayer,
                                                times, GemUtils::strToColor(color));
    else
        mSeq << ActiveSkill::ReduceCooldownTime(mPvPBoard,
                                                mAttacker,
                                                times, GemUtils::strToColor(color));
}

void ActiveTriggerCriticalAttack(int duration, float factor, float probality)
{
    if(isPvE)
        mSeq << ActiveSkill::TriggerCriticalAttack(mBoard, mBoard->mPlayer,
                                                   duration, factor, probality);
    else
        mSeq << ActiveSkill::TriggerCriticalAttack(mPvPBoard,
                                                   mAttacker,
                                                   duration, factor, probality);
}

void ActiveDispelPassiveBuff()
{
    if(isPvE)
        mSeq << ActiveSkill::DispelPassiveBuff(mBoard, mBoard->mPlayer);
    else
        mSeq << ActiveSkill::DispelPassiveBuff(mPvPBoard, mAttacker);
}

void ActiveAbsorbDamage(int duration, int count)
{
    if(isPvE)
        mSeq << ActiveSkill::AbsorbDamage(mBoard,
                                          mHC,
                                          mBoard->mPlayer,
                                          duration, count);
    else
        mSeq << ActiveSkill::AbsorbDamage(mPvPBoard,
                                          mHC,
                                          mAttacker,
                                          duration, count);
}

void ActiveReduceAttackByFactor(float factor, std::string color, int turn)
{
    if(isPvE)
        mSeq << PvEActive::ReduceAttackFactorByColor(mBoard, mHC,
                                                     turn, factor, GemUtils::strToColor(color));
    else
        mSeq << PvPActive::ReduceAttackByFactor(mPvPBoard, mHC, mDefender,
                                                factor, GemUtils::strToColor(color), turn);
}

void ActiveReduceResistance(float value, std::string color,int turn)
{
    if(isPvE)
        SystemInform::alertMessage(runningScene(),
                                   "ActiveReduceResistance not supported in PvE",
                                   "Skill Error",
                                   [](){});
    else
        mSeq << PvPActive::ReduceResistance(mPvPBoard, mHC, mDefender,
                                            GemUtils::strToColor(color), value, turn);
}

void ActiveReduceDamageTakenByFactor(float factor, std::string color, int turn)
{
    auto col = GemUtils::strToColor(color);
    if(isPvE)
        mSeq << ActiveSkill::ReduceDamageByColor(mBoard, mHC, mBoard->mPlayer, turn, factor, col);
    else
        mSeq << ActiveSkill::ReduceDamageByColor(mPvPBoard, mHC, mAttacker, turn, factor, col);
}

void ActiveReduceOpponentDragTime(float secs, int turn)
{
    if(!isPvE)
        mSeq << PvPActive::SlowByValue(mPvPBoard, mHC, mDefender, secs, turn);
    else
        SystemInform::alertMessage(runningScene(),
                                   "ActiveReduceOpponentDragTime not supported in PvP",
                                   "Skill Error",
                                   [](){});
}

void ActiveZombine(int count)
{
    if(isPvE)
        mSeq << ActiveSkill::Zombine(mBoard, mHC, mBoard->mPlayer, count);
    else
        mSeq << ActiveSkill::Zombine(mPvPBoard, mHC, mAttacker, count);
}

void ActiveFightBack(int duration, float factor, float probality)
{
    if(isPvE)
        mSeq << ActiveSkill::FightBack(mBoard, mHC, mBoard->mPlayer,
                                       duration, factor, probality);
    else
        mSeq << ActiveSkill::FightBack(mPvPBoard, mHC, mAttacker,
                                       duration, factor, probality);
}

void ActiveForceAreaAttack(int duration)
{
    if(isPvE)
        mSeq << ActiveSkill::ForceAreaAttack(mBoard, mHC, mBoard->mPlayer, duration);
    else
        mSeq << ActiveSkill::ForceAreaAttack(mPvPBoard, mHC, mAttacker, duration);
}

void ActiveBloodForBlood(float ratio, float factor, bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::BloodForBlood(mBoard, mHC, ratio, factor, isArea);
    else
        SystemInform::alertMessage(runningScene(),
                                   "ActiveBloodForBlood not supported in PvP",
                                   "Skill Error",
                                   [](){});
}

void ActiveEyeForEye(float attacker, float defender, bool isArea)
{
    if(isPvE)
        mSeq << PvEActive::EyeForEye(mBoard, mHC, attacker, defender, isArea);
    else
        SystemInform::alertMessage(runningScene(),
                                   "ActiveEyeForEye not supported in PvP",
                                   "Skill Error",
                                   [](){});
}


void ActiveBonusGemWeight(int duration, float factor, std::string color)
{
    auto col = GemUtils::strToColor(color);
    if(isPvE)
        mSeq << ActiveSkill::BonusGemWeight(mBoard, mBoard->mPlayer, col, duration, factor);
    else
        mSeq << ActiveSkill::BonusGemWeight(mPvPBoard, mAttacker, col, duration, factor);
}

void ActiveBoostEnergyByFactorByColor(int duration, float factor, std::string color)
{
    auto col = GemUtils::strToColor(color);
    if(isPvE)
        mSeq << ActiveSkill::BoostEnergyByFactorByColor(mBoard, mBoard->mPlayer,
                                                        duration, factor, col);
    else
        mSeq << ActiveSkill::BoostEnergyByFactorByColor(mPvPBoard, mAttacker,
                                                        duration, factor, col);
}

//------------------
// Passive Skills
//------------------
void PassiveReflectByFactor(float factor, float probality)
{
    PassiveSkill::SetReflectFactor(mPlayer,
                                   mHC,
                                   factor,
                                   probality);
}

void PassiveAttackFactor(float factor, std::string color)
{
    PassiveSkill::SetAttackFactor(mPlayer,
                                  mHC,
                                  factor,
                                  GemUtils::strToColor(color));
}

void PassiveRandomAttackFactor(float factor, float alterFactor, float probality, std::string color)
{
    PassiveSkill::SetRandAttackFactor(mPlayer, mHC,
                                      factor, alterFactor, probality,
                                      GemUtils::strToColor(color));
}

void PassiveAttackFactorByTitle(float factor, std::string title)
{
    PassiveSkill::SetAttackFactorByTitle(mPlayer,
                                        mHC,
                                        factor,
                                        title);
}

void PassiveRegenFactor(float factor, std::string color)
{
    PassiveSkill::SetRegenFactor(mPlayer,
                                 mHC,
                                 factor,
                                 GemUtils::strToColor(color));
}

void PassiveDamageReduceFactor(float factor, std::string color)
{
    PassiveSkill::SetDamageReduceFactor(mPlayer,
                                        mHC,
                                        factor,
                                        GemUtils::strToColor(color));
}

void PassiveDamageReduceAllFactor(float factor)
{
    PassiveSkill::SetDamageReduceFactor(mPlayer,
                                        mHC,
                                        factor,
                                        GemUtils::AllColor);
}

void PassiveFollowUpAttack(float factor)
{
    PassiveSkill::SetFollowUpAttack(mPlayer,
                                    mHC,
                                    factor,
                                    true);
}

void PassiveFollowUpAttack(float factor, bool isArea)
{
    PassiveSkill::SetFollowUpAttack(mPlayer,
                                    mHC,
                                    factor,
                                    isArea);
}

void PassiveSurviveCriticalHitFactor(float factor)
{
    PassiveSkill::SetSurviveCriticalHitFactor(mPlayer,
                                              mHC,
                                              factor);
}

void PassiveHealByFactor(float factor)
{
    PassiveSkill::SetHealByFactor(mPlayer,
                                  mHC,
                                  factor);
}

void PassiveSetBonusHealthByFactor(float factor, std::string color)
{
    PassiveSkill::SetBonusHealthByFactor(mPlayer,
                                         mHC,
                                         factor,
                                         GemUtils::strToColor(color));
}

void PassiveSetBonusHealthByFactorByTitle(float factor, std::string title)
{
    PassiveSkill::SetBonusHealthByFactorByTitle(mPlayer,
                                                  mHC,
                                                  factor,
                                                  title);
}

void PassiveBonusDragTime(float factor)
{
    PassiveSkill::SetBonusDragTime(mPlayer, factor);
}

void PassiveSetEnergyFactor(float factor, std::string color)
{
    PassiveSkill::SetEnergyFactor(mPlayer,
                                  factor,
                                  GemUtils::strToColor(color));
}

void PassiveSetMinEnergy(float minEnergy, std::string color)
{
    PassiveSkill::SetMinEnergy(mPlayer,
                               minEnergy,
                               GemUtils::strToColor(color));
}

void PassiveSetComboFactor(float factor)
{
    PassiveSkill::SetComboFactor(mPlayer, factor);
}

void PassiveReduceDamageOnHealth(float healthFactor, float reduceFactor)
{
    PassiveSkill::SetReduceDamageOnHealth(mPlayer,
                                          mHC,
                                          healthFactor,
                                          reduceFactor);
}


void PassiveBoostAttackOnHealth(float healthFactor, float attackFactor)
{
    PassiveSkill::SetBoostAttackOnHealth(mPlayer,
                                         mHC,
                                         healthFactor,
                                         attackFactor);
}

void PassiveBleedingByFactor(float factor)
{
    PassiveSkill::SetBleedingByFactor(mPlayer,
                                      mHC,
                                      factor);
}

void PassiveBonusCoins(float percent)
{
    PassiveSkill::SetBonusCoins(mPlayer, mHC, percent);
}

void PassiveBonusExp(float percent)
{
    PassiveSkill::SetBonusExp(mPlayer, mHC, percent);
}

void PassiveBonusLuck(float percent)
{
    PassiveSkill::SetBonusLuck(mPlayer, mHC, percent);
}

void PassiveBonusEnemyStartTurn(int bonus)
{
    PassiveSkill::SetBonusEnemyStartTurn(mPlayer, mHC, bonus);
}

void PassiveSetAreaAttackAndFactor(float factor)
{
    PassiveSkill::SetForceAreaAttackAndFactor(mPlayer, mHC, factor);
}

void PassiveSetGemWeight(std::string color, float factor)
{
    PassiveSkill::SetGemWeight(mPlayer,
                               mHC,
                               GemUtils::strToColor(color),
                               factor);
}

void PassiveSetMinComboCount(int count)
{
    PassiveSkill::SetMinComboCount(mPlayer, count);
}

void PassiveDamageToHealthByFactor(float factor)
{
    PassiveSkill::SetDamageToHealthByFactor(mPlayer, mHC, factor);
}

void PassiveSetEnergyBonusByGemCount(int count, float bonus, std::string color)
{
    PassiveSkill::SetEnergyBonusByGemCount(mPlayer,
                                           mHC,
                                           count,
                                           bonus,
                                           GemUtils::strToColor(color));
}

void PassiveNothingTodo(void){}

void PassiveSetArmorByHealthRatio(float factor, float healthRatio)
{
    PassiveSkill::SetArmorByHealthRatio(mPlayer, mHC, factor, healthRatio);
}

void PassiveSetExtraWaitForEnemy(int round)
{
    PassiveSkill::SetExtraWaitForEnemy(mPlayer, mHC, round);
}

void PassiveSetBonusGemInAreaAttackCheck(int count)
{
    PassiveSkill::SetBonusGemInAreaAttackCheck(mPlayer, mHC, count);
}

void PassiveFollowUpAttackWithoutResistence(float damage, bool isArea)
{
    PassiveSkill::SetFollowUpAttackWithoutResistence(mPlayer, mHC, damage, isArea);
}

void PassiveSetInvicible(float factor)
{
    PassiveSkill::SetInvicible(mPlayer, mHC, factor);
}

void PassiveShield(int last, int gap, int comboCount, int shieldCount, float reflectFactor)
{
    PassiveSkill::SetShieldAndReflect(mPlayer, mHC, last, gap, comboCount, shieldCount, reflectFactor);
}

void PassiveReduceOpponentDragTime(float secs)
{
    PassiveSkill::SetReduceOpponentDragTime(mDefender, mHC, secs);
}
