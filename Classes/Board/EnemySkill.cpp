//
//  EnemySkill.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 11/29/12.
//
//

#include <vector>
#include <cstdlib>
#include <cstdio>
#include <algorithm>

#include "EnemySkill.h"
#include "utils.h"
#include "../Fonts.h"

namespace PH
{
    static TaskPtr CastBoardOnRandom(BoardLayer * layer,
                                     boost::function<TaskPtr(GemPtr host)> cb,
                                     int & count,
                                     GemUtils::GemColor color = GemUtils::AllColor,
                                     bool ignoreBusyGem = true)
    {
        TaskBatchPtr batch = TaskBatch::make();
        GemGrid& grid = layer->mBoardControl->getGrid();
        
        std::vector<int> shuffled_seq;
        for(int i=0; i<grid.size(); i++)
        {
            if( ignoreBusyGem && !grid(i)->isModifiable() )
                continue;
            if(grid(i)->color() == color || color == GemUtils::AllColor)
                shuffled_seq.push_back(i);
        }
        std::random_shuffle(shuffled_seq.begin(), shuffled_seq.end());
        
        int iter = 0;
        for(auto item : shuffled_seq)
        {
            batch << cb(grid(item));
            if(++iter >= count) break;
        }
        
        count = iter;
        
        return TaskSequence::make()
        << batch
        << layer->mBoardControl->fillBoard([=](){ return layer->randGemWeighted(); });
    }
    
    TaskPtr EnemySkill::Poison(BoardLayer* layer, EnemyControlPtr self, int duration, float val)
    {
        auto buff = FloatBuff::make(GemUtils::AllColor, duration, "蛊毒", val);
        return layer->mPlayer->buffGroup.attachBuff(PlayerControl::kGroup_PreTurn,
                                                    "poison", layer, buff);
    }
        
    TaskPtr EnemySkill::Heal(BoardLayer* layer, EnemyControlPtr ec, float val)
    {
        return ec->heal(val);
    }
    
    TaskPtr EnemySkill::DrainEnergyByColor(BoardLayer *layer,
                                           EnemyControlPtr ec,
                                           GemUtils::GemColor c,
                                           float value)
    {
        TaskBatchPtr batch = TaskBatch::make();
        for(HeroControl* hc : layer->mPlayer->team)
        {
            if(hc->getColor() == c)
            {
                batch << createOrb(layer, ec->center(), hc->center());
                batch << hc->addEnergy(-value);
            }
        }
        
        return batch;
    }
    
    TaskPtr EnemySkill::DrainEnergyRandomeHero(PH::BoardLayer *layer,
                                               EnemyControlPtr ec,
                                               int count,
                                               float value)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        HeroControlList team = layer->mPlayer->team;
        std::random_shuffle(team.begin(), team.end());
        
        for(int i=0; i<count && team.size(); i++)
        {
            HeroControl* hc = layer->mPlayer->team[i];
            batch << createOrb(layer, ec->center(), hc->center());
            batch << hc->addEnergy(-value); 
        }

