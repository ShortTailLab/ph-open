#include "BuildConf.h"
#include "DebugMisc.h"
#include "More.h"
#include "extensions/utils.h"
#include "ControlExtensions.h"
#include "GameScene.h"
#include "HeroTable.h"
#include "HeroProfileLayer.h"
#include "Board/Common.h"
#include "BuildConf.h"
#include <boost/make_shared.hpp>
#include <set>
#include <support/user_default/CCUserDefault.h>
#include "SystemInform.h"
#include "WebViewLayer.h"
#include "ZqlScrollView.h"
#include "HeroIcon.h"
#include "UIPresets.h"
#include "PuzzleRPC.h"
#include "URLHelpers.h"
#include "CCNative.h"

#if PH_USE_WEIBO
#include <auth/Weibo.h>
#endif

namespace PH
{

USING_NS_CC;
    
void logOutAux(bool doLogOut=true)
{
    boost::function<void ()> toLogout = [=]()
    {
#if PH_USE_WEIBO
        Weibo<Sina>::instance().clearLoginInfo();
        Weibo<Tencent>::instance().clearLoginInfo();
#endif
        
        PlayerData::getInstance()->userLoggedOff();
        CCUserDefault::sharedUserDefault()->setStringForKey("guestLoginID", "");
        CCUserDefault::sharedUserDefault()->setStringForKey("guestLoginPWD", "");
        CCUserDefault::sharedUserDefault()->flush();
        
        GameScene::getInstance()->stopReloginSchedule();
        GameScene::getInstance()->unscheduleTabMsg();
        GameScene::getInstance()->clearUI();
        GameScene::getInstance()->loginUI();
        
        PuzzleRPC::getInstance()->disconnect();
    };
    

    if(PlayerData::getInstance()->player->accounts.size() == 1 &&
       PlayerData::getInstance()->player->accounts[0].type == AccountType::Guest)
    {
        SystemInform::alertConfirmPanel(GameScene::getInstance(),
                                        "你现在是游客登录状态，登出后将无法再次登录，确定要登出吗？",
                                        "警告",
                                        toLogout,
                                        [](){},
                                        true);
    }
    else if(PlayerData::getInstance()->player->accounts.size() == 1 &&
            PlayerData::getInstance()->player->accounts[0].type == AccountType::Main)
    {
        SystemInform::alertConfirmPanel(GameScene::getInstance(),
                                        "确定要登出吗？",
                                        "警告",
                                        toLogout,
                                        [](){},
                                        true);
    }
    else
        toLogout();
    
    ring("sound/v/button_click.mp3");
}

class GalleryBoard : public HeroBoard
{
public:
    GalleryBoard() { displayPlaceholder = true; }
    virtual ~GalleryBoard(){}
    
    virtual void update(float dt)
    {
        if(this->getParent() == NULL) return;
        
        CCPoint screenPoint = this->getParent()->convertToWorldSpace(this->getPosition());
        
        //[hack] game scene push/pop mechanism may disturb this optimization
        if(this->mKidsCount == this->getChildrenCount()){
            if(std::abs(screenPoint.y-this->mLastPos) < HeroBoard::skHeroIconHeight) return;
        }
        else
            this->mKidsCount = this->getChildrenCount();
        
        this->removeAllChildren();
        
        this->mLastPos = screenPoint.y;
        int firstRow = (screenPoint.y - this->mClipUpper)/HeroBoard::skHeroIconHeight-1;
        int lastRow = (this->mClipUpper - this->mClipLower) / HeroBoard::skHeroIconHeight + firstRow+3;
        
        for(int i=0; i<this->heroImgVec.size(); i++)
        {
            int r = this->mHeroImgFlag[i];
            if(r>=firstRow && r<lastRow)
                this->addChild(this->heroImgVec[i]);
        }
        for(int i=0; i<this->placeHolder.size(); i++)
        {
            int r = this->mHolderImgFlag[i];
            //printf("%d -> %d\n",i, r);
            if(r>=firstRow && r<lastRow)
                this->addChild(this->placeHolder[i]);
        }
    }
    bool initWithSeenAndOwned(const std::vector<int>& seenIds, const std::vector<int>& ownedId)
    {
        if (!ContentLayer::init())
            return false;
     
        this->mClipUpper = 800;
        
        fullHeroProfiles = heroProfiles();
        capacity = fullHeroProfiles.size();
        
        refresh(seenIds, ownedId);
        
        return true;
    }
    
	static HeroBoard* createWithSeenAndOwned(const std::vector<int>& seenIds, const std::vector<int>& ownedId)
	{
		GalleryBoard* o = new GalleryBoard();
		if(o != NULL && o->initWithSeenAndOwned(seenIds, ownedId))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	}
    
