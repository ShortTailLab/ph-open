//
//  EnemyControl.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 12/4/12.
//
//

#include "EnemyControl.h"
#include "CCCallLambda.h"
#include "AnimPreset.h"
#include "Board.h"
#include "EnemySkill.h"
#include "GenericCallLua.h"
#include "AnimPreset.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

namespace PH
{
    
    const size_t EnemyControl::kGroup_PostTurn = 1;
    const size_t EnemyControl::kGroup_PreTurn = 2;
    const size_t EnemyControl::kGroup_NoneOfYouBusiness = 10086;
    
    EnemyControl::EnemyControl(EnemyInfo& inf, bool isBoss)
    :
    info(inf),
    turnLabel(NULL),
    sprite(NULL), reflectSprite(NULL), shieldSprite(NULL),
    mDealColor(inf.color)
    {
        health = info.maxHealth;
        ready = false;
         totalMove = 0;
        if(info.initialTurn == 0)
            readyInTurn = (int) randf(1, info.maxTurn);
        else
            readyInTurn = info.initialTurn;
        
        // root node
        root = CCSprite::create();
        root->retain();
        
        // character sprite
        sprite = GetSprite(info.bodyPath().c_str());
        sprite->setAnchorPoint(ccp(0.5, 0));
        this->recover();
        root->addChild(sprite);
        
        float offx = 0.0f;
        if(sprite->getContentSize().height > 270.0f)
            offx = -40.0f;
        
        // turn text
        turnText = GetSprite("ui/huihe.png");
        turnText->setPosition(ccp(20 + offx, 240));
        turnText->setAnchorPoint(ccp(1.0, 0));
        root->addChild(turnText);
        
        // turn label
        turnLabel = CCLabelBMFont::create(toStr(readyInTurn).c_str(),
                                          "bmfont/Molot_30_red.fnt");
        turnLabel->setPosition(ccp(35 + offx, 255));
        turnLabel->setAnchorPoint(ccp(0.5, 0.5));
        turnLabel->setScale(contentScale());
        root->addChild(turnLabel);
        
        // health label
        healthBox = boost::make_shared<HealthBox>(info.color, isBoss);
        root->addChild(healthBox->get());
        
        this->buffGroup.setEvalFunction([=](int i) -> CCPoint{
            return ccp(this->root->getPositionX() + 60, 880 - i*44);
        });
    }
    
    EnemyControl::~EnemyControl()
    {
        //printf("[destroy]%d\n", this->info.hid);
        healthBox.reset();

        if(this->reflectSprite)
        {
            if(this->reflectSprite->getParent())
                this->reflectSprite->removeFromParent();
            this->reflectSprite = NULL;
        }
        
        if(turnLabel->getParent())
            turnLabel->removeFromParentAndCleanup(true);
        
        if(turnText->getParent())
            turnText->removeFromParentAndCleanup(true);
        
        if(sprite->getParent())
            sprite->removeFromParentAndCleanup(true);
        
        if(root->getParent())
            root->removeFromParentAndCleanup(true);
        root->release();
    }
    
    TaskPtr EnemyControl::invokeTurnBasedSkill(PH::BoardLayer *layer)
    {
        CCLuaEngine* engine = CCLuaEngine::defaultEngine();
        auto seq = TaskSequence::make();
        std::string errorMessage = "";
        if(this->info.turnBasedScript.size())
        {
            
            EnemyControlPtr ec = shared_from_this();
            BeginEnemySkill(layer, ec, seq);
            if(engine->getLuaStack()->executeString(this->info.turnBasedScript.c_str(), errorMessage) != 0)
            {
                std::string error = this->info.nameCN + "(" + toStr(this->info.hid) + ")\n" + this->info.turnBasedScript.c_str() + ":\n" + errorMessage;
                SystemInform::alertMessage(runningScene(),
                                           error.c_str(),
                                           "Skill Error",
                                           [](){});
            }
            EndEnemySkill();
        }
        return seq;
    }
    