        return batch;
    }

    TaskPtr EnemySkill::AttachBombToGems(BoardLayer* layer,
                                         EnemyControlPtr ec,
                                         GemUtils::GemColor color,
                                         int count,
                                         float damage,
                                         int turn)
    {
        return TaskSequence::make()
        << createOrb(layer, ec->center(), layer->mPlayer->center())
        << CastBoardOnRandom(layer,
        [=](GemPtr gem) -> TaskPtr
        {
            auto bomb = BombGem::make(gem->color(), turn, damage);
            return layer->mBoardControl->replaceGem(gem->position, bomb);
        }, count, color, true);
    }
    
    TaskPtr EnemySkill::AttachHandToGems(PH::BoardLayer *layer,
                                         EnemyControlPtr ec,
                                         GemUtils::GemColor color,
                                         int count,
                                         float damage,
                                         int turn)
    {
        return TaskSequence::make()
        << createOrb(layer, ec->center(), layer->mPlayer->center())
        << CastBoardOnRandom(layer,
        [=](GemPtr gem) -> TaskPtr
        {
            auto hand = HandGem::make(gem->color(), turn, damage);
            return layer->mBoardControl->replaceGem(gem->position, hand);
        }, count, color, true);
    }

    TaskPtr EnemySkill::LockGems(BoardLayer * layer,
                                 EnemyControlPtr ec,
                                 GemUtils::GemColor color,
                                 int count,
                                 int turn)
    {
        return TaskSequence::make()
        << createOrb(layer, ec->center(), layer->mPlayer->center())
        << CastBoardOnRandom(layer,
        [=](GemPtr gem) -> TaskPtr
        {
            auto chain = LockedGem::make(gem->color(), turn);
            return layer->mBoardControl->replaceGem(gem->position, chain);
        }, count, color, true);
    }
    
    TaskPtr EnemySkill::BombGemsByColor(BoardLayer* layer,
                                        EnemyControlPtr ec,
                                        GemUtils::GemColor color,
                                        int maxCount)
    {
        GemGrid& grid = layer->mBoardControl->getGrid();
            
        TaskBatchPtr fadeOutBatch = TaskBatch::make();
        int count = 0;
    
        for(int i=0; i<grid.size(); i++)
        {
            if(grid(i)->color() == color)
            {
                TaskSequencePtr seq = TaskSequence::make();

                seq << grid(i)->sweep(layer->mBoardControl.get());
                
                fadeOutBatch << seq;
                count++;
                if(count >= maxCount)
                    break;
            }
        }
         
        return TaskSequence::make()
        << fadeOutBatch
        << layer->mBoardControl->fillBoard([=](){return layer->randGemWeighted(); }, false);
    }
    
    
    TaskPtr EnemySkill::BlindHeroRandom(BoardLayer* layer,
                                        EnemyControlPtr ec,
                                        int count,
                                        int turn)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        HeroControlList team = layer->mPlayer->team;
        std::random_shuffle(team.begin(), team.end());
        
        for(int i=0; i<count && team.size(); i++)
        {
            HeroControl* hc = team[i];
            batch << createOrb(layer, ec->center(), hc->center());
            auto buff = IBuff::make(GemUtils::AllColor, turn, "致盲");
            batch << hc->buffGroup.attachBuff(HeroControl::kGroup_onTurn,
                                              "active.blind", layer, buff);
        }
        
        return batch;
    }
    
    TaskPtr EnemySkill::BlindHeroByColor(PH::BoardLayer *layer,
                                         EnemyControlPtr ec,
                                         GemUtils::GemColor color,
                                         int turn)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        for(HeroControl* hc : layer->mPlayer->team)
        {
            if(hc->getColor() == color)
            {
                batch << createOrb(layer, ec->center(), hc->center());
                auto buff = IBuff::make(GemUtils::AllColor, turn, "致盲");
                batch << hc->buffGroup.attachBuff(HeroControl::kGroup_onTurn,
                                                  "active.blind", layer, buff);
            }
        }
        return batch;
    }

    TaskPtr EnemySkill::ReflectByFactor(BoardLayer * layer,
                                        EnemyControlPtr ec,
                                        float factor,
                                        int turn)
    {
        TaskBatchPtr batch = TaskBatch::make();
        FloatBuffPtr reflect = FloatBuff::make(GemUtils::AllColor, turn, "反弹", factor);
        return batch << ec->buffGroup.attachBuff(EnemyControl::kGroup_PreTurn, "reflect", layer, reflect);
	}

    TaskPtr EnemySkill::ReflectByGems(BoardLayer * layer,
                                      EnemyControlPtr ec,
                                      GemUtils::GemColor color,
                                      float factor,
                                      int count,
                                      int turn)
    {
        return TaskSequence::make()
        << createOrb(layer, ec->center(), layer->mPlayer->center())
        << CastBoardOnRandom(layer, [=](GemPtr gem) -> TaskPtr
        {
            auto shield = ShieldGem::make(gem->color(), turn);
            return layer->mBoardControl->replaceGemRaw(gem->position, shield);
        }, count, color, true)
        << ec->attachGemReflect(layer, factor);
    }
    
    TaskPtr EnemySkill::InjuryFreeExceptDarts(PH::BoardLayer *layer,
                                           EnemyControlPtr ec,
                                           GemUtils::GemColor color,
                                           int count,
                                           int turn)
    {
        return TaskSequence::make()
        //<< createOrb(layer, ec->center(), layer->mPlayer->center())
        << CastBoardOnRandom(layer, [=](GemPtr gem) -> TaskPtr
        {
//            return TaskBatch::make() << gem->attachDarts(turn,
//            [=](Gem * gem) -> TaskPtr
//            {
//                if(ec->injuryFreeDarts.has())
//                    ec->injuryFreeDarts.updateTurn();
//                return TaskIgnore::make();
//            },
//            [=](Gem * host, bool & isCombo, bool & isRemoved) -> TaskPtr
//            {
//                isCombo = true; isRemoved = true;
//                if(ec->injuryFreeDarts.has())
//                {
//                    ec->injuryFreeDarts.updateTurn();
//                    ec->injuryFreeDarts.val() = false;
//                    return TaskSequence::make()
//                    << host->sendDarts(ec->center())
//                    << ec->unbindReflectShield();
//                }
//                return TaskIgnore::make();
//            });
            return TaskIgnore::make();
        }, count, color, true);
        //<< ec->attachDartsReflect(coun);
    }
    
    TaskPtr EnemySkill::SlowByFactor(BoardLayer* layer,
                                     EnemyControlPtr ec,
                                     float factor,
                                     int turn)
    {
        auto buff = FloatBuff::make(GemUtils::AllColor, turn, "迟缓", factor);
        return layer->mPlayer->buffGroup.attachBuff(PlayerControl::kGroup_PreTurn,
                                                    "slowFactor", layer, buff);
    }
    
    TaskPtr EnemySkill::ReduceHealingByFactor(BoardLayer* layer,
                                                EnemyControlPtr ec,
                                                float factor,
                                                int turn)
    {
        auto buff = FloatBuff::make(GemUtils::AllColor, turn, "内伤", factor);
        return layer->mPlayer->buffGroup.attachBuff(PlayerControl::kGroup_PreTurn,
                                                    "reduceHealingFactor", layer, buff);
    }

    TaskPtr EnemySkill::ChangeAttributeByColor(BoardLayer * layer,
                                               EnemyControlPtr ec,
                                               GemUtils::GemColor col,
                                               int turn)
    {
        TaskSequencePtr seq = TaskSequence::make();

        seq << ec->transformColorTo(col);
        
        EnemyControlWeakPtr weak_ec = ec;
        BuffPtr buff = IBuff::make(col, turn, "变色");
        
        return TaskBatch::make()
        << seq
        << ec->buffGroup.attachBuff(EnemyControl::kGroup_PreTurn, "tempColor", layer, buff);
    }

    TaskPtr EnemySkill::Speaking(BoardLayer * layer,
                                 EnemyControlPtr ec,
                                 std::string content)
    {

        return TaskLambda::make([=]()
        {
            const CCPoint point = ccp(ec->center().x, 
                ec->sprite->convertToWorldSpace(ec->sprite->getPosition()).y);

            CCLabelTTF * contentLabel = CCLabelTTF::create(content.c_str(),
                                                           FONT_CN,
                                                           30,
                                                           CCSizeZero,
                                                           kCCTextAlignmentCenter);

            CCScale9Sprite * bg = CCScale9Sprite::create("ui/duihuakuang_diren.png",
                                                         CCRectMake(0, 0, 300, 132),
                                                         CCRectMake(100, 30, 100, 50));

            CCSize contentSize = contentLabel->getContentSize();
            bg->setContentSize(CCSize(std::max(contentSize.width + 60.0f, 80.0f),
                                      std::max(contentSize.height + 60.0f, 132.0f)));
            //bg->setContentSize(CCSize(300,132));
            bg->addChild(contentLabel);
            bg->setScale(0.0f);

            contentLabel->setAnchorPoint(ccp(0.5f, 0.5f));
            contentLabel->setPosition(ccp(bg->getContentSize().width * 0.5,
                                          bg->getContentSize().height * 0.5));

            CCFiniteTimeAction* in = CCEaseElasticOut::create(CCScaleTo::create(0.6f, 1.0f), 1.2f);
            CCFiniteTimeAction* seq = CCSequence::create(in,
                                                         CCDelayTime::create(1.5f),
                                                         CCFadeOut::create(0.3f),
                                                         CCCallLambda::create(removeGen(bg)),
                                                         NULL);

            bg->setAnchorPoint(ccp(0.5f, 1.0f));
            bg->setPosition(ccp(point.x, point.y));
            bg->runAction(seq);
            layer->addChild(bg, 99999);
        });
    }

    TaskPtr EnemySkill::HealAllTheEnmey(BoardLayer * layer,
                                        EnemyControlPtr ec,
                                        float val) {
        TaskBatchPtr batch = TaskBatch::make();
        for(auto iter : layer->mEnemyList) {
            batch << iter->heal(val);
        }

        return batch; 
    }

    TaskPtr EnemySkill::SpreadPlague(BoardLayer * layer,
                                     EnemyControlPtr ec,
                                     GemUtils::GemColor color,
                                     float damage,
                                     int count,
                                     int infectionLimitsPerGem,
                                     int globalLimit,
                                     int turn)
    {
        return TaskSequence::make()
        << createOrb(layer, ec->center(), layer->mPlayer->center())
        << CastBoardOnRandom(layer,
        [=](GemPtr gem) -> TaskPtr
        {
            auto plague = PlagueGem::make(gem->color(), turn, damage,
                                          globalLimit, infectionLimitsPerGem);
            return layer->mBoardControl->replaceGem(gem->position, plague);
        }, count, color, true);
    }
    
    TaskPtr EnemySkill::LeechByFactor(BoardLayer * layer,
                                      EnemyControlPtr ec,
                                      float factor, int duration)
    {
        FloatBuffPtr buff = FloatBuff::make(GemUtils::AllColor, duration+1, "吸血", factor);
        ec->buffGroup.attachBuff(EnemyControl::kGroup_PostTurn, "leech", layer, buff);
        return TaskIgnore::make();
    }
    
    TaskPtr EnemySkill::DamageByCount(BoardLayer * layer,
                                      EnemyControlPtr ec,
                                      float count, float damage)
    {
        if(count <= 0 || damage <= 0) return TaskIgnore::make();
        
        auto batch = TaskBatch::make();
        float delay = 0.f;
        for(int i=0; i<count; i++){
            auto seq = TaskSequence::make();
            seq << createDelay(layer, delay);
            seq << createOrb(layer, ec->center(), layer->getHealthCenter(), true);
            seq << createFloatText(layer,
                                   -damage,
                                   ccp(320,540),
                                   GemUtils::Health,
                                   0.f);
            seq << layer->mPlayer->hit(damage, layer);
            batch << seq;
            delay += 0.3f;
        }
        batch << createFloatWordText(layer,
                                     "连击",
                                     ec->center(),
                                     0.15f);
        batch << TaskSound::make("sound/v/player_hit.mp3");
        
        return batch;
    }
    
    TaskPtr EnemySkill::SetShield(PH::BoardLayer *layer,
                                  EnemyControlPtr ec,
                                  float damage,
                                  float factor)
    {
        return ec->attachShield(layer, damage, factor);
    }
    
    TaskPtr EnemyPassiveSkill::InjuryFree(BoardLayer * layer,
                                          EnemyControlPtr ec,
                                          float factor,
                                          float probality,
                                          GemUtils::GemColor color)
    {
        Float2BuffPtr buff = Float2Buff::make(color, 1, "免伤", factor, probality);
        return ec->buffGroup.attachBuff(EnemyControl::kGroup_NoneOfYouBusiness,
                                        "passiveInjuryFree", layer, buff);
    }
    
    TaskPtr EnemyPassiveSkill::Reflect(BoardLayer * layer,
                                       EnemyControlPtr ec,
                                       float factor,
                                       float probality,
                                       GemUtils::GemColor color,
                                       bool takeDamage)
    {
        auto buff = FLoat2BoolBuff::make(color, 1, takeDamage ? "追杀":"反弹",
                                         factor, probality, takeDamage);
        return ec->buffGroup.attachBuff(EnemyControl::kGroup_NoneOfYouBusiness,
                                        "passiveReflect", layer, buff);
    }
    
    TaskPtr EnemyPassiveSkill::Hound(PH::BoardLayer *layer,
                                     EnemyControlPtr ec,
                                     int count,
                                     float damage)
    {
        auto buff = FloatIntBuff::make(GemUtils::AllColor, 1, "追击", damage, count);
        return ec->buffGroup.attachBuff(EnemyControl::kGroup_NoneOfYouBusiness,
                                        "passiveHound", layer, buff);
    }
    
    TaskPtr EnemyPassiveSkill::Reborn(PH::BoardLayer *layer,
                                      EnemyControlPtr ec,
                                      int count, float factor)
    {
        auto buff = FloatBuff::make(GemUtils::AllColor, count, "复活", factor);
        return ec->buffGroup.attachBuff(EnemyControl::kGroup_NoneOfYouBusiness,
                                        "passiveReborn", layer, buff);
    }
}

