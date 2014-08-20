//
//  Skill.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 11/7/12.
//
//

#include "ActiveSkill.h"
#include "BoardControls.h"
#include "Board/Utils.h"
#include "CCCallLambda.h"

namespace PH
{
    static size_t getHeroIndexFromPlayer(PlayerControlPtr player,
                                         HeroControl* hc)
    {
        for(size_t i=0; i<player->team.size(); i++)
        {
            if(player->team[i] == hc) return i;
        }
        return -1;
    }
    
    TaskPtr ActiveSkill::TransformGemColor(CCNode* gemLayer,
                                           GemGrid& grid,
                                           GemUtils::GemColor fromColor,
                                           GemUtils::GemColor toColor)
    {
        auto batch = TaskBatch::make();
        
        float delay = 0.01f;
        for(int i=0; i<grid.size(); i++)
        {
            GemPtr fromGem = grid(i);
            if( fromGem->color() == fromColor ){
                auto seq = TaskSequence::make();
                seq << createDelay(gemLayer, delay);
                seq << fromGem->transformColor(toColor);
                
                batch << seq;
                delay += 0.02;
            }
        }
        
        return TaskSequence::make() << TaskSound::make("sound/v/evolve_success.mp3")
                                    << batch;
    }
    
    TaskPtr ActiveSkill::RandomizeGem(CCNode* layer,
                                      CCNode* gemLayer,
                                      GemGrid& grid,
                                      boost::function<GemUtils::GemColor()> randGen)
    {
        auto batch = TaskBatch::make();
    
        float delay = 0.01f;
        for(int i=0; i<grid.size(); i++)
        {
            auto seq = TaskSequence::make();
            seq << createDelay(layer, delay);
            seq << grid(i)->transformColor(randGen());
            
            batch << seq;
            delay += 0.02;
        }
        
        return TaskSequence::make() << TaskSound::make("sound/v/evolve_success.mp3")
                                    << batch;
    }
    
    TaskPtr ActiveSkill::InstantEnergy(CCLayer* layer,
                                       HeroControl* hc,
                                       PlayerControlPtr player,
                                       float factor,
                                       GemUtils::GemColor c)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        for(HeroControl* hc : player->team)
        {
            if(hc->getColor() == c || c == GemUtils::AllColor)
                batch << hc->addEnergy( hc->maxEnergy * factor );
        }
        
