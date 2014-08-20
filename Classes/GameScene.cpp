//create by ZQL
#include "BuildConf.h"
#include "DebugMisc.h"

#include "GameScene.h"
#include "BattleScene.h"

#include "HomePage.h"
#include "JiangHu.h"
#include "XiaKe.h"
#include "Login.h"
#include "Store.h"
#include "QiangHua.h"
#include "Sale.h"
#include "ZhuanSheng.h"
#include "Victory.h"
#include "HeroProfileLayer.h"
#include "Board/Common.h"
#include "HeroTable.h"
#include "PlayerData.h"
#include "Board/HeroTable.h"
#include <support/user_default/CCUserDefault.h>
#include "ZhaoMu.h"
#include "friend.h"
#include "More.h"
#include "UIPresets.h"
#include "TouchMaskLayer.h"
#include "HeroSelectionLayer.h"
#include "ResourceSyncMan.h"
#include "ObjectPool.h"
#include "URLHelpers.h"

#include "PVPBoard.h"
#include "PvPLayer.h"
#include "PvPEndGame.h"

#include "ScreenConsole.h"
#include "ZqlScrollView.h"
#include "PageScrollView.h"
#include "PuzzleRPC.h"
#include "SimpleAudioEngine.h"
#include <CCRichLabelTTF.h>
#include "WebViewLayer.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>

#include <extensions/network/HttpClient.h>
#include <extensions/utils.h>
#include <extensions/crypto/CCCrypto.h>
#include <extensions/native/CCNative.h>
#include <extensions/CCBReader/CCBReader.h>

#if PH_USE_WEIBO
#include <auth/Weibo.h>
#endif

static bool firstSyncComplete = false;

using namespace boost;
using namespace boost::algorithm;
using namespace boost::date_time;
using namespace boost::posix_time;

typedef local_adjustor<ptime, 8, no_dst> ChinaCentralTime;

namespace PH
{
    USING_NS_CC;
    USING_NS_CC_EXT;
    
    using namespace std;

#define EXP_MAX_WIDTH 383.0f
    
