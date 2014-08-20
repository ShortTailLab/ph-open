//
//  PvESkill.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 5/20/13.
//
//

#include "PvESkill.h"
#include "EnemyControl.h"
#include "Board.h"

namespace PH
{
    //---------------------------------------------
    //------------- Damage Skills -----------------
    //---------------------------------------------
    static TaskPtr DamageCommon(BoardLayer* layer,
                                HeroControl* hc,
                                boost::function<float (EnemyControlPtr)> f,
                                bool isArea,
                                float& totalDamage)
    {
        TaskBatchPtr areaBatch = TaskBatch::make();

        totalDamage = 0.0f;

        EnemyControlList targets = layer->getEnemiesInAttackOrder(hc->getColor(),
                                                                  isArea);
        float delay = 0.0f;
        for(EnemyControlPtr ec : targets)
        {
            float finalAttack = f(ec);
            float finalDamage;
            areaBatch << layer->calcDamageForTarget(finalAttack,
                                                    hc->getColor(),
                                                    ec,
                                                    finalDamage,
                                                    false,
                                                    delay + 0.5f);

            totalDamage += finalDamage;

            areaBatch << layer->createAttackSequence(hc,
                                                     ec,
                                                     finalDamage,
                                                     hc->getColor(),
                                                     delay);

            float acDelay = -0.15f+delay+calOrbDelay(hc->center(), ec->center());
            TaskPtr hit = layer->dealDamageToOneWithAnim(finalDamage, ec, acDelay);
            areaBatch << TaskSequence::make(createDelay(layer, 0.5),
                                            hit);
            delay += 0.2f;
        }
        //layer->removeDeadEnemies();

        return areaBatch;
    }

    static TaskPtr DamageCommon(BoardLayer* layer,
                                HeroControl* hc,
                                boost::function<float (EnemyControlPtr)> f,
                                bool isArea)
    {
        float totalDamage = 0.0f;
        return DamageCommon(layer, hc, f, isArea, totalDamage);
    }

    TaskPtr PvEActive::DamageByValue(PH::BoardLayer *layer,
                                     HeroControl* hc,
                                     float damage,
                                     bool isArea)
    {
        return DamageCommon(layer,
                            hc,
                            [damage](EnemyControlPtr ec){return damage;},
                            isArea);
    }

    TaskPtr PvEActive::DamageByFactorFromColor(BoardLayer* layer,
                                               HeroControl* hc,
                                               float factor,
                                               GemUtils::GemColor color,
                                               bool isArea)
    {
        TaskBatchPtr batch = TaskBatch::make();
        // account for attack factors
        float damage = 0.0f;
        for(HeroControl* hhh : layer->mPlayer->team)
        {
            if(hhh->getColor() == color || color == GemUtils::AllColor)
            {
                float skillMod;
                batch << layer->mPlayer->getAttackFactor(hc, skillMod, [](){return rand();});
                damage += hhh->attack * (factor * skillMod);
            }
        }

        return batch << DamageByValue(layer, hc, damage, isArea);
    }

    TaskPtr PvEActive::DamageByFactor(BoardLayer* layer, HeroControl* hc, float factor, bool isArea)
    {
        TaskBatchPtr batch = TaskBatch::make();

        float skillMod;
        batch << layer->mPlayer->getAttackFactor(hc, skillMod, [](){return rand();});
        float damage = hc->attack * (factor * skillMod);

        return batch << DamageByValue(layer, hc, damage, isArea);
    }
    
    TaskPtr PvEActive::Damage(PH::BoardLayer *layer,
                              PH::HeroControl *hc,
                              GemUtils::GemColor color,
                              float damage,
                              bool isArea,
                              bool ignoreArmor)
    {
        TaskBatchPtr areaBatch = TaskBatch::make();
        
        EnemyControlList targets = layer->getEnemiesInAttackOrder(hc->getColor(),
                                                                  isArea);
        float delay = 0.0f;
        
        for(EnemyControlPtr ec : targets)
        {
            float finalDamage = 0.f;
            areaBatch << layer->calcDamageForTarget(damage,
                                                    color,
                                                    ec,
                                                    finalDamage,
                                                    ignoreArmor,
                                                    delay + 0.5f);
            areaBatch << layer->createAttackSequence(hc,
                                                     ec,
                                                     finalDamage,
                                                     color,
                                                     delay);
            float acDelay = -0.15f+delay+calOrbDelay(hc->center(), ec->center());
            TaskPtr hit = layer->dealDamageToOneWithAnim(finalDamage, ec, -delay+acDelay);
            
            areaBatch << TaskSequence::make(createDelay(layer, 0.5),
                                            hit);
            delay += 0.2f;
        }
        //layer->removeDeadEnemies();
        
        return areaBatch;
    }