        return batch;
    }

    //-----------------------------------------------
    //------------- Control Skills ------------------
    //-----------------------------------------------
        
    TaskPtr ActiveSkill::SortGem(CCNode* layer,
                                 GemGrid& grid)
    {
        // bucket sort gems by color.
        GemSet buckets[GemUtils::GemColorMax];
        for(int i=0; i<grid.size(); i++)
        {
            GemPtr gem = grid(i);
            buckets[gem->color()].insert(gem);
        }
        
        std::vector<GemPtr> sorted;
        for(int i=0; i<GemUtils::GemColorMax; i++)
            sorted.insert(sorted.end(), buckets[i].begin(), buckets[i].end());

        TaskBatchPtr sortAnim = TaskBatch::make();
        
        for(int y=0; y<grid.Height; y++)
        for(int x=0; x<grid.Width; x++)
        {
            int i = grid.Width*y + x;

            GemPtr gem = sorted[i];
            gem->position = Vec2i(x,y);
            grid(i) = gem;

            sortAnim << gem->moveTo(gem->position, 0.618f);
        }
        
        return sortAnim;
    }

    TaskPtr ActiveSkill::StopMoveTimer(PlayerControlPtr player)
    {
        player->stopTicking();
        
        return TaskIgnore::make();
    }
    
    TaskPtr ActiveSkill::ExtendMoveTimer(PlayerControlPtr player, float secs)
    {
        player->moveTimer.time += secs;
        
        return TaskIgnore::make();
    }
        
    TaskPtr ActiveSkill::DisableSweep(CCLayer* layer, PlayerControlPtr player, float time)
    {
        auto buff = FloatBuff::make(GemUtils::AllColor, 1, "神行", time);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_NoneOfYouBusiness,
                                            "disableSweep", layer, buff);
    }
    
    TaskPtr ActiveSkill::MoveAgain(CCLayer* layer, PlayerControlPtr player, int count)
    {
        // DEPRECATED temporarily due to PvP mode
        
        auto buff = IBuff::make(GemUtils::AllColor, count-1, "连击");
        return player->buffGroup.attachBuff(PlayerControl::kGroup_NoneOfYouBusiness,
                                            "moveAgain", layer, buff);
    }
    
    
    
    TaskPtr ActiveSkill::ReduceDamageByFactor(CCLayer* layer,
                                              HeroControl* hc,
                                              PlayerControlPtr player,
                                              int duration,
                                              float factor)
    {
        auto buff = FloatBuff::make(GemUtils::AllColor, duration, "减伤", factor);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn,
                                            "damageReduceFactor", layer, buff);
    }
    
    TaskPtr ActiveSkill::ReduceDamageByColor(cocos2d::CCLayer* layer,
                                             PH::HeroControl* hc,
                                             PlayerControlPtr player,
                                             int duration,
                                             float factor,
                                             GemUtils::GemColor color)
    {
        auto buff = FloatBuff::make(color, duration, "减伤", factor);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn,
                                            "damageReduceFactor", layer, buff);
    }
    
    
    TaskPtr ActiveSkill::ReflectByFactor(CCLayer* layer,
                                         HeroControl* hc,
                                         PlayerControlPtr player,
                                         int duration,
                                         int count,
                                         float factor,
                                         float probality)
    {
        size_t index = getHeroIndexFromPlayer(player, hc);
        if( index == -1 ) return TaskIgnore::make();
        
        auto buff = ReflectBuff::make(GemUtils::AllColor, duration,
                                      "反弹", factor, count, probality, index);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn, "reflect",
                                            layer, buff);
    }
    
    TaskPtr ActiveSkill::BoostAttackAll(CCLayer* layer,
                                        PlayerControlPtr player,
                                        int duration,
                                        float factor)
    {
        std::string prefix = factor >= 1.0f ? "强" : "弱";
        auto buff = FloatBuff::make(GemUtils::AllColor, duration, prefix + "攻", factor);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PreTurn,
                                            "attackFactor", layer, buff);
    }
    
    TaskPtr ActiveSkill::BoostAttackByColor(CCLayer* layer,
                                            PlayerControlPtr player,
                                            int duration,
                                            float factor,
                                            GemUtils::GemColor color)
    {
        std::string prefix = factor >= 1.0f ? "强" : "弱";
        auto buff = FloatBuff::make(color, duration, prefix + "攻", factor);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PreTurn,
                                            "attackFactor", layer, buff);
    }
    
    TaskPtr ActiveSkill::BoostComboFactor(cocos2d::CCLayer *layer,
                                          PH::PlayerControlPtr player,
                                          int duration, float factor)
    {
        auto buff = FloatBuff::make(GemUtils::AllColor, duration, "强连", factor);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn,
                                            "bonusComboFactor",
                                            layer, buff);
    }

    TaskPtr ActiveSkill::BoostMaxHealth(cocos2d::CCLayer *layer,
                                        PlayerControlPtr player,
                                        int duration, float factor)
    {
        float maxHp = player->getMaxHealth(), hp = player->getHealth();
        auto prevBuff = player->buffGroup.getBuffContent<FloatBuff>("bonusMaxHealth");
        if( prevBuff )
        {
            maxHp = prevBuff->d1;
            hp = (player->getHealth() / player->getMaxHealth())*maxHp;
        }
        
        auto buff = FloatBuff::make(GemUtils::AllColor, duration, "强体", maxHp);
        
        float newMaxHp = maxHp * factor;
        float newHp = hp * factor;
        
        return TaskBatch::make()
        << player->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn,
                                        "bonusMaxHealth", layer, buff)
        << TaskLambda::make([=]()
        {
            player->setMaxHealthDirectly((int)newMaxHp);
            player->setHealthDirectly((int)newHp);
        })
        << player->createHealingAnim();
    }

    TaskPtr ActiveSkill::BoostEnergyByColor(cocos2d::CCLayer *layer,
                                            PlayerControlPtr player,
                                            int duration, int val,
                                            GemUtils::GemColor color)
    {
        auto batch = TaskBatch::make();
        
        for(HeroControl * hc : player->team)
        {
            if(hc->info.profile->color == color || color == GemUtils::AllColor)
            {
                batch << hc->addEnergy(val);
            }
        }
        
        auto seq = TaskSequence::make();
        seq << batch;
        
        if( duration > 1 )
        {
            auto buff = IntBuff::make(color, duration-1, "集气", val);
            seq << player->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn,
                                                "bonusEnergy", layer, buff);
        }
        
        return seq;
    }
    
    TaskPtr ActiveSkill::BoostEnergyByFactorByColor(cocos2d::CCLayer *layer,
                                                    PlayerControlPtr player,
                                                    int duration, float factor,
                                                    GemUtils::GemColor color)
    {
        auto seq = TaskSequence::make();
        
        auto buff = FloatBuff::make(color, duration, "集气", factor);
        seq << player->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn,
                                            "active.boost.energy.by.factor", layer, buff);
        
        return seq;
    }
    
    TaskPtr ActiveSkill::Shield(CCLayer* layer,
                                HeroControl* hc,
                                PlayerControlPtr player,
                                int duration,
                                float factor,
                                GemUtils::GemColor color)
    {
        size_t index = getHeroIndexFromPlayer(player, hc);
        if( index == -1 ) return TaskIgnore::make();
        
        float regenFactor = 1.f;
        player->getRegenFactor(hc, regenFactor);
        float val = regenFactor * hc->regen * factor;
        auto buff = FloatIntBuff::make(color, duration, "气盾", val, index);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn,
                                            "shield", layer, buff);
    }
    
    TaskPtr ActiveSkill::HealByValue(CCLayer* layer,
                                     HeroControl* hc,
                                     PlayerControlPtr player,
                                     float val)
    {
        if(player->isHurt())
            return healWithAnim(layer, player.get(), val);
        
        return TaskIgnore::make();
    }
    
    TaskPtr ActiveSkill::HealByFactor(CCLayer* layer,
                                      HeroControl* hc,
                                      PlayerControlPtr player,
                                      float factor)
    {
        float heal = hc->regen * factor;
        return HealByValue(layer, hc, player, heal);
    }
    
    TaskPtr ActiveSkill::HealByMaxHealth(cocos2d::CCLayer *layer,
                                         PlayerControlPtr player,
                                         int duration, float factor)
    {
        auto buff = FloatBuff::make(GemUtils::AllColor, duration, "恢复", factor);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PreTurn, "healByMaxHealth", layer, buff);
    }
    
    TaskPtr ActiveSkill::HealFull(CCLayer* layer,
                                  HeroControl* hc,
                                  PlayerControlPtr player)
    {
        return HealByValue(layer, hc, player, player->maxHealth);
    }

    TaskPtr ActiveSkill::SuckBloodFromEnemyByRegen(CCLayer * layer,
                                                   HeroControl * hc,
                                                   PlayerControlPtr player,
                                                   int duration,
                                                   float factor,
                                                   bool isArea)
    {
        size_t index = getHeroIndexFromPlayer(player, hc);
        if(index == -1) return TaskIgnore::make();
        
        float total = hc->regen * factor;
        auto buff = IntFloatBoolBuff::make(GemUtils::AllColor, duration,
                                           "吸血", index, total, isArea);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn,
                                            "bloodSucker", layer, buff);
    }
    
    TaskPtr ActiveSkill::Invincible(CCLayer * layer,
                                    HeroControl * hc,
                                    PlayerControlPtr player,
                                    int duration,
                                    float probality,
                                    GemUtils::GemColor color)
    {
        size_t index = getHeroIndexFromPlayer(player, hc);
        if(index == -1) return TaskIgnore::make();
        
        auto buff = FloatIntBuff::make(color, duration, "免伤", probality, index);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn,
                                            "invincible", layer, buff);
    }
    
    TaskPtr ActiveSkill::HealByColor(cocos2d::CCLayer *layer,
                                     PlayerControlPtr player,
                                     int duration,
                                     GemUtils::GemColor color,
                                     bool still)
    {
        auto buff = BoolBuff::make(color, duration, "转血", still);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PreTurn,
                                            "superRegen", layer, buff);
    }
    
    TaskPtr ActiveSkill::ReduceEnergyCost(CCLayer * layer,
                                              PlayerControlPtr player,
                                              int count, float factor)
    {
        auto buff = FloatBuff::make(GemUtils::AllColor, count, "内功", factor);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_NoneOfYouBusiness,
                                            "reduceEnergyCost",
                                            layer,
                                            buff);
    }
    
    TaskPtr ActiveSkill::ReduceCooldownTime(cocos2d::CCLayer *layer,
                                            PlayerControlPtr player,
                                            int times,
                                            GemUtils::GemColor color)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        for(HeroControl* hc : player->team)
        {
            if(hc->info.profile->color == color ||
               GemUtils::AllColor == color)
            {
                batch << hc->updateCooldown(times);
            }
        }
        
        return batch;
    }
    
    TaskPtr ActiveSkill::TriggerCriticalAttack(cocos2d::CCLayer *layer,
                                               PlayerControlPtr player,
                                               int duration, float factor, float probality)
    {
        auto buff = Float2Buff::make(GemUtils::AllColor, duration, "暴击", factor, probality);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn,
                                            "criticalAttack", layer, buff);
    }
    
    TaskPtr ActiveSkill::DispelPassiveBuff(cocos2d::CCLayer * layer,
                                           PlayerControlPtr player)
    {
        return player->dispellPassiveBuff();
    }
    
    TaskPtr ActiveSkill::AbsorbDamage(cocos2d::CCLayer *layer,
                                      HeroControl * hc,
                                      PlayerControlPtr player,
                                      int duration,
                                      int count)
    {
        size_t index = getHeroIndexFromPlayer(player, hc);
        if(index == -1) return TaskIgnore::make();

        auto buff = Int2Buff::make(GemUtils::AllColor, duration, "吸收", index, count);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn,
                                            "absorbDamage", layer, buff);
    }
    
    TaskPtr ActiveSkill::LeechByAttack(cocos2d::CCLayer *layer,
                                       PlayerControlPtr player,
                                       int duration, float factor)
    {
        auto buff = FloatBuff::make(GemUtils::AllColor, duration, "吸血", factor);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn, "leech", layer, buff);
    }
    
    TaskPtr ActiveSkill::Zombine(cocos2d::CCLayer* layer,
                                 HeroControl* hc,
                                 PlayerControlPtr player,
                                 int count)
    {
        size_t index = getHeroIndexFromPlayer(player, hc);
        if(index == -1) return TaskIgnore::make();
        
        auto buff = IntBuff::make(GemUtils::AllColor, count, "僵尸", index);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_NoneOfYouBusiness,
                                            "active.zombine", layer, buff);
    }
    
    TaskPtr ActiveSkill::FightBack(cocos2d::CCLayer *layer,
                                   PH::HeroControl *hc,
                                   PlayerControlPtr player,
                                   int duration, float factor,
                                   float probality)
    {
        size_t index = getHeroIndexFromPlayer(player, hc);
        if(index == -1) return TaskIgnore::make();
        
        auto buff = Float2IntBuff::make(GemUtils::AllColor, duration, "反击", factor, probality, index);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn,
                                            "active.fightback", layer, buff);
    }
    
    TaskPtr ActiveSkill::ForceAreaAttack(cocos2d::CCLayer *layer,
                                         PH::HeroControl *hc,
                                         PlayerControlPtr player,
                                         int duration)
    {
        auto buff = IBuff::make(GemUtils::AllColor, duration, "范围");
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn,
                                            "active.force.area.attack", layer, buff);
    }
    
    TaskPtr ActiveSkill::BonusGemWeight(cocos2d::CCLayer *layer,
                                        PlayerControlPtr player,
                                        GemUtils::GemColor color,
                                        int duration, float factor)
    {
        auto buff = FloatBuff::make(color, duration, "幸运", factor);
        return player->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn,
                                            "active.bonus.gem.weight",
                                            layer, buff);
    }
}