    void refresh(const std::vector<int>& seenIds, const std::vector<int>& ownedId)
    {
        std::set<int> seen(seenIds.begin(), seenIds.end());
        std::set<int> owned(ownedId.begin(), ownedId.end());
        
        this->clear();
        
        for (size_t i=0;i<fullHeroProfiles.size();i++)
        {
            HeroProfile* profile = fullHeroProfiles[i];
            if (seen.count(profile->tableId))
            {
                HeroIcon *icon = HeroIcon::createWithProfile(profile);
                *icon->getHero() = Hero("i", profile->tableId, profile->maxLevel, 0);
                icon->setEnabled(owned.count(profile->tableId));
                icon->showNum(i+1);
                icon->retain();
                
                addHero(icon, i);
                this->mHeroImgFlag.push_back(i/6);
                this->heroImgVec.push_back(icon);
            }
            else
            {
                CCSprite *s = GetSprite("gameui/zhanwei_biankuang.png");
                s->retain();
                
                addHero(s, i);
                this->mHolderImgFlag.push_back(i/6);
                this->placeHolder.push_back(s);
            }
        }
    }
protected:
    virtual void clear()
    {
        HeroBoard::clear();
        this->mHeroImgFlag.clear();
        this->mHolderImgFlag.clear();
    }
    
private:
    std::vector<HeroProfile*> fullHeroProfiles;
    std::vector<int> mHeroImgFlag, mHolderImgFlag;
};

bool ShowLayer::init()
{
	if(!ContentLayer::init())return false;

	scrollView = ZqlScrollView::create();
	scrollView->setViewSize(CCSize(winSize.width, 900),
                            CCRect(0, -110, winSize.width, 610));

	board = GalleryBoard::createWithSeenAndOwned(PlayerData::getInstance()->player->seen, PlayerData::getInstance()->player->owned);
	scrollView->addLayer(board);
	scrollView->setPositionY( 70 );
	this->addChild(scrollView);

	CCMenu *topMenu = CCMenu::create();
	CCMenuItem *itemLeft = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                                    GetSprite("gameui/back_t.png"),
                                                    this,
                                                    menu_selector(ShowLayer::onBack));
	CCMenuItem *itemRight = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                        GetSprite("gameui/hongchengdi_t_tongyong.png"));
    
    LabelX* name = LabelX::create("图鉴", FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp( 141 , 35));
    itemRight->addChild(name);

    {
        int seenCount = PlayerData::getInstance()->player->seen.size();
        int totalCount = heroProfiles().size();
        
        CCSprite* heroNumBg = GetSprite("gameui/duiwurenshu.png");
        heroNumBg->setAnchorPoint(ccp(1 , 0.5));
        heroNumBg->setPosition(ccp(winSize.width*0.5f , -634));

        CCLabelTTF* numLabel = NULL;
        addLabelTTF(heroNumBg, numLabel, "", FONT_NUM, 24, ccp(100 , heroNumBg->getContentSize().height*0.5f) , ccp(0.5 , 0.5));
        numLabel->setString((toStr(seenCount) + "/" + toStr(totalCount)).c_str());
        
        this->addChild(heroNumBg, 1);
    }
    
	itemLeft->setAnchorPoint(ccp(0.5 , 1));
	itemRight->setAnchorPoint(ccp(0.5 , 1));
	topMenu->addChild(itemLeft);
	topMenu->addChild(itemRight);

	topMenu->alignItemsHorizontallyWithPadding(8);
	topMenu->setPosition(ccp(-70 , 18));
    
	addChild(topMenu);
    setTouchEnabled(true);

	return true;
}

void ShowLayer::onBack(cocos2d::CCObject *target)
{
	GameScene::getInstance()->popLayer();
    ring("sound/v/button_click.mp3");
}
    
void ShowLayer::onSingleClick(cocos2d::CCTouch *touch)
{
    onLongClick(touch);
}
    
void ShowLayer::onLongClick(CCTouch* touch)
{
    CCPoint localToScrollView = scrollView->convertTouchToNodeSpace(touch);
    CCPoint localPoint = convertTouchToNodeSpace(touch);
	CCPoint worldPoint = convertToWorldSpace(localPoint);
    
    HeroIcon* icon = NULL;
    if(scrollView->boundingBox().containsPoint(localToScrollView))
        icon = board->getHeroImgByPoint(worldPoint);
    
    if(icon)
    {
        
        auto* layer = HeroProfileLayer::create(icon->getHero() , false);
        GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
    }
}
    