    TaskPtr PvEActive::DamageByFactorWithoutArmor(BoardLayer * layer,
                                                  HeroControl * hc,
                                                  GemUtils::GemColor color,
                                                  float factor,
                                                  bool isArea)
    {
        TaskBatchPtr areaBatch = TaskBatch::make();

        EnemyControlList targets = layer->getEnemiesInAttackOrder(hc->getColor(),
                                                                  isArea);
        float delay = 0.0f;
        float skillMod;
        areaBatch << layer->mPlayer->getAttackFactor(hc, skillMod, [](){return rand();});
        const float finalAttack = hc->attack * ( factor * skillMod );

        for(EnemyControlPtr ec : targets)
        {
            float finalDamage = 0.f;
            areaBatch << layer->calcDamageForTarget(finalAttack,
                                                    hc->getColor(),
                                                    ec,
                                                    finalDamage,
                                                    true,
                                                    delay + 0.5f);
            areaBatch << layer->createAttackSequence(hc,
                                                     ec,
                                                     finalDamage,
                                                     color,
                                                     delay);
            float acDelay = -0.15f+delay+calOrbDelay(hc->center(), ec->center());
            TaskPtr hit = layer->dealDamageToOneWithAnim(finalDamage, ec, acDelay);
            
            areaBatch << TaskSequence::make(createDelay(layer, 0.5),
                                            hit);
            delay += 0.2f;
        }

        return areaBatch;
    }

    // This skill does not account for hero color
    TaskPtr PvEActive::DamageByEnemyHealthFactor(BoardLayer* layer,
                                                 HeroControl* hc,
                                                 float factor,
                                                 bool isArea)
    {
        TaskBatchPtr areaBatch = TaskBatch::make();
        EnemyControlList targets = layer->getEnemiesInAttackOrder(hc->getColor(),
                                                                  isArea);

        float delay = 0.0f;
        for(EnemyControlPtr ec : targets)
        {
            float finalDamage = ec->getHealth() * factor;
            areaBatch << createFloatText(layer, finalDamage, ec->center(), hc->getColor(), delay+0.5f);
            areaBatch << layer->createAttackSequence(hc,
                                                     ec,
                                                     finalDamage,
                                                     hc->getColor(),
                                                     delay);

            float acDelay = -0.15f+delay+calOrbDelay(hc->center(), ec->center());
            TaskPtr hit = layer->dealDamageToOneWithAnim(finalDamage, ec, acDelay);
            areaBatch << TaskSequence::make(createDelay(layer, 0.5),
                                            hit);
            delay += 0.2f;
        }
        //layer->removeDeadEnemies();

        return areaBatch;
    }

    TaskPtr PvEActive::DamageByPlayerHealthFactor(BoardLayer* layer,
                                                  HeroControl* hc,
                                                  float factor,
                                                  bool isArea)
    {
        float damage = factor * layer->mPlayer->getMaxHealth();
        return DamageByValue(layer, hc, damage, isArea);
    }

    TaskPtr PvEActive::DamageBySelfDamageFactor(BoardLayer* layer,
                                                HeroControl* hc,
                                                float healthFactor,
                                                float attackFactor,
                                                bool isArea)
    {
        float hit = layer->mPlayer->getHealth() * healthFactor;

        TaskPtr hitAnim = layer->mPlayer->hit(hit, layer);
        TaskPtr attackAnim = DamageByValue(layer, hc, hit * attackFactor, isArea);

        return TaskBatch::make() << hitAnim << attackAnim;
    }

    TaskPtr PvEActive::DamageFromHealthGem(BoardLayer* layer,
                                           HeroControl* hc,
                                           float factor,
                                           bool isArea)
    {
        TaskSequencePtr seq = TaskSequence::make();

        int healthCount = 0;
        GemGrid & grid = layer->mBoardControl->getGrid();
        for(int i=0; i<grid.size(); i++)
        {
            GemPtr gem = grid(i);
            if(gem->color() == GemUtils::Health)
            {
                if (gem == NULL)
                    continue;

                seq << TaskSound::make("sound/v/gem_sweep.mp3");
                seq << gem->sweep(layer->mBoardControl.get());
                
                healthCount++;
            }
        }

        seq << DamageByFactor(layer, hc, healthCount*factor, isArea);
        seq << layer->mBoardControl->fillBoard([=](){return layer->randGemWeighted();},
                                               false);

        return seq;
    }
    