    TaskPtr EnemyControl::invokeActiveSkill(PH::BoardLayer *layer,
                                            int index,
                                            int &stillAttack)
    {
        auto seq = TaskSequence::make();
        if(this->info.script.size())
        {
            CCLuaEngine* engine = CCLuaEngine::defaultEngine();
            
            EnemyControlPtr ec = shared_from_this();
            BeginEnemySkill(layer, ec, seq);
            std::string errorMessage = "";
            if(engine->getLuaStack()->executeString(this->info.script.c_str(), errorMessage) == 0)
            {
                {
                    std::string errorMessage = "";
                    if(this->buffGroup.invokeBuff("silence", seq))
                    {
                        stillAttack = 1;
                    }
                    else
                        stillAttack = luacall("Run",
                                              boost::make_tuple(ec->totalMove, index),
                                              errorMessage);
                    
                    if(errorMessage.size())
                    {
                        std::string error = this->info.nameCN + "(" + toStr(this->info.hid) + ")\n" + this->info.script.c_str() + ":\n" + errorMessage;
                        SystemInform::alertMessage(runningScene(),
                                                   error.c_str(),
                                                   "Script Error",
                                                   [](){});
                    }
                }
            }
            else
            {
                if(errorMessage.size())
                {
                    std::string error = this->info.nameCN + "(" + toStr(this->info.hid) + ")\n" + this->info.script.c_str() + ":\n" + errorMessage;
                    SystemInform::alertMessage(runningScene(),
                                               error.c_str(),
                                               "Script Error",
                                               [](){});
                }
            }
            EndEnemySkill();
        }
        return seq;
    }
    
    TaskPtr EnemyControl::invokePassiveSkill(BoardLayer * layer)
    {
        CCLuaEngine* engine = CCLuaEngine::defaultEngine();
        auto seq = TaskSequence::make();
        std::string errorMessage = "";
        if(this->info.passiveScript.size())
        {
            
            EnemyControlPtr ec = shared_from_this();
            BeginEnemySkill(layer, ec, seq);
            if(engine->getLuaStack()->executeString(this->info.passiveScript.c_str(), errorMessage) != 0)
            {
                std::string error = this->info.nameCN + "(" + toStr(this->info.hid) + ")\n" + this->info.passiveScript.c_str() + ":\n" + errorMessage;
                SystemInform::alertMessage(runningScene(),
                                           error.c_str(),
                                           "Skill Error",
                                           [](){});
            }
            EndEnemySkill();
        }
        return seq;
    }
    
    void EnemyControl::recover()
    {
        // HACK: manually shift down high level heroes
        if(info.star < 5)
            sprite->setPositionY(20);
        else if(info.star >= 6)
            sprite->setPositionY(-60);
    }
    
    TaskPtr EnemyControl::onHit(int damage)
    {
        auto seq = TaskSequence::make();
        auto batch = TaskBatch::make();
        
        float t = 0.06f;
        CCPoint p = this->sprite->getPosition();
        auto shake = createShake(t, p, 3.0f, 4.0f);
        health = std::max(health - damage, 0);
        float ratio = healthRatio();
        
        // recover states
        seq << TaskLambda::make([=]()
        {
            this->sprite->stopAllActions();
            this->recover();
        });
        
        batch << TaskAnim::make(this->sprite, shake, false);
        batch << healthBox->scaleTo(ratio);
     
        seq << batch;
        
        if(this->isDead())
        {
            seq << this->dieAndFade();
        }
        return seq;
    }
    
    TaskPtr EnemyControl::getAttack(float &attack)
    {
        auto batch = TaskBatch::make();
        // enemy attack * skill modifier
        float skillMod = 1.f;
        
        if( this->buffGroup.invokeBuff("attackFactor", batch) )
        {
            skillMod *= this->buffGroup.getBuffContent<FloatBuff>("attackFactor")->d1;
        }
        
        attack = this->info.attack * skillMod;
        
        return batch;
    }
    
    TaskPtr EnemyControl::onAttack(CCLayer * layer, float damage)
    {
        auto batch = TaskBatch::make();
        
        float toHeal = 0;
        if(this->buffGroup.invokeBuff("leech", batch))
        {
            TaskBatchPtr localBatch = TaskBatch::make();
            toHeal += this->buffGroup.getBuffContent<FloatBuff>("leech")->d1;
            batch << createFloatWordText(layer, "吸血", this->center(), 0.f);
        }
        
        if(this->isHurt() && !this->isDead() && toHeal > 0.f)
            batch << heal(toHeal);
        
        return batch;
    }
    