bool ResDownloadLayer::init()
{
    if(! CCLayer::init() )
        return false;
    
    this->setTouchEnabled(true);
    
    CCLayerColor* background = CCLayerColor::create(ccc4(0, 0, 0, 100));
    this->addChild(background, -1);
    
    //------------------
    // create bakcground
    CCScale9Sprite* bg = Get9SpriteFromFrame("dialog_bg.png", CCRectMake(50, 50, 208, 112));
    bg->setContentSize(CCSize(500, 250));
    this->addChild(bg);
    
    mText = CCLabelTTF::create("检查新资源...", FONT_CN, 28);
    mText->setPosition(ccp(250, 150));
    bg->addChild(mText);
    
    CCLabelTTF* title = CCLabelTTF::create("下载资源", FONT_CN, 30);
    title->setAnchorPoint(ccp(0.5, 0.0));
    title->setPosition(ccp(250, 185));
    bg->addChild(title);
    
    mProgressBar = GetSprite("loading_b_2.png");
    mProgressBar->setAnchorPoint(ccp(0, 0.5));
    mProgressBar->setPosition(ccp(28, 108));
    setProgress(0);
    bg->addChild(mProgressBar);
    
    // this must be created after progress bar
    CCSprite* barBG = GetSprite("loading_b_1.png");
    barBG->setAnchorPoint(ccp(0, 0.5));
    barBG->setPosition(ccp(22, 108));
    bg->addChild(barBG);
    
    bg->setPosition(ccp(320, 480));
    
    /*
    ButtonSprite* cancel = ButtonSprite::create(createBtn("取消", ButtonType::NORMAL_RED),
                                                createBtn("取消", ButtonType::NORMAL_RED),
                                                UI_TOUCH_ORDER_MODAL_MENU);
    cancel->setPosition(ccp(250, 60));
    cancel->setOnClickListener([=]()
    {
        syncMan->stop();
        this->removeFromParentAndCleanup(true);
    });
    bg->addChild(cancel);
    */
    runOnMainThread([=](){ download(); });
    
//    this->setTouchEnabled(true);
    
    return true;
}
 /*
void ResDownloadLayer::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,
                                                                            UI_TOUCH_ORDER_MODAL,
                                                                            true);
}*/

bool ResDownloadLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    return true;
}
    
void ResDownloadLayer::setProgress(float progress)
{
    float val = clampx(progress, 0.0f, 1.0f);
    mProgressBar->setScaleX(val);
}

std::string ResDownloadLayer::bytesToReadableString(int bytes)
{
    char stringBuf[256] = {};
    const char* postfixes[] = {"Bytes", "KB", "MB", "GB"};
    int pfixCount = sizeof(postfixes) / sizeof(postfixes[0]);
    int pfix = 0;
    float fbytes = bytes;
    
    while (fbytes > 1100.f && pfix < pfixCount-1)
    {
        fbytes /= 1024.f;
        pfix++;
    }
    
    sprintf(stringBuf, "%.2f%s", fbytes, postfixes[pfix]);
    return stringBuf;
}
    
void ResDownloadLayer::download()
{
    if(syncMan)
        syncMan.reset();
    
    syncMan = boost::make_shared<ResourceSyncMan>(true);
    syncMan->resetLastMod();
    
    syncMan->startSync([=](const ResourceSyncMan& man, const SyncProgress& s)
    {
        if (s.state < DATA_DOWNLOADING)
        {
            setProgress(0.1 * s.state);
            LOGD("progress %.2f\n", s.state * 0.1);
        }
        else if (s.state == DATA_DOWNLOADING)
        {
            int percent = (float)s.downloadedBytes/(float)s.totalBytes * 100;
            
            setProgress(0.2 + 0.6*((float)s.downloadedBytes/(float)s.totalBytes));
            char stringBuf[256] = {0};
//            sprintf(stringBuf, "下载中: %s/%s...",
//                    bytesToReadableString(s.downloadedBytes).c_str(),
//                    bytesToReadableString(s.totalBytes).c_str());
//            sprintf(stringBuf, "下载中: %.0f%...", (float)s.downloadedBytes / (float)s.totalBytes * 100);
            sprintf(stringBuf, "下载中: %d%%", percent);
            mText->setString(stringBuf);
        }
        else if (s.state == DECOMPRESS)
        {
            setProgress(0.9);
            mText->setString("解压资源中....");
            LOGD("解压资源中...\n");
        }
        else if (s.state == DONE)
        {
            setProgress(1);
            LOGD("更新完成...\n");
            mText->setString("更新完成....");
            GameConfig::shared()->setAndroidHDEnabled(true);
            
            runOnMainThread([=]()
            {
                runOnMainThread([=]()
                {
                    this->removeFromParentAndCleanup(true);
                    SystemInform::alertMessage(runningScene(),
                                               "资源已经更新完毕，请在后台关闭游戏后重新打开，将会使用新资源",
                                               "资源更新完毕",
                                               [](){});
                });
            });
        }
        else if (s.state == NOUPDATE)
        {
            setProgress(1);
            mText->setString("已是最新版本....");
            
            runOnMainThread([=]()
            {
                runOnMainThread([=]()
                {
                    this->removeFromParentAndCleanup(true);
                    
                    SystemInform::alertMessage(runningScene(),
                                               "资源已经更新完毕，请在后台关闭游戏后重新打开，将会使用新资源",
                                               "资源更新完毕",
                                               [](){});
                });
            });

            LOGD("已是最新版本...\n");
            GameConfig::shared()->setAndroidHDEnabled(true);
        }
        else
        {
            mText->setString("更新错误，请检查你的网络设置....");
            LOGD("更新错误，请检查你的网络设置...\n");
        }

        PuzzleRPC::getInstance()->setNetworkTimeOut(10);
    });
}

