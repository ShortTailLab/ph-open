//
//  Skill.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 11/7/12.
//
//

#include "Board/Skill.h"
#include "Board/Board.h"
#include "Board/Utils.h"
#include "CCCallLambda.h"

namespace PH
{
    TaskPtr ActiveSkill::TransformGemColor(BoardLayer* layer,
                                           Gem::GemColor fromColor,
                                           Gem::GemColor toColor)
    {
        GemGrid& grid = layer->mGemGrid;
        
        TaskBatchPtr fadeOutTask = TaskBatch::make();
        TaskBatchPtr fadeInTask = TaskBatch::make();
        
        for(int i=0; i<grid.size(); i++)
        {
            Gem* fromGem = grid(i);
            
            if( fromGem->color == fromColor )
            {
                fadeOutTask << removeGemFromGridWithAnim(layer->mGemGrid, fromGem, 0.3f);
                
                // replacement gem
                Gem* toGem = new Gem(fromGem->position, toColor);
                toGem->sprite->setOpacity(0);
                
                CCFadeTo* fadeIn  = CCFadeTo::create(0.3, 255);
                fadeInTask << TaskAnim::make(toGem->sprite, fadeIn);
                
                grid(i) = toGem;
                layer->mGemLayer->addChild(toGem->sprite);
            }
        }
        
        return TaskSequence::make() << TaskSound::make("sound/v/evolve_success.mp3")
                                    << fadeOutTask
                                    << fadeInTask;
    }
    
    TaskPtr ActiveSkill::TransformAllGem(BoardLayer* layer)
    {
        GemGrid& grid = layer->mGemGrid;
        
        TaskBatchPtr fadeOutTask = TaskBatch::make();
        TaskBatchPtr fadeInTask = TaskBatch::make();
        
        for(int i=0; i<grid.size(); i++)
        {
            Gem* fromGem = grid(i);
            
            Gem::GemColor toColor = Gem::weak(fromGem->color);

            fadeOutTask << removeGemFromGridWithAnim(layer->mGemGrid, fromGem, 0.3f);
                
            // replacement gem
            Gem* toGem = new Gem(fromGem->position, toColor);
            toGem->sprite->setOpacity(0);
            
            CCFadeTo* fadeIn  = CCFadeTo::create(0.3, 255);
            fadeInTask << TaskAnim::make(toGem->sprite, fadeIn);
            
            grid(i) = toGem;
            layer->mGemLayer->addChild(toGem->sprite);
        }
        
        return TaskSequence::make() << TaskSound::make("sound/v/evolve_success.mp3")
                                        << fadeOutTask
                                        << fadeInTask;
    }

    TaskPtr ActiveSkill::SweepRandomGem(BoardLayer* layer, HeroControl* hc, int count)
    {
        /*
        GemGrid& grid = layer->mGemGrid;
        
        TaskSequencePtr sweepSeq = TaskSequence::make();
        
        Gem::GemColor heroColor = hc->info.profile->color;

        sweepSeq->add(TaskLambda::make([hc](){ hc->attackLabel->setOpacity(255); }));
        
        float baseAttack = 0.0f;
        
        // randomize count points in grid
        for(int i=0; i < count; i++)
        {
            int x = rand() % 6;
            int y = rand() % 5;
            
            Gem* gem = grid(x,y);
            if (gem == NULL)
                continue;
            
            // make gem fade
            TaskSequencePtr seq = TaskSequence::make();
            seq << TaskSound::make("sound/v/gem_sweep.mp3");
            seq << layer->removeGemFromGridWithAnim(gem, 0.2f);
            
            TaskBatchPtr batch = TaskBatch::make();
            batch->add(seq);
            
            float factor = gem->color == heroColor ? 0.75f : 0.25f; // expected value is (0.5 * count) * attack

            float prevAttack = baseAttack;
            baseAttack += hc->attack * factor;
            CCCountTextNumber* countAttack = CCCountTextNumber::create(prevAttack, baseAttack, 0.2f);
            batch->add(TaskAnim::make(hc->attackLabel, countAttack));
            
            sweepSeq->add(batch);
        }

        TaskBatchPtr areaBatch = TaskBatch::make();
        
        for(EnemyControlPtr ec : layer->mEnemyList)
        {
            float finalAttack = baseAttack;
            float finalDamage;
            areaBatch << layer->calcDamageForTarget(finalAttack, heroColor, ec, finalDamage);
            
            areaBatch << layer->createAttackSequence(hc,
                                                     ec,
                                                     finalAttack,
                                                     finalDamage,
                                                     heroColor,
                                                     0.0f);
            
            TaskPtr hit = layer->dealDamageToOneWithAnim(finalDamage, ec, 0.0f);
            areaBatch << TaskSequence::make(createDelay(layer, 0.5), hit);
        }
        layer->removeDeadEnemies();
        
        sweepSeq << areaBatch;
        sweepSeq << TaskAnim::make(hc->attackLabel, CCFadeOut::create(0.3f));
        
        TaskSequencePtr seq = TaskSequence::make();
        seq << sweepSeq
            << layer->fill();
        */
        TaskSequencePtr seq = TaskSequence::make();
        return seq;
    }
        
