//
//  HeroSelectionLayer.h
//  HelloCpp
//
//  Created by Zeyang Li on 1/5/13.
//
//

#pragma once

#include "cocos2d.h"
#include "ContentLayer.h"
#include "UIComponent.h"
#include "ControlExtensions.h"

namespace PH
{
    USING_NS_CC;
    
    class HeroJuanZhou : public JuanZhou
    {
    public:
        virtual bool init(HeroPtr hero);
        void switchHero(HeroPtr hero);
        
        static HeroJuanZhou* create(HeroPtr hero)
        {
            HeroJuanZhou* o = new HeroJuanZhou();
            if(o && o->init(hero))
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return o;
        };
    private:
        Hero* hero;
        HeroIcon* icon;
        CCSprite* heroName;
        CCLabelTTF* pSkillLabel;
        LabelX* pSkillDetailLabel;
        CCLabelTTF* aSkillLabel;
        LabelX* aSkillDetailLabel;
        CCLabelBMFont* hpLabel;
        CCLabelBMFont* attackLabel;
        CCLabelBMFont* regenLabel;
        
    };
    
    class HeroSelectionLayer : public ContentLayer
    {
    public:
        virtual bool init();
        virtual void onEnterAnimationDone();
        
        void confirmHeroSelection(int heroId);
        void cancelHeroSelection(CCObject* o);
        
        void disableUI()
        {
            //heroSelectionMenu->setTouchEnabled(false);
            this->setTouchEnabled(false);
        }
        void enableUI()
        {
            //heroSelectionMenu->setTouchEnabled(true);
            this->setTouchEnabled(true);
        }
        
        static HeroSelectionLayer* create()
        {
            HeroSelectionLayer* o = new HeroSelectionLayer();
            if(o && o->init())
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return o;
        }
        
    private:
        CCLayer* confirmLayer;
        HeroJuanZhou* juanzhou;
        JuanZhouLayer* jz;
        int curId;
        CCLabelTTF* nameLabel;
        
        std::string getHeroName(int id);
    };
}
