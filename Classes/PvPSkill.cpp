//
//  PvPSkill.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 5/17/13.
//
//

#include "PvPSkill.h"
#include "ActiveSkill.h"
#include "BoardControls.h"
#include "PVPBoard.h"

namespace PH
{
    //---------------------------------------------
    //------------- Damage Skills -----------------
    //---------------------------------------------
    static TaskPtr DamageCommon(CCLayer* layer,
                                HeroControl* hc,
                                PlayerControlPtr defender,
                                float attack,
                                bool isArea,
                                float& totalDamage)
    {
        PVPBoardLayer* cast = dynamic_cast<PVPBoardLayer*>(layer);
        if(!cast) return TaskIgnore::make();
        
        TaskBatchPtr batch = TaskBatch::make();
        
        totalDamage = 0.0f;
        batch << cast->calcDamageWithReflect(attack,
                                             hc->getColor(),
                                             cast->getAttacker(),
                                             cast->getDefender(),
                                             0.f,
                                             totalDamage);
        
        batch << createOrb(layer,
                           hc->center(),
                           defender->center(),
                           0.0f);
        
        batch << createFloatText(layer,
                                 -totalDamage,
                                 defender->center(),
                                 GemUtils::Health,
                                 0.1f);
        
        batch << defender->hit(totalDamage, layer);
        
        return batch;
    }
    
    static TaskPtr DamageCommon(CCLayer* layer,
                                HeroControl* hc,
                                PlayerControlPtr defender,
                                float attack,
                                bool isArea)
    {
        float totalDamage = 0.0f;
        return DamageCommon(layer, hc, defender, attack, isArea, totalDamage);
    }
    
    TaskPtr PvPActive::DamageByValue(CCLayer *layer,
                                     HeroControl* hc,
                                     PlayerControlPtr defender,
                                     float attack,
                                     bool isArea)
    {
        return DamageCommon(layer,
                            hc,
                            defender,
                            attack,
                            isArea);
    }
    
    TaskPtr PvPActive::DamageByFactor(CCLayer* layer,
                                      HeroControl* hc,
                                      PlayerControlPtr attacker,
                                      PlayerControlPtr defender,
                                      float factor,
                                      bool isArea)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        float skillMod;
        batch << attacker->getAttackFactor(hc, skillMod, [](){return PvPRand::rand();});
        float damage = hc->attack * (factor * skillMod);
        