    TaskPtr ActiveSkill::RandomizeGem(BoardLayer* layer)
    {
        GemGrid& grid = layer->mGemGrid;
        
        TaskBatchPtr fadeOutBatch = TaskBatch::make();
        
        for(int i=0; i<grid.size(); i++)
        {
            fadeOutBatch << removeGemFromGridWithAnim(layer->mGemGrid, grid(i), 0.3f);
        }
        
        return TaskSequence::make() << fadeOutBatch
                                    << fillBoard(layer,
                                                 layer->mGemLayer,
                                                 grid,
                                                 [=](){ return layer->randGemWeighted();});
    }
        
    TaskPtr ActiveSkill::HealByValue(BoardLayer* layer, HeroControl* hc, float val)
    {
        if(layer->mPlayer->isHurt())
            return layer->healWithAnim(val);
        
        return TaskIgnore::make();
    }
    
    TaskPtr ActiveSkill::HealByFactor(BoardLayer* layer, HeroControl* hc, float factor)
    {
        float heal = hc->regen * factor;
        return HealByValue(layer, hc, heal);
    }
    
    TaskPtr ActiveSkill::HealFull(BoardLayer* layer, HeroControl* hc)
    {
        return HealByValue(layer, hc, layer->mPlayer->maxHealth);
    }
    
    TaskPtr ActiveSkill::InstantEnergy(BoardLayer* layer,
                                       HeroControl* hc,
                                       float factor,
                                       Gem::GemColor c)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        for(HeroControl* hc : layer->mPlayer->team)
        {
            if(hc->getColor() == c || c == Gem::AllColor)
                batch << hc->addEnergy( hc->maxEnergy * factor );
        }
        