    TaskPtr EnemyControl::onHitCalculation(BoardLayer * layer, GemUtils::GemColor atkColor,
                                           const float attack,
                                           float &reflectFactor, float & damageFactor,
                                           bool ignoreArmor)
    {
        if( attack <= 0.f ) return TaskIgnore::make();
        
        auto seq = TaskSequence::make();
        auto batch = TaskBatch::make();
        auto armorBatch = TaskBatch::make();
        
        float strongMod = GemUtils::modifier(atkColor, mDealColor);
        float armor = 0.f;
        float damage = attack*strongMod;
        
        if(!ignoreArmor){
            armor = this->info.armor;
            if(this->buffGroup.invokeBuff("armorFactor", armorBatch))
            {
                armor *= this->buffGroup.getBuffContent<FloatBuff>("armorFactor")->d1;
            }
        }
        
        damage = std::max(damage - armor, 1.0f);
        
        // amplify damages
        if(this->buffGroup.invokeBuffWhen("amplifyDamage", armorBatch, atkColor))
        {
            damage *= this->buffGroup.getBuffContent<FloatBuff>("amplifyDamage")->d1;
        }
        
        float realAtk = damage;
        
        seq << armorBatch;
        
        reflectFactor = 0.f;
        damageFactor = 1.f;
        
        if(this->buffGroup.invokeBuff("reflect", batch))
        {
            reflectFactor = this->buffGroup.getBuffContent<FloatBuff>("reflect")->d1;
            damage *= (1-reflectFactor);
        }
        else if( this->buffGroup.invokeBuff("active.reflect.by.gems", batch) )
        {
            
            int gemCount = 0;
            auto grid = layer->mBoardControl->getGrid();
            for(int i=0; i<grid.size(); i++)
            {
                if(typeid(*grid(i).get()) == typeid(ShieldGem))
                    gemCount++;
            }
            
            if(gemCount > 0)
            {
                damage = 0.f;
                reflectFactor = this->buffGroup.getBuffContent<FloatBuff>("active.reflect.by.gems")->d1*gemCount;
                if(this->reflectSprite){
                    auto pre = CCSpawn::create(
                                               CCScaleTo::create(0.3f, 3.f),
                                               CCFadeTo::create(0.3f, 255),
                                               NULL);
                    auto post= CCSpawn::create(
                                               CCScaleTo::create(0.3f, 1.0f),
                                               CCFadeTo::create(0.3f, 55),
                                               NULL
                                               );
                    CCFiniteTimeAction* anim_seq = CCSequence::create(pre, post, NULL);
                    batch << TaskAnim::make(this->reflectSprite, anim_seq, false);
                }
            }
            else
            {
                batch << this->unbindReflectShield();
            }
        }
        //else if(this->passiveReflect.has())
        else if( this->buffGroup.invokeBuffWhen("passiveReflect", batch, atkColor))
        {
            auto buff = this->buffGroup.getBuffContent<FLoat2BoolBuff>("passiveReflect");
            float p = (float)(rand() % 1000) / 1000.f;
            if( p < buff->d2 && buff->d1 > 0.f )
            {
                reflectFactor = buff->d1;
                damage *= buff->d3 ? 1.f : 0.f;
            }
        }
        //else if(this->passiveInjuryFree.has())
        else if( this->buffGroup.invokeBuffWhen("passiveInjuryFree", batch, atkColor))
        {
            auto buff = this->buffGroup.getBuffContent<Float2Buff>("passiveInjuryFree");
            float p = (float)(rand() % 1000) / 1000.f;
            if(p<buff->d2 && buff->d1 < 1.0f && buff->d1 >= 0.f)
            {
                damage *= buff->d1;
            }
        }
        
        float realDamageFactor = damage/realAtk;
        
        float delay = 0.f;
        if(reflectFactor > 0.f)
        {
            batch << createFloatWordText(layer, "反弹", this->center(), delay);
            delay += 0.2f;
        }else if(realDamageFactor < 1.f)
        {
            batch << createFloatWordText(layer, "闪避", this->center(), delay);
            delay += 0.2f;
        }
        
        seq << batch;
        
        auto localBatch = TaskBatch::make();
        if(this->buffGroup.invokeBuff("shield", localBatch))
        {
            auto shield = this->buffGroup.getBuffContent<Float2Buff>("shield");

            //damageFactor *= shield.factor;
            
            float shieldDamage = shield->d1 * damage;
            shieldDamage = std::min(shield->d2, shieldDamage);
            
            damage *= (1-shieldDamage/damage);
            
            shield->d2 -= shieldDamage;
            
            auto size = this->sprite->getContentSize();
            
            float t = 0.06f;
            CCPoint sp = this->shieldSprite->getPosition();
            auto shake = createShake(t, sp, 2.0f, 3.0f);
            localBatch << TaskAnim::make(this->shieldSprite, shake, false);
            localBatch << createFloatText(this->sprite, -shieldDamage,
                                          ccp(0, size.height/2), atkColor, 0.f);
            //localBatch << createFloatWordText(layer, "泄劲", this->center(), atkColor, delay);
            
            seq << localBatch;
            if(shield->d2 <= 0.f)
            {
                seq << this->buffGroup.updateBuff("shield");
                seq << TaskAnim::make(this->shieldSprite, CCFadeOut::create(0.3f));
            }
        }
        
        damageFactor = damage/attack;
         
        return seq;
    }
    
