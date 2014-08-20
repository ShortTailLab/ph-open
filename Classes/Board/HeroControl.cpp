//
//  HeroControl.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 12/6/12.
//
//

#include "HeroControl.h"
#include "CCScale9Sprite.h"
#include "Fonts.h"
#include "AnimPreset.h"
#include "LayoutUtils.h"
#include "extensions/utils.h"

namespace PH
{
    USING_NS_CC_EXT;
    
    class HeroStatsPopup : public CCSprite
    {
    public:
        static HeroStatsPopup* create(const Hero& h, bool isPvP)
        {
            HeroStatsPopup* o = new HeroStatsPopup();
            if(o && o->init(h, isPvP))
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }
        
        virtual bool init(const Hero& h, bool isPvP);
        
        
        CCLabelBMFont* energyNum;
    };
    
    bool HeroStatsPopup::init(const Hero& h, bool isPvP)
    {
        if(!CCSprite::initWithFile("ui/tishiban_zhandou.png"))
            return false;
        
        // title 30, text 24, number 20
        
        //------------------------------------
        // now set label according to bg size
        CCLabelTTF* name = CCLabelTTF::create(h.profile->nameCN.c_str(), FONT_CN, 30);
        name->setAnchorPoint(ccp(0.0f, 1.0f));
        name->setPosition(ccp(17, 235));
        name->enableShadow(ccp(2, -2), 1.0f, 0.0f);
        this->addChild(name);
        
        const SkillInfo& skl = isPvP ? AST(h.profile->pvpActiveSkillId) : AST(h.profile->activeSkillId);
        
        CCLabelBMFont* attackNum = CCLabelBMFont::create(toStr(h.attack).c_str(),
                                                         "bmfont/Sumo_yellow_22.fnt");
        attackNum->setAnchorPoint(ANCHOR_TOP_CENTER);
        attackNum->setPosition(ccp(236, 193));
        attackNum->setScale(contentScale());
        
        CCLabelBMFont* regenNum = CCLabelBMFont::create(toStr(h.regen).c_str(),
                                                        "bmfont/Sumo_yellow_22.fnt");
        regenNum->setScale(contentScale());
        regenNum->setAnchorPoint(ANCHOR_TOP_CENTER);
        regenNum->setPosition(ccp(236, 162));
        
        energyNum = CCLabelBMFont::create(toStr(skl.energy).c_str(),
                                          "bmfont/Sumo_yellow_22.fnt");
        energyNum->setScale(contentScale());
        energyNum->setAnchorPoint(ANCHOR_TOP_CENTER);
        energyNum->setPosition(ccp(148, 126));
        
        CCLabelBMFont* cdNum = CCLabelBMFont::create(toStr(skl.maxCooldown).c_str(),
                                                     "bmfont/Sumo_yellow_22.fnt");
        cdNum->setScale(contentScale());
        cdNum->setAnchorPoint(ANCHOR_TOP_CENTER);
        cdNum->setPosition(ccp(351, 126));
        
        CCLabelTTF* skillName = CCLabelTTF::create(skl.name.c_str(), FONT_CN, 24);
        skillName->setColor(ccc3(108, 46, 0));
        skillName->setAnchorPoint(ANCHOR_TOP_CENTER);
        skillName->setPosition(ccp(236, 93));
        
        CCLabelTTF* skillDesc = CCLabelTTF::create(skl.desc.c_str(), FONT_CN, 24,
                                                   CCSize(390, 392),
                                                   kCCTextAlignmentCenter,
                                                   kCCVerticalTextAlignmentTop);
        skillDesc->setAnchorPoint(ANCHOR_TOP_LEFT); // top left anchor
        skillDesc->setPosition(ccp(22, 65));
        skillDesc->setContentSize(ccp(0, 390));
        
        for(CCNode* n : VA(attackNum, regenNum, energyNum, cdNum, skillName, skillDesc, NULL))
            this->addChild(n);
        
        return true;
    }
    
