#pragma once

#include "ContentLayer.h"
#include "BattleScene.h"
#include "cocos2d.h"
#include "BuildConf.h"
#include <boost/function.hpp>
#include "ControlExtensions.h"
#include "UIComponent.h"
#include "ButtonSprite.h"

class ZqlScrollView;

namespace PH{

enum VIPJuanZhouType
{
    Normal = 1,
    With_Btn_Charge,
    With_Btn_Detail
};

class VIPJuanZhou : public JuanZhou
{
public:
    virtual bool init(const VIPJuanZhouType& type, bool isPush);
    void update();
    //the isPush is used to controll the btn onclickaction.
    static VIPJuanZhou* create(const VIPJuanZhouType& type = VIPJuanZhouType::Normal, bool isPush = true)
	{
		VIPJuanZhou* o = new VIPJuanZhou();
		if(o != NULL && o->init(type, isPush))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
private:
    CCScale9Sprite* spentBar;
    CCLabelBMFont* spentLabel;
    CCSprite* vipIcon;
    CCLabelTTF* text;
    VIPJuanZhouType type;
    int vipLevel;
    int spent;
};

class GoodItem : public ContentLayer
{
public:
	bool init(StoreItem item , std::string label, std::string bgPath , std::string goodPath , std::string btnPath);
	void buyAnima();
	virtual void setTouchEnabled(bool value);
	virtual void onClick(cocos2d::CCObject *target);

	StoreItem targetItem;
	std::string _goodPath;
	LabelX *priceLabel;

	static GoodItem* create(StoreItem item , std::string label, std::string bgPath , std::string goodPath , std::string btnPath )
	{
		GoodItem* o = new GoodItem();
		if(o != NULL && o->init(item ,label , bgPath , goodPath , btnPath ))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}

private:
	CCMenu *menu;

};

class ChongZhiLayer;
class SystemInform;

class GoldItem : public GoodItem
{
public:
	virtual bool init(const StoreItem& item);
	void buyAnima();
	void blink(cocos2d::CCSprite *target);
	virtual void onClick(cocos2d::CCObject *target);
    void handlePayOurs(std::string serial);

    ChongZhiLayer *czLayer;

	static GoldItem* create(StoreItem item)
	{
		GoldItem* o = new GoldItem();
		if(o != NULL && o->init(item ))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
};
    
class MonthlyItem : public GoldItem
{
public:
    virtual bool init(const StoreItem& item);
    
    static MonthlyItem* create(const StoreItem& item)
	{
		MonthlyItem* o = new MonthlyItem();
		if(o != NULL && o->init(item ))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
};

class VIPLayer : public ContentLayer
{
public:
    virtual bool init(bool isPushToCharge);
    virtual void onEnter();
    virtual void onEnterAnimationDone();

    static VIPLayer* create(bool isPushToCharge = true)
	{
		VIPLayer* o = new VIPLayer();
		if(o != NULL && o->init(isPushToCharge))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}

private:
    VIPJuanZhou* juanzhou;
    JuanZhouLayer* jz;
    ZqlScrollView* scrollView;

    void onBack();
    void makeBoards();
    ContentLayer* makeItem(ContentLayer* parent, const std::string& type,
                           int value1, std::string value2 = "");
    ContentLayer* makeVIPBoard(int level, bool isSelf, bool isDarkPack);
};

class SpiritLayer : public ContentLayer
{
public:
    bool init();
    void onBack();

    CREATE_FUNC(SpiritLayer);
};

class XiweiLayer : public ContentLayer
{
public:
    bool init();
    void onBack();

    CREATE_FUNC(XiweiLayer);
};

class VIPPacksLayer : public ContentLayer
{
public:
    bool init();
    void onBack();

    CREATE_FUNC(VIPPacksLayer);
private:
    ZqlScrollView* scrollView;

    void makeItems();
    ContentLayer* createPackItem(const VIPPack& pack);
};

class StoreLayer : public ContentLayer
{
public:
    static std::string Channel_APPStore;
public:
	virtual bool init(bool hasBackBtn);
    virtual void onEnter();
    virtual void onEnterAnimationDone();
	virtual void setTouchEnabled(bool value);
	void onChongZhiClick();
    void onVIPClick();
	ContentLayer* makeItems();

	static StoreLayer* create(bool hasBackBtn = false)
	{
		StoreLayer* o = new StoreLayer();
		if(o != NULL && o->init(hasBackBtn))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}

    static StoreLayer* createWithoutBtn()
    {
        return create(false);
    }

private:
    ButtonSprite* chargeBtn;
    CCRect chongzhiRect;
	ZqlScrollView *scrollView;
	std::vector<GoodItem*> items;
    ContentLayer *container;
    bool isClick;
    VIPJuanZhou* juanzhou;
    JuanZhouLayer* jz;
    CCSprite* tips;


};

class ChongZhiLayer : public ContentLayer
{
public:
    static bool AppStoreProductIdsHasInit;
public:
    VIPJuanZhou* juanzhou;

	virtual bool init(bool inbattle, bool isPushToVIP);
    virtual void onEnter();
    virtual void setTouchEnabled(bool value);
    virtual void onEnterAnimationDone();
	void makeItems();
	void onBackClick(cocos2d::CCObject *target);

	ZqlScrollView *scrollView;
    CCMenu *topMenu;
    std::vector<ContentLayer*> goldItems;

    boost::function<void()> inBattleReturnCall;

	static ChongZhiLayer* create(bool inbattle = false, bool isPushToVIP = true)
	{
		ChongZhiLayer* o = new ChongZhiLayer();
		if(o != NULL && o->init(inbattle, isPushToVIP))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}

private:
    ContentLayer *container;
    JuanZhouLayer* jz;
    bool inBattle;

};
    extern unsigned long long mmLastPurchaseTime;
}