    TaskPtr PvEActive::DamageByGems(PH::BoardLayer *layer,
                                    PH::HeroControl *hc,
                                    float factor,
                                    GemUtils::GemColor gemColor,
                                    GemUtils::GemColor attColor,
                                    bool isArea)
    {
        auto batch = TaskBatch::make();
        
        int count = 0;
        GemGrid & grid = layer->mBoardControl->getGrid();
        for(int i=0; i < grid.size(); i++)
        {
            GemPtr gem = grid(i);
            if(gem->color() == gemColor)
            {
                if (gem == NULL)
                    continue;
                
                auto seq = TaskSequence::make();
                
                seq << TaskSound::make("sound/v/gem_sweep.mp3");
                seq << gem->sweep(layer->mBoardControl.get());
                
                batch << seq;
                count++;
            }
        }
        
        return TaskSequence::make()
        << batch
        << PvEActive::DamageByFactorWithoutArmor(layer,
                                                     hc,
                                                     attColor,
                                                     factor*count,
                                                     isArea)
        << layer->mBoardControl->fillBoard([=](){return layer->randGemWeighted();},
                                           false);
    }

    TaskPtr PvEActive::DamageAndHealByFactor(BoardLayer * layer,
                                             PH::HeroControl *hc,
                                             float factor,
                                             bool isArea)
    {
        TaskSequencePtr seq = TaskSequence::make();
        
        float skillMod = 0;
        seq << layer->mPlayer->getAttackFactor(hc, skillMod, [](){return rand();});
        float damage = hc->attack * (factor * skillMod);
        
        auto batch = TaskBatch::make();
        float finalDamage = 0.f;
        batch << DamageCommon(layer,
                              hc,
                              [damage](EnemyControlPtr ec){return damage;},
                              isArea,
                              finalDamage);
        
        batch << ActiveSkill::HealByValue(layer, hc, layer->mPlayer, finalDamage);
        return seq << batch;
    }
    
    //----------------------------------------------------
    //--------------- Turn Based Skills ------------------
    //----------------------------------------------------
    static TaskPtr CastTurnBasedSkill(BoardLayer* layer,
                                      HeroControl* hc,
                                      boost::function<TaskPtr (EnemyControlPtr ec)> f,
                                      bool isArea = true)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        EnemyControlList targets = layer->getEnemiesInAttackOrder(hc->getColor(),
                                                                  isArea);
        for(EnemyControlPtr ec : targets)
        {
            TaskPtr anim = f(ec);
            batch << anim;
        }
        
        //layer->removeDeadEnemies();
        
