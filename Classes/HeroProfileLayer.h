//
//
//  Created by ZQL on 11.29
//
//

#pragma once

#include "ContentLayer.h"
#include "BattleScene.h"
#include "cocos2d.h"
#include "XiaKe.h"
#include "ControlExtensions.h"
#include "ButtonSprite.h"
#include "PageScrollView.h"

namespace PH
{
    
class HeroRelationItem : public cocos2d::CCSprite
{
public:
    HeroRelation relation;
    
    virtual bool init(const HeroRelation& relation);
    void onPress(bool value);
    
    static HeroRelationItem* create(const HeroRelation& relation)
	{
		HeroRelationItem* o = new HeroRelationItem();
		if(o != NULL && o->init(relation))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
    
private:
    cocos2d::CCSprite* item2;
    
};

class ProfileJuanZhou : public JuanZhou
{
public:
	virtual bool init( Hero *hero );
	void switchHero(Hero *hero);
	void switchHeroImg(Hero *hero);
	virtual void addHero(PH::Hero *hero , int id );
    void initHeroRelations(const Hero* hero);
    void updateRelationBoard(const HeroRelation* relation);
    void schedule2Sec();
    
	virtual void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);

	static ProfileJuanZhou* create(Hero *hero)
	{
		ProfileJuanZhou* o = new ProfileJuanZhou();
		if(o != NULL && o->init(hero))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}

private:
	Hero *targetHero;
    CCSprite* bg2;
	cocos2d::CCSprite *heroIcon;
    cocos2d::CCLayer *relations;
    cocos2d::CCSprite *relationBoard;
    cocos2d::CCLabelTTF *rName;
    cocos2d::CCLabelTTF *rIsReady;
    cocos2d::CCLayer *rHeroesLayer;
    cocos2d::CCLabelTTF *rEffect;
    std::vector<HeroRelation> relats;
    std::vector<HeroRelationItem*> relationItems;
    HeroRelationItem * tempRItem;

	cocos2d::CCLabelBMFont *lpLabel;
	cocos2d::CCLabelBMFont *hpLabel;
	cocos2d::CCLabelBMFont *regenLabel;
	cocos2d::CCLabelBMFont *attackLabel;
	cocos2d::CCLabelBMFont *levelLabel;
    
    cocos2d::CCLabelBMFont *hpPlusLabel;
	cocos2d::CCLabelBMFont *regenPlusLabel;
	cocos2d::CCLabelBMFont *attackPlusLabel;
    CCSprite* heroType;

	cocos2d::CCLabelTTF *aSkillLabel;
	cocos2d::CCLabelTTF *aSkillDetailLabel;
	cocos2d::CCLabelTTF *pSkillLabel;
	cocos2d::CCLabelTTF *pSkillDetailLabel;
    cocos2d::CCLabelTTF *aSkillEnergy;
    cocos2d::CCLabelTTF *aSkillCoolDown;
    
    cocos2d::CCLabelTTF* sourceLabel;
    cocos2d::CCLabelTTF* functionLabel;
    
	cocos2d::CCLabelBMFont *expLabel;
	cocos2d::extension::CCScale9Sprite *expBar;
    ButtonSprite* normalSkillBtn;
    ButtonSprite* pvpSkillBtn;
    ContentLayer* labels;
    
    bool isShowNormal;
    bool isHero;
    bool scheduleFlag;
    
    void updateSkillType(Hero* hero);
    CCSprite* getTypeImg(std::string type);
};

class HeroProfileLayer : public ContentLayer
{
public:
    ~HeroProfileLayer()
    {
        //sv->release();
    }

    PageScrollView* sv;
    
	bool init(Hero *hero , bool isShowStorage  , bool isShowSelect);
	bool init(const HeroProfile *heroProfile);
    
    virtual void onExit();
    virtual void onEnterAnimationDone();
    
	void showStar(const HeroProfile* profile);
    void setSelectCall(boost::function<void ()> call);
    void disableLock(bool value);

	static HeroProfileLayer* create(Hero *hero , bool isShowStorage = true ,bool isShowSelect = false)
	{
		HeroProfileLayer* o = new HeroProfileLayer();
		if(o != NULL && o->init(hero , isShowStorage , isShowSelect))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
    
	static HeroProfileLayer* create(const HeroProfile *profile)
	{
		HeroProfileLayer* o = new HeroProfileLayer();
		if(o != NULL && o->init(profile))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}

private:
    bool initCommon(const HeroProfile *heroProfile , bool isShowStorage = true , bool isShowSelect = false);
    void initScrollViewWithHeroes(const std::vector<Hero*>&, int current);
    void initHeroStasScroll(ProfileJuanZhou* statsView);
    void initScrollArrow();

	Hero *targetHero;
	ProfileJuanZhou *juanzhou;
	JuanZhouLayer *jz;
	CCMenuItemSprite *itemLeft;
	CCMenuItemSprite *itemRight;
	LabelX *heroName;
    CCLayer *star;
	std::vector<CCSprite*> heroBodys;
    CCMenu *topMenu;
    std::auto_ptr<Hero> fakeHero;
    CCSprite* titleImg;
    CCSprite *arrow1;
    CCSprite *arrow2;
    CCLabelTTF *numLabel;
    CCLabelTTF* fightValueLabel;
    boost::function<void ()> selectCall;
    bool isShowStorage;
    bool canLock;

	void lock(bool value);
	void onBackClick(cocos2d::CCObject *target);
	void onLockClick(cocos2d::CCObject *target);
    void onSelect(cocos2d::CCObject *target);
    CCSprite* getTitleImg(std::string title);
};

}
