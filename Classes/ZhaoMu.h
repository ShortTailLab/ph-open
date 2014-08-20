#pragma once

#include "ContentLayer.h"
#include "cocos2d.h"
#include "Board/HeroTable.h"
#include "UIComponent.h"
#include "ZqlScrollView.h"
#include "ButtonSprite.h"

namespace PH
{
    enum InformType{
        NormalDraw = 0,
        NiceDraw,
        Hire10Draw,
        DanYaoDraw,
        ToolDraw
    };
    enum ZhaoMuItemType{
        JueDing = 0,
        JueDing10,
        DanYao10,
        Tool10
    };
    
    
class AlertMenu : public JuanZhouLayer
{
public:
    virtual void update(float dt);
    virtual void onEnterAnimationDone();
    
    CREATE_FUNC(AlertMenu);
};
    
class HeroCard : public cocos2d::CCSprite
{
public:
	virtual bool init(const char* cardPath ,Hero *hero);

	void hideHero(bool value);
	void showStar(bool withAnima = true);
	void showDes();
	virtual void update(float dt);
	void onAnimaDone(cocos2d::CCNode *target , void *light);

	Hero *hero;
	CCSprite *light;
    CCSprite* front;
	bool isShowHero;
	int count;

	static HeroCard* create(const char* cardPath , Hero *hero)
	{
		HeroCard* o = new HeroCard();
		if(o != NULL && o->init(cardPath , hero))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
    
private:
    std::vector<CCSprite*> stars;
};
    
    
inline void runPickCardAnima(CCLayer* par,CCSprite* card,Hero *hero,boost::function<void ()> afterCall);
    
class PickNorHeroLayer : public ContentLayer
{
public:
    cocos2d::CCSprite* card;
    HeroPtr targetHero;
    JuanZhouLayer* alertMenu;
    
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void pickACard(cocos2d::CCSprite *card);
    virtual void makeMenu();
    void deleteMenu();
    void onGoOn();
    void onGiveUp();
    
	virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
    
    CREATE_FUNC(PickNorHeroLayer);
};
    
class PickSuperHeroLayer : public ContentLayer
{
public:
    int pickHeroCount;
    HeroPtr targetHero;
    std::vector<CCSprite*> cards;
    std::vector<int> drawPrice;
    std::vector<CCPoint> locations;
    std::vector<HeroPtr> herosToDraw;
    bool isAllowPickCard;
    int drawTimes;
    HeroCard* currentHeroCard;
    AlertMenu* alertMenu;
    
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    void calDrawTimes();
    void showAllCards(bool value);
    void hideAllCards();
    void reshuffleCards();
    void dropCard();
    CCSprite* getCard(CCPoint point);
    void pickACard(CCSprite* card);
    void makeMenu();
    void deleteMenu();
    void onGoOn();
    void onGiveUp();
    void onShare();
    
    CCFiniteTimeAction* createCardFlippingAnim(CCSprite* card, Hero* hero);
    CCFiniteTimeAction* createCardFaceDownAnim(CCSprite *cardBack , HeroCard *cardFront);

	virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
    
    CREATE_FUNC(PickSuperHeroLayer);
};
    
class Hire10Layer : public ContentLayer
{
public:
    virtual bool init(InformType& type);
    void drawAnim();
    void addBox(int i);
    void showMenu();
    void onBack();
	void onConfirm();
    void onShare();
    
    virtual void onLongClick(CCTouch* touch);
    
    static Hire10Layer* create(InformType& type)
	{
		Hire10Layer* o = new Hire10Layer();
		if(o != NULL && o->init(type))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
    
private:
    CCSprite* bg;
    std::vector<CCSprite*> boxes;
    std::vector<HeroIcon*> icons;
    Hero10Draw heroesDraw;
    HeroIcon* tempIcon;
    InformType type;
    
    CCMenu* topMenu;
    CCMenu* goOnMenu;
    int star5HeroesLine;
    
    CCActionInterval* createSwardJumpoutAction(std::vector<CCSprite*> swards);
    CCActionInterval* createSwardDropAction(std::vector<CCSprite*> swards);
    CCActionInterval* createIconsAction(std::vector<CCSprite*> swards);
    CCActionInterval* createShakeAction(CCSprite* target);
    ccColor4F getHeroColor(const Hero* hero);
};
    
class InformLayer : public ContentLayer
{
public:
	virtual bool init(const InformType& type);
    virtual void onEnter();
    virtual void onEnterAnimationDone();
    
    virtual void setTouchEnabled(bool value);
	void callBack();
	void onBack(cocos2d::CCObject *target);
	void onConfirm(cocos2d::CCObject *target);

	JuanZhouLayer *jz;
    CCMenu *topMenu;
    CCLabelTTF *msgLabel;
	InformType type;
    int times;

	static InformLayer* create(const InformType& type)
	{
		InformLayer* o = new InformLayer();
		if(o != NULL && o->init(type))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
private:
    ZqlScrollView* scrollView;
    CCLabelBMFont* juedingFreeTime;
    CCLabelBMFont* transTimes;
    CCLabelBMFont* expTimes;
    
    void updateTimer(float dt);
    ContentLayer* createItem(const ZhaoMuItemType& type);
};

class ZhaoMuLayer : public ContentLayer
{
public:
	bool init();
    virtual void onEnter();
	virtual void setTouchEnabled(bool value);
    virtual void onEnterAnimationDone();
    void updateTimer(float dt);
    std::string toStr2(int t);

    ButtonSprite* normalHireBtn;
    ButtonSprite* superHireBtn;
    ButtonSprite* toolBtn;
    
    CCMenu *topMenu;
	CCSprite *upMenu;
	CCSprite *downMenu;
    
    CCLabelBMFont *gaoshouLabel;
    CCLabelBMFont *juedingLabel;
    
	int gaoshouTimeInterval;
	int juedingTimeInterval;

	static ZhaoMuLayer* create()
	{
		ZhaoMuLayer* o = new ZhaoMuLayer();
		if(o != NULL && o->init())
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}

public:
	void onZhaoMu(const InformType& type);
};
}