        return batch;
    }
    
    static TaskPtr CastTurnBasedSkillByColor(BoardLayer * layer,
                                             HeroControl * hc,
                                             boost::function<TaskPtr (EnemyControlPtr ec)> f,
                                             GemUtils::GemColor color = GemUtils::AllColor)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        EnemyControlList targets = layer->getEnemiesByColor(color);
        
        for( EnemyControlPtr ec : targets )
        {
            TaskPtr anim = f(ec);
            batch << anim;
        }
        
        //layer->removeDeadEnemies();
        return batch;
    }
    
    TaskPtr PvEActive::DamageByFactorAndEnemyHealth(PH::BoardLayer *layer,
                                                    PH::HeroControl *hc,
                                                    float factor, float healthRatio,
                                                    float damageFactor, bool isArea)
    {
        TaskSequencePtr seq = TaskSequence::make();
        
        float skillMod = 0;
        seq << layer->mPlayer->getAttackFactor(hc, skillMod, [](){return rand();});
        float atk = hc->attack * ( factor * skillMod );
        
        return CastTurnBasedSkill(layer, hc, [=](EnemyControlPtr ec) -> TaskPtr
        {
            TaskBatchPtr batch = TaskBatch::make();
            float finalAttack = atk;
            if(ec->healthRatio() < healthRatio)
            {
                finalAttack += (hc->attack * damageFactor);
            }
            
            float finalDamage = 0.f;
            batch << layer->calcDamageForTarget(finalAttack,
                                       hc->getColor(),
                                       ec,
                                       finalDamage,
                                       false,
                                       0.5f);
            batch << layer->createAttackSequence(hc,
                                                 ec,
                                                 finalDamage,
                                                 hc->getColor(),
                                                 0.f);
            batch << layer->dealDamageToOneWithAnim(finalDamage, ec, 0.5f);
            return batch;
        }, isArea);
    }

    //----------------------------------------------------
    //--------------- Multiple Attacks -------------------
    //----------------------------------------------------
    TaskPtr PvEActive::MultipleAttacksByFactor(BoardLayer* layer,
                                               HeroControl* hc,
                                               int count,
                                               float factor,
                                               bool isArea)
    {
        TaskBatchPtr batch = TaskBatch::make();
        for(int i=0; i<count; i++)
        {
            TaskSequencePtr seq = TaskSequence::make();
            seq << createDelay(layer, 0.2f * i);
            seq << DamageByFactor(layer, hc, factor, isArea);
            batch << seq;
        }

        return batch;
    }

    TaskPtr PvEActive::LeechByFactor(BoardLayer *layer,
                                       HeroControl* hc,
                                       float attackFactor,
                                       float leechFactor)
    {
        TaskBatchPtr batch = TaskBatch::make();

        float skillMod;
        batch << layer->mPlayer->getAttackFactor(hc, skillMod, [](){return rand();});

        float damage = hc->attack * (attackFactor * skillMod);

        float totalDamage = 0.0f;
        batch << DamageCommon(layer,
                              hc,
                              [=](EnemyControlPtr ec){return damage;},
                              true,
                              totalDamage);

        float healAmount = totalDamage * leechFactor;
        batch << ActiveSkill::HealByValue(layer, hc, layer->mPlayer, healAmount);

        return batch;
    }
    
    TaskPtr PvEActive::AmplifyDamageByFactor(PH::BoardLayer *layer,
                                             PH::HeroControl *hc,
                                             int duration, float factor, GemUtils::GemColor color,
                                             bool isArea)
    {
        return CastTurnBasedSkill(layer, hc, [=](EnemyControlPtr ec) -> TaskPtr
        {
            FloatBuffPtr buff = FloatBuff::make(color, duration, "加深", factor);
            return ec->buffGroup.attachBuff(EnemyControl::kGroup_PostTurn, "amplifyDamage", layer, buff);
        }, isArea);
    }

    TaskPtr PvEActive::PoisonByValue(BoardLayer* layer, HeroControl* hc, int duration, float val)
    {
        return CastTurnBasedSkill(layer, hc, [=](EnemyControlPtr ec) -> TaskPtr
        {
            TaskSequencePtr seq = TaskSequence::make();
            seq << createOrb(layer, hc->center(), ec->center());
            
            FloatBuffPtr buff = FloatBuff::make(GemUtils::Dark, duration, "蛊毒", val);
            seq << ec->buffGroup.attachBuff(EnemyControl::kGroup_PostTurn, "poison", layer, buff);
            return seq;
        });
    }

    TaskPtr PvEActive::PoisonByFactor(PH::BoardLayer *layer, PH::HeroControl *hc, int duration, float factor)
    {
        float val = hc->attack * factor;
        return PoisonByValue(layer, hc, duration, val);
    }

    TaskPtr PvEActive::Blind(BoardLayer* layer, HeroControl* hc, int duration, bool toAll)
    {
        return CastTurnBasedSkill(layer, hc, [=](EnemyControlPtr ec) -> TaskPtr
        {
            TaskSequencePtr seq = TaskSequence::make();
            seq << createOrb(layer, hc->center(), ec->center());
            
            BuffPtr buff = IBuff::make(GemUtils::AllColor, duration, "致盲");
            seq << ec->buffGroup.attachBuff(EnemyControl::kGroup_PreTurn, "blind", layer, buff);
            return seq;
        });
    }

    TaskPtr PvEActive::Silence(BoardLayer* layer, HeroControl* hc, int duration, bool toAll)
    {
        return CastTurnBasedSkill(layer, hc, [=](EnemyControlPtr ec) -> TaskPtr
        {
            TaskSequencePtr seq = TaskSequence::make();
            seq << createOrb(layer, hc->center(), ec->center());
            
            BuffPtr buff = IBuff::make(GemUtils::AllColor, duration, "封印");
            seq << ec->buffGroup.attachBuff(EnemyControl::kGroup_PostTurn, "silence", layer, buff);
            return seq;
        }, toAll);
    }

    TaskPtr PvEActive::ReduceEnemyArmorByFactor(PH::BoardLayer *layer, PH::HeroControl *hc, int duration, float factor)
    {
        return CastTurnBasedSkill(layer, hc, [=](EnemyControlPtr ec) -> TaskPtr
        {
            TaskSequencePtr seq = TaskSequence::make();
            seq << createOrb(layer, hc->center(), ec->center());
            FloatBuffPtr buff = FloatBuff::make(GemUtils::AllColor, duration, "弱防", factor);
            seq << ec->buffGroup.attachBuff(EnemyControl::kGroup_PostTurn, "armorFactor", layer, buff);
            return seq;
        });
    }

    TaskPtr PvEActive::WeakenEnemyAttackByFactor(PH::BoardLayer * layer,
                                                 PH::HeroControl * hc,
                                                 int duration,
                                                 float factor,
                                                 bool isArea)
    {
        // set property only to display a label,
        return CastTurnBasedSkill(layer, hc, [=](EnemyControlPtr ec) -> TaskPtr
        {
            TaskSequencePtr seq = TaskSequence::make();
            seq << createOrb(layer, hc->center(), ec->center(), 0.0f);
            
            FloatBuffPtr buff = FloatBuff::make(GemUtils::AllColor, duration, "弱攻", factor);
            seq << ec->buffGroup.attachBuff(EnemyControl::kGroup_PostTurn, "attackFactor", layer, buff);

            return seq;
        }, isArea);
    }
    
    TaskPtr PvEActive::ReduceAttackFactorByColor(PH::BoardLayer *layer,
                                                 PH::HeroControl *hc,
                                                 int duration, float factor,
                                                 GemUtils::GemColor color)
    {
        return CastTurnBasedSkillByColor(layer, hc,
        [=](EnemyControlPtr ec) -> TaskPtr
        {
            auto seq = TaskSequence::make();
            seq << createOrb(layer, hc->center(), ec->center(), 0.f);
            auto buff = FloatBuff::make(GemUtils::AllColor, duration, "弱攻", factor);
            seq << ec->buffGroup.attachBuff(EnemyControl::kGroup_PostTurn,
                                            "attackFactor",
                                            layer, buff);
            return seq;
        }, color);
    }
    
    TaskPtr PvEActive::ExtendEnemyWaitTurn(PH::BoardLayer* layer,
                                           PH::HeroControl * hc,
                                           int val,
                                           GemUtils::GemColor color)
    {
        return CastTurnBasedSkillByColor(layer, hc, [=](EnemyControlPtr ec) -> TaskPtr
        {
            return TaskBatch::make()
            << ec->setReadyInTurnWithAnim(val+ec->getReadyInTurn());
        }, color);
    }
    
    TaskPtr PvEActive::TransformAllGem(BoardLayer* layer)
    {
        auto fadeOutTask = TaskBatch::make();
        auto & grid = layer->mBoardControl->getGrid();
        
        float delay = 0.01f;
        for(int i=0; i<grid.size(); i++)
        {
            GemPtr fromGem = grid(i);
          
            auto seq = TaskSequence::make();
            seq << createDelay(layer, delay);
            seq << fromGem->sweep(layer->mBoardControl.get());
        
            fadeOutTask << seq;
            delay += 0.02;
        }
        
        return TaskSequence::make()
        << TaskSound::make("sound/v/evolve_success.mp3")
        << fadeOutTask
        << layer->mBoardControl->fillBoard([=](){return layer->randGemWeighted();},
                                           false);
    }
    
    TaskPtr PvEActive::BloodForBlood(PH::BoardLayer *layer,
                                     PH::HeroControl *hc,
                                     float ratio,
                                     float factor,
                                     bool isArea)
    {
        TaskBatchPtr batch = TaskBatch::make();
        // account for attack factors
        float damage = 0.0f;
        if (ratio <= 0.f)
            damage = layer->mPlayer->getHealth() - 1;
        else
            damage = ratio * layer->mPlayer->getHealth();
        
        batch << layer->mPlayer->hit(damage, layer);
        batch << DamageByValue(layer, hc, damage*factor, isArea);
        
        return batch;
    }
    
    TaskPtr PvEActive::EyeForEye(PH::BoardLayer *layer,
                                 PH::HeroControl *hc,
                                 float attacker, float defender,
                                 bool isArea)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        float val = layer->mPlayer->getHealth()*attacker;
        if(attacker <= 0.f)
            val = layer->mPlayer->getHealth()*attacker - 1;
            

        batch << layer->mPlayer->hit(val, layer);
        batch << DamageByEnemyHealthFactor(layer, hc, defender, isArea);
        
        return batch;
    }

}