    bool PlayerMsgBoard::init()
    {
        if(!ContentLayer::init())
            return false;
        
        CCScale9Sprite* window = Get9SpriteFromFrame("dialog_bg.png",
                                                     CCRectMake(50, 50, 208, 112));
        window->setContentSize(CCSizeMake(360, 270));
        window->setAnchorPoint(ccp(0.5, 1));
        addChild(window);
        
        addLabelTTF(this, ("用户Id："+toStr(PlayerData::getInstance()->player->referCode)).c_str(),
                    FONT_CN, 26, ccp(-160, -65), ccp(0, 0.5));
        currTime = addLabelTTF(this, "当前时间：", FONT_CN, 26, ccp(-160, -105), ccp(0, 0.5));
        drawTime = addLabelTTF(this, "免抽冷却：", FONT_CN, 26, ccp(-160, -145), ccp(0, 0.5));
        addLabelTTF(this, "充值VIP可享受VIP特权", FONT_CN, 26, ccp(-160, -210), ccp(0, 0.5), TITLE_YELLOW);
        
        onShecdule();
        schedule(schedule_selector(PlayerMsgBoard::onShecdule) , 1);
        
        return true;
    }
    void PlayerMsgBoard::onShecdule()
    {
        using namespace boost::date_time;
        using namespace boost::posix_time;
        ptime juedingPTime = from_time_t(PlayerData::getInstance()->drawFreeTime[1]);
        ptime now = boost::posix_time::second_clock::universal_time() +
                    PlayerData::getInstance()->delta_local_server;
        auto interval = juedingPTime - now;
        int h = interval.hours();
        int m = interval.minutes();
        int s = interval.seconds();
        if(!interval.is_negative())
        {
            char ss[128];
            if(h)
                sprintf(ss, "%02d:%02d:%02d", h, m, s);
            else
                sprintf(ss, "%02d:%02d", m, s);
            drawTime->setString(("免抽冷却："+toStr(ss)).c_str());
        }
        else
            drawTime->setString("可免抽一次");
        
        ptime local = boost::posix_time::second_clock::local_time()+
                      PlayerData::getInstance()->delta_local_server;
        h = local.time_of_day().hours();
        m = local.time_of_day().minutes();
        s = local.time_of_day().seconds();
        char ss2[128];
        if(h)
            sprintf(ss2, "%02d:%02d:%02d", h, m, s);
        else
            sprintf(ss2, "%02d:%02d", m, s);
        currTime->setString(("当前时间："+toStr(ss2)).c_str());
    }
    
bool PlayerBoard::init()
{
	if(!ContentLayer::init())
        return false;

	spiritTime = 0;
    vip = NULL;
    this->setTouchPriority(-1000);
    this->setTouchMode(kCCTouchesOneByOne);

	Player *player = PlayerData::getInstance()->getPlayer();

	bg = GetSprite("gameui/yonghu.png");
	bg->setAnchorPoint(ccp( 0.5 , 1));
	bg->setPosition(ccp(winSize.width*0.5f , 0));
	addChild(bg);

	addLabelTTF(this, nameLabel, player->name.c_str(), FONT_CN, 28, ccp(39, -10), ccp(0, 1));
    setVipLevel(player->vipLevel);
    
	addLabelBMFont(this, levelLabel, toStr(player->level).c_str(), "bmfont/Sumo_22_yellow.fnt", ccp(373, -13));
    levelLabel->setAnchorPoint(ccp(0.5, 1));
    levelLabel->setScale(contentScale());
    
	addLabelBMFont(this, coinsLabel, toStr(player->coins).c_str(), "bmfont/Sumo_22_yellow.fnt",ccp(590, -27));
    coinsLabel->setAnchorPoint(ccp(1,0.5));
    coinsLabel->setScale(contentScale());
    
    addLabelBMFont(this, goldsLabel, toStr(player->golds).c_str(), "bmfont/Sumo_22_green.fnt",ccp(556, -95));
    goldsLabel->setScale(contentScale());
    
    {
        // spirit bar
        spiritBar = Get9SpriteFromFrame("tili_tiao.png", 7, 7, 387, 5);
        spiritBar->setAnchorPoint(ccp(0, 1));
        spiritBar->setPosition(ccp(110, -53));
		spiritBar->setContentSize(CCSize(std::max(EXP_MAX_WIDTH * player->spirit / player->maxSpirit, 10.f), 23));

        spiritBar->setVisible(player->spirit != 0);
        this->addChild(spiritBar);
        
        // spirit label
        std::string numText = toStr(player->spirit) + "/" + toStr(player->maxSpirit);
        spiritLabel = CCLabelBMFont::create(numText.c_str(),
                                            "bmfont/Sumo_22_white.fnt");
        spiritLabel->setScale(contentScale());
        spiritLabel->setAnchorPoint(ccp(0.5, 0.5));
        spiritLabel->setPosition(ccp(285, -66));
        this->addChild(spiritLabel);

		spiritTimeLabel = CCLabelBMFont::create("", "bmfont/Sumo_20_white.fnt");
		spiritTimeLabel->setScale(contentScale());
        spiritTimeLabel->setAnchorPoint(ccp(0.5, 0.5));
        spiritTimeLabel->setPosition(ccp(450, -66));
        this->addChild(spiritTimeLabel);

		this->schedule(schedule_selector(PlayerBoard::spiritTimeCount), 1);
    }
    
    {
        // exp bar
        expBar = Get9SpriteFromFrame("jingyan_tiao.png", 7, 7, 387, 5);
        expBar->setVisible(player->exp);
        
        expBar->setAnchorPoint(ccp(0, 1.0));
        float scaleX =  (float)player->exp / player->nextLevelExp;
        expBar->setContentSize(CCSize(std::max(EXP_MAX_WIDTH  * scaleX, 10.0f), 23));
        expBar->setPosition(ccp(110, -94));
        
        this->addChild(expBar);
        
        std::string expText = toStr(player->exp)+"/"+toStr(player->nextLevelExp);
        addLabelBMFont(this, expLabel, expText.c_str(), "bmfont/Sumo_20_white.fnt", ccp(285, -108));
        expLabel->setScale(contentScale());
        expLabel->setAnchorPoint(ccp(0.5f, 0.5f));
    }
    
	return true;
}

void PlayerBoard::setName(string value)
{
    int max = 190;
    nameLabel->setString(value.c_str());
    if(nameLabel->getContentSize().width > max)
    {
        max -= 30;
        wstring wStr = URLHelper::str2wstr(nameLabel->getString());
        while(nameLabel->getContentSize().width > max)
        {
            wStr = wStr.substr(0, wStr.size()-1);
            nameLabel->setString(URLHelper::wstr2str(wStr).c_str());
        }
        nameLabel->setString(URLHelper::wstr2str(wStr).append("...").c_str());
    }
    
}
    
void PlayerBoard::setVipLevel(int vipLevel)
{
    int posX = 0;
    if(vip)
        vip->removeFromParentAndCleanup(true);
    vip = GetSprite("gameui/vip"+toStr(vipLevel)+".png");
    vip->setPosition(ccp(39, -12));
    vip->setAnchorPoint(ccp(0, 1));
    vip->setScale(0.7f);
    addChild(vip);
    posX  = vip->getContentSize().width*0.7f;
    setName(nameLabel->getString());
    nameLabel->setPositionX(39+posX);
}

void PlayerBoard::setLevel(float value)
{
	levelLabel->setString(toStr(value).c_str());
}

void PlayerBoard::setCoins(float prev, float curr)
{
	CCCountBMTextNumber *count = CCCountBMTextNumber::create(prev, curr, 1);
	coinsLabel->runAction(count);
}

void PlayerBoard::setGolds(float prev, float curr)
{
	CCCountBMTextNumber *count = CCCountBMTextNumber::create(prev, curr, 1);
	goldsLabel->runAction(count);
}

void PlayerBoard::setSpirit(float prev, float prevMax, float curr, float currMax)
{
    if(prev == curr)
        return;
            
    float prevScaleX = prev / prevMax;
    float currScaleX = curr / currMax;
    float distance = (currScaleX - prevScaleX) * EXP_MAX_WIDTH;
    if(distance != 0.0f)
    {
        float duration = std::abs(distance) / EXP_MAX_WIDTH * 2.0f;
        spiritBar->runAction(CCAnimate9SpriteProgress::create(duration, 10, EXP_MAX_WIDTH, distance, true));
    }
    
    std::string postStr = std::string("/") + toStr(currMax);
    CCCountBMTextNumber* count = CCCountBMTextNumber::create(prev, curr, 0.3, postStr);
    spiritLabel->runAction(count);
}

void PlayerBoard::setExp(float prev, float prevMax, float curr, float currMax, float levelDiff)
{
    if(prev == curr)
        return;
    
    float from = (float)prev / prevMax;
    float to = (float)curr / currMax;
    float distance = (levelDiff * 1.0f - from + to) * 380.0f;
    if(distance != 0.0f)
    {
        float duration = std::abs(distance) / 380.0f * 2.0f;

        CCActionInterval* action = CCAnimate9SpriteProgress::create(duration, 10, 380.0f, distance);
        expBar->runAction(action);
    }
    
    expLabel->setString((toStr(curr)+"/"+toStr(currMax)).c_str());
}

void PlayerBoard::spiritTimeCount(float dt)
{
    using namespace boost::posix_time;
	PlayerData *data = PlayerData::getInstance();
    char label[64] = {0};
    
    if (data->player->maxSpirit != data->player->spirit)
    {
        auto checkTime = data->spiritCheckTime;
        ptime now = boost::posix_time::second_clock::universal_time() +
                    PlayerData::getInstance()->delta_local_server;
        int time = 10*60 - (now - checkTime).total_seconds();

        int spiritGain = 0;
        while (time <= 0)
        {
            spiritGain++;
            time += 10*60;
            checkTime += minutes(10);
        }
        
        if (spiritGain)
        {
            data->setSpirit(std::min(data->getSpirit() + spiritGain, data->player->maxSpirit), data->player->maxSpirit );
            data->spiritCheckTime = checkTime;
        }

        int m = time/60;
        int s = time%60;

        sprintf(label, "%02d:%02d" , m , s);
    }
    spiritTimeLabel->setString( label );
}
    
// default implements are used to call script callback if exist
bool PlayerBoard::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint localPoint = this->convertTouchToNodeSpace(pTouch);
    if(bg->boundingBox().containsPoint(localPoint))
    {
        CCLayer* msgBoard = PlayerMsgBoard::create();
        msgBoard->setPosition(ccp(winSize.width*0.5f, -140));
        msgBoard->setTag(10001);
        addChild(msgBoard);
        return true;
    }
    return false;
}
void PlayerBoard::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint currPoint = pTouch->getLocationInView();
	CCPoint prevPoint = pTouch->getPreviousLocationInView();
    
	if(ccpDistance(currPoint, prevPoint) > 5 && getChildByTag(10001))
    {
        this->removeChildByTag(10001, true);
    }
}
void PlayerBoard::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    if(this->getChildByTag(10001))
        this->removeChildByTag(10001, true);
}