// -----------------------------
// must be outside PH namespace.
// lua wrapper implementation.

// states for lua calls

using namespace PH;

static BoardLayer* layerForLua = NULL;
static EnemyControlPtr ecForLua;
static TaskSequencePtr seqForLua;

void BeginEnemySkill(BoardLayer* layer, EnemyControlPtr ec, TaskSequencePtr seq)
{
    layerForLua = layer;
    ecForLua = ec;
    seqForLua = seq;
}

void EndEnemySkill()
{
    layerForLua = NULL;
    ecForLua.reset();
    seqForLua.reset();
}

static inline void assertStates()
{
    assert(layerForLua != NULL && ecForLua && seqForLua && "BeginEnemySkill not called");
}

void EnemyHeal(float val)
{
    assertStates();
    
    seqForLua << EnemySkill::Heal(layerForLua,
                                  ecForLua,
                                  val);
}

void EnemyPoison(int turn, float val)
{
    assertStates();
    
    seqForLua << EnemySkill::Poison(layerForLua,
                                    ecForLua,
                                    turn,
                                    val);
}

void EnemyDrainEnergyByColor(std::string color, float val)
{
    assertStates();
    
    seqForLua << EnemySkill::DrainEnergyByColor(layerForLua,
                                                ecForLua,
                                                GemUtils::strToColor(color),
                                                val);
}

