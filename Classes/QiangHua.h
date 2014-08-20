#pragma once

#include "BattleScene.h"
#include "XiaKe.h"

namespace PH{

    class LabelX;


class QiangHua 
{
public:
	QiangHua(void);
	~QiangHua(void);
};

class QiangHuaJuanZhou : public JuanZhou
{
public:
	virtual bool init();
	void onTrainHeroSuccess(const Hero& prev, const Hero& curr);
    void playSuccessAnim(int neiliUp);
	int getMatsNum();

	static QiangHuaJuanZhou* create()
	{
		QiangHuaJuanZhou* o = new QiangHuaJuanZhou();
		if(o != NULL && o->init())
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
protected:
	virtual void updateData();

private:
    void updateUIFromHeroWithAnim(const Hero& prev, const Hero& curr);
	void updateUIFromHero(Hero *hero);
    int calElite();
    void updateOwnCoinsNum();
    
    void updateExpGain();
	Hero targetHero;
    CCSprite *sucBar;
    CCSprite* eliteMask;

    CCScale9Sprite *expBar;
    //CCLabelBMFont *levelTitle;
    
    CCLabelBMFont* levelLabel;
    CCLabelBMFont* hpLabel;
    CCLabelBMFont* attackLabel;
    CCLabelBMFont* regenLabel;  
    
    CCLabelBMFont* levelPlusLabel;
    CCLabelBMFont* hpPlusLabel;
    CCLabelBMFont* attackPlusLabel;
    CCLabelBMFont* regenPlusLabel;
    CCLabelTTF *skillLabel;

    //CCLabelTTF *expNowLabel;
    LabelX *expNeedLabel;
    LabelX *expGainLabel;
    LabelX* ownCoinsLabel;
    LabelX *coinsLabel;
    
    LabelX* eliteLabel;

};

class QiangHuaBoard : public HeroBoard
{
public:
	virtual bool init(const std::vector<PH::Hero *>& heroVec);
	virtual void refresh(const std::vector<PH::Hero *>& heroVec);
	virtual Hero *getHero(cocos2d::CCTouch *touch);

	static QiangHuaBoard* create(const std::vector<PH::Hero *>& heroVec)
	{
		QiangHuaBoard* o = new QiangHuaBoard();
		if(o != NULL && o->init(heroVec))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
};

class QiangHuaLayer : public BianDuiLayer
{
private:
    CCMenuItem* itemLeft;
    CCMenuItem* itemMiddle;
    CCMenuItem* itemRight;
    
public:
	virtual bool init();
    virtual void onEnter();
    virtual void onEnterAnimationDone();
    virtual void onExit();
    virtual void cleanup();
    
	virtual void initTargetHero(PH::Hero *hero);
	void onStartClick(cocos2d::CCObject *target);
	virtual void onBackClick(cocos2d::CCObject *target);
    void onTitleClick(cocos2d::CCObject *target);
	virtual void putHeroToJz(HeroIcon *icon);
    virtual void putBackHeroToBoard(Hero *hero);
    virtual void refreshBoard();
	void success();
    
    // HACK, use this to invalidate BianDuiLayer onTabClick
    virtual bool tryExit(int msg = -1)
    {
        return true;
    }

	static QiangHuaLayer* create()
	{
		QiangHuaLayer* o = new QiangHuaLayer();
		if(o != NULL && o->init())
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
    
private:
    cocos2d::CCSprite* heroNumBg;
    cocos2d::CCLabelTTF* numLabel;
	
};

}