////////////////////////////////////////////////////
//BroadCast
////////////////////////////////////////////////////

bool BroadCast::init()
{
	if(!ContentLayer::init())
        return false;

	CCSprite *bg = GetSprite("gameui/gundongzimu.png");
	bg->setAnchorPoint(ccp(0 , 0.5));
	addChild(bg);

//	addLabelTTF(this , label , "" , FONT_CN , 28 , ccp( 0 , 0) , ccp(0 , 0.5));
    label = cocos2d::CCRichLabelTTF::create("", FONT_CN, 28);
    label->setPosition(ccp(0, 0));
    label->setAnchorPoint(ccp(0, 0.5));
    label->setColor(ccWHITE);
    this->addChild(label);

    actionRunning = false;

	return true;
}
    
void BroadCast::schedule()
{
    actionRunning = !msgQueue.empty();
    if (actionRunning)
    {
        label->setString(msgQueue.front().c_str());
        msgQueue.pop_front();
        
        CCDelayTime *delay = CCDelayTime::create(random1()*2.0f);
        CCActionInstant *a_place = CCPlace::create(ccp(winSize.width , 0));
        CCAction *a_moveto = CCMoveTo::create(0.5 , ccp(0,0));
        CCAction *a_stop = CCDelayTime::create(5);
        CCAction *a_moveby = CCMoveBy::create(2 , ccp(-winSize.width , 0));
        CCCallLambda *aReSchedule = CCCallLambda::create(boost::bind(&BroadCast::schedule, this));
        CCAction *aSeq = CCSequence::create(delay, a_place , a_moveto , a_stop , a_moveby , aReSchedule, NULL);
        
        label->runAction(aSeq);
    }
}

void BroadCast::show(const std::string& msg)
{
    msgQueue.push_back(msg);
    if(!actionRunning)
        schedule();
}

//////////////////////////////////////////////////
//GameScene
//////////////////////////////////////////////////

static ContentLayer::ContentCreatorFunc cCreators[] = {
    NULL,
    (ContentLayer::ContentCreatorFunc)HomePage::create,
    (ContentLayer::ContentCreatorFunc)XiaKe::create,
    (ContentLayer::ContentCreatorFunc)ZhaoMuLayer::create,
    (ContentLayer::ContentCreatorFunc)StoreLayer::createWithoutBtn,
    (ContentLayer::ContentCreatorFunc)FriendLayer::create,
    (ContentLayer::ContentCreatorFunc)MoreLayer::create,
};

// Initialize static members;
GameScene* GameScene::inst = NULL;
    
GameScene::GameScene() : top(NULL), middle(NULL), bottom(NULL), zimu(NULL), loginLayer(NULL)
{
    navStack = new CCArray();
}

GameScene::~GameScene()
{
    PuzzleRPC::getInstance()->removeAllHandlersForDomain("broadcast");
    navStack->release();
}

extern void logOutAux(bool doLogOut);
    
static int tickCount = 0;
void GameScene::myTick(float tick)
{
    LOGD("ticking %d\n", tickCount++);
}
    
bool GameScene::init()
{
	if (!CCScene::init())
        return false;

	LOGD("PuzzeHero: GameScene init start...\n");

    srand(time(NULL));
    
    loadPersistentTextureCache();
    loadTextureCache();

	winSize = CCDirector::sharedDirector()->getVisibleSize();
    
    initBG();
    
    // Back button dispatch
    {
        CCDirector::sharedDirector()->getKeypadDispatcher()->addDelegate(this);
    }
    
#if PH_USE_WEIBO
    if (!Weibo<Sina>::instance().userId().empty())
    {
        Weibo<Sina>::instance().authorize([=](auto_ptr<AuthError> authError)
        {
            if (authError.get())
                return loginUI();
            
            CCUserDefault::sharedUserDefault()->setStringForKey("guestLoginID", "");
            CCUserDefault::sharedUserDefault()->setStringForKey("guestLoginPWD", "");
            CCUserDefault::sharedUserDefault()->flush();
            GameScene::getInstance()->loginOurs(AccountType::SinaWeibo, Weibo<Sina>::instance().userId(), Weibo<Sina>::instance().token(),"");
        });
    }
    else if (!Weibo<Tencent>::instance().userId().empty())
    {
        Weibo<Tencent>::instance().authorize([=](auto_ptr<AuthError> authError)
        {
            if (authError.get())
                return loginUI();
            
            CCUserDefault::sharedUserDefault()->setStringForKey("guestLoginID", "");
            CCUserDefault::sharedUserDefault()->setStringForKey("guestLoginPWD", "");
            CCUserDefault::sharedUserDefault()->flush();
            GameScene::getInstance()->loginOurs(AccountType::QQ, Weibo<Tencent>::instance().userId(), Weibo<Tencent>::instance().token(),"");
        });
    }
    else if (!CCUserDefault::sharedUserDefault()->getStringForKey("guestLoginID").empty())
    {
        loginOurs(AccountType::Guest,
                  CCUserDefault::sharedUserDefault()->getStringForKey("guestLoginID"),
                  CCUserDefault::sharedUserDefault()->getStringForKey("guestLoginPWD"),
                  "");
    }
    else
        loginUI();
#endif
    
	return true;
}

