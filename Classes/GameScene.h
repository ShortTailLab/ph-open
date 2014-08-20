//create by ZQL

#pragma once

#include <cocos2d.h>
#include "Store.h"
#include "Fonts.h"
#include <extensions/utils.h>
#include "BuildConf.h"
#include "ButtonSprite.h"
#include "FlowerSystem.h"
#include "PvPMessageHandler.h"
#include <boost/shared_ptr.hpp>
#include <list>

class ScrollMenu;

namespace cocos2d
{
    class CCRichLabelTTF;
}

namespace PH
{
    USING_NS_CC;
    USING_NS_CC_EXT;
    
    class Player;

    class PlayerMsgBoard : public ContentLayer
    {
    public:
        virtual bool init();
        void onShecdule();
        
        CREATE_FUNC(PlayerMsgBoard);
    private:
        CCLabelTTF* currTime;
        CCLabelTTF* drawTime;
    };
    
class PlayerBoard : public ContentLayer
{
public:
	CCLabelTTF* nameLabel;
	CCLabelBMFont* levelLabel;
    
	CCLabelBMFont* coinsLabel;
	CCLabelBMFont* goldsLabel;
    
    CCLabelBMFont* spiritLabel;
	CCLabelBMFont* spiritTimeLabel;
    CCScale9Sprite* spiritBar;
    
    //CCLabelBMFont* expLabel;
    CCScale9Sprite* expBar;
    CCLabelBMFont* expLabel;

	int spiritTime;
	

	bool init();

	void setName(std::string value);
    void setVipLevel(int vipLevel);
	void setLevel(float value);
	void setCoins(float prev, float curr);
	void setGolds(float prev, float curr);
    
	void setSpirit(float prev, float prevMax, float curr, float currMax);
	void setExp(float prev, float prevMax, float curr, float currMax, float levelDiff);
	void spiritTimeCount(float dt);
    
    // default implements are used to call script callback if exist
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

	void syncSpiritTime();

	CREATE_FUNC(PlayerBoard);
private:
    CCSprite *bg;
    CCSprite* vip;
};

class BroadCast : public ContentLayer
{
public:
	virtual bool init();

	void show(const std::string&);

	CCRichLabelTTF *label;

	CREATE_FUNC(BroadCast);
private:
    void schedule();
    
    std::list<std::string> msgQueue;
    bool actionRunning;
};
    
    class LoginLayer;

class GameScene : public CCScene, public CCKeypadDelegate, public IPvPWaitMatchReceiver
{
public:
	LoginLayer *loginLayer;
	FlowerDrop *flowersDrop;
    CCSprite* background;
    ScrollMenu* menu;
  	BroadCast *zimu;
    
    // cache login info for relogins
	std::string type;
	std::string id, token, referCode;

    GameScene();
	virtual ~GameScene();

	virtual bool init();
	void loginUI();
    
	static void sync();
    static void downloadHeroRecruitImage();
    
	void loginOurs(const std::string& type, const std::string& id, const std::string& token,const std::string& referCode);
	void initMsgServer();
    void initBG();
    
	void initMenuUI();
    void startMenu();

    bool handleMissingPurchase();

	void showBottom(bool value);
	void showTop(bool value);

    void clearUI();
    void clearBG();
	void recoverUI();

    void initTabMsg();
    void scheduleTabMsg();
    void sendTabMsg(int tab , std::string msg);
    void unscheduleTabMsg();
    void tabMsgSchedule(float dt);
    
    void scheduleRelogin();
    void stopReloginSchedule();
    void checkNeedRelogin();
    void clearUIAndReLogin(float dt);

	void setTouchEnabled(bool value);
    
    void loadPersistentTextureCache();
    void loadTextureCache();
    
    void calcDimension();
    // navigation related methods
    
    // use this method to switch between tabs. since there are more bookkeeping to do.
    void selectTab(int tab);
    void saveJiangHuState();
    void loadJiangHuState();
    
    PlayerBoard* statusLayer() { return this->getChildren()->containsObject(top) ? top : NULL; }
    
    void battleSceneEnded(const std::string& gotoView = "");
    
    void pushLayer(ContentLayer*, CCFiniteTimeAction* animation=NULL);
    void pushLayerWithDefaultAnimation(ContentLayer* layer);
    void popLayer();
    void popAllLayers();
    
    virtual void keyBackClicked();
    
    void myTick(float);
    
    // receivers
    virtual bool onChallenge(const Player& other);
    virtual bool onRandomMatchFound(const Player& other);
    virtual bool onStartGame(const Player& first, const Player& second, int seed, const PvPConfig& config);
    virtual bool onRivalTimeoutOrRejected();
    virtual bool onRecover(int localSeqId,
                           int seed,
                           int round,
                           const Player& first,
                           const Player& second,
                           const rapidjson::Value& battleState,
                           float skillTimeout,
                           float moveTimeout,
                           float skillTimeLeft,
                           float moveTimeLeft,
                           const rapidjson::Value& events);

public:
    // Static methods
	static GameScene *getInstance()
    {
        if(!inst)
        {
            assert(inst == NULL && "GameScene is not initialized.");
            return NULL;
        }

		return inst;
	}
    
    static void initInstance()
    {
        assert(inst == NULL && "GameScene is already initialized.");
        if(!inst)
		{
			inst = new GameScene();
			inst->init();
		}
    }
    
    static bool isInitialized()
    {
        return inst != NULL;
    }
    
private:
    static GameScene* inst;
    
    CCSize winSize;
	PlayerBoard *top;
	ContentLayer *middle;
    CCLayer *bottom;
    
    //ScrollMenu* menu;
    std::vector<CCSprite*> tabs;
    int preTab;
    std::map<int, cocos2d::CCNode*> tabMsgs;

    CCArray* navStack;

	void tabSelected(CCMenuItemSprite *target);
    CCSprite* createATab(int i, std::string type);
    ButtonSprite* getTab(int index);
};

}
