//
//  EnemySkill.h
//  HelloCpp
//
//  Created by Zeyang Li on 11/29/12.
//
//

#pragma once

#include "Board/Board.h"
#include "Board/EnemySkill.h"

namespace PH {
    
    class EnemySkill
    {
    public:
    
        static TaskPtr Heal(BoardLayer* layer,
                            EnemyControlPtr ec,
                            float val);
        
        static TaskPtr Poison(BoardLayer* layer,
                              EnemyControlPtr ec,
                              int turn,
                              float val);

        static TaskPtr DrainEnergyByColor(BoardLayer* layer,
                                          EnemyControlPtr ec,
                                          GemUtils::GemColor c,
                                          float value);
        
        static TaskPtr DrainEnergyRandomeHero(BoardLayer* layer,
                                              EnemyControlPtr ec,
                                              int count,
                                              float value);
        
        static TaskPtr BombGemsByColor(BoardLayer* layer,
                                       EnemyControlPtr ec,
                                       GemUtils::GemColor color,
                                       int maxCount);
        
        static TaskPtr LockGems(BoardLayer * layer,
                                EnemyControlPtr ec,
                                GemUtils::GemColor color,
                                int count,
                                int turn);
        
        static TaskPtr AttachBombToGems(BoardLayer* layer,
                                        EnemyControlPtr ec,
                                        GemUtils::GemColor color,
                                        int count,
                                        float damage,
                                        int turn);
        
        static TaskPtr AttachHandToGems(BoardLayer* layer,
                                        EnemyControlPtr ec,
                                        GemUtils::GemColor color,
                                        int count,
                                        float damage,
                                        int turn);
        
        static TaskPtr BlindHeroRandom(BoardLayer* layer,
                                       EnemyControlPtr ec,
                                       int count,
                                       int turn);
    
        static TaskPtr BlindHeroByColor(BoardLayer* layer,
                                        EnemyControlPtr ec,
                                        GemUtils::GemColor color,
                                        int turn);

		static TaskPtr ReflectByFactor(BoardLayer * layer,
										EnemyControlPtr ec,
										float factor,
										int turn);

        static TaskPtr ReflectByGems(BoardLayer * layer,
                                     EnemyControlPtr ec,
                                     GemUtils::GemColor color,
                                     float factor,
                                     int count,
                                     int turn);
        
        static TaskPtr InjuryFreeExceptDarts(BoardLayer * layer,
                                          EnemyControlPtr ec,
                                          GemUtils::GemColor color,
                                          int count,
                                          int turn);
        
        static TaskPtr SlowByFactor(BoardLayer* layer,
                                    EnemyControlPtr ec,
                                    float factor,
                                    int turn);
    
        static TaskPtr ReduceHealingByFactor(BoardLayer* layer,
                                             EnemyControlPtr ec,
                                             float factor,
                                             int turn);

        static TaskPtr ChangeAttributeByColor(BoardLayer * layer,
                                              EnemyControlPtr ec,
                                              GemUtils::GemColor c,
                                              int turn); 

        static TaskPtr Speaking(BoardLayer * layer,
                                EnemyControlPtr ec,
                                std::string content);
        
        static TaskPtr HealAllTheEnmey(BoardLayer *layer,
                                       EnemyControlPtr ec,
                                       float);

        static TaskPtr SpreadPlague(BoardLayer * layer,
                                    EnemyControlPtr ec,
                                    GemUtils::GemColor color,
                                    float damage,
                                    int count,
                                    int infectionLimitPerGems,
                                    int globalLimit,
                                    int turn);
        
        static TaskPtr LeechByFactor(BoardLayer * layer,
                                     EnemyControlPtr ec,
                                     float factor,
                                     int duration);
        
        static TaskPtr DamageByCount(BoardLayer * layer,
                                     EnemyControlPtr ec,
                                     float count,
                                     float damage);
        
        static TaskPtr SetShield(BoardLayer * layer,
                                 EnemyControlPtr ec,
                                 float damage,
                                 float factor);
    };
    
    class EnemyPassiveSkill
    {
    public:
        static TaskPtr InjuryFree(BoardLayer * layer,
                                  EnemyControlPtr ec,
                                  float factor,
                                  float probality,
                                  GemUtils::GemColor color);
        
        static TaskPtr Reflect(BoardLayer * layer,
                               EnemyControlPtr ec,
                               float factor,
                               float probality,
                               GemUtils::GemColor color,
                               bool takeDamge);
        
        static TaskPtr Hound(BoardLayer * layer,
                             EnemyControlPtr ec,
                             int count,
                             float damage);
        
        static TaskPtr Reborn(BoardLayer * layer,
                              EnemyControlPtr ec,
                              int count,
                              float factor);
    };
}

using namespace PH;

void BeginEnemySkill(BoardLayer* layer, EnemyControlPtr ec, TaskSequencePtr seq);
void EndEnemySkill();

// tolua_begin
void EnemyHeal(float val);
void EnemyPoison(int turn, float val);
void EnemyDrainEnergyByColor(std::string color, float val);
void EnemyDrainEnergyRandomHero(int count, float val);
void EnemyBombGemsByColor(std::string color, int maxCount);

void EnemyLockGems(std::string color, int count, int turn);

void EnemyAttachBombToGems(std::string color, int count, float damage, int turn);
void EnemyAttachHandToGems(std::string color, int count ,float damage, int turn);

void EnemyBlindHeroRandom(int count, int turn);
void EnemyBlindHeroByColor(std::string color, int turn);
void EnemySlowByFactor(float factor, int turn);
void EnemyReduceHealingByFactor(float factor, int turn);

void EnemyReflectByFactor(float factor, int turn);
void EnemyReflectByGems(std::string color, float factor, int count, int turn);

void EnemyInjuryFreeExceptDarts(std::string color, int count, int turn);

void EnemySpreadPlague(std::string color, 
                       float damage, 
                       int count, 
                       int infectionLimitPerGems,
                       int globalLimit,
                       int turn);

void EnemyChangeAttributeByColor(std::string color, int turn);
void EnemySpeaking(std::string content);
void EnemyHealAllTheEnemy(float val);

void EnemyLeechByFactor(float factor, int duration);
void EnemyDamageByCount(float count, float damage);

void EnemySetShield(float damage, float factor);
// tolua_end

// tolua_begin
void EnemyPassiveInjuryFree(float factor, float probality, std::string color);
void EnemyPassiveReflect(float factor, float probality,
                         std::string color,
                         bool takeDamage);
void EnemyPassiveHound(int count, float damage);
void EnemyPassiveReborn(int count, float healthFactor);
// tolua_end