void GameScene::keyBackClicked()
{
    // Note that, once keypad delegate is registered(manually), it's never
    // removed, meaning it's effective in BattleScene as well, but you should
    // not invoke any GameScene context stuff since it will be purged when
    // entering BattleScene.
    CCNative::promptForExit();
}

void GameScene::loadPersistentTextureCache()
{
    LOGD("load persistent texture\n");
    for (int i=0;i<100;i++)
    {
        string plistName = "icons" + toStr(i) +".plist";
        
        LOGD("load plist %s\n", plistName.c_str());
        if (PH::resourcePresentForLoading(plistName))
            CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(plistName.c_str());
        else
            break;
    }
    
    LOGD("load plist alert.plist\n");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("alert.plist");
    LOGD("load plist gems.plist\n");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("gems.plist");
    LOGD("load plist tutorialMisc.plist\n");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("tutorialMisc.plist");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("sound/bg/ui.mp3");
    LOGD("done loading persistent texture\n");
}

void GameScene::loadTextureCache()
{
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("gameuiBase.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("gameui.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("gameui1.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("gameui2.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("gameui3.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("gameuiQiuAnim.plist");
}

void GameScene::loginUI()
{
	loginLayer = LoginLayer::create();
	this->addChild(loginLayer);
}
    
static bool compareVersion(const std::string& v1, const std::string& v2)
{
    try
    {
        return boost::lexical_cast<float>(v1) < boost::lexical_cast<float>(v2);
    }
    catch (const boost::bad_lexical_cast& )
    {
        return false;
    }
}

void GameScene::sync()
{    
    SystemInform* loading = SystemInform::alertLoading(GameScene::getInstance());
    
	PuzzleRPC::getInstance()->setNetworkTimeOut(30);
	PuzzleRPC::getInstance()->sync([=](std::vector<HeroProfile*> profiles,
                                       std::vector<SkillInfo*> skills,
                                       Control control,
                                       std::auto_ptr<Error> error)
    {
        loading->cancel();
        
		if(error.get())
        {
			LOGD("!!! sync error msg: %s\n", error->toStr().c_str() );
            SystemInform::alertMessage(GameScene::getInstance(),
                                       "无法连接到服务器，请检查网络连接后重试。如果持续遇到问题请联系我们。",
                                       "连接错误",
                                       [=](){sync();} );
            
			return;
		}
        
        if (!control.serverURL.empty())
        {
            PuzzleRPC::getInstance()->setServerURL(control.serverURL);
            sync();
            return;
        }
        
        for(HeroProfile* profile : profiles)
        {
			addHero(profile);
        }

        SkillInfo* noSkill = new SkillInfo();
        noSkill->name = "无技能";
        noSkill->desc = "没有任何效果";
        noSkill->energy = 0;
        noSkill->sid = -1;
        noSkill->skillScript = "";
        addASkill(noSkill);

		for(SkillInfo* skill : skills)
		{
            //LOGD("--- skill: %d %d %s {%s}\n", skill->sid, skill->energy, skill->name.c_str(), skill->skillScript.c_str());
            phassert(skill->skillScript.size() != 0 && "skill script is empty");
            addASkill(skill);
		}
        LOGD("*** Skill sync done. total skill count %zd\n", skills.size());
        
        if (compareVersion(PH_VERSION, control.headVersion) && control.forceUpgrade)
        {
            CCNative::createAlert("发现新版本", "请先更新客户端", "确定");
            CCNative::showAlert();
            LOGD("New version found. Will not continue. Restart app.");
        }
        else
        {
            LOGD("setting sync flag to true\n");
            firstSyncComplete = true;
        }
	});
}

void GameScene::downloadHeroRecruitImage()
{
    if (PlayerData::getInstance()->recruitHeroImgURL.empty())
        return;
    
    std::string downloadPath = PlayerData::getInstance()->recruitHeroImgURL;
    downloadPath = CCNative::getDocPath() + downloadPath.substr(downloadPath.find_last_of('/')+1);
        
    const std::string heroRecruitLastModKey = "HeroRecruitImgLastModified";
    std::string lastMod = CCUserDefault::sharedUserDefault()->getStringForKey(heroRecruitLastModKey.c_str());
    
    CCHttpRequest* req = new CCHttpRequest;

    req->setRequestType(CCHttpRequest::kHttpGet);
    req->setUrl(PlayerData::getInstance()->recruitHeroImgURL.c_str());
    req->setDownloadPath(downloadPath + ".tmp");
    if (!lastMod.empty())
        req->addHeader("If-Modified-Since: " + lastMod);
    req->setResponseCallback([heroRecruitLastModKey, downloadPath](CCHttpResponse* resp)
    {
        if (resp->getResponseCode() == 200)
        {
            std::string lastModHeader = "Last-Modified: ";
            CCNative::moveFile(downloadPath + ".tmp", downloadPath);
            for (const auto& header : resp->getHeaders())
                if (starts_with(header, lastModHeader))
                {
                    string lastModString = trim_copy(header.substr(lastModHeader.length()));
                    CCUserDefault::sharedUserDefault()->setStringForKey(heroRecruitLastModKey.c_str(), lastModString);
                    CCUserDefault::sharedUserDefault()->flush();
                    break;
                }
        }
    });
    CCHttpClient::getInstance()->send(req);
    
    req->release();
}

void GameScene::loginOurs(const std::string& _type,
                          const std::string& _id,
                          const std::string& _token,
                          const std::string& _referCode)
{
    LOGD("GameScene::loginOurs...\n");
    
    if(loginLayer)
    {
        loginLayer->removeFromParentAndCleanup(true);
        loginLayer = NULL;
    }
    type = _type;
    id = _id;
    token = _token;
    referCode = _referCode;


    runOnMainThreadWhen([&](){ return firstSyncComplete; },
                        [=]()
                        {
                            LOGD("Clear UI And Relogin in main thread\n");
                            clearUIAndReLogin(0);
                        });
}

void GameScene::initMsgServer()
{
    if (!PuzzleRPC::getInstance()->messageServiceRunning())
    {
        PuzzleRPC::getInstance()->connectMessageService(PlayerData::getInstance()->getToken());
        PuzzleRPC::getInstance()->getPvP()->setWaitMatchReceiver(this);
    }
    
    PuzzleRPC::getInstance()->removeAllHandlersForDomain("broadcast");
	PuzzleRPC::getInstance()->handlerMessagesForServiceDomain("broadcast", [=](const string &s)
    {
		zimu->show(s);
	});
}
    
bool GameScene::onChallenge(const Player& otherPlayer)
{
    std::string otherUID = otherPlayer.uid;
    
    std::string name = otherPlayer.name;
    std::string text = "玩家 " + name + " 想你发出对战邀请.";
    SystemInform::alertConfirmPanel(runningScene(),
                                    text.c_str(),
                                    "游戏邀请",
                                    [=](){
                                        PuzzleRPC::getInstance()->getPvP()->sendAcceptChallenge(otherUID);
                                    },
                                    [=](){
                                        PuzzleRPC::getInstance()->getPvP()->sendRejectChallenge(otherUID);
                                    });
    return true;
}
    
bool GameScene::onRandomMatchFound(const Player& other)
{
    return true;
}
    
bool GameScene::onStartGame(const Player& first, const Player& second, int seed, const PvPConfig& config)
{
    PvPRand::seed(seed);
    
    this->clearBG();
    this->popAllLayers();
    
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
    
    // in case we queued a SearchCancel
    PuzzleRPC::getInstance()->getPvP()->clearWaitJobs();
    
    BattleScene* battleScene = BattleScene::create(first, second, config);
    CCDirector::sharedDirector()->replaceScene(battleScene);
    
    return true;
}
    
bool GameScene::onRivalTimeoutOrRejected()
{
}
    
bool GameScene::onRecover(int localSeqId,
                          int seed,
                          int round,
                          const Player& first,
                          const Player& second,
                          const rapidjson::Value& battleState,
                          float skillTimeout,
                          float moveTimeout,
                          float skillTimeLeft,
                          float moveTimeLeft,
                          const rapidjson::Value& events)
{
    this->clearBG();
    this->popAllLayers();
    
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
    
    BattleScene* battleScene = BattleScene::create(seed,
                                                   round,
                                                   first,
                                                   second,
                                                   battleState,
                                                   skillTimeout,
                                                   moveTimeout,
                                                   skillTimeLeft,
                                                   moveTimeLeft,
                                                   events);
    ScreenConsole::getInstance()->attach(battleScene);
    CCDirector::sharedDirector()->replaceScene(battleScene);
    
    return true;
}

void GameScene::initBG()
{
    background = GetSprite("gameui/background.jpg");
    background->setPosition(ccp(winSize.width/2,winSize.height/2));
    this->addChild(background);
    
    flowersDrop = FlowerDrop::create();
    flowersDrop->setPosition(ccp(-50 , winSize.height+50));
    this->addChild(flowersDrop);
    
//    CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    playBG("sound/bg/ui.mp3");
    
    
#if PH_DEBUG_BUILD
    string buildNum = CCNative::getBuildNum();
    buildNum = "Test\n" + buildNum;

    CCLabelTTF* serverLabel = CCLabelTTF::create(buildNum.c_str(), FONT_CN, 30);
    serverLabel->setPosition(ccp(640, 0));
    serverLabel->setAnchorPoint(ccp(1, 0));
    serverLabel->setColor(ccc3(255, 0, 0));
    this->addChild(serverLabel, 1000);
#endif
}

void GameScene::clearBG()
{
    if(background)
    {
        background->removeFromParentAndCleanup(true);
        background = NULL;
    }
    
    if(flowersDrop)
    {
        flowersDrop->stop();
        flowersDrop->removeFromParentAndCleanup(true);
        flowersDrop = NULL;
    }
}
    
void GameScene::startMenu()
{
    if(GameConfig::shared()->getTutorialProgress() == TutorialProfile ||
       GameConfig::shared()->getTutorialProgress() == TutorialProfileDone)
    {
        GameConfig::shared()->setTutorialProgress(TutorialQiangHua);
    }
    
    // Attempt to go back to a previous tutorial state
    // if tutorial was interrupted last time.
    if(GameConfig::shared()->getTutorialProgress() == TutorialQiangHua ||
       GameConfig::shared()->getTutorialProgress() == TutorialTeam)
    {
        selectTab(2);
    }
    else if(GameConfig::shared()->getTutorialProgress() == TutorialSpendGoldOnHero)
    {
        selectTab(3);
    }
    else if(GameConfig::shared()->getTutorialProgress() == TutorialJustFinished)
    {
        ArrowMask::shared()->clear();
        
        boost::function<void()>* funcPtr = new boost::function<void()>();
        (*funcPtr) = [=]()
        {
            SystemInform* loading = SystemInform::alertLoading(this);
            PuzzleRPC::getInstance()->setTutorialProgress(PlayerData::getInstance()->getToken(),
                                                          0,
                                                          [=](std::auto_ptr<Error> e)
            {
                loading->cancel();
                if(e.get())
                {
                    std::cout << "!!! set tutorial progress error: " << e->toStr() << std::endl;
                    
                    SystemInform::alertMessage(GameScene::getInstance(),
                                               "无法连接服务器，请稍后重连.",
                                               "提示" ,
                                               [funcPtr](){ (*funcPtr)(); });
                    return;
                }
                
                GameConfig::shared()->setTutorialProgress(TutorialSkip);
                clearUIAndReLogin(0);
                
                delete funcPtr;
            });
        };
        (*funcPtr)();
    }
    else
    {
        // normal flow
        selectTab(1);
        
//        startPvP(*PlayerData::getInstance()->player, *PlayerData::getInstance()->player);
//        pushLayerWithDefaultAnimation(PvPEndGameWin::create(1987, 1988, 2000));
//        pushLayerWithDefaultAnimation(PvPEndGameLoss::create(1987, 1988, 2000));
    }
}

void GameScene::initMenuUI()
{
	top = PlayerBoard::create();
	top->setPositionY(winSize.height);
	
	middle = ContentLayer::create();
	bottom = CCLayer::create();
	bottom->setPositionY(50);
    
    calcDimension();
    
    CCSprite* tabBg = GetSprite("gameui/tab_bg.png");
    tabBg->setAnchorPoint(ccp(0, 0.5));
    bottom->addChild(tabBg);
    tabs.clear();
    for(int i = 1; i < 7; i++)
    {
        ButtonSprite* btn = ButtonSprite::create(createATab(i, "b"),createATab(i, "a"), kCCMenuHandlerPriority-10, true);
        btn->setOnClickListener([=](){this->selectTab(i);});
        btn->setPosition(ccp(50+108*(i-1), 0));
        tabs.push_back(btn);
        bottom->addChild(btn);
    }
    //this use to record the previous clicked tab to switch the normal and selected state
    preTab = -1;

	zimu = BroadCast::create();
	zimu->setPositionY(119);
	
	this->addChild(middle); 
	this->addChild(top);
    this->addChild(zimu);
	this->addChild(bottom);
    
    //get the mail
    PuzzleRPC::getInstance()->checkMail(PlayerData::getInstance()->getToken(),
                                        [=](std::auto_ptr<MailBox> box,std::auto_ptr<Error> e)
                                        {
                                            if(e.get())
                                                return;
                                            int count = 0;
                                            for(const Mail& mail : box->mails)
                                                if(mail.sender != PlayerData::getInstance()->player->uid &&
                                                   !mail.read)
                                                    count++;
                                            PlayerData::getInstance()->newMailsNum = count;
                                        });
    

}
CCSprite* GameScene::createATab(int i, string type)
{
    CCSprite* bg = GetSprite("gameui/yuandi.png");
    CCSprite* tab = GetSprite("gameui/tab_" + toStr(i) + "_"+type+".png");
    tab->setPosition(ccp(bg->getContentSize().width*0.5f, bg->getContentSize().height*0.5f));
    bg->addChild(tab);
    return bg;
}

void GameScene::clearUI()
{
    if (middle)
    {
        // basically the opposite of init menu ui
        PuzzleRPC::getInstance()->removeAllHandlersForDomain("broadcast");
        
        popAllLayers();
        
        if(top)
        {
            top->removeFromParentAndCleanup(true);
            top = NULL;
        }
        
        if(middle)
        {
            middle->removeFromParentAndCleanup(true);
            middle = NULL;
        }
        
        if(zimu)
        {
            zimu->removeFromParentAndCleanup(true);
            zimu = NULL;
        }
        
        if(bottom)
        {
            bottom->removeFromParentAndCleanup(true);
            bottom = NULL;
        }
    }
    
    ObjectPool<JiangHuMenuItem>::getInstance()->clear();
}
    
bool GameScene::handleMissingPurchase()
{
    return false;
}

void GameScene::recoverUI()
{
    loadTextureCache();
    initMenuUI();
}

void GameScene::scheduleRelogin()
{
    ptime nowCST = ChinaCentralTime::utc_to_local(boost::posix_time::second_clock::universal_time());
    ptime tomorrow = ptime(nowCST.date()) + hours(24);
    
	float secondsToTomorrow = (tomorrow - nowCST).total_seconds();

    unschedule(schedule_selector(GameScene::clearUIAndReLogin)); // avoid duplicate login attempts
	scheduleOnce(schedule_selector(GameScene::clearUIAndReLogin), secondsToTomorrow + 5.f + random1()*10);
}
    
void GameScene::stopReloginSchedule()
{
    unschedule(schedule_selector(GameScene::clearUIAndReLogin));
}
    
void GameScene::scheduleTabMsg()
{
    schedule(schedule_selector(GameScene::tabMsgSchedule) , 1);
}
void GameScene::unscheduleTabMsg()
{
    unschedule(schedule_selector(GameScene::tabMsgSchedule));
}

void GameScene::tabMsgSchedule(float dt)
{
    PlayerData* data = PlayerData::getInstance();
    //check done tasks number
    if(GameConfig::shared()->showShuffleRelation && data->freeRelationTimes > 0)
        sendTabMsg(2, toStr(data->freeRelationTimes));
    else
        sendTabMsg(2, "");
    
    //check if there are free draws
    {
        int freeCount = 0;
        freeCount += PlayerData::getInstance()->isDrawFree(0) ? PlayerData::getInstance()->drawFreeLeft : 0;
        freeCount += PlayerData::getInstance()->isDrawFree(1) ? 1 : 0;

        if(freeCount)
            sendTabMsg(3, toStr(freeCount));
        else
            sendTabMsg(3, "");
    }
    
    //check if pendingfriends
    
    int n = data->player->pendingFriendRequests+data->newMailsNum;
    if(n > 0)
        sendTabMsg(5, n < 10 ? toStr(n) : "n");
    else
        sendTabMsg(5, "");
}

void GameScene::checkNeedRelogin()
{
    if (!PlayerData::getInstance()->player)
        return;
    
    ptime nowCST = ChinaCentralTime::utc_to_local(boost::posix_time::second_clock::universal_time() + PlayerData::getInstance()->delta_local_server);
    ptime lastLoginCST = ChinaCentralTime::utc_to_local(from_time_t(PlayerData::getInstance()->player->lastLoginTS));
    
    if (nowCST.date() != lastLoginCST.date())
    {
        stopReloginSchedule();
        unscheduleTabMsg();
        clearUIAndReLogin(0);
    }
}
    
void GameScene::clearUIAndReLogin(float dt)
{
    LOGD("clearUIAndReLogin.\n");
    clearUI();
    
    SystemInform* loading = SystemInform::alertLoading(this);
	PuzzleRPC::getInstance()->login(type, id, token, referCode,
                                    [=](auto_ptr<Player> player,
                                        auto_ptr<Control> control,
                                        auto_ptr<Error> error)
    {
        loading->cancel();
        if(error.get())
        {
            LOGD("!!! login error: %s\n", error->toStr().c_str());
            
            boost::function<void ()> relogin = [=]()
            {
#if PH_USE_WEIBO
                Weibo<Sina>::instance().clearLoginInfo();
                Weibo<Tencent>::instance().clearLoginInfo();
#endif
                loginUI();
            };
            
            if(error->code == 1004)
            {
                LOGD("type %s, id %s, token %s\n", type.c_str(), id.c_str(), token.c_str());
                SystemInform::alertMessage(runningScene(), "账号或密码错误！", "提示", relogin);
                return;
            }

            std::string msg = error->code == ErrorCode::RegClosed ? error->msg : error->toStrCN()+ "，重试？";
            boost::function<void ()> reconnect =  error->code == ErrorCode::RegClosed ? relogin : [=](){clearUIAndReLogin(0);};
            
            SystemInform::alertConfirmPanel(GameScene::getInstance(),
                                            msg.c_str(),
                                            "登录错误",
                                            reconnect,
                                            relogin);
            return;
        }
        LOGD("login succeeded, uid: %s\n", player->uid.c_str());
        
        if (compareVersion(PH_VERSION, control->headVersion) && control->forceUpgrade)
        {
            CCNative::createAlert("发现新版本", "请先更新客户端", "确定");
            CCNative::showAlert();
            return;
        }

        std::string boardMessage = control->annoucement;
        Reward reward = control->rewards;
        std::string rewardStr = control->getRewardsString();
        
        PlayerData::getInstance()->update(player, control);
        // this must be called after PlayerData is initialized.
        GameConfig::shared()->load();
        if(PlayerData::getInstance()->player->level > UNLOCK_EVERYTHING)
        {
            GameConfig::shared()->showSoulExchange = true;
            GameConfig::shared()->showShuffleRelation = true;
            GameConfig::shared()->showInstanceStage = true;
            GameConfig::shared()->firstTimeNeili = false;
        }
      
        downloadHeroRecruitImage();
        handleMissingPurchase();
        
        if (!GameConfig::shared()->pushToken.empty())
            PuzzleRPC::getInstance()->updateUserProfile(PlayerData::getInstance()->getToken(),
                                                        "",
                                                        GameConfig::shared()->pushToken,
                                                        [](auto_ptr<Player> playerUpdate,
                                                           auto_ptr<Error> errorUpdate){});
    
        // set tutorial progress according to server state
        
        TutorialProgress remoteProgress = (TutorialProgress)PlayerData::getInstance()->tutorialStage;
        LOGD("remote progress is %d\n", remoteProgress);
        if(remoteProgress == TutorialSkip || remoteProgress == TutorialDrawMainHero)
            GameConfig::shared()->setTutorialProgress(remoteProgress);
        else
            ; // use local progress
        
        LOGD("%s, tutorial progress %d\n",
             PlayerData::getInstance()->player->name.c_str(),
             GameConfig::shared()->getTutorialProgress());
        
        if(GameConfig::shared()->getTutorialProgress() == TutorialDrawMainHero)
        {
            
                                     HeroSelectionLayer* intro = HeroSelectionLayer::create();
                                     intro->onEnterAnimationDone();
                                     this->addChild(intro);
        }
        else
        {
            initMsgServer();
            initMenuUI();
            startMenu();
            scheduleRelogin();
            scheduleTabMsg();
            
            if(GameConfig::shared()->getTutorialProgress() == TutorialSkip)
            {
                // Display annoucement. This should only happen during login.
                boost::function<void ()> alertLandReward = [=]()
                {
                    PlayerData* data = PlayerData::getInstance();
                    if(data->loginRewards.size()==21)
                    {
                        SystemInform::alertLandReward(GameScene::getInstance(),
                                                      data->loginRewards,
                                                      data->rewardPath,
                                                      [](){});
                        
                    }
                };
                
                boost::function<void ()> alertReward = [=]()
                {
                    if(!rewardStr.empty())
                        SystemInform::alertAnnoucement(this,
                                                       "奖励",
                                                       rewardStr,
                                                       [=]()
                                                       {
                                                           PlayerData* playerData = PlayerData::getInstance();
                                                           playerData->setCoins(playerData->player->coins + reward.totalCoins);
                                                           playerData->setGolds(playerData->player->golds + reward.totalGolds);
                                                           alertLandReward();
                                                       });
                    else
                        alertLandReward();
                };

                if(!boardMessage.empty())
                {
                    SystemInform::alertAnnoucement(this,
                                                   "公告",
                                                   boardMessage,
                                                   alertReward);
                }
                else if(!rewardStr.empty())
                    alertReward();
                else
                    alertLandReward();
            }
        }
    });
}

void GameScene::setTouchEnabled(bool value)
{
    ((ContentLayer*)navStack->lastObject())->setTouchEnabled(value);
}

void GameScene::tabSelected(CCMenuItemSprite *target)
{
    selectTab(target->getTag());
    ring("sound/v/button_click.mp3");
}

void GameScene::showTop(bool value)
{
	top->setVisible(value);
    calcDimension();
    top->setTouchEnabled(value);
}

void GameScene::showBottom(bool value)
{
	bottom->setVisible(value);
	zimu->setVisible(value);
    calcDimension();
    
    for(CCSprite* btn : tabs)
        ((ButtonSprite*)btn)->setTouchEnabled(value);
    //menu->setTouchEnabled(value);
    
}
    
void GameScene::calcDimension()
{
    CCSize winSize = getContentSize();
    
    float y = winSize.height - (top->isVisible() ? 160 : 0);
//    float height = y - (bottom->isVisible() ? 100 : 0);
    middle->setPosition(ccp(winSize.width * 0.5f, y));
}

//-----------------------
// Nav stack operations.
void GameScene::pushLayer(ContentLayer* layer, CCFiniteTimeAction* animation)
{
    if (navStack->count())
    {
        ContentLayer* last = (ContentLayer*)navStack->lastObject();

        // onExit does this
        // last->pauseSchedulerAndActions();
        
        // do not clean up animation and schedulers since
        // we may use them after popping the pushed layer.
        last->removeFromParentAndCleanup(false);
    }
    
    navStack->addObject(layer);
    
    // always wrap animation in a CCSpawn. This handles animation == NULL.
    CCFiniteTimeAction* anim = CCSequence::create(CCSpawn::create(animation, NULL),
                                                  CCCallLambda::create([=]() { layer->onEnterAnimationDone();}),
                                                  NULL);
    anim ? middle->addChildWithAnim(layer, true, anim) : middle->addChild(layer);
    
    showBottom(layer->isShowBottom);
    showTop(layer->isShowTop);
}

void GameScene::pushLayerWithDefaultAnimation(ContentLayer* layer)
{
    // hide it before animation starts playing.
    layer->setVisible(false);
    
    CCFiniteTimeAction* defaultSlideIn = CCSequence::create(CCCallLambda::create([=]()
                                                            {
                                                                layer->setVisible(true);
                                                                layer->setPosition(ccp(0.0f, -winSize.height));
                                                            }),
                                                            CCEaseSineOut::create(CCMoveTo::create(0.5, ccp(0, 0))),
                                                            NULL);
    pushLayer(layer, defaultSlideIn);
}
    
void GameScene::popLayer()
{
    if(navStack->count())
    {
        ContentLayer* topLayer = (ContentLayer*)navStack->lastObject();
        
        // top layer must also be a direct child of this->middle
        assert(topLayer->getParent() == middle && "top layer is not a child of middle layer");
        
        // top layer is removed forever.
        topLayer->removeFromParentAndCleanup(true);
        navStack->removeLastObject();
    
        if(navStack->count())
        {
            ContentLayer* layer = (ContentLayer*)navStack->lastObject();

            // onEnter does this
            // layer->resumeSchedulerAndActions();
            middle->addChild(layer);

            showBottom(layer->isShowBottom);
            showTop(layer->isShowTop);
        }
    }
}
    
void GameScene::popAllLayers()
{
//	while (navStack->count())
//        popLayer();

    // DO NOT repeatedly invoke onEnter
    while(navStack->count())
    {
        ContentLayer* topLayer = (ContentLayer*)navStack->lastObject();
        
        // top layer must also be a direct child of this->middle
//        assert(topLayer->getParent() == middle && "top layer is not a child of middle layer");
        
        // top layer is removed forever.
        if(topLayer->getParent())
            topLayer->removeFromParentAndCleanup(true);
        else
            topLayer->cleanup();
        
        navStack->removeLastObject();
    }
}
    
void GameScene::selectTab(int tab)
{
    assert(tab>0 && tab<8 && "tab not in range.");

    ring("sound/v/button_click.mp3");
    
    if(navStack->count())
    {
        ContentLayer* layer = (ContentLayer*)(navStack->lastObject());
        if(!layer->tryExit(tab))
            return;
    }
    
    if(preTab >0 && preTab < 8)
        getTab(preTab-1)->onRelease();
    getTab(tab-1)->onPressed();
    preTab = tab;
    // is tabbing allowed in current layer?l

    popAllLayers();
    pushLayerWithDefaultAnimation(cCreators[tab]());
    //if(tab == 1)
        //GameScene::getInstance()->loadJiangHuState();
    
}
    
ButtonSprite* GameScene::getTab(int tab)
{
    return (ButtonSprite*)tabs[tab];
}
    
void GameScene::saveJiangHuState()
{
    /*if(preTab == 1 && navStack->count())
    {
        JiangHu* backLayer = (JiangHu*)navStack->objectAtIndex(0);
        
        if(backLayer->zhanyiItem == backLayer->itemSelected)
            GameConfig::shared()->stageTab = 0;
        else if(backLayer->jingjiItem == backLayer->itemSelected)
            GameConfig::shared()->stageTab = 1;
        else
            GameConfig::shared()->stageTab = -1;
        
        GameConfig::shared()->stageId = -1;
        for(JiangHuMenuItem* item: backLayer->items)
        {
            if(item->isItemOpen())
            {
                GameConfig::shared()->stageId = item->stage.stage;
                break;
            }
        }
    }*/
}
    
void GameScene::loadJiangHuState()
{
    /*if(preTab == 1 && navStack->count())
    {
        JiangHu* jianghu = (JiangHu*)navStack->objectAtIndex(0);
        if(GameConfig::shared()->stageTab == 0)
            jianghu->changeStage(jianghu->zhanyiItem);
        else if(GameConfig::shared()->stageTab == 1)
            jianghu->changeStage(jianghu->jingjiItem);
        
        for(JiangHuMenuItem* item : jianghu->items)
        {
            if(item->stage.stage == GameConfig::shared()->stageId)
                item->onTitleClick();
        }
    }*/
}

void GameScene::sendTabMsg(int tab , std::string msg)
{
    assert(tab>0 && tab<7 && "tab not in range.");
    
    //make sure it will not crash.
    if (!bottom || !bottom->getParent())
        return;
    
    ButtonSprite* chosenTab = getTab(tab-1);
    if(msg.empty() && bottom->getChildByTag(100*tab))
    {
        bottom->removeChildByTag(100*tab);
        bottom->removeChildByTag(200*tab);
    }
    else if(!msg.empty())
    {
        if(!bottom->getChildByTag(100*tab))
        {
            CCSprite* symbol = GetSprite("gameui/shuliang.png");
            symbol->setPosition(ccp(chosenTab->getPositionX()+50 ,chosenTab->getPositionY()+40));
            symbol->setTag(100*tab);
            bottom->addChild(symbol);
            CCLabelTTF* number = addLabelTTF(bottom, msg.c_str() , FONT_NUM, 26,
                                             ccp(chosenTab->getPositionX()+50 ,chosenTab->getPositionY()+40),
                                             ccp(0.5, 0.5) , ccc3(83, 0, 70));
            number->setTag(200*tab);
        }
        ((CCLabelTTF*)bottom->getChildByTag(200*tab))->setString(msg.c_str());
    }

}
 
void GameScene::battleSceneEnded(const std::string& gotoView)
{
    // load UI resources and recreate interface.
    scheduleRelogin();
    scheduleTabMsg();
    
    selectTab(1);
    
    if(gotoView == "pvp")
    {
        GameScene::getInstance()->pushLayerWithDefaultAnimation(PvPLayer::create());
    }
    else
    {
        JiangHu* jianghu = JiangHu::create((JiangHuType)GameConfig::shared()->stageTab);
        for(JiangHuMenuItem* item : jianghu->items)
        {
            if(item->stage.stage == GameConfig::shared()->stageId)
                item->onTitleClick();
        }
        GameScene::getInstance()->pushLayerWithDefaultAnimation(jianghu);
    }
    
    checkNeedRelogin();
}

}
