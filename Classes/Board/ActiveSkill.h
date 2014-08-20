//
//  Skill.h
//  HelloCpp
//
//  Created by Zeyang Li on 11/7/12.
//
//

#pragma once

#include "Board/Gem.h"
#include "Board/Grid2D.h"
#include "Board/Common.h"
#include "Board/HeroControl.h"
#include "Board/PlayerControl.h"

namespace PH
{
    class ActiveSkill
    {
    public:
        static TaskPtr TransformGemColor(CCNode* gemLayer,
                                         GemGrid& grid,
                                         GemUtils::GemColor from,
                                         GemUtils::GemColor to);
        
        static TaskPtr RandomizeGem(CCNode* layer,
                                    CCNode* gemLayer,
                                    GemGrid& grid,
                                    boost::function<GemUtils::GemColor()> randGen);
        
        static TaskPtr SortGem(CCNode* layer,
                               GemGrid& grid);
        
        static TaskPtr StopMoveTimer(PlayerControlPtr player);
        
        static TaskPtr ExtendMoveTimer(PlayerControlPtr player, float secs);
        
        static TaskPtr DisableSweep(CCLayer* layer,
                                    PlayerControlPtr player,
                                    float time);
        
        static TaskPtr MoveAgain(CCLayer* layer,
                                 PlayerControlPtr player,
                                 int count);

        static TaskPtr HealByValue(CCLayer* layer,
                                   HeroControl* hc,
                                   PlayerControlPtr player,
                                   float val);
        
        static TaskPtr HealByFactor(CCLayer* layer,
                                    HeroControl* hc,
                                    PlayerControlPtr player,
                                    float factor);
        
        static TaskPtr HealFull(CCLayer* layer,
                                HeroControl* hc,
                                PlayerControlPtr player);
        
        static TaskPtr HealByColor(CCLayer * layer,
                                   PlayerControlPtr player,
                                   int duration,
                                   GemUtils::GemColor color,
                                   bool still);
        
        static TaskPtr HealByMaxHealth(CCLayer * layer,
                                       PlayerControlPtr player,
                                       int duration,
                                       float factor);
        
        static TaskPtr ReduceDamageByFactor(CCLayer* layer,
                                            HeroControl* hc,
                                            PlayerControlPtr player,
                                            int duration,
                                            float factor);
        
        static TaskPtr ReduceDamageByColor(CCLayer* layer,
                                           HeroControl* hc,
                                           PlayerControlPtr player,
                                           int duration,
                                           float factor,
                                           GemUtils::GemColor color);
        
        static TaskPtr ReflectByFactor(CCLayer* layer,
                                       HeroControl* hc,
                                       PlayerControlPtr player,
                                       int duration,
                                       int count,
                                       float val,
                                       float probality);
        
        static TaskPtr InstantEnergy(CCLayer* layer,
                                     HeroControl* hc,
                                     PlayerControlPtr player,
                                     float factor,
                                     GemUtils::GemColor c);
        
        static TaskPtr BoostAttackAll(CCLayer* layer,
                                      PlayerControlPtr player,
                                      int duration,
                                      float factor);
        
        static TaskPtr BoostAttackByColor(CCLayer* layer,
                                          PlayerControlPtr player,
                                          int duration,
                                          float factor,
                                          GemUtils::GemColor color);
        
        static TaskPtr BoostComboFactor(CCLayer * layer,
                                        PlayerControlPtr player,
                                        int duration,
                                        float factor);
        
        static TaskPtr BoostMaxHealth(CCLayer * layer,
                                      PlayerControlPtr player,
                                      int duration,
                                      float factor);
        
        static TaskPtr BoostEnergyByColor(CCLayer * layer,
                                          PlayerControlPtr player,
                                          int duration,
                                          int val,
                                          GemUtils::GemColor color);
        
        static TaskPtr BoostEnergyByFactorByColor(CCLayer* layer,
                                                  PlayerControlPtr player,
                                                  int duration,
                                                  float factor,
                                                  GemUtils::GemColor color);
        
        static TaskPtr Shield(CCLayer* layer,
                              HeroControl* hc,
                              PlayerControlPtr player,
                              int duration,
                              float damage,
                              GemUtils::GemColor color);

        static TaskPtr SuckBloodFromEnemyByRegen(CCLayer * layer,
                                                 HeroControl * hc,
                                                 PlayerControlPtr player,
                                                 int duration,
                                                 float factor,
                                                 bool isArea);

        static TaskPtr Invincible(CCLayer * layer,
                                  HeroControl * hc,
                                  PlayerControlPtr player,
                                  int duration,
                                  float probality,
                                  GemUtils::GemColor color);
        
        static TaskPtr ReduceEnergyCost(CCLayer * layer,
                                        PlayerControlPtr player,
                                        int count,
                                        float factor);
        
        static TaskPtr ReduceCooldownTime(CCLayer * layer,
                                          PlayerControlPtr player,
                                          int times,
                                          GemUtils::GemColor color);
        
        static TaskPtr TriggerCriticalAttack(CCLayer * layer,
                                             PlayerControlPtr player,
                                             int duration,
                                             float factor,
                                             float probality);
        
        static TaskPtr DispelPassiveBuff(CCLayer * layer,
                                         PlayerControlPtr player);
        
        static TaskPtr AbsorbDamage(CCLayer * layer,
                                    HeroControl * hc,
                                    PlayerControlPtr player,
                                    int duration,
                                    int count);
        
        static TaskPtr LeechByAttack(CCLayer * layer,
                                     PlayerControlPtr player,
                                     int duration,
                                     float factor);
        
        static TaskPtr Zombine(CCLayer* layer,
                               HeroControl* hc,
                               PlayerControlPtr player,
                               int count);
        
        static TaskPtr FightBack(CCLayer* layer,
                                 HeroControl* hc,
                                 PlayerControlPtr player,
                                 int duration,
                                 float factor, float probality);
        
        static TaskPtr ForceAreaAttack(CCLayer* layer,
                                       HeroControl* hc,
                                       PlayerControlPtr player,
                                       int duration);
        
        static TaskPtr BonusGemWeight(CCLayer* layer,
                                      PlayerControlPtr player,
                                      GemUtils::GemColor color,
                                      int duration,
                                      float factor);
    };

}