bool SettingJuanZhou::init()
{
	if(!JuanZhou::init())
        return false; 

	viewPort = CCSize(20 , 330);
    setTouchEnabled(true);
    
	CCSprite *bg = GetSprite("gameui/shezhi_1.png");
	bg->setAnchorPoint(ccp(0.5 , 1));
	addChild(bg);

    int maxButtonCount = 4;
#if 0//(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    maxButtonCount = 6;
#endif
    
    for(int i = 0; i < maxButtonCount; i++)
    {
        int px = -49+179*(i%2);
        int py = -67-i/2*95;
        CCSprite* circle = GetSprite("gameui/shezhi_xuanzhong.png");
        circle->setPosition(ccp(px, py));
        circles.push_back(circle);
        addChild(circle);
        
        btns.push_back(CCRectMake(px-15, py-30, 140, 50));
    }
    
	if(GameConfig::shared()->isBGMEnabled())
    {
        circles[0]->setVisible(true);
        circles[1]->setVisible(false);
    }
    else
    {
        circles[1]->setVisible(true);
        circles[0]->setVisible(false);
    }
    
	if(GameConfig::shared()->isSFXEnabled())
    {
        circles[2]->setVisible(true);
        circles[3]->setVisible(false);
    }
    else
    {
        circles[3]->setVisible(true);
        circles[2]->setVisible(false);
    }
    
#if 0//(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if(GameConfig::shared()->isAndroidHDEnabled())
    {
        circles[4]->setVisible(true);
        circles[5]->setVisible(false);
    }
    else
    {
        circles[5]->setVisible(true);
        circles[4]->setVisible(false);
    }
#else
    CCSprite* blocker = GetSprite("gameui/shezhi_1_zhe.png");
    blocker->setAnchorPoint(ccp(1.0, 1.0));
    blocker->setPosition(ccp(282, -70-3.0/2*95));
    this->addChild(blocker, 3);
#endif

	return true;
}

void SettingJuanZhou::onSingleClick(CCTouch* touch)
{
    CCPoint localPoint = convertTouchToNodeSpace(touch);
    for(int i = 0; i < btns.size(); i++)
        if(btns[i].containsPoint(localPoint))
        {
            if(i == 0)
                onMusicOn();
            else if(i == 1)
                onMusicOff();
            else if(i == 2)
                onSoundOn();
            else if(i == 3)
                onSoundOff();
            else if(i == 4)
                onUseHD();
            else
                onUseSD();
            break;
        }
}

void SettingJuanZhou::onMusicOn()
{
	circles[0]->setVisible(true);
    circles[1]->setVisible(false);
    
    GameConfig::shared()->setBGMEnabled(true);
    playBG("sound/bg/ui.mp3");
    ring("sound/v/button_click.mp3");
}
void SettingJuanZhou::onMusicOff()
{
    circles[1]->setVisible(true);
    circles[0]->setVisible(false);
    
    GameConfig::shared()->setBGMEnabled(false);
    stopBgMusic();
    
    ring("sound/v/button_click.mp3");
}

void SettingJuanZhou::onSoundOn()
{
    circles[2]->setVisible(true);
    circles[3]->setVisible(false);
    
    GameConfig::shared()->setSFXEnabled(true);
    ring("sound/v/button_click.mp3");
}
void SettingJuanZhou::onSoundOff()
{
    circles[3]->setVisible(true);
    circles[2]->setVisible(false);
    
    GameConfig::shared()->setSFXEnabled(false);
    ring("sound/v/button_click.mp3");
}
    
void SettingJuanZhou::onUseHD()
{    
    SystemInform::alertConfirmPanel(runningScene(),
                                    "高分辨率游戏包需要60M，\n请使用wifi网络进行下载。\n是否继续？",
                                    "下载资源",
                                    [=]()
    {
        // make sure we download a whole copy of tar.
        std::string docPath = CCNative::getDocPath();
        CCNative::removeFile(docPath + "iphonehd.3.1.tar");
        
        ResDownloadLayer* downloader = ResDownloadLayer::create();
        runningScene()->addChild(downloader);
    },
                                    [](){});
    
    circles[4]->setVisible(true);
    circles[5]->setVisible(false);

    LOGD("use HD\n");
}

void SettingJuanZhou::onUseSD()
{
    circles[5]->setVisible(true);
    circles[4]->setVisible(false);

    GameConfig::shared()->setAndroidHDEnabled(false);
    LOGD("use SD\n");
}
    