void EnemyDrainEnergyRandomHero(int count, float val)
{
    assertStates();
    
    seqForLua << EnemySkill::DrainEnergyRandomeHero(layerForLua,
                                                    ecForLua,
                                                    count,
                                                    val);
}

void EnemyAttachBombToGems(std::string color, int count, float damage, int turn)
{
    assertStates();
    seqForLua << EnemySkill::AttachBombToGems(layerForLua,
                                              ecForLua,
                                              GemUtils::strToColor(color),
                                              count,
                                              damage,
                                              turn);
}

void EnemyAttachHandToGems(std::string color, int count ,float damage, int turn)
{
    assertStates();
    seqForLua << EnemySkill::AttachHandToGems(layerForLua,
                                              ecForLua,
                                              GemUtils::strToColor(color),
                                              count,
                                              damage, turn);
}

void EnemyLockGems(std::string color, int count, int turn){
    assertStates();
    seqForLua << EnemySkill::LockGems(layerForLua, 
                                      ecForLua,
                                      GemUtils::strToColor(color),
                                      count,
                                      turn);
}

void EnemyBombGemsByColor(std::string color, int maxCount)
{
    assertStates();
    
    seqForLua << EnemySkill::BombGemsByColor(layerForLua,
                                             ecForLua,
                                             GemUtils::strToColor(color),
                                             maxCount);
}

