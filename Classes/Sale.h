#pragma once

#include "ContentLayer.h"
#include "UIComponent.h"
#include "ZqlScrollView.h"

namespace PH
{

class SaleBoard :public HeroBoard
{
public:

	virtual bool init(const std::vector<PH::Hero *>& heroVec, bool displayHolder);
	void updateHero();
	virtual void refresh(const std::vector<PH::Hero *>& heroVec);

	static SaleBoard* create(const std::vector<PH::Hero *>& heroVec, bool displayHolder)
	{
		SaleBoard* o = new SaleBoard();
		if(o != NULL && o->init(heroVec, displayHolder))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
};


class SaleJuanZhou : public JuanZhou
{
public:
	bool init();
	void setPrice(int price);
	void setNum(int num);

	CREATE_FUNC(SaleJuanZhou);

private:
	CCLabelBMFont* priceLabel;
	CCLabelBMFont* numLabel;
};

class SaleLayer : public ContentLayer
{
public:

	virtual bool init();
    virtual void onEnter();
    virtual void onEnterAnimationDone();
    virtual void cleanup();
    
	virtual void setTouchEnabled(bool value);
	void onConfirm(cocos2d::CCObject *target);
	void onBackClick(cocos2d::CCObject *target);
	void onTitleClick(cocos2d::CCObject *target);

	void addToSale(Hero *hero);
	void removeFromSale(Hero *hero);
	bool isOnSale(Hero *hero);
    void clear();
	void refreshBoard();
	void updateMsg();
    void initHeroNumTips();

	SaleJuanZhou *board;
	JuanZhouLayer *jz;
	SaleBoard *heroBoard;
	ZqlScrollView *scrollView;
	std::vector<Hero*> onSaleHeros;
	CCLabelTTF *title;
	CCMenu *topMenu;
    CCLabelTTF *numLabel;
    
    virtual void onLongClick(CCTouch* touch);
    virtual void onSingleClick(CCTouch* touch);

	static SaleLayer* create()
	{
		SaleLayer* o = new SaleLayer();
		if(o != NULL && o->init())
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}

private:
	int totalprice;
	int num;
    std::vector<Hero*> heroesOfCurrType;
    HeroBoardSwitcher* switcher;
};

}