bool SettingLayer::init(bool isInStack)
{
	if(!ContentLayer::init())
        return false;

    mIsInStack = isInStack;
    
	CCMenu *topMenu = CCMenu::create();
	CCMenuItem *itemLeft = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"), GetSprite("gameui/back_t.png"),
													this, menu_selector(SettingLayer::onBack));
	CCMenuItem *itemRight = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"), GetSprite("gameui/hongchengdi_t_tongyong.png"));
	itemLeft->setAnchorPoint(ccp(0.5 , 1));
	itemRight->setAnchorPoint(ccp(0.5 , 1));
    
    LabelX* name = LabelX::create("设置", FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp( 141 , 34));
    itemRight->addChild(name);
    
	topMenu->addChild(itemLeft );
	topMenu->addChild(itemRight );

	topMenu->alignItemsHorizontallyWithPadding(8);
	topMenu->setPosition(ccp(-70 , 18));
	addChild(topMenu);


    JuanZhou *bg = SettingJuanZhou::create();
    
	CCSprite *zhou1 = GetSprite("gameui/shezhi_2.png");
	CCSprite *zhou2 = GetSprite("gameui/shezhi_2.png");
    zhou2->setFlipX(true);
    
	jz = JuanZhouLayer::create();
	jz->setPositionY(-30);
	jz->addSkin(bg , zhou1 , zhou2);
	addChild(jz);

	return true;
}

void SettingLayer::onEnterAnimationDone()
{
	jz->startAnim();
}

void SettingLayer::onBack(CCObject *target)
{
    if(mIsInStack)
        GameScene::getInstance()->popLayer();
    else
        this->removeFromParent();
    ring("sound/v/button_click.mp3");
}
    
    
/////////////////////////////////
//AboutLayer
/////////////////////////////////
    
bool AccountJuanZhou::init()
{
    if(!JuanZhou::init())
        return false;
    
    viewPort = CCSize(20 , 496);
    setTouchEnabled(true);
    editText = NULL;
    
    const std::vector<Account>& account = PlayerData::getInstance()->player->accounts;
    isBindSina = find_if(account.begin() , account.end() , [=](const Account& a){return a.type == AccountType::SinaWeibo;} ) != account.end();
    isBindQQ = find_if(account.begin() , account.end() , [=](const Account& a){return a.type == AccountType::QQ;} ) != account.end();

    int adjust = 0;

    bg = GetSprite("gameui/zhanghu_1_3.png");
    bg->setAnchorPoint(ccp(0.5 , 1));
    addChild(bg);
    
    CCSprite *temp1 = isBindSina ? createBtn("已绑定", ButtonType::THREE_WORD_BLUE) : createBtn("绑 定", ButtonType::THREE_WORD_RED);
    CCSprite *temp2 = isBindQQ  ? createBtn("已绑定", ButtonType::THREE_WORD_BLUE) : createBtn("绑 定", ButtonType::THREE_WORD_RED);
    
    CCMenu *menu2 = CCMenu::create();
    bindSina = CCMenuItemSprite::create(temp1, temp1, this, menu_selector(AccountJuanZhou::onBindSina));
    bindQQ = CCMenuItemSprite::create(temp2, temp2, this, menu_selector(AccountJuanZhou::onBindQQ));
	menu2->addChild(bindSina);
    menu2->addChild(bindQQ);
    menu2->alignItemsVerticallyWithPadding(15);
	menu2->setPosition(ccp(180 , -260));
	addChild(menu2);
    
    addLabelTTF(this, PlayerData::getInstance()->player->referCode.c_str(),
                FONT_CN, 30, ccp(-160, -380), ccp(0, 0.5), ccWHITE);
    
    menu = CCMenu::create();
    CCMenuItemSprite *change = CCMenuItemSprite::create(createBtn("修 改", ButtonType::THREE_WORD_RED),
                                                        createBtn("修 改", ButtonType::THREE_WORD_RED),
                                                        this,
                                                        menu_selector(AccountJuanZhou::onChangeName));
	menu->addChild(change );
	menu->setPosition(ccp(180 , -93+adjust));
	addChild(menu);
    
    CCScale9Sprite* sp = Get9SpriteFromFrame("zhanghu_1_4.png", 5, 5, 536, 36);
    
    //    CCScale9Sprite* sp = CCScale9Sprite::create("gameui/suosou_1_3_k.png", CCRectMake(0, 0, 488, 60), CCRectMake(10, 10, 468, 40));
    editText = CCEditBox::create(CCSizeMake(540, 45), sp);
    editText->setFontColor(ccWHITE);
    editText->setPlaceholderFontColor(ccc3(0xdc, 0xdc, 0xdc));
    editText->setText(PlayerData::getInstance()->player->name.c_str());
    editText->setPosition(ccp(-270, -148+adjust));
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#else
    editText->setPlaceHolder("昵称在20字以内");
#endif
    editText->setAnchorPoint(ccp(0, 0.5));
    editText->setInputMode(kEditBoxInputModeAny);
    addChild(editText,100);
    
    return true;
};
    