void EnemyBlindHeroRandom(int count, int turn)
{
    assertStates();
    seqForLua << EnemySkill::BlindHeroRandom(layerForLua,
                                             ecForLua,
                                             count,
                                             turn);
}

void EnemyBlindHeroByColor(std::string color, int turn)
{
    assertStates();
    seqForLua << EnemySkill::BlindHeroByColor(layerForLua,
                                              ecForLua,
                                              GemUtils::strToColor(color),
                                              turn);
}

void EnemySlowByFactor(float factor, int turn)
{
    assertStates();
    seqForLua << EnemySkill::SlowByFactor(layerForLua,
                                          ecForLua,
                                          factor,
                                          turn);
}

void EnemyReduceHealingByFactor(float factor, int turn)
{
    assertStates();
    seqForLua << EnemySkill::ReduceHealingByFactor(layerForLua,
                                                   ecForLua,
                                                   factor,
                                                   turn);
}

void EnemyReflectByFactor(float factor, int turn)
{
    assertStates();
    seqForLua << EnemySkill::ReflectByFactor(layerForLua, 
                                             ecForLua, 
                                             factor, 
                                             turn);
}

void EnemyReflectByGems(std::string color, float factor, int count, int turn)
{
    assertStates();
    seqForLua << EnemySkill::ReflectByGems(layerForLua, 
                                           ecForLua, 
                                           GemUtils::strToColor(color), 
                                           factor, 
                                           count, 
                                           turn);
}