    //----------------------
    // HeroIconItem
    //----------------------
    bool HeroIconItem::init(const PH::Hero& h, bool isPvP)
    {
        std::string path = h.profile->iconPath();
        if(!GetSprite(path))
            path = "heroes/placeholder_icon.png";
        
        if(!CCMenuItemSprite::initWithNormalSprite(GetSprite(path),
                                                   GetSprite(path),
                                                   NULL,
                                                   this,
                                                   menu_selector(HeroIconItem::click)))
        {
            return false;
        }

        enabledInTutorial = true;
        
        mClickCall = [](CCObject* o) { phassert(false && "!!! need setOnClick first"); };
        
        // hero icon frame
        mIconFrame = GetSprite(GemUtils::frame(h.profile->color));
        mIconFrame->setAnchorPoint(ccp(0, 0));
        mIconFrame->setPosition(ccp(-6, -6));
        this->addChild(mIconFrame);
        
        CCSpriteFrame* frame = GemUtils::energy(h.profile->color, 0);
        mEnergyIcon = CCSprite::createWithSpriteFrame(frame);
        
        mEnergyProgress = CCProgressTimer::create(mEnergyIcon);
        mEnergyProgress->setPosition(ccp(88, 19));
        mEnergyProgress->setAnchorPoint(ccp(0.5, 0.5));
        mEnergyProgress->setType(kCCProgressTimerTypeBar);
        mEnergyProgress->setBarChangeRate(ccp(0, 1));
        mEnergyProgress->setMidpoint(ccp(0, 1));
        mEnergyProgress->setPercentage(100);
        mIconFrame->addChild(mEnergyProgress);
        
        mEnergyIconFull = CCSprite::createWithSpriteFrame(GemUtils::energy(h.profile->color, 2));
        mEnergyIconFull->setPosition(ccp(88, 19));
        mEnergyIconFull->setAnchorPoint(ccp(0.5, 0.5));
        mEnergyIconFull->setOpacity(0);
        mIconFrame->addChild(mEnergyIconFull);
        
        mHeroDetail = HeroStatsPopup::create(h, isPvP);
        mHeroDetail->setVisible(false);
        
        mIconFrame->addChild(mHeroDetail);
        
        return true;
    }
    
    
    void HeroIconItem::selected()
    {
        CCPoint lp = mIconFrame->convertToNodeSpace(ccp(320, 780));
        mHeroDetail->setPosition(lp);
        mHeroDetail->setVisible(true);
        
        mHeroDetail->energyNum->setString((toStr((int)mHC->energy) + std::string("/") + toStr((int)mHC->maxEnergy)).c_str());
    }
    
    void HeroIconItem::unselected()
    {
        mHeroDetail->setVisible(false);
    }

    // ---------------------
    // Hero Control methods
    
    HeroControl::HeroControl(const Hero& h,
                             const CCPoint& p,
                             bool isPvP)
    :
    info(h),
    cooldown(0)
    {
        energy = 0;
        
        //maxEnergy = std::max(0, AST(info.profile->activeSkillId).energy + info.skillBonus);
        if(info.profile->activeSkillId == -1)
            maxEnergy = 0;
        else
            maxEnergy = AST(info.profile->activeSkillId).energy + info.skillBonus;
        
        if(info.profile->pvpActiveSkillId == -1)
            maxEnergyPvP = 0;
        else
            maxEnergyPvP = AST(info.profile->pvpActiveSkillId).energy + info.skillBonus;
        
        // elite level is precalculated
        attack = info.attack;// + info.profile->eliteAttack.eval(info.eliteLevel);
        health = info.health;// + info.profile->eliteHP.eval(info.eliteLevel);
        regen = info.regen;// + info.profile->eliteRegen.eval(info.eliteLevel);

        // root node
        icon = HeroIconItem::create(h, isPvP);
        icon->setPosition(p);
        iconPositionFixed = p;
        icon->setAnchorPoint(ccp(0, 0));
        icon->retain();
        
        CCSprite* iconBG = GetSprite("ui/1x1_b.png");
        iconBG->setScale(icon->getContentSize().width * contentScale());
        iconBG->setAnchorPoint(ccp(0.0f, 0.0f));
        iconBG->setOpacity(100);
        icon->addChild(iconBG, -1);
        
        // A hack, make hero control available to icon
        icon->setHeroControl(this);
        
        // Particle effects when energy reaches max
        readyEffect = CCParticleSystemQuad::create("particle/skill_ready.plist");
        readyEffect->setPosition(0.0f, 0.0f);
        readyEffect->setAnchorPoint(ccp(0.5, 0.5));
        readyEffect->stopSystem();
        icon->addChild(readyEffect, 1);
        

        // Cooldown Mask
        cooldownMask = CCProgressTimer::create(GetSprite("ui/CooldownMask.png"));
        cooldownMask->setPosition(ccp(1, 1));
        cooldownMask->setAnchorPoint(ccp(0, 0));
        cooldownMask->setReverseProgress(true);
        cooldownMask->setType(kCCProgressTimerTypeRadial);
        cooldownMask->setVisible(false);
        cooldownMask->setOpacity(200);
        icon->addChild(cooldownMask);
        
        cooldownText = CCLabelBMFont::create("", "bmfont/Molot_56_Cooldown.fnt");
        cooldownText->setAnchorPoint(ccp(0.5f, 0.5f));
        cooldownText->setPosition(ccp(50, 45));
        cooldownText->setScale(contentScale());
        cooldownText->setOpacity(215);
        icon->addChild(cooldownText, 1);
        
        this->buffGroup.setEvalFunction(
        [=](int i)
        {
            return ccp(this->icon->getPositionX()+15,
                       this->icon->getPositionY()+100);
        });
    }
    
    HeroControl::~HeroControl()
    {
        icon->removeFromParentAndCleanup(true);
        icon->release();
    }
    
    CCActionInterval* HeroControl::createEffectAnim()
    {
        float duration = 3.0f;

        std::vector<Vec2f> points;
        points.push_back(Vec2f(0.0f, 0.0f));
        points.push_back(Vec2f(0.0f, 95.0f));
        points.push_back(Vec2f(95.0f, 95.0f));
        points.push_back(Vec2f(95.0f, 0.0f));
        
        CCArray* circle = CCArray::createWithCapacity(points.size());
        
        for(int i=0; i<points.size(); i++)
        {
            Vec2f p = points[i];
            circle->addObject(CCMoveTo::create(duration / points.size(),
                                               ccp(p[0], p[1])));
        }
        
        return CCRepeatForever::create((CCActionInterval*)CCSequence::create(circle));
    }
    