    TaskPtr EnemyControl::onAfterSweep(BoardLayer * layer,
                                       int count)
    {
        auto batch = TaskBatch::make();
        
//        if(this->passiveHound.has()){
        if( this->buffGroup.invokeBuff("passiveHound", batch))
        {
            auto buff = this->buffGroup.getBuffContent<FloatIntBuff>("passiveHound");
            if(count < buff->d2)
            {
                auto seq = TaskSequence::make();
                seq << createOrb(layer, this->center(), layer->getHealthCenter(), true);
                
                seq << createFloatText(layer,
                                       -buff->d1,
                                       ccp(320,540),
                                       GemUtils::Health,
                                       0.f)
                << TaskSound::make("sound/v/player_hit.mp3")
                << layer->mPlayer->hit(buff->d1, layer);
                batch << seq;
                batch << createFloatWordText(layer, "连击数不足"+toStr(buff->d1)+" 追杀",
                                             this->center(), 0.f);
            }
        }
        return batch;
    }
    
    TaskPtr EnemyControl::heal(int val)
    {
        float prevHealth = health;
        health = std::min(health + val, info.maxHealth);
        float ratio = healthRatio();
        
        float delta = health - prevHealth;

        TaskBatchPtr batch = TaskBatch::make();
        if(delta > 0.0f)
        {
            batch << TaskLambda::make([=]()
            {
                CCParticleSystemQuad* effect = CCParticleSystemQuad::create("particle/enemy_health_effect.plist");
                effect->setPosition(ccp(30, 8));
                healthBox->get()->addChild(effect);
                effect->runAction(CCSequence::create(CCMoveBy::create(0.75f,
                                                                      ccp(100.0f, 0)),
                                                     CCCallLambda::create([=]()
                                                                          {
                                                                              effect->removeFromParentAndCleanup(true);
                                                                          }),
                                                     NULL));
            });
        }
        
        batch << createFloatText(root,
                                 val,
                                 boundCenter(sprite->boundingBox()),
                                 GemUtils::Health,
                                 0.0f);

        batch << healthBox->scaleTo(ratio);
        
        return batch;
    }
    
    TaskPtr EnemyControl::fadeTo(float t, int val)
    {
        TaskBatchPtr batch = TaskBatch::make();
        batch << TaskAnim::make(turnLabel, CCFadeTo::create(t, val))
            << TaskAnim::make(sprite, CCFadeTo::create(t, val))
            << TaskAnim::make(turnText, CCFadeTo::create(t, val))
            << healthBox->fadeTo(t, val);
        return batch;
    }
    
    TaskPtr EnemyControl::dieAndFade()
    {
        TaskSequencePtr seq = TaskSequence::make();
        seq << fadeTo(0.5f, 0);
        
        EnemyControlPtr mself = shared_from_this();
        seq << TaskLambda::make([mself]()
        {
            // -------- [HACK] ---------
            // 'this' could be released here safely
            auto tmp = mself;
        });
         
        return seq;
    }
    
    TaskPtr EnemyControl::transformColorTo(GemUtils::GemColor toCol)
    {
        auto seq = TaskSequence::make();
        
        healthBox->setOpacity(255);
        seq << healthBox->fadeTo(0.5f, 0);
        seq << TaskLambda::make([=]() {
            healthBox->reset(toCol);
            root->addChild(healthBox->get());
        });
        seq << healthBox->fadeTo(0.5f, 255);
        
        mDealColor = toCol;
        
        return seq;
    }
    
