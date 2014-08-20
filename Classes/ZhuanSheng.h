//
//
//  Created by ZQL on 11.23
//
//

#pragma once


#include "ContentLayer.h"
#include "BattleScene.h"
#include "cocos2d.h"
#include "XiaKe.h"
#include "ButtonSprite.h"

namespace PH
{
    
class ZhuanShengBoard : public HeroBoard
{
public:

	virtual bool init();
	virtual void refresh(const std::vector<PH::Hero *>& heroVec);

	static ZhuanShengBoard* create()
	{
		ZhuanShengBoard* o = new ZhuanShengBoard();
		if(o != NULL && o->init())
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
};

class  ZhuanShengLayer : public ContentLayer
{
public:
	virtual bool init();
    virtual void onEnter();
    virtual void onExit();
	void onTitleClick(cocos2d::CCObject *target);
	void onBackClick(cocos2d::CCObject *target);

	ZhuanShengBoard *board;
	Player *player;
	CCLabelTTF *title;
    
    virtual void onLongClick(CCTouch* touch);
    virtual void onSingleClick(CCTouch* touch);
    
    void onPressedTimer(float dt);

	static ZhuanShengLayer* create()
	{
		ZhuanShengLayer* o = new ZhuanShengLayer();
		if(o != NULL && o->init())
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}

private:
    std::vector<Hero*> heroesOfCurrType;
    ZqlScrollView *scrollView;
    HeroBoardSwitcher* switcher;
};

class ZhuanShengJuanZhou : public JuanZhou
{
public:
	~ZhuanShengJuanZhou();

	bool init(Hero *hero);

	static ZhuanShengJuanZhou* create(Hero *hero)
	{
		ZhuanShengJuanZhou* o = new ZhuanShengJuanZhou();
		if(o != NULL && o->init(hero))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}

private:
    CCLabelBMFont* hpLabel;
	CCLabelBMFont* attackLabel;
	CCLabelBMFont* regenLabel;
	CCLabelBMFont* levelLabel;
	CCLabelBMFont* lpLabel;
	CCLabelTTF *skillLabel;

	Hero *tempHero;
};

class TransBoard : public ContentLayer
{
public:
	virtual bool init(Hero *hero);
	void transSuccess();

	void success();
    void afterSuccess();
    Hero* getHero(CCTouch *touch);

	std::vector<cocos2d::CCPoint> locations;
	std::vector<float> angles;
	std::vector<HeroIcon*> heroIcons;
	Hero *targetHero;
    std::string targetId;
	HeroIcon *transHeroIcon;
    

	static TransBoard* create(Hero *hero)
	{
		TransBoard* o = new TransBoard();
		if(o != NULL && o->init(hero))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
    
private:
    Hero heroCopy;
};
    
class SourceBoard : public ContentLayer
{
public:
    virtual bool init();
    void update(const std::vector<std::string>& sources);
    
    static SourceBoard* create()
    {
        SourceBoard* o = new SourceBoard();
        if(o != NULL && o->init())
        {
            o->autorelease();
            return o;
        }
        CC_SAFE_DELETE(o);
        return NULL;
    }
    
private:
    CCScale9Sprite* bg;
    CCLabelTTF* content;
};

class ZhuanShengConfirmLayer : public ContentLayer
{
public:
    ~ZhuanShengConfirmLayer();
    
	virtual bool init(Hero *hero);
    virtual void setTouchEnabled(bool value);
	void onBackClick(cocos2d::CCObject *target);
	void onConfirmClick(cocos2d::CCObject *target);
    virtual void onEnterAnimationDone();
    void onPressTimer(float dt);
    void onTranSuc(std::string targetId);
    
    virtual void onSingleClick(CCTouch* touch);
    virtual void onLongClick(CCTouch* touch);
    
    Hero *heroOnTouch;
    CCMenu *topMenu;
	JuanZhouLayer *jz;
	TransBoard *transBoard;
	bool isTrans;

	static ZhuanShengConfirmLayer* create(Hero *hero)
	{
		ZhuanShengConfirmLayer* o = new ZhuanShengConfirmLayer();
		if(o != NULL && o->init(hero))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}

private:
    Hero* hero;
    Hero* transHero;
    bool isShowNormal;
    CCLabelTTF* pSkillLabel;
    CCLabelTTF* pSkillDetailLabel;
    CCLabelTTF* aSkillLabel;
    CCLabelTTF* aSkillDetailLabel;
    CCLabelTTF* aSkillCoolDown;
    CCLabelTTF* aSkillEnergy;
    ButtonSprite* normalSkillBtn;
    ButtonSprite* pvpSkillBtn;
    std::vector<HeroIcon*> mats;
    std::vector<HeroIcon*> heroIcons;
    SourceBoard* matSourceBoard;
    ContentLayer* container;
    
    ContentLayer* createOrgHeroBoard(Hero* hero);
    ContentLayer* createMatsBoard(Hero* hero);
    ContentLayer* createTargetBoard(Hero* hero);
    void addLabel(ContentLayer* layer, Hero* hero);
    void updateSkillType(Hero* hero);
};

}