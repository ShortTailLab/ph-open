//
//  PlayerControl.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 12/5/12.
//
//

#include "Board/Board.h"
#include "Board/PlayerControl.h"
#include "Board/EnemyControl.h"
#include "Board/AnimPreset.h"

#include "CCCallLambda.h"
#include <extensions/utils.h>
#include <boost/functional.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

namespace PH
{
    //-------------
    // Skill Chain
    //-------------
    void SkillChain::startCooldown()
    {
        for(HeroControl* hc : chain)
            hc->startCooldown();
    }
    
    void SkillChain::add(HeroControl *hc)
    {
        chain.push_back(hc);
    }
    
    const size_t PlayerControl::kGroup_PostTurn = 1;
    const size_t PlayerControl::kGroup_PreTurn = 2;
    const size_t PlayerControl::kGroup_NoneOfYouBusiness = 10086;
    
    const float  PlayerControl::kComboFactor = 0.25f;
    
    //-------------------
    // Player Control
    //-------------------
    PlayerControl::PlayerControl(const CCPoint& p, Style style)
    :
    minComboCount(0),
    minGemCount(0),
    comboFactor(PlayerControl::kComboFactor)
    {
        mStyle = style;
        mSrcPos = p;
        
        PlayerHealthBox::Style boxStyle = PlayerHealthBox::ORANGE;
        if(style == PVP_RIVAL)
            boxStyle = PlayerHealthBox::ORANGE;
        else if(style == PVP_SELF)
            boxStyle = PlayerHealthBox::PINK;
        else if(style == PVE_PLAYER)
            boxStyle = PlayerHealthBox::HEART;
        
        healthBox = boost::make_shared<PlayerHealthBox>(1.0f,
                                                        boxStyle);
        
        setHealthBoxPosition();

        healEffect = CCParticleSystemQuad::create("particle/health_effect.plist");
        healEffectOrigin = ccp(50, 13);
        healEffectDst = ccp(610, 13);
        healEffect->setPosition(healEffectOrigin);
        healEffect->stopSystem();
        healthBox->get()->addChild(healEffect);
        
        menu = CCMenu::create();
        menu->setAnchorPoint(ccp(0, 0));
        menu->setPosition(0, 0);
        menu->retain();
        
        for(int i=0; i<GemUtils::GemColorMax; i++)
        {
            mEnergyFactor[i] = 1.0f;
            mMinEnergy[i] = 0.0f;
        }
        
        for(int i=0; i<GemUtils::AllColor; i++)
        {
            mGemWeights[i] = 1.0f;
            mTotalGemCount[i] = 0;
        }
        
        this->buffGroup.setEvalFunction([=](int i) -> CCPoint
                                        {
                                            return ccp(this->healthBox->getPosition().x + 75 + i*50,
                                                       this->healthBox->getPosition().y + 10);
                                        });
    }
    
    PlayerControl::~PlayerControl()
    {
        menu->removeFromParentAndCleanup(true);
        menu->release();
    }
    
    void PlayerControl::vSerialize(rapidjson::Value& o, rapidjson::Document::AllocatorType& alloc) const
    {
        rapidjson::Value heroArr(rapidjson::kArrayType);
        for(HeroControl* hc : team)
        {
            rapidjson::Value hcVal(rapidjson::kObjectType);
            hc->toJson(hcVal, alloc);
            heroArr.PushBack(hcVal, alloc);
        }
        o.AddMember("team", heroArr, alloc);
        o.AddMember("comboFactor", comboFactor, alloc);
    }
    
    void PlayerControl::vDeserialize(rapidjson::Value& o)
    {
        assert(false && "not implemented");
    }
    
    void PlayerControl::setHealthBoxPosition()
    {
        if(mStyle == PVP_RIVAL)
            healthBox->setPosition(ccp(mSrcPos.x-108, mSrcPos.y-35));
        else if(mStyle == PVP_SELF)
            healthBox->setPosition(ccp(mSrcPos.x, mSrcPos.y+105));
        else if(mStyle == PVE_PLAYER)
            healthBox->setPosition(ccp(mSrcPos.x, mSrcPos.y-30));
    }
    