    TaskPtr EnemyControl::setReadyInTurnWithAnim(int val)
    {
        readyInTurn = val;
        CCFiniteTimeAction* seq = CCSequence::create(CCCallLambda::create([=]()
        {
            turnLabel->setString(toStr(readyInTurn).c_str());
        }),
                                                     CCScaleTo::create(0.25f, 2.0f * contentScale()),
                                                     CCScaleTo::create(0.1f, 1.0f * contentScale()),
                                                     NULL);
        return TaskAnim::make(this->turnLabel, seq);
    }
    
    TaskPtr EnemyControl::updateReadyTurn(BoardLayer* layer, int turn)
    {
        auto seq = TaskSequence::make();
        auto batch = TaskBatch::make();
        
        // wrap around on turn 0
        int nextTurn = readyInTurn - 1;
        if(nextTurn <= 0)
        {
            ready = true;
            nextTurn = info.maxTurn;
            totalMove++;
        }
      
        return TaskSequence::make()
        << setReadyInTurnWithAnim(nextTurn)
        << this->invokeTurnBasedSkill(layer);
    }
    
    TaskPtr EnemyControl::updateStatusPreTurn(BoardLayer* layer)
    {
        auto batch = TaskBatch::make();
        if( this->buffGroup.isBuffLastTurn("tempColor") )
        {
            batch << this->transformColorTo(this->info.color);
        }
        
        batch << this->buffGroup.updateBuff(EnemyControl::kGroup_PreTurn);
        batch << updateReflectShield(layer);
        
        return batch;
    }
    
    TaskPtr EnemyControl::updateStatusPostTurn(BoardLayer* layer)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        batch << this->buffGroup.updateBuff(EnemyControl::kGroup_PostTurn);
        
        batch << updateReflectShield(layer);
        
