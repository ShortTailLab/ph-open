#pragma once
#include "ContentLayer.h"
#include "BattleScene.h"
#include "ZqlScrollView.h"
#include "UIComponent.h"
#include "ControlExtensions.h"
#include "PlayerData.h"
#include "DebugMisc.h"
#include "Tutorial.h"
#include "ButtonSprite.h"

namespace PH
{
class RelationsEffect : public ContentLayer
{
public:
    virtual bool init(ScrollRelations* view,JuanZhou* jz);
    void showEffect();
    
    
    static RelationsEffect* create(ScrollRelations* view,JuanZhou* jz)
    {
        RelationsEffect* o = new RelationsEffect();
        if(o != NULL && o->init(view, jz))
        {
            o->autorelease();
            return o;
        }
        CC_SAFE_DELETE(o);
        return NULL;
    };
    
private:
    ScrollRelations* view;
    JuanZhou* jz;
    std::map<HeroIcon*, int> effectsNumOnEachIcon;
    std::map<int, CCLabelBMFont*> effectsOnIcons;
    std::vector<Hero*> heroes;
    std::vector<HeroIcon*> icons;
    std::vector<CCSprite*> items;
    
    bool isOwnRelation(const Hero* hero, const HeroRelation& relation);
    CCAction* createEachItemEffect(CCSprite* item);
    CCAction* createEachIconEffect(int iconNum, const HeroRelation& relation);
    CCAction* createNumAction(const std::string& name,
                              const std::string& fontName,
                              int from,
                              int to,
                              const CCPoint& pos,
                              int index);
    CCPoint getEffectPosition(HeroIcon* icon);
    int getEffectIndex(int iconNum, const RelationEffects& effect);
};
    
class TeamConfirmJuanZhou : public JuanZhou
{
public:
    virtual bool init(const HeroList& team, Level& level);
    virtual void initHeroRelations();
    virtual bool addHero(PH::Hero *hero , int id ,bool isUpdate = true);
    virtual void openDone();
    virtual void onLongClick(CCTouch* touch);
    
    static TeamConfirmJuanZhou* create(const HeroList& team, Level& level)
    {
        TeamConfirmJuanZhou* o = new TeamConfirmJuanZhou();
        if(o != NULL && o->init(team, level))
        {
            o->autorelease();
            return o;
        }
        CC_SAFE_DELETE(o);
        return NULL;
    }

public:
    ScrollRelations* relationsView;
    
private:
    HeroList confirmTeam;
    CCLabelTTF* skillLabel;
    LabelX* skillDesLabel;
    CCLabelTTF* helperSkillLabel;
    LabelX* helperSkillDesLabel;
    CCLabelBMFont* hpLabel;
    CCLabelBMFont* attackLabel;
    CCLabelBMFont* regenLabel;
    BianDuiSwitcher* switcher;
    TeamData teamData;

    std::vector<HeroRelation> teamRelations;
    RelationsEffect* effectLayer;
    
    void refresh();
};
    
class TeamConfirmLayer : public ContentLayer
{
public:
    virtual bool init(PlayerPtr player, Level& level);
    virtual void onEnter();
    virtual void onEnterAnimationDone();
    void onStartGame();
    void onShare();
    
    void onBack(cocos2d::CCObject* target);
    
    static TeamConfirmLayer* create(PlayerPtr player, Level& level)
	{
		TeamConfirmLayer* o = new TeamConfirmLayer();
		if(o != NULL && o->init(player, level))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
    
    
private:
    CCMenu* menu;
    JuanZhouLayer* jz;
    PlayerPtr targetPlayer;
    Level targetLevel;
};


class HelpItem : public ContentLayer
{
public:
	virtual bool init(PlayerPtr& player , const Level& level, const HelpersSortType& type);
	virtual void setTouchEnabled(bool value);
    virtual void onSingleClick(CCTouch* touch);
    virtual void onLongClick(CCTouch* touch);
    
    void initRelation();
    
	void onConfirm(CCObject *target);

	PlayerPtr targetPlayer;
    Hero *targetHero;
	Level targetLevel;
	CCMenu *menu;
	CCSprite *btn;
    CCSprite *heroImg;
    

	static HelpItem* create(PlayerPtr& player , const Level& level,const HelpersSortType& type)
	{
		HelpItem* o = new HelpItem();
		if(o != NULL && o->init(player , level, type))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
    
private:
    cocos2d::CCSprite *bg;
    CCSprite* sigh;
};
 
class HelpersLayer : public ContentLayer
{
public:
	virtual bool init(const Level &level);
	virtual void setTouchEnabled(bool value);
    virtual void onEnter();
	void makeItems(const std::vector<PlayerPtr>& friends);
	void onBack(cocos2d::CCObject *target);
    void onChangeSortType(cocos2d::CCObject *target);
    std::vector<PlayerPtr> getHelpersBySortType();
    std::string getSortStr();
    void checkIfStageEnd();

    CCLabelTTF* title;
	ZqlScrollView *scrollView;
	std::vector<PlayerPtr> helpers;
	std::vector<HelpItem*> items;
	Level targetLevel;
    Stage targetStage;

	static HelpersLayer* create(const Level& level)
	{
		HelpersLayer* o = new HelpersLayer();
		if(o != NULL && o->init(level))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}

private:
	CCMenu *menu;
    
    void onStageEnd(float dt);