    TaskPtr PlayerControl::createHealingAnim()
    {
        // heal fx
        TaskSequencePtr effectSeq = TaskSequence::make();
        effectSeq << TaskLambda::make([=]()
                                      {
                                          healEffect->resetSystem();
                                          healEffect->setPosition(healEffectOrigin);
                                      });
        effectSeq << TaskAnim::make(healEffect,
                                    CCMoveTo::create(1.0, healEffectDst));
        effectSeq << TaskLambda::make([=]()
                                      {
                                          healEffect->stopSystem();
                                      });
        return effectSeq;
    }
    
    TaskPtr PlayerControl::onAttack(CCLayer * layer,
                                    float damage,
                                    float & leechFactor)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        leechFactor = 0.f;
        // Handle passive skill DamageToHealth only when player is hurt.
        if(this->passiveDamageToHealthFactor.isValid())
        {
            leechFactor = sumPP(this->passiveDamageToHealthFactor);
        }
        
        //if(this->leech.has())
        if( this->buffGroup.invokeBuff("leech", batch) )
        {
            leechFactor += this->buffGroup.getBuffContent<FloatBuff>("leech")->d1;
        }
        
        return batch;
    }
    
    TaskPtr PlayerControl::onHitCalculation(CCLayer * layer,
                                            GemUtils::GemColor atkColor,
                                            float damage,
                                            float & damageFactor,
                                            float & reflectFactor,
                                            GemUtils::GemColor & reflectColor,
                                            boost::function<int ()> randGen)
    {
        auto batch = TaskBatch::make();
        
        const float prev = damage;
        if( this->buffGroup.invokeBuffWhen("amplifyDamage", batch, atkColor) )
        {
            damage *= this->buffGroup.getBuffContent<FloatBuff>("amplifyDamage")->d1;
        }
        
        damageFactor = damage/prev;
        reflectFactor = 0.f;
        
        // reflect damage
        HeroControl * hc = NULL;
        
        //if(this->reflect.has())
        if( this->buffGroup.invokeBuffWhen("reflect", batch,
            [=](BuffPtr b)
            {
                auto buff = boost::dynamic_pointer_cast<ReflectBuff>(b);
                float p = (float)(randGen() % 1000) / 1000.f;
                return p < buff->probality;
            }) )
        {
            auto buff = this->buffGroup.getBuffContent<ReflectBuff>("reflect");
            hc = this->team[buff->hc];
            reflectFactor = buff->factor;
            damageFactor = 0.f;
            reflectColor = hc->getColor();
            
            if(--buff->count <= 0)
            {
                batch << this->buffGroup.detachBuff("reflect");
            }
        }
        else if(this->passiveReflectFactor.isValid())
        {
            float pro = 0.f, rf = 0.f;
            for(auto pair : this->passiveReflectFactor.map())
            {
                hc = pair.first;
                
                rf = std::max(pair.second.first.first, reflectFactor);
                pro += pair.second.first.second;
            }
            
            float p = (float)(randGen() % 1000) / 1000.f;
            if( p < pro )
            {
                damageFactor = 0.f;
                reflectColor = hc->getColor();
                reflectFactor = rf;
            }
            
        }
        else if( this->buffGroup.invokeBuffWhen("active.fightback", batch, [=](BuffPtr b)
        {
            auto buff = boost::dynamic_pointer_cast<Float2IntBuff>(b);
            float p = (float)(randGen() % 1000) / 1000.f;
            return p < buff->d2;
        }) )
        {
            auto buff = this->buffGroup.getBuffContent<Float2IntBuff>("active.fightback");
            hc = this->team[buff->d3];
            reflectFactor = std::max(buff->d1, reflectFactor);
            reflectColor = hc->getColor();
        }
        
        {
            auto batch = TaskBatch::make();
            if( this->buffGroup.invokeBuff("passive.shield.invoke", batch) )
            {
                auto buff = this->buffGroup.getBuffContent<FloatIntBuff>("passive.shield.invoke");
                
                damageFactor = 0.f;
                reflectFactor = std::max(buff->d1, reflectFactor);
                reflectColor = buff->getColor();
                
                buff->d2 --;
                if(buff->d2 <= 0)
                {
                    batch << this->buffGroup.detachBuff("passive.shield.invoke");
                }
                
                batch << createFloatWordText(layer, "抵挡",
                                             this->center(), 0.2f);
                return batch;
            }
        }
        
        
        float delay = 0.f;
        if(reflectFactor > 0.f)
        {
            //
            batch << createFloatWordText(layer, "反击",
                                         this->center(),delay);
            delay += 0.2f;
        }
        
        if( this->passiveArmorByHealthRatio.isValid() )
        {
            for(auto pair : this->passiveArmorByHealthRatio.map() )
            {
                if(pair.second.second <= this->getHealth()/this->getMaxHealth())
                {
                    damageFactor *= pair.second.first;
                }
            }
        }
        
        //if(this->absorbDamage.has())
        if( this->buffGroup.invokeBuff("absorbDamage", batch) )
        {
            auto buff = this->buffGroup.getBuffContent<Int2Buff>("absorbDamage");
            //auto hc = this->team[buff->d1];
            TaskSequencePtr localSeq = TaskSequence::make();
            localSeq << healWithAnim(layer, this, damage);

            if( (--buff->d2) <= 0)
            {
                localSeq << this->buffGroup.detachBuff("absorbDamage");
            }
            damageFactor = 0.f;
            batch << localSeq;
            batch << createFloatWordText(layer, "吸收",
                                         this->center(),
                                         delay);
            delay += 0.2f;
            return batch;
        }

        bool invincible = false;
        HeroControl* targetHC = NULL;
        if ( this->buffGroup.invokeBuffWhen("invincible", batch, [=](BuffPtr b)
        {
            auto buff = boost::dynamic_pointer_cast<FloatIntBuff>(b);
            float p = (float)(randGen() % 1000) / 1000.f;
            if (p >= buff->d1) return false;
            if( atkColor != b->getColor() &&
               b->getColor() != GemUtils::AllColor) return false;
            return true;
        }))
        {
            auto buff = this->buffGroup.getBuffContent<FloatIntBuff>("invincible");
            invincible = true;
            targetHC = this->team[buff->d2];
        }
        
        if( invincible )
        {
            damageFactor = 0.f;
            batch << createFloatWordText(layer, "免伤",
                                         this->center(),
                                         delay);
            delay += 0.2f;
            return batch;
        }
        
        if( this->passiveInvincible.isValid() && !invincible)
        {
            float tP = 0.f;
            for(auto iter : this->passiveInvincible.map() )
            {
                tP += iter.second;
                targetHC = iter.first;
            }
            float p = (float)(randGen() % 1000) / 1000.f;
            if (p < tP) invincible = true;
            
            
        }
        
        if( invincible )
        {
            damageFactor = 0.f;
            batch << createFloatWordText(layer, "闪避",
                                         this->center(),
                                         delay);
            delay += 0.2f;
            return batch;
        }
        
        //if(this->shield.has() && this->shield.val().second > 0 &&
        //   (this->shield.color == ec->info.color ||
        //    this->shield.color == GemUtils::AllColor))
        if( this->buffGroup.invokeBuffWhen("shield", batch, atkColor) )
        {
            auto buff = this->buffGroup.getBuffContent<FloatIntBuff>("shield");
            //auto hc = this->team[buff->d2];
            float shieldLeft = buff->d1;
            if(shieldLeft >= damage)
            {
                shieldLeft -= damage;
                damageFactor = 0.f;
                //damageFactor *= (shieldLeft/prevDamage);
            }
            else
            {
                batch << this->buffGroup.detachBuff("shield");
                damageFactor *= 1-(shieldLeft/damage);
                shieldLeft = 0.0f;
                //damageFactor = 0.f;
            }
            buff->d1 = shieldLeft;
            
            batch << createFloatWordText(layer, "防御",
                                         this->center(),
                                         delay);
            delay += 0.2f;
        }
        
        float damageReduceFactor = 1.0f;
        batch << this->getDamageReduceFactor(atkColor, damageReduceFactor);
        damageFactor *= damageReduceFactor;
    
        return batch;
    }
    
    TaskPtr PlayerControl::onAfterSweep()
    {
        return TaskIgnore::make();
    }
    
    TaskPtr PlayerControl::hit(float damage, CCLayer * layer)
    {
        if(damage == 0.f) return TaskIgnore::make();
        
        float prevHealth = this->getHealth();
        
        health = std::max(0.0f, health - damage);
        TaskBatchPtr batch = TaskBatch::make();

        batch << healthBox->setHealthWithAnim(health);
        batch << shake();
        
        auto seq = TaskSequence::make();
        seq << batch;
        
        if(this->isDead() && layer != NULL)
        {
            // see if we get a second chance
            if(this->passiveSurviveHitFactor.isValid())
            {
                float factor = minPP( this->passiveSurviveHitFactor );
                if((prevHealth / this->getMaxHealth()) > factor)
                {
                    auto local = TaskBatch::make();
                    local << healWithAnim(layer, this, 1.0f, true);
                    
                    auto caster = this->passiveSurviveHitFactor.casters();
                    if(caster.size() > 0)
                    {
                        local << createFloatWordText(layer, "复活",
                                                    this->center(),
                                                    0.f);
                    }
                    
                    seq << local;
                }
            }
            
            if( this->isDead() )
            {
                auto local = TaskBatch::make();
                if( this->buffGroup.invokeBuff("active.zombine", local) )
                {
                    //auto hc = this->team[this->buffGroup.getBuffContent<IntBuff>("active.zombine")->d1];
                    local << healWithAnim(layer, this, 1.0f, true);
                    local << createFloatWordText(layer, "复活",
                                                 this->center(),
                                                 0.f);
                    local << this->buffGroup.updateBuff("active.zombine");
                    
                    seq << local;
                }
            }
        }
    
        return seq;
    }
    
    TaskPtr PlayerControl::shake()
    {
        CCPoint src = healthBox->getPosition();
        CCFiniteTimeAction* shake = createShake(0.05f,
                                                src,
                                                2.5f,
                                                5.0f);
        
        TaskPtr clear = TaskLambda::make([=]()
        {
            this->setHealthBoxPosition();
            CCDirector::sharedDirector()->getActionManager()->removeAllActionsFromTarget(healthBox->get());
        });

        TaskSequencePtr seq = TaskSequence::make();
        seq << clear << TaskAnim::make(healthBox->get(), shake, false);
        return seq;
    }
    
    TaskPtr PlayerControl::heal(float val)
    {
        health = std::min((float)maxHealth, (float)health+val);
        return healthBox->setHealthWithAnim(health);
    }
    
    CCLabelBMFont* PlayerControl::createHealLabel(CCNode* parent)
    {
        CCLabelBMFont* label = CCLabelBMFont::create("", GemUtils::numberFont(GemUtils::Health));
//        label->setPosition(healthBox->center());
        label->setPosition(parent->convertToNodeSpace(healthBox->center()));
//        label->setVisible(false);
        parent->addChild(label, ORDER_FLOAT);
        
        return label;
    }
    
    int PlayerControl::totalRegen()
    {
        int regen = 0;
        for(HeroControl * hc : this->team){
            float totalRegenFactor = 0.f;
            this->getRegenFactor(hc, totalRegenFactor);
            regen += hc->regen * totalRegenFactor;
        }
        return regen;
    }
    
    TaskPtr PlayerControl::getAttackFactor(HeroControl* hc,
                                           float& atkFactor,
                                           boost::function<int()> randGen)
    {
        atkFactor = 1.0f;
        
        TaskBatchPtr batch = TaskBatch::make();
        
        //if(attackFactor.has())
        if( this->buffGroup.invokeBuffWhen("attackFactor", batch, hc->getColor()) )
        {
            atkFactor *= this->buffGroup.getBuffContent<FloatBuff>("attackFactor")->d1;
        }
        
        if(passiveAttackFactor[hc->getColor()].isValid())
            atkFactor *= productPP( passiveAttackFactor[hc->getColor()] );
        
        if(passiveAttackFactor[GemUtils::AllColor].isValid())
            atkFactor *= productPP( passiveAttackFactor[GemUtils::AllColor] );
        
        if(passiveTitleAttackFactorMap.count(hc->info.profile->title))
            atkFactor *= productPP( passiveTitleAttackFactorMap[hc->info.profile->title] );
        
        if( passiveRandAttackFactor[GemUtils::AllColor].isValid() )
        {
            for(auto x : passiveRandAttackFactor[GemUtils::AllColor].map() )
            {
                float p = (float)(randGen() % 1000) / 1000.f;
                if( p < x.second.second )
                    atkFactor *= x.second.first.second;
                else
                    atkFactor *= x.second.first.first;
            }
        }
        
        if( passiveRandAttackFactor[hc->getColor()].isValid() )
        {
            for( auto x : passiveRandAttackFactor[hc->getColor()].map() )
            {
                float p = (float)(randGen() % 1000) / 1000.f;
                if( p < x.second.second )
                    atkFactor *= x.second.first.second;
                else
                    atkFactor *= x.second.first.first;
            }
        }
        
        
        if(passiveAttackFactorOnHealth.isValid())
        {
            const std::map<HeroControl*, std::pair<float, float>>& factors = passiveAttackFactorOnHealth.map();
            float healthRatio = getHealth() / getMaxHealth();
            for(auto x : factors)
            {
                if(healthRatio < x.second.first)
                    atkFactor *= x.second.second;
                if(x.second.first == 0 && this->getHealth() == 1)
                    atkFactor *= x.second.second;
            }
        }
        
        // apply critical attack
        //if (criticalAttack.has()) {
        if( this->buffGroup.invokeBuffWhen("criticalAttack", batch,
            [=](BuffPtr b)
            {
                auto buff = boost::dynamic_pointer_cast<Float2Buff>(b);
                float p = (float)(randGen() % 1000) / 1000.f;
                return p < buff->d2;
            }) )
        {
            auto buff = this->buffGroup.getBuffContent<Float2Buff>("criticalAttack");
            atkFactor *= buff->d1;
        }
        
        if( hc->buffGroup.invokeBuff("active.reduce.attack.factor", batch) )
        {
            atkFactor *=
                hc->buffGroup.getBuffContent<FloatBuff>("active.reduce.attack.factor")->d1;
        }
        
        return batch;
    }
    
    TaskPtr PlayerControl::getResistanceByColor(GemUtils::GemColor color,
                                                float& val)
    {
        val = this->resist.val[color];
        
        auto batch = TaskBatch::make();
        if( this->buffGroup.invokeBuffWhen("active.reduce.resistance", batch, color) )
        {
            val -= this->buffGroup.getBuffContent<FloatBuff>("active.reduce.resistance")->d1;
        }
        
        val = std::min(val, 1.0f);
        
        return batch;
    }
    
    TaskPtr PlayerControl::getRegenFactor(PH::HeroControl *hc,
                                          float &regenFactor)
    {
        regenFactor = 1.f;
        TaskBatchPtr batch = TaskBatch::make();
        
        if(this->passiveRegenFactor[hc->getColor()].isValid())
            regenFactor *= productPP( this->passiveRegenFactor[hc->getColor()] );
        if(this->passiveRegenFactor[GemUtils::AllColor].isValid())
            regenFactor *= productPP( this->passiveRegenFactor[GemUtils::AllColor] );
        
        return batch;
    }
    
    TaskPtr PlayerControl::getExtraGemsFactor(float & ret)
    {
        TaskBatchPtr batch = TaskBatch::make();
        ret = this->comboFactor;
        
        /*
        if(this->bonusComboFactor.has())
        {
            batch << createPropertyAnim(this->bonusComboFactor.getLabel());
            ret += this->bonusComboFactor.val();
        }
        */
        
        return batch;
    }
    
    TaskPtr PlayerControl::getComboFactor(float & ret, bool our)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        if(!our)
            ret = this->comboFactor;
        
        //if(this->bonusComboFactor.has())
        if( this->buffGroup.invokeBuff("bonusComboFactor", batch) )
        {
            ret *= this->buffGroup.getBuffContent<FloatBuff>("bonusComboFactor")->d1;
        }
        
        return batch;
    }
    
    TaskPtr PlayerControl::dispellPassiveBuff()
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        batch << this->buffGroup.detachBuff("reduceHealingFactor");
        batch << this->buffGroup.detachBuff("poison");
        batch << this->buffGroup.detachBuff("slowFactor");
        batch << this->buffGroup.detachBuff("active.slow.val");
        batch << this->buffGroup.detachBuff("active.reduce.resistance");
        batch << this->buffGroup.detachBuff("active.reduce.attack.factor");
        
        return batch;
    }
    
    TaskPtr PlayerControl::getDamageReduceFactor(GemUtils::GemColor color, float& factor)
    {
        float passiveFactor = 1.0f;
        float activeFactor = 1.0f;
        
        TaskBatchPtr batch = TaskBatch::make();
        
        
        //if(damageReduceFactor.has())
        if( this->buffGroup.invokeBuffWhen("damageReduceFactor", batch, color) )
        {
            activeFactor *= this->buffGroup.getBuffContent<FloatBuff>("damageReduceFactor")->d1;
        }
        
        if(passiveDamageReduceFactor[color].isValid())
            passiveFactor *= productPP(passiveDamageReduceFactor[color]);
        
        if(passiveDamageReduceFactor[GemUtils::AllColor].isValid())
            passiveFactor *= productPP(passiveDamageReduceFactor[GemUtils::AllColor]);
        
        if(passiveReduceDamageFactorOnHealth.isValid())
        {
            auto& factors = passiveReduceDamageFactorOnHealth.map();
            float healthRatio = this->getHealth() / this->getMaxHealth();
            for(auto x : factors)
            {
                if(healthRatio < x.second.first)
                    passiveFactor *= x.second.second;
            }
        }
        
        factor = activeFactor * passiveFactor;
        return batch;
    }
    
    TaskPtr PlayerControl::updateStatusTurn()
    {
        // update property labels
        
        auto batch = TaskBatch::make();
        if( this->buffGroup.isBuffLastTurn("active.reduce.resistance") )
        {
            auto buff = this->buffGroup.getBuffContent<FloatBuff>("active.reduce.resistance");
            batch << TaskLambda::make([=]()
            {
                std::vector<GemUtils::GemColor> colVec;
                
                if( buff->getColor() != GemUtils::AllColor ) colVec.push_back(buff->getColor());
                else
                {
                    for(int i=GemUtils::Fire; i<=GemUtils::Light; i++)
                    {
                        colVec.push_back(static_cast<GemUtils::GemColor>(i));
                    }
                }
                
                for(auto col : colVec)
                {
                    int mid = (int)(this->resist.val[col]*100) + 100;
                    this->resist.label[col]->setString(toStr(mid).c_str());
                }
            });
        }
        batch << this->buffGroup.updateBuff(PlayerControl::kGroup_PreTurn);
        return batch;
    }
    
    TaskPtr PlayerControl::updateStatusTurnPostEnemy()
    {
        auto batch = TaskBatch::make();
        if( this->buffGroup.isBuffLastTurn("bonusMaxHealth") )
        {
            float m = this->buffGroup.getBuffContent<FloatBuff>("bonusMaxHealth")->d1;
            float h = m*this->getHealth()/this->getMaxHealth();
            this->healthBox->resetHealth(h, m);
            this->setMaxHealthDirectly((int)m);
            this->setHealthDirectly((int)h);
            
            batch   << this->createHealingAnim()
                    << this->healthBox->setHealthWithAnim(h, m);
        }
        batch << this->buffGroup.updateBuff(PlayerControl::kGroup_PostTurn);
        return batch;
    }
    
    TaskPtr PlayerControl::startTicking(int turn, boost::function<void()> tickCall)
    {
        onTickOut = tickCall;
        // so ulgy
        CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(PlayerControl::tick), this, 0.25f, false);
        
        //if(disableSweep.has())
        if( this->buffGroup.isBuffActive("disableSweep") )
            moveTimer.time = this->buffGroup.getBuffContent<FloatBuff>("disableSweep")->d1;
        else
            moveTimer.time = moveTimer.maxTime;
        
        auto batch = TaskBatch::make();
        
        if( this->buffGroup.invokeBuff("slowFactor", batch) )
            moveTimer.time *= this->buffGroup.getBuffContent<FloatBuff>("slowFactor")->d1;
        
        if( this->buffGroup.invokeBuff("active.slow.val", batch) )
            moveTimer.time -= this->buffGroup.getBuffContent<FloatBuff>("active.slow.val")->d1;
        
        return batch;
    }
    
    void PlayerControl::stopTicking()
    {
        CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(PlayerControl::tick), this);
        moveTimer.hide();
    }
    
    void PlayerControl::tick(float dt)
    {
        if(moveTimer.time <= 0.0f)
        {
            onTickOut();
            return;
        }
        
        moveTimer.time -= dt;
        
        //printf("%.2f %.2f\n", mMoveTimer.time, mMoveTimer.threshold);
        
        if(moveTimer.time < moveTimer.threshold)
        {
            if(!moveTimer.visible)
            {
                moveTimer.show();
            }
            
            float scale = moveTimer.time > moveTimer.threshold ? 1.0f : moveTimer.time / moveTimer.threshold;
            moveTimer.fill->setScaleX(scale);
        }
    }
}