        return batch;
    }

    //-----------------------------------------------
    //------------- Control Skills ------------------
    //-----------------------------------------------
        
    TaskPtr ActiveSkill::SortGem(BoardLayer* layer)
    {
        GemGrid& grid = layer->mGemGrid;
        // bucket sort gems by color.
        GemSet buckets[Gem::GemColorMax];
        for(int i=0; i<grid.size(); i++)
        {
            Gem* gem = grid(i);
            buckets[gem->color].insert(gem);
        }
        
        std::vector<Gem*> sorted;
        for(int i=0; i<Gem::GemColorMax; i++)
            sorted.insert(sorted.end(), buckets[i].begin(), buckets[i].end());

        TaskBatchPtr sortAnim = TaskBatch::make();
        
        for(int y=0; y<grid.Height; y++)
        for(int x=0; x<grid.Width; x++)
        {
            int i = grid.Width*y + x;

            Gem* gem = sorted[i];
            gem->position = Vec2i(x,y);
            
            grid(i) = gem;
            CCMoveTo* move = CCMoveTo::create(0.618f, g2w(gem->position));
            sortAnim->add(TaskAnim::make(gem->sprite, move));
        }
        
        return sortAnim;
    }

    TaskPtr ActiveSkill::StopMoveTimer(BoardLayer* layer)
    {
        layer->mPlayer->stopTicking();
        
        return TaskEmpty::make();
    }
    
    TaskPtr ActiveSkill::ExtendMoveTimer(BoardLayer* layer, float secs)
    {
        layer->mPlayer->moveTimer.time += secs;
        
        return TaskEmpty::make();
    }
    
    TaskPtr ActiveSkill::ExtendEnemyWaitTurn(BoardLayer* layer, int val)
    {
        TaskBatchPtr batch = TaskBatch::make();
        for(EnemyControlPtr ec : layer->mEnemyList)
        {
            TaskPtr t = ec->setReadyInTurnWithAnim(val + ec->getReadyInTurn());
            batch->add(t);
        }
        return batch;
    }
        
    TaskPtr ActiveSkill::DisableSweep(BoardLayer* layer, float time)
    {
        layer->mNoSweepOnDragEnd = true;
        
        // set property only to display a label,
        // not really using it as a turn skill
        return layer->mPlayer->setPropertyWithAnim(layer,
                                                   layer->mPlayer->disableSweep,
                                                   1,
                                                   time,
                                                   "神行",
                                                   Gem::AllColor);
    }
    
    TaskPtr ActiveSkill::MoveAgain(BoardLayer* layer, int count)
    {
        layer->mMoveCount = count;
        // set property only to display a label,
        // not really using it as a turn skill
        return layer->mPlayer->setPropertyWithAnim(layer,
                                                   layer->mPlayer->multipleMove,
                                                   1,
                                                   0.0f,
                                                   "連擊",
                                                   Gem::AllColor);
    }

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

        DamageTable emptyTable(layer->mEnemyList);
        EnemyControlList targets = layer->getEnemiesInAttackOrder(hc->getColor(),
                                                                  emptyTable,
                                                                  isArea);
        float delay = 0.0f;
        for(EnemyControlPtr ec : targets)
        {
            float finalAttack = f(ec);
            float finalDamage;
            areaBatch << layer->calcDamageForTarget(finalAttack,
                                                    hc->getColor(),
                                                    ec,
                                                    finalDamage);
            
            totalDamage += finalDamage;
            
            areaBatch << layer->createAttackSequence(hc,
                                                     ec,
                                                     finalAttack,
                                                     finalDamage,
                                                     hc->getColor(),
                                                     delay);
            
            TaskPtr hit = layer->dealDamageToOneWithAnim(finalDamage, ec, delay);
            areaBatch << TaskSequence::make(createDelay(layer, 0.5),
                                            hit);
            delay += 0.2f;
        }
        layer->removeDeadEnemies();
        
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
    
    TaskPtr ActiveSkill::DamageByValue(PH::BoardLayer *layer,
                                       HeroControl* hc,
                                       float damage,
                                       bool isArea)
    {
        return DamageCommon(layer,
                            hc,
                            [damage](EnemyControlPtr ec){return damage;},
                            isArea);
    }
    
    TaskPtr ActiveSkill::DamageByFactorFromColor(BoardLayer* layer,
                                                 HeroControl* hc,
                                                 float factor,
                                                 Gem::GemColor color,
                                                 bool isArea)
    {
        TaskBatchPtr batch = TaskBatch::make();
        // account for attack factors
        float damage = 0.0f;
        for(HeroControl* hhh : layer->mPlayer->team)
        {
            if(hhh->getColor() == color || color == Gem::AllColor)
            {
                float skillMod;
                batch << layer->mPlayer->getAttackFactor(layer->mTurn, hc, skillMod);
                damage += hhh->attack * factor * skillMod;
            }
        }
        
        return batch << DamageByValue(layer, hc, damage, isArea);
    }
        
    TaskPtr ActiveSkill::DamageByFactor(BoardLayer* layer, HeroControl* hc, float factor, bool isArea)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        float skillMod;
        batch << layer->mPlayer->getAttackFactor(layer->mTurn, hc, skillMod);
        float damage = hc->attack * factor * skillMod;
        
        return batch << DamageByValue(layer, hc, damage, isArea);
    }
    
    // This skill does not account for hero color
    TaskPtr ActiveSkill::DamageByEnemyHealthFactor(BoardLayer* layer, HeroControl* hc, float factor)
    {
        TaskBatchPtr areaBatch = TaskBatch::make();
        
        DamageTable emptyTable(layer->mEnemyList);
        
        float delay = 0.0f;
        for(EnemyControlPtr ec : layer->mEnemyList)
        {
            float finalDamage = ec->getHealth() * factor;
            
            areaBatch << layer->createAttackSequence(hc,
                                                     ec,
                                                     finalDamage,
                                                     finalDamage,
                                                     hc->getColor(),
                                                     delay);
            
            TaskPtr hit = layer->dealDamageToOneWithAnim(finalDamage, ec, delay);
            areaBatch << TaskSequence::make(createDelay(layer, 0.5),
                                            hit);
            delay += 0.2f;
        }
        layer->removeDeadEnemies();
        
        return areaBatch;
    }
    
    TaskPtr ActiveSkill::DamageByPlayerHealthFactor(BoardLayer* layer,
                                                    HeroControl* hc,
                                                    float factor,
                                                    bool isArea)
    {
        float damage = factor * layer->mPlayer->getMaxHealth();
        return DamageByValue(layer, hc, damage, isArea);
    }
    
    TaskPtr ActiveSkill::DamageBySelfDamageFactor(BoardLayer* layer,
                                                  HeroControl* hc,
                                                  float healthFactor,
                                                  float attackFactor,
                                                  bool isArea)
    {
        float attack = layer->mPlayer->getHealth() * healthFactor;
        
        TaskPtr hitAnim = layer->mPlayer->hit(attack);
        TaskPtr attackAnim = DamageByValue(layer, hc, attack, isArea);
        
        return TaskBatch::make() << hitAnim << attackAnim;
    }
    
    TaskPtr ActiveSkill::DamageFromHealthGem(BoardLayer* layer,
                                             HeroControl* hc,
                                             float factor,
                                             bool isArea)
    {
        TaskSequencePtr seq = TaskSequence::make();
        
        int healthCount = 0;
        for(int i=0; i<layer->mGemGrid.size(); i++)
        {
            Gem* gem = layer->mGemGrid(i);
            if(gem->color == Gem::Health)
            {
                if (gem == NULL)
                    continue;

                seq << TaskSound::make("sound/v/gem_sweep.mp3");
                seq << removeGemFromGridWithAnim(layer->mGemGrid, gem, 0.2f);
                
                healthCount++;
            }
        }
        
        seq << DamageByFactor(layer, hc, healthCount*factor, isArea);
        seq << fillBoard(layer,
                         layer->mGemLayer,
                         layer->mGemGrid,
                         [=](){return layer->randGemWeighted();});

        return seq;
    }
    
    //----------------------------------------------------
    //--------------- Multiple Attacks -------------------
    //----------------------------------------------------
    TaskPtr ActiveSkill::MultipleAttacksByFactor(BoardLayer* layer,
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

    //----------------------------------------------------
    //--------------- Leech Skills -----------------------
    //----------------------------------------------------
    TaskPtr ActiveSkill::LeechByFactor(PH::BoardLayer *layer,
                                       HeroControl* hc,
                                       float attackFactor,
                                       float leechFactor)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        float skillMod = 1.0f;
        batch << layer->mPlayer->getAttackFactor(layer->mTurn, hc, skillMod);

        float damage = hc->attack * attackFactor * skillMod;
        
        float totalDamage = 0.0f;
        batch << DamageCommon(layer,
                              hc,
                              [damage](EnemyControlPtr ec){return damage;},
                              true,
                              totalDamage);
        
        float healAmount = totalDamage * leechFactor;
        batch << HealByValue(layer, hc, healAmount);

        return batch;
    }
    
    //----------------------------------------------------
    //--------------- Turn Based Skills ------------------
    //----------------------------------------------------
    static TaskPtr CastTurnBasedSkill(BoardLayer* layer,
                                      HeroControl* hc,
                                      boost::function<TaskPtr (EnemyControlPtr ec)> f)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        for(EnemyControlPtr ec : layer->mEnemyList)
        {
            TaskPtr anim = f(ec);
            batch->add(anim);
        }
        layer->removeDeadEnemies();

        return batch;
    }

    TaskPtr ActiveSkill::PoisonByValue(BoardLayer* layer, HeroControl* hc, int duration, float val)
    {
        return CastTurnBasedSkill(layer, hc, [=](EnemyControlPtr ec) -> TaskPtr
        {
            TaskSequencePtr seq = TaskSequence::make();
            seq << createOrb(layer, hc->center(), ec->center(), 0.0f);
            seq << ec->setPropertyWithAnim(layer, ec->poison, duration, val, "毒", Gem::AllColor);
            return seq;
        });
    }
    
    TaskPtr ActiveSkill::PoisonByFactor(PH::BoardLayer *layer, PH::HeroControl *hc, int duration, float factor)
    {
        float val = hc->attack * factor;
        return PoisonByValue(layer, hc, duration, val);
    }
        
    TaskPtr ActiveSkill::Blind(BoardLayer* layer, HeroControl* hc, int duration, bool toAll)
    {
        return CastTurnBasedSkill(layer, hc, [=](EnemyControlPtr ec) -> TaskPtr
        {
            TaskSequencePtr seq = TaskSequence::make();
            seq << createOrb(layer, hc->center(), ec->center(), 0.0f);
            seq << ec->setPropertyWithAnim(layer, ec->blind, duration, true, "盲", Gem::AllColor);
            return seq;
        });
    }
        
    TaskPtr ActiveSkill::Silence(BoardLayer* layer, HeroControl* hc, int duration, bool toAll)
    {
        return CastTurnBasedSkill(layer, hc, [=](EnemyControlPtr ec) -> TaskPtr
        {
            TaskSequencePtr seq = TaskSequence::make();
            seq << createOrb(layer, hc->center(), ec->center(), 0.0f);
            seq << ec->setPropertyWithAnim(layer, ec->silence, duration, true, "封", Gem::AllColor);
            return seq;
        });
    }

    TaskPtr ActiveSkill::ReduceEnemyArmorByFactor(PH::BoardLayer *layer, PH::HeroControl *hc, int duration, float factor)
    {
        return CastTurnBasedSkill(layer, hc, [=](EnemyControlPtr ec) -> TaskPtr
        {
            TaskSequencePtr seq = TaskSequence::make();
            seq << createOrb(layer, hc->center(), ec->center(), 0.0f);
            seq << ec->setPropertyWithAnim(layer, ec->armorFactor, duration, factor, "弱防", Gem::AllColor);
            return seq;
        });
    }
    
    TaskPtr ActiveSkill::ReduceDamageByFactor(BoardLayer* layer, HeroControl* hc, int duration, float factor)
    {
        return layer->mPlayer->setPropertyWithAnim(layer,
                                                   layer->mPlayer->damageReduceFactor,
                                                   duration,
                                                   factor,
                                                   "防",
                                                   Gem::AllColor);
    }
    
    TaskPtr ActiveSkill::ReflectByFactor(BoardLayer* layer, HeroControl* hc, int duration, float val)
    {
        TaskPtr anim = layer->mPlayer->setPropertyWithAnim(layer,
                                                           layer->mPlayer->reflect,
                                                           duration,
                                                           HCFloat(hc, val),
                                                           "反彈",
                                                           Gem::AllColor);
        return anim;
    }
    
    TaskPtr ActiveSkill::BoostAttackAll(BoardLayer* layer, int duration, float factor)
    {
        std::string prefix = factor >= 1.0f ? "強" : "弱";
        
        TaskPtr t = layer->mPlayer->setPropertyWithAnim(layer,
                                                        layer->mPlayer->attackFactor,
                                                        duration,
                                                        ColorFloat(Gem::AllColor, factor),
                                                        prefix + "攻",
                                                        Gem::AllColor);
        return t;
    }
        
    TaskPtr ActiveSkill::BoostAttackByColor(BoardLayer* layer, int duration, float factor, Gem::GemColor color)
    {
        std::string prefix = factor >= 1.0f ? "強" : "弱";
        
        TaskPtr t = layer->mPlayer->setPropertyWithAnim(layer,
                                                        layer->mPlayer->attackFactor,
                                                        duration,
                                                        ColorFloat(color, factor),
                                                        prefix + "攻",
                                                        color);
        return t;
    }
    
    TaskPtr ActiveSkill::Shield(BoardLayer* layer,
                                HeroControl* hc,
                                float damage,
                                Gem::GemColor color)
    {
        TaskPtr t = layer->mPlayer->setPropertyWithAnim(layer,
                                                        layer->mPlayer->shield,
                                                        99999,
                                                        ColorFloat(color, damage),
                                                        "盾",
                                                        color);
        return t;
    }

    //----------------
    // Invoke skills
    //----------------
    TaskPtr ActiveSkill::invoke(BoardLayer* layer, HeroControl* hc, int currTurn)
    {
        // no skill
        if(hc->info.profile->activeSkillId == -1)
            return TaskEmpty::make();
        
        const SkillInfo& skill = AST(hc->info.profile->activeSkillId);
        
        printf("* Hero %s, skill %s, id %d, call %s\n", hc->info.profile->nameCN.c_str(),
               skill.name.c_str(),
               skill.sid,
               skill.script.c_str());
        
        if(skill.script == "ActiveTransformGemColor")
        {
            return TransformGemColor(layer,
                                     Gem::strToColor(skill.params[0]),
                                     Gem::strToColor(skill.params[1]));
        }
        else if(skill.script == "ActiveTransformAllGem")
        {
            return TransformAllGem(layer);
        }
        else if(skill.script == "ActiveSweepRandomGem")
        {
            return SweepRandomGem(layer,
                                  hc,
                                  toInt(skill.params[0]));
        }
        else if(skill.script == "ActiveStopMoveTimer")
        {
            return StopMoveTimer(layer);
        }
        else if(skill.script == "ActiveExtendMoveTimer")
        {
            float secs = toFloat(skill.params[0]);
            return ExtendMoveTimer(layer, secs);
        }
        else if(skill.script == "ActiveDisableSweep")
        {
            float secs = toFloat(skill.params[0]);
            return DisableSweep(layer, secs);
        }
        else if(skill.script == "ActiveExtendEnemyWaitTurn")
        {
            int turn = toInt(skill.params[0]);
            return ExtendEnemyWaitTurn(layer, turn);
        }
        else if(skill.script == "ActiveRandomizeGem")
        {
            return RandomizeGem(layer);
        }
        else if(skill.script == "ActiveSortGem")
        {
            return SortGem(layer);
        }
        else if(skill.script == "ActiveDamageByValue")
        {
            float damage = toFloat(skill.params[0]);
            float isArea = skill.asBool(1, true);
            return DamageByValue(layer, hc, damage, isArea);
        }
        else if(skill.script == "ActiveDamageByPlayerHealthFactor")
        {
            float factor = toFloat(skill.params[0]);
            bool isArea = skill.asBool(1, true);
            return DamageByPlayerHealthFactor(layer, hc, factor, isArea);
        }
        else if(skill.script == "ActiveDamageByHeroHealthFactor")
        {
            float factor = toFloat(skill.params[0]);
            bool isArea = skill.asBool(1, true);
            return DamageByValue(layer, hc, hc->health * factor, isArea);
        }
        else if(skill.script == "ActiveDamageByFactorFromColor")
        {
            float factor = toFloat(skill.params[0]);
            bool isArea = skill.asBool(1, true);
            return DamageByFactorFromColor(layer, hc, factor, hc->getColor(), isArea);
        }
        else if(skill.script == "ActiveDamageByFactorFromTeam")
        {
            float factor = toFloat(skill.params[0]);
            bool isArea = skill.asBool(1, true);
            return DamageByFactorFromColor(layer, hc, factor, Gem::AllColor, isArea);
        }
        else if(skill.script == "ActiveDamageByFactor")
        {
            float factor = toFloat(skill.params[0]);
            bool isArea = skill.asBool(1, true);
            return DamageByFactor(layer, hc, factor, isArea);
        }
        else if(skill.script == "ActiveDamageByEnemyHealthFactor")
        {
            float percent = toFloat(skill.params[0]);
            return DamageByEnemyHealthFactor(layer, hc, percent);
        }
        else if(skill.script == "ActiveHeal")
        {
            float val = toFloat(skill.params[0]);
            return HealByValue(layer, hc, val);
        }
        else if(skill.script == "ActiveHealByFactor")
        {
            float val = toFloat(skill.params[0]);
            return HealByFactor(layer, hc, val);
        }
        else if(skill.script == "ActiveHealFull")
        {
            return HealFull(layer, hc);
        }
        else if(skill.script == "ActiveBoostAttackByColor")
        {
            int duration = toInt(skill.params[0]);
            float val = toFloat(skill.params[1]);
            Gem::GemColor color = Gem::strToColor(skill.params[2]);
            
            return BoostAttackByColor(layer, duration, val, color);
        }
        else if(skill.script == "ActiveBoostAttackAll")
        {
            int duration = toInt(skill.params[0]);
            float val = toFloat(skill.params[1]);
            
            return BoostAttackAll(layer, duration, val);
        }
        else if(skill.script == "ActivePoisonByValue")
        {
            int duration = toInt(skill.params[0]);
            float damage = toFloat(skill.params[1]);
            return PoisonByValue(layer, hc, duration, damage);
        }
        else if(skill.script == "ActivePoisonByFactor")
        {
            int duration = toInt(skill.params[0]);
            float factor = toFloat(skill.params[1]);
            return PoisonByFactor(layer, hc, duration, factor);
        }
        else if(skill.script == "ActiveBlind")
        {
            int duration = toInt(skill.params[0]);
            return Blind(layer, hc, duration, true);
        }
        else if(skill.script == "ActiveSilence")
        {
            int duration = toInt(skill.params[0]);
            return Silence(layer, hc, duration, true);
        }
        else if(skill.script == "ActiveReflectByFactor")
        {
            int duration = toInt(skill.params[0]);
            float factor = toFloat(skill.params[1]);
            return ReflectByFactor(layer, hc, duration, factor);
        }
        else if(skill.script == "ActiveReduceEnemyArmorByFactor")
        {
            int duration = toInt(skill.params[0]);
            float factor = toFloat(skill.params[1]);
            return ReduceEnemyArmorByFactor(layer, hc, duration, factor);
        }
        else if(skill.script == "ActiveReduceDamageByFactor")
        {
            int duration = toInt(skill.params[0]);
            float factor = toFloat(skill.params[1]);
            return ReduceDamageByFactor(layer, hc, duration, factor);
        }
        else if(skill.script == "ActiveMoveAgain")
        {
            int count = toInt(skill.params[0]);
            return MoveAgain(layer, count);
        }
        else if(skill.script == "ActiveLeechByFactor")
        {
            float attackFactor = toFloat(skill.params[0]);
            float leechFactor = toFloat(skill.params[1]);
            return LeechByFactor(layer, hc, attackFactor, leechFactor);
        }
        else if(skill.script == "ActiveDamageBySelfDamageFactor")
        {
            float healthFactor = toFloat(skill.params[0]);
            float attackFactor = toFloat(skill.params[1]);
            bool isArea = skill.asBool(2, true);
            return DamageBySelfDamageFactor(layer, hc, healthFactor, attackFactor, isArea);
        }
        else if(skill.script == "ActiveDamageFromHealthGem")
        {
            float factor = toFloat(skill.params[0]);
            bool isArea = skill.asBool(1, true);
            return DamageFromHealthGem(layer, hc, factor, isArea);
        }
        else if(skill.script == "ActiveMultipleAttacksByFactor")
        {
            int count = toInt(skill.params[0]);
            float factor = toFloat(skill.params[1]);
            bool isArea = skill.asBool(2, true);
            return MultipleAttacksByFactor(layer, hc, count, factor, isArea);
        }
        else if(skill.script == "ActiveInstantEnergy")
        {
            float factor = toFloat(skill.params[0]);
            Gem::GemColor color = Gem::strToColor(skill.params[1]);
            return InstantEnergy(layer, hc, factor, color);
        }
        else if(skill.script == "ActiveShield")
        {
            float damage = toFloat(skill.params[0]);
            Gem::GemColor color = Gem::strToColor(skill.params[1]);
            return Shield(layer, hc, damage, color);
        }

        LOGD("unknown active skill %s, %d", skill.name.c_str(), skill.sid);
        phassert(false && "unknown skill");
        return TaskEmpty::make();
    }
    
    
    // **********************************************************
    //                    Passive Skills
    // **********************************************************
    
    void PassiveSkill::SetAttackFactor(PlayerControlPtr player,
                                       HeroControl* hc,
                                       float factor,
                                       Gem::GemColor color)
    {
        player->passiveAttackFactor[color](hc, factor);
    }
    
    void PassiveSkill::SetAttackFactorByFaction(PlayerControlPtr player,
                                                HeroControl* hc,
                                                float factor,
                                                const std::string& faction)
    {
        player->passiveFactionAttackFactorMap[faction](hc, factor);
    }
    
    void PassiveSkill::SetDamageReduceFactor(PlayerControlPtr player,
                                             HeroControl* hc,
                                             float factor,
                                             Gem::GemColor color)
    {
        player->passiveDamageReduceFactor[color](hc, factor);
    }
    
    void PassiveSkill::SetFollowUpAttack(PlayerControlPtr player,
                                         HeroControl* hc,
                                         float factor)
    {
        player->passiveFollowUpAttacks(hc, factor);
    }
    
    void PassiveSkill::SetSurviveCriticalHitFactor(PlayerControlPtr player,
                                                   HeroControl* hc,
                                                   float factor)
    {
        player->passiveSurviveHitFactor(hc, factor);
    }
    
    void PassiveSkill::SetHealByFactor(PlayerControlPtr player,
                                       HeroControl* hc,
                                       float factor)
    {
        player->passiveHealByFactor(hc, factor);
    }
    
    
    //--------------------------
    // Bonus health skills
    //--------------------------
    static void SetBonusHealthFactor(PlayerControlPtr player,
                                    float factor,
                                    std::vector<HeroControl*> heroes)
    {
        // healthDiff can be negative.
        float healthDiff = 0.0f;
        for(HeroControl* hc : heroes)
                healthDiff += hc->health * factor - hc->health;
        player->setMaxHealth(player->getMaxHealth() + healthDiff);
    }
    
    // By color
    void PassiveSkill::SetBonusHealthByFactor(PlayerControlPtr player,
                                              PH::HeroControl *hc,
                                              float factor,
                                              Gem::GemColor c)
    {
        auto predicate = [=](HeroControl* hc) -> bool
                            {
                                return hc->getColor() == c || c == Gem::AllColor;
                            };
        SetBonusHealthFactor(player,
                             factor,
                             filter<HeroControl*>(player->team, predicate));
    }
    
    // By faction
    void PassiveSkill::SetBonusHealthByFactorByFaction(PlayerControlPtr player,
                                                       PH::HeroControl *hc,
                                                       float factor,
                                                       const std::string& faction)
    {
        auto predicate = [=](HeroControl* hc) -> bool
        {
            return hc->info.profile->faction == faction;
        };
        
        SetBonusHealthFactor(player,
                             factor,
                             filter<HeroControl*>(player->team, predicate));
    }
    //----------------------------
    
    void PassiveSkill::SetBonusDragTime(PlayerControlPtr player, float time)
    {
        player->moveTimer.maxTime = std::max(player->moveTimer.maxTime,
                                             player->moveTimer.baseMaxTime + time);
        player->moveTimer.time = player->moveTimer.maxTime;
    }
    
    void PassiveSkill::SetEnergyFactor(PlayerControlPtr player, float factor, Gem::GemColor c)
    {
        player->mEnergyFactor[c] *= factor;
    }
    
    void PassiveSkill::SetMinEnergy(PlayerControlPtr player, float minEnergy, Gem::GemColor c)
    {
        player->mMinEnergy[c] += minEnergy;
    }
    
    void PassiveSkill::SetComboFactor(PlayerControlPtr player, float factor)
    {
        player->comboFactor = std::max(player->comboFactor, factor);
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
    
    void PassiveSkill::SetDamageToHealthByFactor(PlayerControlPtr player,
                                                 HeroControl* hc,
                                                 float factor)
    {
        player->passiveDamageToHealthFactor(hc, factor);
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
                                    Gem::GemColor color,
                                    float factor)
    {
        player->mGemWeights[color] *= factor;
    }
    
    void PassiveSkill::SetMinComboCount(PlayerControlPtr player, int count)
    {
        player->minComboCount += count;
    }
    
    void PassiveSkill::SetMinGemCount(PlayerControlPtr player, int count)
    {
        player->minGemCount = count;
    }
    
    void PassiveSkill::SetEnergyBonusByGemCount(PlayerControlPtr player,
                                                HeroControl* hc,
                                                int count,
                                                float energy,
                                                Gem::GemColor color)
    {
        player->passiveEnergyBonusByGemCount[color](hc,
                                                    std::pair<float, float>((float)count, energy));
    }
    
    void PassiveSkill::invoke(CCLayer* layer, PlayerControlPtr player, HeroControl* hc)
    {
        if(hc->info.profile->passiveSkillId == -1)
            return;

        const SkillInfo& skill = PST(hc->info.profile->passiveSkillId);
        
        if(skill.script == "PassiveAttackFactor")
        {
            PassiveSkill::SetAttackFactor(player,
                                          hc,
                                          toFloat(skill.params[0]),
                                          Gem::strToColor(skill.params[1]));
        }
        else if(skill.script == "PassiveAttackFactorByFaction")
        {
            PassiveSkill::SetAttackFactorByFaction(player,
                                                   hc,
                                                   toFloat(skill.params[0]),
                                                   skill.params[1]);
        }
        else if(skill.script == "PassiveDamageReduceFactor")
        {
            PassiveSkill::SetDamageReduceFactor(player,
                                                hc,
                                                toFloat(skill.params[0]),
                                                Gem::strToColor(skill.params[1]));
        }
        else if(skill.script == "PassiveDamageReduceAllFactor")
        {
            SetDamageReduceFactor(player,
                                  hc,
                                  toFloat(skill.params[0]),
                                  Gem::AllColor);
        }
        else if(skill.script == "PassiveFollowUpAttack")
        {
            PassiveSkill::SetFollowUpAttack(player,
                                            hc,
                                            toFloat(skill.params[0]));
        }
        else if(skill.script == "PassiveSurviveCriticalHitFactor")
        {
            PassiveSkill::SetSurviveCriticalHitFactor(player,
                                                      hc,
                                                      toFloat(skill.params[0]));
        }
        else if(skill.script == "PassiveHealByFactor")
        {
            PassiveSkill::SetHealByFactor(player,
                                          hc,
                                          toFloat(skill.params[0]));
        }
        else if(skill.script == "PassiveSetBonusHealthByFactor")
        {
            PassiveSkill::SetBonusHealthByFactor(player,
                                                 hc,
                                                 toFloat(skill.params[0]),
                                                 Gem::strToColor(skill.params[1]));
        }
        else if(skill.script == "PassiveSetBonusHealthByFactorByFaction")
        {
            PassiveSkill::SetBonusHealthByFactorByFaction(player,
                                                          hc,
                                                          toFloat(skill.params[0]),
                                                          skill.params[1]);
        }
        else if(skill.script == "PassiveBonusDragTime")
        {
            PassiveSkill::SetBonusDragTime(player,
                                           toFloat(skill.params[0]));
        }
        else if(skill.script == "PassiveSetEnergyFactor")
        {
            PassiveSkill::SetEnergyFactor(player,
                                          toFloat(skill.params[0]),
                                          Gem::strToColor(skill.params[1]));
        }
        else if(skill.script == "PassiveSetMinEnergy")
        {
            PassiveSkill::SetMinEnergy(player,
                                       toFloat(skill.params[0]),
                                       Gem::strToColor(skill.params[1]));
        }
        else if(skill.script == "PassiveSetComboFactor")
        {
            PassiveSkill::SetComboFactor(player, toFloat(skill.params[0]));
        }
        else if(skill.script == "PassiveReduceDamageOnHealth")
        {
            PassiveSkill::SetReduceDamageOnHealth(player,
                                                  hc,
                                                  toFloat(skill.params[0]),
                                                  toFloat(skill.params[1]));
        }
        else if(skill.script == "PassiveBoostAttackOnHealth")
        {
            PassiveSkill::SetBoostAttackOnHealth(player,
                                                 hc,
                                                 toFloat(skill.params[0]),
                                                 toFloat(skill.params[1]));
        }
        else if(skill.script == "PassiveBleedingByFactor")
        {
            PassiveSkill::SetBonusHealthByFactor(player,
                                                 hc,
                                                 toFloat(skill.params[0]),
                                                 hc->getColor());
            PassiveSkill::SetBleedingByFactor(player,
                                              hc,
                                              toFloat(skill.params[1]));
        }
        else if(skill.script == "PassiveBonusCoin")
        {
            
        }
        else if(skill.script == "PassiveBonusGold")
        {
            
        }
        else if(skill.script == "PassiveSetAreaAttackAndFactor")
        {
            SetForceAreaAttackAndFactor(player, hc, toFloat(skill.params[0]));
        }
        else if(skill.script == "PassiveSetGemWeight")
        {
            SetGemWeight(player,
                         hc,
                         Gem::strToColor(skill.params[0]),
                         toFloat(skill.params[1]));
        }
        else if(skill.script == "PassiveSetMinComboCount")
        {
            SetMinComboCount(player, toInt(skill.params[0]));
        }
        else if(skill.script == "PassiveSetMinGemCount")
        {
            SetMinGemCount(player, toInt(skill.params[0]));
        }
        else if(skill.script == "PassiveSetEnergyBonusByGemCount")
        {
            SetEnergyBonusByGemCount(player,
                                     hc,
                                     toInt(skill.params[0]),
                                     toFloat(skill.params[1]),
                                     Gem::strToColor(skill.params[2]));
        }
        else if(skill.script == "PassiveDamageToHealthByFactor")
        {
            SetDamageToHealthByFactor(player, hc, toFloat(skill.params[0]));
        }
        else
        {
            LOGD("unknown passive skill %s, %d", skill.name.c_str(), skill.sid);
            phassert(false && "unknown skill");
        }
    }
}