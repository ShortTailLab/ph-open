//
//  PlayerControl.h
//  HelloCpp
//
//  Created by Zeyang Li on 12/5/12.
//
//

#pragma once

#include "Board/Common.h"
#include "Board/AnimPreset.h"
#include "Board/Property.h"
#include "Board/HeroControl.h"
#include "Board/MoveTimer.h"
#include "CCScale9Sprite.h"
#include "HealthBox.h"
#include "EnemyControl.h"

namespace PH {   // tolua_export

    USING_NS_CC_EXT;
    
    struct Resistance
    {
        float val[GemUtils::Health];
        CCLabelBMFont* label[GemUtils::Health];
        
        Resistance()
        {
            memset(val, 0, sizeof(val));
            memset(label, 0, sizeof(label));
        }
    };
    
    // Control chained cooldown
    class SkillChain
    {
    private:
        HeroControlList chain;
        
    public:
        void startCooldown();
        void add(HeroControl* hc);
    };
    
    typedef std::map<int, SkillChain> SkillChainMap;
    
    struct ReflectBuff;
    typedef boost::shared_ptr<ReflectBuff> ReflectBuffPtr;
    
    class ReflectBuff : public IBuff
    {
    public:
        float factor, probality;
        int count, hc;
        
        static ReflectBuffPtr make(GemUtils::GemColor col,
                                   int turn,
                                   const std::string& text,
                                   float f, int c, float p, int h)
        {
            ReflectBuffPtr ret(new ReflectBuff);
            ret->init(col, turn, text);
            ret->factor = f;
            ret->count = c;
            ret->probality = p;
            ret->hc = h;
            return ret;
        }
    };
    struct FactorProb
    {
        float factor, probality;
    };
    
    struct PPShield
    {
        int comboCount,shieldCount;
        int lastRound, gapRound;
        float reflectFactor;
        PPShield():comboCount(99), shieldCount(0), lastRound(0), gapRound(0), reflectFactor(0){}
    };
    typedef PassiveProperty<HeroControl, PPShield> ShieldPassiveProperty;

    typedef PassiveProperty<HeroControl,std::pair<std::pair<float, float>, GemUtils::GemColor> >   FactorProbColorPassiveProperty;
    
    typedef Buff3<int, float, bool>             IntFloatBoolBuff;
    
    class BoardLayer;
    class EnemyControl;
    typedef boost::shared_ptr<EnemyControl> EnemyControlPtr;
    
    //---------------------------------
    // Player Control
    // PlayerControl inherits CCObject only to make it available to cocos2dx
    // scheduler. But it's managed my share ptr instead of the retain/release pattern.
    // This is just one other 2dx stupidity.
    //---------------------------------
    struct PlayerControl : public IObject, public CCObject {   // tolua_export
        
        static const size_t kGroup_PostTurn;
        static const size_t kGroup_PreTurn;
        static const size_t kGroup_NoneOfYouBusiness; // never update this
        static const float  kComboFactor;
        
        enum Style
        {
            PVP_RIVAL,
            PVP_SELF,
            PVE_PLAYER,
        };
        
        std::string uid;
        
        HeroControlList team;
        
        CCMenu* menu;
        
        int health;
        int maxHealth;
        
        Resistance resist;
        
        boost::shared_ptr<PlayerHealthBox> healthBox;
        
        MoveTimer       moveTimer;
        
        BuffGroup buffGroup;
        
        // passive properties
        FloatPassiveProperty            passiveAttackFactor[GemUtils::GemColorMax];
        Float3PassiveProperty           passiveRandAttackFactor[GemUtils::GemColorMax];
        FloatPassiveProperty            passiveDamageReduceFactor[GemUtils::GemColorMax];
        FloatPassiveProperty            passiveRegenFactor[GemUtils::GemColorMax];
        
        std::map<std::string, FloatPassiveProperty> passiveTitleAttackFactorMap;

        BoolFloatPassiveProperty        passiveFollowUpAttacks;
        BoolFloatPassiveProperty        passiveFollowUpAttackWithoutResistence;
        
        FloatPassiveProperty            passiveSurviveHitFactor;
        FloatPassiveProperty            passiveHealByFactor;
        FloatPassiveProperty            passiveDamageToHealthFactor;
        
        FloatFloatPassiveProperty       passiveAttackFactorOnHealth;
        FloatFloatPassiveProperty       passiveReduceDamageFactorOnHealth;
        FactorProbColorPassiveProperty  passiveReflectFactor;
        FloatFloatPassiveProperty       passiveAttackWithEnergyUp; //todo
    