    HeroBoard * board;
public:
    virtual void onSingleClick(CCTouch * touch);
};

class  JiangHuMenuItemBg : public ContentLayer
{
public:
	virtual bool init(const Stage& stage, bool progressive);
	void makeSubMenu(std::vector<std::string> &items);
	void onTouch(int id);
	virtual void visit();
    
    void setLevelsVisible(bool);

	int levelNum;
    Stage stage;
	std::vector<cocos2d::CCSprite*> items;
	std::vector<CCRect> itemRect;
	cocos2d::CCSprite *targetItem;
	int targetId;
	int getTouchId(cocos2d::CCPoint localPoint);
	cocos2d::CCSize viewPort;
    
    virtual void onSingleClick(cocos2d::CCTouch* touch);
    virtual void onLongClick(cocos2d::CCTouch* touch);

	static JiangHuMenuItemBg* create(const Stage& stage, bool progressive)
	{
		JiangHuMenuItemBg* o = new JiangHuMenuItemBg();
		if(o != NULL && o->init(stage, progressive))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
    
private:
    std::vector<CCNode*> levelsNode;
    ContentLayer* hintBoard;
	//std::string WideByte2UTF8(const std::wstring& text);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	bool IConvConvert(const char *from_charset, const char *to_charset, const char *inbuf, int inlen, char *outbuf, int outlen); 
	std::string IConvConvert_GBKToUTF8(const std::string& str);
#endif
};

class JiangHu;

class JiangHuMenuItem : public ContentLayer
{
public:
	cocos2d::CCSize viewPort;
	cocos2d::CCRect zhou1Rect;
	cocos2d::CCRect zhou2Rect;
	JiangHu *jianghu;
    Stage stage;
	bool isClick;
	cocos2d::CCSprite *marginLeft;
	cocos2d::CCSprite *marginRight;
    ContentLayer* hintBoard;
    bool isOpen;

	virtual bool init(const Stage& stage, bool progressive);
    
	virtual void update(float dt);
    virtual void setTouchEnabled(bool value);
    void createHintBoard(const Level& level, CCPoint pos);
    void cancelHintBoard();
	void itemOpenSwitch();
	void setIsOpen(bool value);
	void onTitleClick();
	void makeTouchRegion();
	void updateStage(const Stage& stage);

	void checkEvent();
	void endEvent(float dt);
    void eventTimeCount(float dt);
    void stopAllSchedule(bool value);
    
    bool isItemOpen() { return isOpen; }
    
    void setStage(const Stage& s);

    virtual void onExit();
    
	virtual void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);

	static JiangHuMenuItem* create(const Stage& stage, bool progressive=true)
	{
		JiangHuMenuItem* o = new JiangHuMenuItem();
		if(o != NULL && o->init(stage, progressive))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}

private:
	cocos2d::CCSprite *zhou1;
	cocos2d::CCSprite *zhou2;
    cocos2d::CCLabelBMFont* timeLabel;
    cocos2d::CCSprite *eventLabel;
    cocos2d::CCLabelBMFont* quoteTimesLabel;
    cocos2d::CCLabelBMFont *eventTime;
    std::vector<HeroIcon*> icons;
    
    bool progressive;
    
    CCSprite* gemSprite;
    LabelX* title;
    CCSprite* sigh;
    ButtonSprite* rewardBtn;
    
	JiangHuMenuItemBg *bg;
	int padding;
	bool easing(float tY);
    
    CCSprite* createRewardSpr(const Stage& stage);
    void saveJiangHuState();
};


class JiangHuMenuItemBlack : public ContentLayer
{
public:
    Stage stage;
    CCLabelBMFont* timeLabel;
    LabelX* title;
    JiangHu* jianghu;
    
    virtual bool init(const Stage& stage, int kind);
    virtual void update(float dt);
    
    static JiangHuMenuItemBlack* create(const Stage& stage, int kind = 0)
    {
        JiangHuMenuItemBlack* o = new JiangHuMenuItemBlack();
        if(o != NULL && o->init(stage, kind))
        {
            o->autorelease();
            return o;
        }
        CC_SAFE_DELETE(o);
        return NULL;
    }
    
    
};

    
enum class JiangHuType
{
    ZhanYi = 0,
    HuoDong
};
    
class JiangHu : public ContentLayer, public TutorialCaller
{
public:
    JiangHuType type;
    
	MenuScrollView *scrollView;
	std::vector<JiangHuMenuItem*> items;

	virtual bool init(JiangHuType type);
    
    virtual void onEnter();
    virtual void onExit();
    virtual void onEnterAnimationDone();
	virtual void setTouchEnabled(bool value);
    
    virtual void tutorialCall(TutorialControl* control);
    
	void makeMenu(std::vector<boost::shared_ptr<Stage>>& stages, bool);

    void changeStage(CCObject* object);
    void initStage();
	void itemOpenHandle(JiangHuMenuItem *item);
    
    virtual void cleanup();
    bool isLevelsAllOpen(const Stage& stage);

	static JiangHu* create(JiangHuType type)
	{
		JiangHu* o = new JiangHu();
		if(o != NULL && o->init(type))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}

private:
	JiangHuMenuItem* currOpenItem;
    std::string jingjiSelectPath;
    std::string jingjiNorPath;
    
    void onBack();
    int getStagePriority(boost::shared_ptr<Stage>& s);
};

}

