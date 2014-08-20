//
//  HeroControl.h
//  HelloCpp
//
//  Created by Zeyang Li on 12/6/12.
//
//

#pragma once

#include "cocos2d.h"

#include "BuffGroup.h"
#include "Board/Property.h"
#include "Board/StatusLabel.h"
#include "Board/HeroTable.h"
#include "Board/Utils.h"
#include "SystemInform.h"
#include <boost/move/move.hpp>
#include <vector>

USING_NS_CC;

namespace PH { // tolua_export
    
    class HeroControl;
    class HeroStatsPopup;
    
    class HeroIconItem : public CCMenuItemSprite
    {
    private:
        boost::function<void(HeroIconItem*)> mClickCall;
        CCSprite* mIconFrame;
        CCSprite* mEnergyIcon;
        CCSprite* mEnergyIconFull;
        CCProgressTimer* mEnergyProgress;
        
        HeroStatsPopup* mHeroDetail;
        
        HeroControl* mHC;

    public:

        // ungodly hack
        bool enabledInTutorial;
        
        static HeroIconItem* create(const Hero& hero, bool isPvP)
        {
            HeroIconItem* o = new HeroIconItem();
            if(o && o->init(hero, isPvP))
            {
                o->autorelease();
                return o;
            }
            
            CC_SAFE_DELETE(o);
            return NULL;
        }
        
        virtual bool init(const Hero& h, bool isPvP);
        
        virtual void selected();
        virtual void unselected();

        void click(CCObject* o)
        {
            if(enabledInTutorial)
                mClickCall(this);
        }
        
        void setHeroControl(HeroControl* hc)
        {
            mHC = hc;
        }
        
        HeroControl* getHeroControl()
        {
            return mHC;
        }
        
        void setOnClick(boost::function<void(HeroIconItem*)> f)
        {
            mClickCall = boost::move(f);
        }
        
        friend class HeroControl;
    };
    
    struct HeroControl {  // tolua_export
        
        static const size_t kGroup_onTurn = 1;
        
        const Hero info;

        float energy;
        float maxEnergy;
        float maxEnergyPvP;
        
        float attack;
        float health;
        float regen;
        
        int cooldown;

        //BoolProperty blind;
        BuffGroup buffGroup;
        
        CCPoint iconPositionFixed;
        
        HeroIconItem* icon;
        CCParticleSystemQuad* readyEffect;

        CCProgressTimer* cooldownMask;
        CCLabelBMFont* cooldownText;
        
        HeroControl(const Hero& h,
                    const CCPoint& p,
                    bool isPvP);
        ~HeroControl();
        
        CCActionInterval* createEffectAnim();
        CCLabelBMFont* createAttackLabel();
        
        CCPoint center() const
        {
            return boundCenter(icon->boundingBox());
        }
        
        TaskPtr addEnergy(float e)
        {
            if(isSkillReady() && e >= 0.0f)
                return TaskIgnore::make();
            return setEnergy(e + energy);
        }
        
        TaskPtr setEnergy(float e);

        TaskPtr updateStatusTurn();
        TaskPtr updateCooldown(int delta = 1);
        
        void setOnClick(boost::function<void(HeroIconItem*)> f);
        
        // lua exports
        int getHeroId() { return info.heroId; } // tolua_export
        PH::GemUtils::GemColor getColor() { return info.profile->color; } // tolua_export
        int getEnergy() { return energy; } // tolua_export
        
        bool isSkillReady()
        {
            return energy == maxEnergy && maxEnergy > 0 && cooldown == 0;
        }
        
        bool isEnergyFull()
        {
            return energy == maxEnergy && maxEnergy > 0;
        }
        
        void startCooldown();
        
        void toJson(rapidjson::Value& o,
                    rapidjson::Document::AllocatorType& alloc);
        
    };  // tolua_export
    
    typedef std::vector<HeroControl*> HeroControlList;
    
} // tolua_export