void EnemyInjuryFreeExceptDarts(std::string color, int count, int turn)
{
    assertStates();
    seqForLua << EnemySkill::InjuryFreeExceptDarts(layerForLua,
                                                   ecForLua,
                                                   GemUtils::strToColor(color),
                                                   count,
                                                   turn);
}

void EnemySpreadPlague(std::string color, 
                       float damage, 
                       int count, 
                       int infectionLimitsPerGem,
                       int globalLimit,
                       int turn)
{
    assertStates();
    seqForLua << EnemySkill::SpreadPlague(layerForLua, 
                                          ecForLua, 
                                          GemUtils::strToColor(color),
                                          damage, 
                                          count, 
                                          infectionLimitsPerGem,
                                          globalLimit,
                                          turn);
}

void EnemyChangeAttributeByColor(std::string color, int turn)
{
    assertStates();
    seqForLua << EnemySkill::ChangeAttributeByColor(layerForLua, 
                                                    ecForLua, 
                                                    GemUtils::strToColor(color), 
                                                    turn);
}

void EnemySpeaking(std::string content)
{
    assertStates();
    seqForLua << EnemySkill::Speaking(layerForLua, ecForLua, content);
}

void EnemyHealAllTheEnemy(float val)
{
    assertStates();
    seqForLua << EnemySkill::HealAllTheEnmey(layerForLua, ecForLua, val);
}

void EnemyLeechByFactor(float factor, int duration)
{
    assertStates();
    seqForLua << EnemySkill::LeechByFactor(layerForLua, ecForLua, factor, duration);
}

void EnemyDamageByCount(float count, float damage)
{
    assertStates();
    seqForLua << EnemySkill::DamageByCount(layerForLua, ecForLua, count, damage);
}

void EnemySetShield(float damage, float factor)
{
    assertStates();
    seqForLua << EnemySkill::SetShield(layerForLua, ecForLua, damage, factor);
}


void EnemyPassiveInjuryFree(float factor, float probality, std::string color)
{
    assertStates();
    seqForLua << EnemyPassiveSkill::InjuryFree(layerForLua, ecForLua,
                                               factor, probality, GemUtils::strToColor(color));
}

void EnemyPassiveReflect(float factor, float probality,
                         std::string color,
                         bool takeDamage)
{
    assertStates();
    seqForLua << EnemyPassiveSkill::Reflect(layerForLua, ecForLua,
                                            factor, probality, GemUtils::strToColor(color), takeDamage);
}

void EnemyPassiveHound(int count, float damage)
{
    assertStates();
    seqForLua << EnemyPassiveSkill::Hound(layerForLua, ecForLua,
                                          count, damage);
}

void EnemyPassiveReborn(int count, float healthFactor)
{
    assertStates();
    seqForLua << EnemyPassiveSkill::Reborn(layerForLua, ecForLua,
                                           count, healthFactor);
}