void AccountJuanZhou::onChangeName(cocos2d::CCObject *target)
{
    if(!editText)
        return;
    
    std::string name = editText->getText();
    std::wstring wName = URLHelper::str2wstr(name);
    std::string msg = wName.empty() ? "昵称不能为空" : (wName.size() > 20 ? "字数不能超过20" : "");
    if(!msg.empty()){
        SystemInform::alertMessage(GameScene::getInstance(), msg.c_str(), "警告", [](){});
        return;
    }
    
    SystemInform *inform = SystemInform::alertLoading(GameScene::getInstance());
    
    LOGD("show text: %s\n", editText->getText());
    
    PuzzleRPC::getInstance()->updateUserProfile(PlayerData::getInstance()->getToken(),
                                                editText->getText(),
                                                "",
                                                [=](std::auto_ptr<Player> player, std::auto_ptr<Error> e)
                                                {
                                                    inform->cancel();
                                                    if(e.get())
                                                    {
                                                        if(e->msg.size())
                                                            SystemInform::alertMessage(runningScene(),
                                                                                       e->msg.c_str(),
                                                                                       "改名错误",
                                                                                       [](){});
                                                        else
                                                            e->handleErrorInform();
                                                            
                                                        return;
                                                    }
                                                    
                                                    PlayerData::getInstance()->updatePlayer(player.get());
                                                    
                                                    editText->setText(PlayerData::getInstance()->player->name.c_str());
                                                });
    
    ring("sound/v/button_click.mp3");
}
void AccountJuanZhou::onBindSina(cocos2d::CCObject *target)
{
#if PH_USE_WEIBO
    
    if(!isBindSina)
    {
        SystemInform *inform = SystemInform::alertLoading(GameScene::getInstance());
        
        Weibo<Sina>::instance().authorize([=](std::auto_ptr<AuthError> err)
        {
            if (!err.get())
            {
                PuzzleRPC::getInstance()->bindAccount(PlayerData::getInstance()->getToken(),
                                                      AccountType::SinaWeibo,
                                                      Weibo<Sina>::instance().userId(),
                                                      Weibo<Sina>::instance().token(),
                                                      [=](std::auto_ptr<Player> p, std::auto_ptr<Error> err)
                {
                    inform->cancel();
                    if(err.get())
                    {
                        err->handleErrorInform();
                        Weibo<Sina>::instance().clearLoginInfo();
                        return;
                    }
                    PlayerData::getInstance()->updatePlayer(p.get());
                    auto boundBtn = createBtn("已绑定", ButtonType::THREE_WORD_BLUE);
                    
                    bindSina->setNormalImage(boundBtn);
                    bindSina->setSelectedImage(boundBtn);
                    isBindSina = true;
                    
                    SystemInform::alertMessage(GameScene::getInstance(), "帐号绑定成功！", "提示", [](){});
                });
            }
            else
            {
                inform->cancel();
                if (!err->errorDesc.empty())
                    SystemInform::alertMessage(GameScene::getInstance(), err->errorDesc.c_str(), "提示", [](){});
            }
        });
        ring("sound/v/button_click.mp3");
    }
#endif
}
void AccountJuanZhou::onBindQQ(cocos2d::CCObject *target)
{
#if PH_USE_WEIBO
    if(!isBindQQ)
    {
        SystemInform *inform = SystemInform::alertLoading(GameScene::getInstance());
        Weibo<Tencent>::instance().authorize([=](std::auto_ptr<AuthError> err)
        {
            if (!err.get())
            {
                PuzzleRPC::getInstance()->bindAccount(PlayerData::getInstance()->getToken(),
                                                      AccountType::QQ,
                                                      Weibo<Tencent>::instance().userId(),
                                                      Weibo<Tencent>::instance().token(),
                                                      [=](std::auto_ptr<Player> p, std::auto_ptr<Error> err)
                {
                    inform->cancel();
                    if(err.get())
                    {
                        err->handleErrorInform();
                        Weibo<Tencent>::instance().clearLoginInfo();
                        return;
                    }
                    PlayerData::getInstance()->updatePlayer(p.get());
                    auto boundBtn = createBtn("已绑定", ButtonType::THREE_WORD_BLUE);
                    
                    bindQQ->setNormalImage(boundBtn);
                    bindQQ->setSelectedImage(boundBtn);
                    isBindQQ = true;
                    
                    SystemInform::alertMessage(GameScene::getInstance(), "帐号绑定成功！", "提示", [](){});
                });
            }
            else
            {
                inform->cancel();
                if (!err->errorDesc.empty())
                    SystemInform::alertMessage(GameScene::getInstance(), err->errorDesc.c_str(), "提示", [](){});
            }
        });
        ring("sound/v/button_click.mp3");
    }
#endif
}
    

