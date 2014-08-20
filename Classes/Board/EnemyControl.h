//
//  EnemyControl.h
//  HelloCpp
//
//  Created by Zeyang Li on 12/4/12.
//
//

#pragma once

#include <utility>
#include <limits.h>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "HeroTable.h"
#include "Property.h"
#include "AnimPreset.h"
#include "Utils.h"
#include "CCLuaEngine.h"
#include "HealthBox.h"

#include "BuffGroup.h"

namespace PH {  // tolua_export
    class BoardLayer;
    
    class EnemyControl;
    typedef boost::shared_ptr<EnemyControl> EnemyControlPtr;
    typedef boost::weak_ptr<EnemyControl> EnemyControlWeakPtr;

    class EnemyControl : public boost::enable_shared_from_this<EnemyControl>, public PH::IObject { //tolua_export

    public:
        static const size_t kGroup_PostTurn;
        static const size_t kGroup_PreTurn;
        static const size_t kGroup_NoneOfYouBusiness; // never update this

    public:
        const EnemyInfo     info;           //
        bool                ready;          // #
        int                 totalMove;      // #

    private:
        int                 health;         // #
        int                 readyInTurn;    // #
        GemUtils::GemColor  mDealColor;     // #

    protected:
        CCSprite *          shieldSprite;   // in

    public:
        // turn based properties
        BuffGroup           buffGroup;      // #
    
        // the life of this buff is determinated by the gems with reflect flag;
        //FloatProperty       reflectGems;
        IntProperty         injuryFreeDarts;
        CCSprite *          reflectSprite;

        CCSprite* root;
        boost::shared_ptr<HealthBox> healthBox;
        CCSprite* turnText;
        CCLabelBMFont* turnLabel;
        CCSprite* sprite;

        EnemyControl(EnemyInfo& inf, bool isBoss = false);
        virtual ~EnemyControl();
        
        INITIALIZE_OBJECT();
        virtual void vSerialize(std::ostringstream& out) const{};
        virtual void vDeserialize(std::istringstream& in){};
        
        virtual void vSerialize(rapidjson::Value& val, rapidjson::Document::AllocatorType& doc) const {}
        virtual void vDeserialize(rapidjson::Value& val) {}

        TaskPtr invokeActiveSkill(BoardLayer * layer, int index, int & stillAttack);
        TaskPtr invokePassiveSkill(BoardLayer * layer);
        TaskPtr invokeTurnBasedSkill(BoardLayer * layer);

        TaskPtr onAttack(CCLayer * layer, float damage);
        TaskPtr onAfterSweep(BoardLayer * layer,
                             int count);
        
        TaskPtr onHitCalculation(BoardLayer * layer, GemUtils::GemColor atkColor,
                                 const float attack,
                                 float & reflectFactor, float & damageFactor,
                                 bool ignoreArmor);
        TaskPtr onHit(int damage);
        
        // Enemy takes damage and returns relevant animation task.
        // Death animation is included if the damage kills the enemy, and
        // it also removes the root sprite from its parent.
        TaskPtr heal(int val);
        TaskPtr fadeTo(float t, int val);
        TaskPtr dieAndFade();

        TaskPtr transformColorTo(GemUtils::GemColor toCol);

        // lua exports
        float getHealth() { return health; }                // tolua_export
        float getMaxHealth() { return info.maxHealth; }     // tolua_export
        int   getHeroId() { return info.hid; }              // tolua_export
        PH::GemUtils::GemColor  getColor() { return info.color; } // tolua_export
        CCSprite* getSprite() { return sprite; }             // tolua_export

        int   getReadyInTurn() { return readyInTurn; }

        bool isDead() { return health == 0; }
        bool isHurt() { return health < info.maxHealth; }

        CCPoint center()
        {
            return root->convertToWorldSpace(boundCenter(sprite->boundingBox()));
        }

        CCPoint getPosition()
        {
            return root->getPosition();
        }

        CCPoint getAttackPoint()
        {
            CCPoint p = root->getPosition();
            p.y += 100;
            return p;
        }

        CCPoint topLeft()
        {
            return root->convertToWorldSpace(boundTopLeft(sprite->boundingBox()));
        }

        float healthRatio()
        {
            return health / (float)info.maxHealth;
        }

        void setOpacity(int val)
        {
            healthBox->setOpacity(val);
            turnLabel->setOpacity(val);
            turnText->setOpacity(val);
            sprite->setOpacity(val);
        }
        
        void    recover();
        
        TaskPtr setReadyInTurnWithAnim(int val);
        TaskPtr getAttack(float & attack);
        TaskPtr updateReadyTurn(BoardLayer* layer, int turn);
        
        TaskPtr updateStatusPostTurn(BoardLayer* layer);
        TaskPtr updateStatusPreTurn(BoardLayer* layer);
        
        TaskPtr updateReflectShield(BoardLayer* layer);

        TaskPtr attachGemReflect(CCLayer* layer, float factor);
//        TaskPtr attachDartsReflect(int count);
        
        TaskPtr bindReflectShield();
        TaskPtr unbindReflectShield();
        
        TaskPtr attachShield(CCLayer * layer, float damage, float factor);
        
        TaskPtr reborn();
    }; // tolua_export

    typedef std::vector<EnemyControlPtr> EnemyControlList;
} // tolua_export