        FloatPassiveProperty            passiveBleeding;
        // do not allow AllColor
        FloatFloatPassiveProperty       passiveEnergyBonusByGemCount[GemUtils::AllColor];
        FloatPassiveProperty            passiveForceAreaAttackAndFactor;
        
        FloatPassiveProperty            passiveBonusCoins;
        FloatPassiveProperty            passiveBonusExp;
        FloatPassiveProperty            passiveBonusLuck;
        IntPassiveProperty              passiveBonusEnemyStartTurn;
        
        FloatFloatPassiveProperty       passiveArmorByHealthRatio;
        IntPassiveProperty              passiveExtraWaitForEnemy;
        IntPassiveProperty              passiveBonusGemInAreaAttackCheck;
        
        FloatPassiveProperty            passiveInvincible;
        ShieldPassiveProperty           passiveShiled;
        
        IntPassiveProperty              passiveReduceOtherDragTime;
        
        SkillChainMap skillChain;
        
        float comboFactor;
        int minComboCount;
        int minGemCount;
    
        float mEnergyFactor[GemUtils::GemColorMax];
        float mMinEnergy[GemUtils::GemColorMax];

        int mTotalGemCount[GemUtils::AllColor];
        float mGemWeights[GemUtils::AllColor];
        
    private:
        boost::function<void()> onTickOut;
        
        // To animate effect particle system.
        CCPoint healEffectOrigin;
        CCPoint healEffectDst;
        CCParticleSystemQuad* healEffect;
        
        Style mStyle;
        CCPoint mSrcPos;
        void setHealthBoxPosition();
        
    public:
        PlayerControl(const CCPoint& p, Style style);
        virtual ~PlayerControl();
        
        TaskPtr createHealingAnim();
        
        TaskPtr onAttack(CCLayer * layer, float damage, float & leechFactor);
        TaskPtr onAfterSweep();
        
        //
        TaskPtr onHitCalculation(CCLayer * layer,
                                 GemUtils::GemColor atkColor,
                                 float prevDamage,
                                 float & damageFactor,
                                 float & reflectFactor,
                                 GemUtils::GemColor & reflectColor,
                                 boost::function<int()> randGen);
        TaskPtr hit(float damage, CCLayer * layer);
        TaskPtr heal(float val);
        
        // lua interface
        float getHealth() { return health; }          // tolua_export
        float getMaxHealth() { return maxHealth; }    // tolua_export

        void setHealthDirectly(float hp)
        {
            health = (int)hp;
            healthBox->resetHealth(hp, maxHealth);
        }
        
        // Reset max health.
        // This also sets current health to max health
        void setMaxHealthDirectly(float maxhp, bool resetHealth = false)
        {
            maxHealth = (int)maxhp;
            if(resetHealth)
                health = (int)maxhp;
            healthBox->resetHealth(health, maxHealth);
        }
        
        TaskPtr getAttackFactor(HeroControl* hc, float& atkFactor, boost::function<int()> randGen);
        TaskPtr getRegenFactor(HeroControl * hc, float& regenFactor);
        TaskPtr getExtraGemsFactor(float & extraGemsFactor);
        TaskPtr getComboFactor(float & comboFactor, bool our=false);
        TaskPtr dispellPassiveBuff();
        
        TaskPtr getResistanceByColor(GemUtils::GemColor color,
                                     float& val);
        
        CCLabelBMFont* createHealLabel(CCNode* parent);
        
        int totalRegen() ;
        
        CCPoint center() { return healthBox->center(); }
        
        bool isDead() { return health == 0; }
        bool isAlive() { return !isDead(); }
        bool isHurt() { return health < maxHealth; }
        float healthRatio() { return (float)health/(float)maxHealth; }
        
        TaskPtr shake();
        
        TaskPtr updateStatusTurn();
        TaskPtr updateStatusTurnPostEnemy();
        
        TaskPtr startTicking(int turn, boost::function<void()> tickCall);
        void stopTicking();
        void tick(float dt);
        
        TaskPtr getDamageReduceFactor(GemUtils::GemColor color, float& factor);
        
        virtual void vSerialize(rapidjson::Value& o, rapidjson::Document::AllocatorType& alloc) const;
        virtual void vDeserialize(rapidjson::Value& o);
    }; // tolua_export
    typedef boost::shared_ptr<PlayerControl> PlayerControlPtr;
} // tolua_export