bool AccountLayer::init()
{
    if(!ContentLayer::init())
        return false;
    
    CCMenu *topMenu = CCMenu::create();
	CCMenuItem *itemBack = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                                    GetSprite("gameui/back_t.png"),
													this,
                                                    menu_selector(AccountLayer::onBack));
    
	CCMenuItem *itemTitle = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                     GetSprite("gameui/hongchengdi_t_tongyong.png"));
	itemBack->setAnchorPoint(ccp(0.5 , 1));
	itemTitle->setAnchorPoint(ccp(0.5 , 1));
    
    LabelX* name = LabelX::create("帐户设置", FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp( 141 , 34));
    itemTitle->addChild(name);
    
    CCMenuItem *itemLogout = CCMenuItemSprite::create(createBtn("登出", ButtonType::NORMAL_RED),
                                                      createBtn("登出", ButtonType::NORMAL_RED) ,
                                                      this,
                                                      menu_selector(AccountLayer::onLogout));
    itemLogout->setAnchorPoint(ccp(0.5 , 1));
    
	topMenu->addChild(itemBack);
	topMenu->addChild(itemTitle);
    topMenu->addChild(itemLogout);
    
	topMenu->alignItemsHorizontallyWithPadding(8);
	topMenu->setPosition(ccp(0 , 18));
	addChild(topMenu);
    
    JuanZhou *bg = AccountJuanZhou::create();
    CCSprite *zhou1 = GetSprite("gameui/zhanghu_1_2_m.png");
	CCSprite *zhou2 = GetSprite("gameui/zhanghu_1_2_m.png");
    
    zhou2->setFlipX(true);
    
	jz = JuanZhouLayer::create();
	jz->setPositionY(-30);
	jz->addSkin(bg , zhou1 , zhou2);
	addChild(jz);
    
    return true;
}
    
void AccountLayer::onEnterAnimationDone()
{
    jz->startAnim();
}

void AccountLayer::onBack(CCObject *target)
{
    GameScene::getInstance()->popLayer();
    ring("sound/v/button_click.mp3");
}

void AccountLayer::onLogout(cocos2d::CCObject *target)
{
    logOutAux();
}
    
    
/////////////////////////////////
//AboutLayer
/////////////////////////////////
bool AboutLayer::init()
{
    if(!ContentLayer::init())return false;
    
    CCMenu *topMenu = CCMenu::create();
	CCMenuItem *itemLeft = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                                    GetSprite("gameui/back_t.png"),
													this,
                                                    menu_selector(AboutLayer::onBack));
	CCMenuItem *itemRight = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"), GetSprite("gameui/hongchengdi_t_tongyong.png"));
	itemLeft->setAnchorPoint(ccp(0.5 , 1));
	itemRight->setAnchorPoint(ccp(0.5 , 1));
    
    LabelX* name = LabelX::create("关于我们", FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp( 141 , 34));
    itemRight->addChild(name);
    
	topMenu->addChild(itemLeft );
	topMenu->addChild(itemRight );
    
	topMenu->alignItemsHorizontallyWithPadding(8);
	topMenu->setPosition(ccp(-70 , 18));
	addChild(topMenu);
    
    JuanZhou *bg = JuanZhou::create();
    bg->viewPort = CCSize(20 , 604);
    CCSprite *bgImg = GetSprite("gameui/lianxiwomen_1_3.png");
    
    bgImg->setAnchorPoint(ccp(0.5 , 1));
    bg->addChild(bgImg);
    
	CCSprite *zhou1 = GetSprite("gameui/lianxiwomenzhou.png");
	CCSprite *zhou2 = GetSprite("gameui/lianxiwomenzhou.png");
    zhou2->setFlipX(true);
    
	jz = JuanZhouLayer::create();
	jz->setPositionY(-30);
	jz->addSkin(bg , zhou1 , zhou2);
	addChild(jz);
    
    return true;
}
    
void AboutLayer::onEnterAnimationDone()
{
    jz->startAnim();
}

void AboutLayer::onBack(CCObject *target)
{
    GameScene::getInstance()->popLayer();
    ring("sound/v/button_click.mp3");
}

//----------------------
// Help Layer
//----------------------
bool HelpLayer::init()
{
    if(!ContentLayer::init())
        return false;
    
    WebViewLayer* webview = WebViewLayer::create(CCRect(20, 150, 600, 590));
    webview->opaque(false);
    
#if PH_DEBUG_BUILD
    webview->openURL("http://ph-test.shorttaillab.com/static/gl.html");
#else
    webview->openURL("http://pt.shorttaillab.com/static/gl.html");
#endif

    webview->beforeStartLoadRequest = [](CCHttpRequest&)->bool{ return true; };
    
    this->addChild(webview);

    
    CCMenu *topMenu = CCMenu::create();

	CCMenuItem *itemLeft = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                                    GetSprite("gameui/back_t.png"),
													this,
                                                    menu_selector(HelpLayer::onBack));
	itemLeft->setAnchorPoint(ccp(0.5 , 1));
    topMenu->addChild(itemLeft);
    
    CCMenuItem *itemRight = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                     GetSprite("gameui/hongchengdi_t_tongyong.png"));
	itemRight->setAnchorPoint(ccp(0.5 , 1));
    
    LabelX* name = LabelX::create("游戏攻略", FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp( 141 , 35));
    itemRight->addChild(name);
    topMenu->addChild(itemRight);

    topMenu->alignItemsHorizontallyWithPadding(8);
	topMenu->setPosition(ccp(-70 , 18));
    this->addChild(topMenu);
    
    return true;
}
    