        return batch;
    }
    
    TaskPtr EnemyControl::reborn()
    {
        auto seq = TaskSequence::make();
        if( this->buffGroup.invokeBuff("passiveReborn", seq) )
        {
            CCSprite* medium = GetSprite("reborn_1.png");
            auto size = this->root->getContentSize();
            medium->setAnchorPoint(ccp(0.5f, 0.5f));
            //CCPoint pos = g2w_center(this->position);
            medium->setPosition(ccp(this->center().x+25,this->center().y+50));
            medium->setOpacity(0);
            this->root->getParent()->addChild(medium);
            
            auto anim =
            CCAnimationCache::sharedAnimationCache()->animationByName("skill_reborn");
            
            auto spawn = CCSpawn::create(CCAnimate::create(anim),
                                         CCMoveBy::create(0.4f, ccp(0, 75)),
                                         CCFadeTo::create(0.2f, 255), NULL);
            seq
            //<< this->fadeTo(0.5f, 0)
            << TaskLambda::make([=]()
                                {
                                    medium->setOpacity(185);
                                })
            << TaskSound::make("sound/v/player_hit.mp3")
            << TaskAnim::make(medium, spawn)
            << TaskAnim::make(medium, CCFadeOut::create(0.3f))
            << heal(info.maxHealth*this->buffGroup.getBuffContent<FloatBuff>("passiveReborn")->d1)
            << this->fadeTo(0.3f, 255)
            << TaskLambda::make([=]()
                                {
                                    medium->removeFromParent();
                                })
            << this->buffGroup.updateBuff("passiveReborn");
        }
        return seq;
    }
    
    TaskPtr EnemyControl::updateReflectShield(BoardLayer* layer)
    {
        int gemCount = 0;
        auto grid = layer->mBoardControl->getGrid();
        for(int i=0; i<grid.size(); i++)
        {
            if(typeid(*grid(i).get()) == typeid(ShieldGem))
                gemCount++;
        }
        
        auto batch = TaskBatch::make();
        if(gemCount == 0)
        {
            batch << unbindReflectShield();
        }
        return batch;
    }
    
    TaskPtr EnemyControl::bindReflectShield()
    {
        if(this->injuryFreeDarts.has()) this->injuryFreeDarts.val() = true;
        
        if(!this->reflectSprite){
            this->reflectSprite = GetSprite("shield_large.png");
            auto size = this->sprite->getContentSize();
            this->reflectSprite->setPosition(ccp(size.width/2, size.height/2));
            this->reflectSprite->setOpacity(0);
        
            this->sprite->addChild(this->reflectSprite, 1000);
        }
        auto atomic_1 = CCSpawn::create(
                                        CCScaleTo::create(1.0f, 1.2f),
                                        CCFadeTo::create(1.0f, 55),
                                        NULL);
        auto atomic_2 = CCSpawn::create(
                                        CCScaleTo::create(1.0, 1.0f),
                                        CCFadeTo::create(1.0f, 155),
                                        NULL);
        auto atomic = CCSequence::create( atomic_1, atomic_2, NULL );
        
        auto animation = CCRepeatForever::create(atomic);
        
        auto spawn = CCSpawn::create(CCFadeTo::create(0.3f, 155),
                                     CCScaleTo::create(0.3f, 1.f), NULL);
        return TaskSequence::make()
        << TaskLambda::make([=]()
        {
            this->reflectSprite->setScale(4.0f);
            this->reflectSprite->setOpacity(0);
        })
        << TaskAnim::make(this->reflectSprite, spawn)
        << TaskAnim::make(this->reflectSprite, animation, false);
    }
    
    TaskPtr EnemyControl::unbindReflectShield()
    {
        if(this->reflectSprite == NULL ||
           !this->buffGroup.isBuffActive("active.reflect.by.gems")) return TaskIgnore::make();
        
        auto act = CCSpawn::create(CCFadeOut::create(0.3f),
                                   CCScaleTo::create(0.3f, 4.0f), NULL);
        return TaskSequence::make()
        << TaskLambda::make([=]()
        {
            this->reflectSprite->stopAllActions();
            this->reflectSprite->setScale(1.f);
            this->reflectSprite->setOpacity(155);
        })
        <<
        (TaskBatch::make()
            << TaskAnim::make(this->reflectSprite, act)
            << this->buffGroup.detachBuff("active.reflect.by.gems")
         );
    }

    TaskPtr EnemyControl::attachGemReflect(CCLayer* layer, float factor)
    {
        if(this->buffGroup.isBuffActive("active.reflect.by.gems")) return TaskIgnore::make();
        
        auto buff = FloatBuff::make(GemUtils::AllColor, 10, "反弹", factor);
        return TaskBatch::make()
        << this->bindReflectShield()
        << this->buffGroup.attachBuff(EnemyControl::kGroup_NoneOfYouBusiness,
                                      "active.reflect.by.gems", layer, buff);
    }
    
//    TaskPtr EnemyControl::attachDartsReflect(int count)
//    {
//        if(count ==0 || this->reflectGems.has()) return TaskIgnore::make();
//        
//        if(this->injuryFreeDarts.has())
//            this->injuryFreeDarts.set(this->injuryFreeDarts.turnCount()+count, this->injuryFreeDarts.val());
//        else this->injuryFreeDarts.set(count, true);
//        
//        this->isReflectStarted = true;
//        
//        return this->bindReflectShield();
//    }
    
    TaskPtr EnemyControl::attachShield(CCLayer * layer, float damage, float factor)
    {
        auto shield = Float2Buff::make(GemUtils::AllColor, 1, "泄劲", factor, damage);
        if(this->buffGroup.isBuffActive("shield"))
        {
            auto prevShield = this->buffGroup.getBuffContent<Float2Buff>("shield");
            shield->d1 = std::max(factor, prevShield->d1);
            shield->d2 += damage;
        }
        
        if(this->shieldSprite == NULL)
        {
            this->shieldSprite = GetSprite("broken_shield_1.png");
            this->sprite->addChild(this->shieldSprite, ORDER_STATUS_LABEL);
            
            auto size = this->sprite->getContentSize();
            this->shieldSprite->setPosition(ccp(0, size.height/2));
            this->shieldSprite->setOpacity(0);
        }
        
        auto seq = TaskSequence::make();
        seq << TaskLambda::make([=]()
        {
            this->shieldSprite->setOpacity(0);
        });
        seq << TaskAnim::make(this->shieldSprite, CCFadeTo::create(0.3f, 155), false);
        seq << this->buffGroup.attachBuff(EnemyControl::kGroup_NoneOfYouBusiness,
                                          "shield", layer, shield);
        return seq;
    }
    
}