    CCLabelBMFont* HeroControl::createAttackLabel()
    {
        CCLabelBMFont* label = CCLabelBMFont::create("0", GemUtils::numberFont(info.profile->color));
        label->setAnchorPoint(ccp(0.5, 0));
        label->setScale(contentScale());
        label->setPosition(ccp(50, 20));
        label->setOpacity(0);
        icon->addChild(label, 5);

        return label;
    }
    
    void HeroControl::setOnClick(boost::function<void(HeroIconItem*)> f)
    {
        icon->setOnClick(boost::move(f));
    }
    
    TaskPtr HeroControl::setEnergy(float e)
    {
        if(e == energy)
            return TaskIgnore::make();
        
        float prevEnergy = (int)energy;
        energy = std::max(0.0f, std::min(e, maxEnergy));
        
        // view changes.
        TaskSequencePtr seq =  TaskSequence::make();

        // only display energy text and progress when there is change
        if(energy - prevEnergy != 0.0f)
        {
            float fromPercent = 100 - prevEnergy/maxEnergy * 100.0f;
            float toPercent = 100 - energy/maxEnergy * 100.0f;
            CCProgressFromTo* progress = CCProgressFromTo::create(0.3,
                                                                  fromPercent,
                                                                  toPercent);
            
            seq << TaskAnim::make(icon->mEnergyProgress, progress, false);
            seq << createEnergyText(icon,
                                    energy - prevEnergy,
                                    ccp(70.0f, 0.0f));
        }

        if(prevEnergy != maxEnergy && isEnergyFull())
        {
            seq << TaskLambda::make([=]()
            {
                readyEffect->resetSystem();
                readyEffect->getActionManager()->removeAllActionsFromTarget(readyEffect);
                readyEffect->runAction(createEffectAnim());
            });
        }
        else if(!isEnergyFull())
        {
            seq << TaskLambda::make([=]
            {
                if(!isSkillReady())
                {
                    readyEffect->setPosition(0.0f, 0.0f);
                    readyEffect->stopAllActions();
                    readyEffect->stopSystem();
                }
            });
        }
        
        return seq;
    }
    
    TaskPtr HeroControl::updateStatusTurn()
    {
        return TaskBatch::make() << this->buffGroup.updateBuff(HeroControl::kGroup_onTurn);
    }
    
    TaskPtr HeroControl::updateCooldown(int delta)
    {
        const SkillInfo& s = AST(info.profile->activeSkillId);
        if(!s.maxCooldown || cooldown == 0 || delta <=0)
            return TaskIgnore::make();
     
        cooldown = std::max(0, cooldown-delta);
        
        TaskBatchPtr batch = TaskBatch::make();
        {
            float precent = cooldown / (float)s.maxCooldown * 100;
            CCProgressTo* progress = CCProgressTo::create(0.5, precent);
            batch << TaskAnim::make(cooldownMask, progress);
            
            
            TaskSequencePtr seq = TaskSequence::make();
            if(cooldown > 0)
            {
                seq << TaskLambda::make([=]()
                {
                    cooldownText->setString(toStr(cooldown).c_str());
                });
                seq << TaskAnim::make(cooldownText,
                                      CCScaleTo::create(0.3, 1.2*contentScale()));
                seq << TaskLambda::make([=]()
                {
                    cooldownText->setScale(contentScale());
                });
            }
            else
            {
                seq << TaskLambda::make([=]()
                {
                    cooldownText->setString("");
                    cooldownText->setVisible(false);
                });
            }
            batch << seq;
        }
        
        return batch;
    }
    
    void HeroControl::startCooldown()
    {
        const SkillInfo& s = AST(info.profile->activeSkillId);
        if(s.maxCooldown)
        {
            cooldown = s.maxCooldown;
            
            cooldownMask->setVisible(true);
            cooldownMask->setPercentage(99.999);
            
            cooldownText->setVisible(true);
            cooldownText->setString(toStr(s.maxCooldown).c_str());
        }
    }
    
    
    static void toJson(const BoolProperty& p,
                       rapidjson::Value& o,
                       rapidjson::Document::AllocatorType& alloc)
    {
        using namespace rapidjson;
        o.AddMember("turn", p.turnLeft, alloc);
        o.AddMember("value", p.value, alloc);
    }
    
    void HeroControl::toJson(rapidjson::Value& o,
                             rapidjson::Document::AllocatorType& alloc)
    {
        using namespace rapidjson;
        
        o.AddMember("energy", energy, alloc);
        o.AddMember("maxEnergy", maxEnergy, alloc);
        o.AddMember("attack", attack, alloc);
        o.AddMember("health", health, alloc);
        o.AddMember("regen", regen, alloc);
        o.AddMember("cooldown", cooldown, alloc);
    }
}