        return batch << DamageByValue(layer, hc, defender, damage, isArea);
    }

    TaskPtr PvPActive::DamageByFactorWithoutResistence(CCLayer * layer,
                                                       HeroControl * hc,
                                                       PlayerControlPtr attacker,
                                                       PlayerControlPtr defender,
                                                       GemUtils::GemColor color,
                                                       float factor)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        float skillMod;
        batch << attacker->getAttackFactor(hc, skillMod, [](){return PvPRand::rand();});

        float totalDamage = hc->attack * ( skillMod * factor );
 
        batch << createOrb(layer,
                           hc->center(),
                           defender->center(),
                           0.0f);
        
        batch << createFloatText(layer,
                                 -totalDamage,
                                 defender->center(),
                                 GemUtils::Health,
                                 0.1f);
        
        batch << defender->hit(totalDamage, layer);

        return batch;
    }
    
    TaskPtr PvPActive::DamageByFactorFromColor(CCLayer* layer,
                                               HeroControl* hc,
                                               PlayerControlPtr attacker,
                                               PlayerControlPtr defender,
                                               float factor,
                                               GemUtils::GemColor color,
                                               bool isArea)
    {
        TaskBatchPtr batch = TaskBatch::make();
        // account for attack factors
        float damage = 0.0f;
        for(HeroControl* hhh : attacker->team)
        {
            if(hhh->getColor() == color || color == GemUtils::AllColor)
            {
                float skillMod;
                batch << attacker->getAttackFactor(hc, skillMod, [](){return PvPRand::rand();});
                damage += hhh->attack * (factor * skillMod);
            }
        }
        
        return batch << DamageByValue(layer, hc, defender, damage, isArea);
    }
    
    // This skill does not account for hero color
    TaskPtr PvPActive::DamageByEnemyHealthFactor(CCLayer* layer,
                                                 HeroControl* hc,
                                                 PlayerControlPtr attacker,
                                                 PlayerControlPtr defender,
                                                 float factor,
                                                 bool isArea)
    {
        TaskBatchPtr areaBatch = TaskBatch::make();
        float damage = defender->getHealth() * factor;
        return DamageByValue(layer, hc, defender, damage, isArea);
    }
    
    TaskPtr PvPActive::DamageByPlayerHealthFactor(CCLayer* layer,
                                                  HeroControl* hc,
                                                  PlayerControlPtr attacker,
                                                  PlayerControlPtr defender,
                                                  float factor,
                                                  bool isArea)
    {
        float damage = factor * attacker->getMaxHealth();
        return DamageByValue(layer, hc, defender, damage, isArea);
    }
    
    TaskPtr PvPActive::DamageByFactorAndEnemyHealth(cocos2d::CCLayer *layer,
                                                    PH::HeroControl *hc,
                                                    PlayerControlPtr attacker,
                                                    PlayerControlPtr defender,
                                                    float factor,
                                                    float healthRatio, float damageFactor)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        float skillMod;
        batch << attacker->getAttackFactor(hc, skillMod, [](){return PvPRand::rand();});
        
        if( defender->healthRatio() < healthRatio )
            skillMod *= damageFactor;
        
        float totalDamage = hc->attack * ( skillMod * factor );
        
        batch << createOrb(layer,
                           hc->center(),
                           defender->center(),
                           0.0f);
        
        batch << createFloatText(layer,
                                 -totalDamage,
                                 defender->center(),
                                 GemUtils::Health,
                                 0.1f);
        
        batch << defender->hit(totalDamage, layer);
        return batch;
    }
    
    TaskPtr PvPActive::DamageBySelfDamageFactor(CCLayer* layer,
                                                HeroControl* hc,
                                                PlayerControlPtr attacker,
                                                PlayerControlPtr defender,
                                                float healthFactor,
                                                float attackFactor,
                                                bool isArea)
    {
        float hit = attacker->getHealth() * healthFactor;
        TaskPtr hitAnim = attacker->hit(hit, layer);
        TaskPtr attackAnim = DamageByValue(layer, hc, defender, hit * attackFactor, isArea);
        
        return TaskBatch::make() << hitAnim << attackAnim;
    }
    
    TaskPtr PvPActive::DamageFromHealthGem(CCLayer* layer,
                                           GemGrid& grid,
                                           CCLayer* gemLayer,
                                           boost::function<GemUtils::GemColor()> randGen,
                                           HeroControl* hc,
                                           PlayerControlPtr attacker,
                                           PlayerControlPtr defender,
                                           float factor,
                                           bool isArea)
    {
        TaskSequencePtr seq = TaskSequence::make();
        
//        int healthCount = 0;
//        for(int i=0; i<grid.size(); i++)
//        {
//            GemPtr gem = grid(i);
//            if(gem->color == GemUtils::Health)
//            {
//                if (gem == NULL)
//                    continue;
//                
//                seq << TaskSound::make("sound/v/gem_sweep.mp3");
//                bool isCombo = true, isRemoved = true;
//                seq << gem->onSweep(isCombo, isRemoved);
//                if(isRemoved) grid(i) = NULL;
//                
//                healthCount++;
//            }
//        }
//        
//        seq << DamageByFactor(layer, hc, attacker, defender, healthCount*factor, isArea);
//        seq << layer->mBoardControl->fillBoard(randGen,
//                                               false);
        
        return seq;
    }
    
    //---------------------
    // Turn based stuff
    //---------------------
    
    TaskPtr PvPActive::PoisonByValue(CCLayer* layer,
                                     HeroControl* hc,
                                     PlayerControlPtr defender,
                                     int duration,
                                     float val)
    {
        TaskSequencePtr seq = TaskSequence::make();
        
        seq << createOrb(layer,
                         hc->center(),
                         defender->center(),
                         0.0f);
        auto buff = FloatBuff::make(GemUtils::AllColor, duration, "蛊毒", val);
        return defender->buffGroup.attachBuff(PlayerControl::kGroup_PreTurn,
                                              "poison", layer, buff);
    }
    
    TaskPtr PvPActive::TransformAllGem(PVPBoardLayer* layer)
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
    
    TaskPtr PvPActive::MultipleAttacksByFactor(CCLayer* layer,
                                               HeroControl* hc,
                                               PlayerControlPtr attacker,
                                               PlayerControlPtr defender,
                                               int count,
                                               float factor,
                                               bool isArea)
    {
        TaskBatchPtr batch = TaskBatch::make();
        for(int i=0; i<count; i++)
        {
            TaskSequencePtr seq = TaskSequence::make();
            seq << createDelay(layer, 0.2f * i);
            seq << DamageByFactor(layer, hc, attacker, defender, factor, isArea);
            batch << seq;
        }
        
        return batch;
    }
    
    TaskPtr PvPActive::PoisonByFactor(CCLayer *layer,
                                      HeroControl *hc,
                                      PlayerControlPtr defender,
                                      int duration,
                                      float factor)
    {
        float val = hc->attack * factor;
        return PoisonByValue(layer, hc, defender, duration, val);
    }

    TaskPtr PvPActive::BlindHeroRandom(CCLayer* layer,
                                       HeroControl* caster,
                                       PlayerControlPtr defender,
                                       int count,
                                       int turn)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        HeroControlList team = defender->team;
        std::random_shuffle(team.begin(), team.end());
        
        for(int i=0; i<count && team.size(); i++)
        {
            HeroControl* hc = team[i];
            batch << createOrb(layer, caster->center(), hc->center(), 0.0f);
            auto buff = IBuff::make(GemUtils::AllColor, turn, "致盲");
            batch << hc->buffGroup.attachBuff(HeroControl::kGroup_onTurn,
                                              "active.blind", layer, buff);
        }
        
        return batch;
    }
    
    TaskPtr PvPActive::BlindHeroByColor(CCLayer *layer,
                                        HeroControl* caster,
                                        PlayerControlPtr defender,
                                        GemUtils::GemColor color,
                                        int turn)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        for(HeroControl* hc : defender->team)
        {
            if(hc->getColor() == color)
            {
                batch << createOrb(layer, caster->center(), hc->center(), 0.0f);
                auto buff = IBuff::make(GemUtils::AllColor, turn, "致盲");
                batch << hc->buffGroup.attachBuff(HeroControl::kGroup_onTurn,
                                                  "active.blind", layer, buff);
            }
        }
        return batch;
    }
    
    
    TaskPtr PvPActive::ReduceAttackByFactor(CCLayer* layer,
                                            HeroControl* caster,
                                            PlayerControlPtr defender,
                                            float factor,
                                            GemUtils::GemColor color,
                                            int turn)
    {
        auto batch = TaskBatch::make();
        for( auto hc : defender->team )
        {
            if(hc->getColor() == color)
            {
                batch << createOrb(layer, caster->center(), hc->center(), 0.f);
                auto buff = FloatBuff::make(color, turn, "弱攻", factor);
                batch << hc->buffGroup.attachBuff(HeroControl::kGroup_onTurn,
                                                  "active.reduce.attack.factor", layer, buff);
            }
        }
        return TaskIgnore::make();
    }
    
    TaskPtr PvPActive::ReduceResistance(CCLayer* layer,
                                        HeroControl* caster,
                                        PlayerControlPtr defender,
                                        GemUtils::GemColor color,
                                        float value,
                                        int turn)
    {
        auto buff = FloatBuff::make(color, turn, "弱防", value);
        
        std::vector<GemUtils::GemColor> colVec;
        
        if( color != GemUtils::AllColor ) colVec.push_back(color);
        else
        {
            for(int i=GemUtils::Fire; i<=GemUtils::Light; i++)
            {
                colVec.push_back(static_cast<GemUtils::GemColor>(i));
            }
        }
        
        return TaskSequence::make()
        << createOrb(layer, caster->center(), defender->center(), 0.f)
        << defender->buffGroup.attachBuff(PlayerControl::kGroup_PreTurn,
                                          "active.reduce.resistance", layer, buff)
        << TaskLambda::make([=]()
        {
            for(auto col : colVec)
            {
                int mid = (int)((defender->resist.val[col]-value)*100 + 100);
                defender->resist.label[col]->setString(toStr(mid).c_str());
            }
        });
    }
    
    TaskPtr PvPActive::ExplodeGem(PVPBoardLayer* layer,
                                  HeroControl* caster,
                                  PlayerControlPtr attacker,
                                  PlayerControlPtr defender,
                                  float factor,
                                  GemUtils::GemColor gemColor,
                                  GemUtils::GemColor atkColor)
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
        << PvPActive::DamageByFactorWithoutResistence(layer,
                                                      caster,
                                                      attacker,
                                                      defender,
                                                      atkColor,
                                                      factor*count)
        << layer->mBoardControl->fillBoard([=](){return layer->randGemWeighted();},
                                           false);
    }
    
    TaskPtr PvPActive::SlowByValue(CCLayer* layer,
                                   PH::HeroControl *caster,
                                   PlayerControlPtr defender,
                                   float val,
                                   int duration)
    {
        auto buff = FloatBuff::make(GemUtils::AllColor, duration, "迟缓", val);
        return TaskSequence::make()
        << createOrb(layer, caster->center(), defender->center(), 0.f)
        << defender->buffGroup.attachBuff(PlayerControl::kGroup_PreTurn,
                                                    "active.slow.val", layer, buff);
    }
    
    TaskPtr PvPActive::AmplifyDamageByFactor(CCLayer *layer,
                                             PH::HeroControl *hc,
                                             PlayerControlPtr defender,
                                             int duration, float factor,
                                             GemUtils::GemColor color)
    {
        FloatBuffPtr buff = FloatBuff::make(color, duration, "加深", factor);
        return TaskSequence::make()
        << createOrb(layer, hc->center(), defender->center())
        << defender->buffGroup.attachBuff(PlayerControl::kGroup_PreTurn,
                                          "amplifyDamage",
                                          layer,
                                          buff);
    }
    
}