void HelpLayer::onBack(CCObject *target)
{
    GameScene::getInstance()->popLayer();
    ring("sound/v/button_click.mp3");
}

//----------------------
// More Layer
//----------------------
bool MoreLayer::init()
{
	if(!ContentLayer::init())
        return false;

	ContentLayer::setTouchEnabled(true);
    isClick = false;
    
    CCPoint pos = ccp(264, 60);
    
    ZqlScrollView* scrollView = ZqlScrollView::create();
    scrollView->setViewSize(CCSizeMake(winSize.width, 900), CCRectMake(0, -50, winSize.width, 650));
    scrollView->setPosition(ccp(0, 70));
    addChild(scrollView);
    
    CCSprite* gonglue = GetSprite("gameui/caidan_1_ty.png");
    gonglue->addChild(createTitleLabel("攻略", pos));
    gonglue->setContentSize(CCSizeMake(gonglue->getContentSize().width, gonglue->getContentSize().height+8));
    gonglue->setTag(0);
    items.push_back(gonglue);
    scrollView->addLayer(gonglue);
    CCSprite*  tujian= GetSprite("gameui/caidan_1_ty.png");
    tujian->addChild(createTitleLabel("图鉴", pos));
    tujian->setContentSize(CCSizeMake(tujian->getContentSize().width, tujian->getContentSize().height+8));
    tujian->setTag(1);
    items.push_back(tujian);
    scrollView->addLayer(tujian);
    CCSprite* setting = GetSprite("gameui/caidan_1_ty.png");
    setting->setContentSize(CCSizeMake(setting->getContentSize().width, setting->getContentSize().height+8));
    setting->setTag(2);
    items.push_back(setting);
    setting->addChild(createTitleLabel("设置", pos));
    scrollView->addLayer(setting);
    
    CCSprite* account = GetSprite("gameui/caidan_1_ty.png");
    account->addChild(createTitleLabel("帐号设置", pos));
    account->setContentSize(CCSizeMake(account->getContentSize().width, account->getContentSize().height+8));
    account->setTag(3);
    items.push_back(account);
    scrollView->addLayer(account);
    
    CCSprite* contact = GetSprite("gameui/caidan_1_ty.png");
    contact->addChild(createTitleLabel("关于我们", pos));
    contact->setContentSize(CCSizeMake(contact->getContentSize().width, contact->getContentSize().height+8));
    contact->setTag(6);
    items.push_back(contact);
    scrollView->addLayer(contact);

	return true;
}

void MoreLayer::setTouchEnabled(bool value)
{
	ContentLayer::setTouchEnabled(value);
	//menu->setTouchEnabled(value);
}
    
void MoreLayer::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    isClick = true;
}
void MoreLayer::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    CCTouch *touch = (CCTouch *)pTouches->anyObject();
	CCPoint currPoint = touch->getLocationInView();
	CCPoint prevPoint = touch->getPreviousLocationInView();
    
	if( ccpDistance(currPoint, prevPoint) > 5)
        isClick = false;
}
void MoreLayer::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    CCTouch *touch = (CCTouch *)pTouches->anyObject();
    if(isClick)
    {
        for(CCSprite* item : items)
        {
            CCPoint localPoint = item->getParent()->convertTouchToNodeSpace(touch);
            if(item->boundingBox().containsPoint(localPoint))
            {
                switch (item->getTag())
                {
                    case 0:
                        onHelp(NULL);
                        break;
                    case 1:
                        onTuJian(NULL);
                        break;
                    case 2:
                        onSetting(NULL);
                        break;
                    case 3:
                        onAccountSettings(NULL);
                        break;
                    case 6:
                        onAboutUs(NULL);
                        break;
                    case 11:
                        exitGame();
                        break;
                    default:
                        break;
                }
            }
        }
        isClick = false;
    }
}

void MoreLayer::onTuJian(cocos2d::CCObject *target)
{
	GameScene::getInstance()->pushLayerWithDefaultAnimation(ShowLayer::create());
    ring("sound/v/button_click.mp3");
}
    
void MoreLayer::onSetting(cocos2d::CCObject *target)
{
    auto layer = SettingLayer::create();
    GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
    ring("sound/v/button_click.mp3");
}
    
void MoreLayer::onAccountSettings(cocos2d::CCObject *target)
{
    auto layer = AccountLayer::create();
    GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
    ring("sound/v/button_click.mp3");
}
void MoreLayer::onGongGao()
{
    SystemInform::alertAnnoucement(GameScene::getInstance(),
                                   "公告",
                                   PlayerData::getInstance()->annoucement,
                                   [=](){});
}
    
void MoreLayer::onAboutUs(cocos2d::CCObject *target)
{
    AboutLayer *layer = AboutLayer::create();
    GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
    ring("sound/v/button_click.mp3");
}
    
void MoreLayer::onHelp(CCObject* target)
{
    HelpLayer *layer = HelpLayer::create();
    GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
    ring("sound/v/button_click.mp3");
}
    
void MoreLayer::exitGame()
{
}

}
