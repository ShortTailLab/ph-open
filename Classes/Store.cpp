#include "Store.h"
#include "BuildConf.h"
#include "DebugMisc.h"
#include "GameScene.h"
#include "PlayerData.h"
#include "extensions/utils.h"
#include "network/PuzzleRPC.h"
#include "Board/Utils.h"
#include "SystemInform.h"
#include "UIPresets.h"
#include "CCCallLambda.h"
#include "ZqlScrollView.h"
#include "LayoutUtils.h"
#include "Common.h"
#include <CCRichLabelTTF.h>
#include <boost/move/move.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace std;
using namespace boost::algorithm;

namespace PH
{
    
std::string StoreLayer::Channel_APPStore = "APPL";
    
bool ChongZhiLayer::AppStoreProductIdsHasInit = false;

//////////////////////////////////////////////
//VIPJuanZhou
//////////////////////////////////////////////
bool VIPJuanZhou::init(const VIPJuanZhouType& type, bool isPush)
{
    if(!JuanZhou::init())
        return false;
    
    viewPort = CCSize(20 , 130);
    this->type = type;
    vipLevel = PlayerData::getInstance()->player->vipLevel;
    spent = PlayerData::getInstance()->player->spent;
    
    string path = (type == VIPJuanZhouType::Normal ? "gameui/vip_1_3.png" : "gameui/vip_1_3_2.png");
    CCSprite* background = GetSprite(path);
    background->setAnchorPoint(ccp(0.5 , 1));
    addChild(background);
    vipIcon = NULL;
    
    {
        spentBar = Get9SpriteFromFrame("jingyan_tiao.png", 7, 7, 387, 5);
        spentBar->setAnchorPoint(ccp(0, 0.5));
        spentBar->setPosition(ccp(-150, -44));
        this->addChild(spentBar);
        
        int labelPosX = type == VIPJuanZhouType::Normal ? 60 : 20;
        addLabelBMFont(this, spentLabel, "", "bmfont/Sumo_20_white.fnt", ccp(labelPosX, -44));
        spentLabel->setScale(contentScale());
        spentLabel->setAnchorPoint(ccp(0.5f, 0.5f));
        spentLabel->setZOrder(10);
    }
    
    addLabelTTF(this, text, "再充值1000元宝，你将升为vip2！", FONT_CN, 28, ccp(-275, -86), ccp(0, 0.5));
    
    if(type == VIPJuanZhouType::With_Btn_Charge)
    {
        ButtonSprite* btn = ButtonSprite::create("gameui/VIP_chongzhi.png", "gameui/VIP_chongzhi.png");
        btn->setPosition(ccp(240, -62));
        btn->setOnClickListener([=](){
            if(isPush)
            {
                GameScene::getInstance()->pushLayerWithDefaultAnimation(ChongZhiLayer::create(false, false));
            }
            else
            {
                GameScene::getInstance()->popLayer();
            }

            
            ring("sound/v/button_click.mp3");
        });
        addChild(btn);
    }
    else if(type == VIPJuanZhouType::With_Btn_Detail)
    {
        ButtonSprite* btn = ButtonSprite::create("gameui/VIP_tequan.png", "gameui/VIP_tequan.png");
        btn->setPosition(ccp(240, -62));
        btn->setOnClickListener([=](){
            if(isPush)
                GameScene::getInstance()->pushLayerWithDefaultAnimation(VIPLayer::create(false));
            else
                GameScene::getInstance()->popLayer();
        });
        addChild(btn);
    }
    
    update();
    
    return true;
}

void VIPJuanZhou::update()
{
    Player* player = PlayerData::getInstance()->getPlayer();
    int preVipLevel = this->vipLevel;
    int preSpent = this->spent;
    this->vipLevel = player->vipLevel;
    this->spent = player->spent;

    if(vipIcon)
        vipIcon->removeFromParentAndCleanup(true);
    vipIcon = GetSprite("gameui/vip"+toStr(vipLevel)+".png");
    vipIcon->setPosition(ccp(-217, -45));
    addChild(vipIcon);

    int maxLevel = PlayerData::getInstance()->vipLevelSpent.size();
    int preLevelUpSpent = PlayerData::getInstance()->vipLevelSpent[std::min(preVipLevel, maxLevel-1)];
    int levelUpSpent = PlayerData::getInstance()->vipLevelSpent[std::min(vipLevel, maxLevel-1)];
    
    int barLength =  (type == VIPJuanZhouType::Normal ? 418.0f : 340.0f);
    std::string msg = vipLevel < maxLevel ?
                        "再充值￥"+toStr(levelUpSpent-spent)+"，你将升级为VIP"+toStr(vipLevel+1)+"!" :
                        "恭喜你已经达到最大VIP等级！";
    text->setString(msg.c_str());
    
    float prevScaleX = min((float)preSpent / preLevelUpSpent, 1.0f);
    float currScaleX = min((float)spent / levelUpSpent, 1.0f);
    float distance = (currScaleX - prevScaleX) * barLength; 
    if(distance != 0.0f)
    {
        float duration = std::abs(distance) / barLength * 2.0f;
        spentBar->runAction(CCAnimate9SpriteProgress::create(duration, 10, barLength, distance, true));
    }
    else
        spentBar->setContentSize(CCSize(std::max(barLength * currScaleX, 10.0f), 26));
    
    std::string postStr = std::string("/") + toStr(levelUpSpent);
    CCCountBMTextNumber* count = CCCountBMTextNumber::create(preSpent, spent, 0.3, postStr);
    spentLabel->runAction(count);
}
    
    
//////////////////////////////////////////////
//GoodItem
//////////////////////////////////////////////

bool GoodItem::init(StoreItem item , std::string label , std::string bgPath , std::string goodPath , std::string btnPath)
{
	if(!ContentLayer::init())return false;

	targetItem = item;
	CCSprite *bg = GetSprite(bgPath);
	addChild(bg);
    setContentSize(bg->getContentSize());

	_goodPath = goodPath;

	menu = CCMenu::create();
	CCMenuItem *btn = CCMenuItemSprite::create(createBtn(btnPath, ButtonType::NORMAL_RED),
                                               createBtn(btnPath, ButtonType::NORMAL_RED),
                                               this,
                                               menu_selector(GoodItem::onClick));
	btn->setUserData(this);
	menu->addChild(btn);
	menu->setPosition(ccp(75 , -45));
	addChild(menu);

	CCLabelBMFont *amountLabel = CCLabelBMFont::create(label.c_str() , "bmfont/Sumo_38_orange.fnt");
    amountLabel->setScale(contentScale());
    amountLabel->setPosition(ccp(0 , -20));
	amountLabel->setAnchorPoint(ccp(1 , 1));
	addChild(amountLabel);

	//addLabelTTF(this , amountLabel , label.c_str() , FONT_CN , 35 , ccp(30 , 20) , ccp(0.5 , 1) , ccYELLOW);
	//addLabelTTF(this , priceLabel , boost::lexical_cast<string>(item.price).c_str() , FONT_NUM , 35 , ccp(20 , -30) , ccp(0.5 , 1) );
    priceLabel = LabelX::create( boost::lexical_cast<string>(item.price).c_str(), FONT_NUM, 30);
    priceLabel->setColor(TITLE_YELLOW);
    priceLabel->setPosition(ccp(95 , 30));
    priceLabel->setAnchorPoint(ccp(0.5 , 1));
    addChild(priceLabel);

	return true;
}

void GoodItem::setTouchEnabled(bool value)
{
	ContentLayer::setTouchEnabled(value);
	menu->setTouchEnabled(value);
}

void GoodItem::onClick(CCObject *target)
{
    boost::function<void ()> toBuy = [=]()
    {
        // show loading in current layer.
        SystemInform *inform = SystemInform::alertLoading(runningScene());
        if(targetItem.productId == "1")
        {
            SpiritPurchaseAmount amount = (SpiritPurchaseAmount)targetItem.amount;
            
            int prevGold = PlayerData::getInstance()->getPlayer()->golds;
            
            PuzzleRPC::getInstance()->restoreSpirit(PlayerData::getInstance()->getToken(),
                                                    amount,
                                                    [=](auto_ptr<Player> player,
                                                        auto_ptr<Error> e)
            {
                inform->cancel();
                if(e.get()){
                    CCLog("restore spirit error: %s", e.get()->toStr().c_str());
                    e->handleErrorInform();
                    return;
                }
                
                CCLog("restore spirit success!!");
                PlayerData::getInstance()->updatePlayer(player.get());
                
                buyAnima();
            });
        }
        else if(targetItem.productId == "2")
        {
            int type = targetItem.amount/6-1;
            
            int prevGold = PlayerData::getInstance()->getPlayer()->golds;
            
            PuzzleRPC::getInstance()->purchaseStorageRoom(PlayerData::getInstance()->getToken(),
                                                          type,
                                                          [=](auto_ptr<Player> player,
                                                              auto_ptr<Error> e)
            {
                inform->cancel();
                if(e.get())
                {
                    CCLog("add storage room error: %s", e.get()->toStr().c_str());
                    e->handleErrorInform();
                    return;
                }
                
                CCLog("purchase storage success.");
                PlayerData::getInstance()->updatePlayer(player.get());
                
                buyAnima();
            });
        }
        else
            inform->cancel();
    };
    
    SystemInform::alertConfirmPanel(runningScene(), "确定购买吗？", "警告", toBuy, [=](){});

    ring("sound/v/button_click.mp3");
}

void GoodItem::buyAnima()
{
	CCFiniteTimeAction *action1 = CCMoveBy::create(0.5 , ccp(0,100));
	CCFiniteTimeAction *action2 = CCFadeTo::create(0.5 , 0);
	CCFiniteTimeAction *actionSpawn = CCSpawn::create(action1 , action2 , NULL);

	CCSprite* good = GetSprite(_goodPath.c_str());
	good->setPosition(ccp(-68 , -3));
	addChild(good);
	good->runAction(CCSequence::create(actionSpawn,
                                       CCCallLambda::create([=]()
    {
        good->removeFromParentAndCleanup(true);
    }),
                                       NULL));
}

//////////////////////////////////////////////
//GoldItem
//////////////////////////////////////////////
    
unsigned long long mmLastPurchaseTime = 0;
    
bool GoldItem::init(const StoreItem& item)
{
	if(!GoodItem::init(item, "+" + toStr(item.amount), "gameui/yuanbao.png" , " ", "购买"))
        return false;
    
    czLayer = NULL;
    
    if(item.promotion == 2)
    {
        CCSprite *doubleTips = GetSprite("gameui/shouchongliangbei_2.png");
        doubleTips->setAnchorPoint(ccp(0.5 , 1));
        doubleTips->setPosition(ccp(-68,90));
        addChild(doubleTips , 10);
    } 

    if (item.localizePrice.empty())
        priceLabel->setString(("￥"+boost::lexical_cast<string>(item.price)).c_str());
    else
        priceLabel->setString(trim_right_copy_if(trim_right_copy_if(item.localizePrice, is_any_of("0")), is_any_of(".")).c_str());
    priceLabel->setColor(TITLE_YELLOW);
	priceLabel->setPosition(ccp(75 , 30));

	return true;
}

static inline std::string FormatTime(boost::posix_time::ptime now)
{
    using namespace boost::posix_time;
    static std::locale loc(std::cout.getloc(), new time_facet("%Y%m%d%H%M%S"));
    
    std::basic_stringstream<char> wss;
    wss.imbue(loc);
    wss << now;
    return wss.str();
}

void GoldItem::onClick(CCObject *target)
{
    LOGD("In-App Purchase is disabled in the open sourced version\n");
    ring("sound/v/button_click.mp3");
}


void GoldItem::handlePayOurs(std::string serial)
{
    string keyPrefix;
    {
        keyPrefix = "p";
    }

    CCUserDefault::sharedUserDefault()->setStringForKey((keyPrefix+ "serial").c_str(), serial.c_str());
    CCUserDefault::sharedUserDefault()->setIntegerForKey((keyPrefix+"amount").c_str(), targetItem.amount);
    CCUserDefault::sharedUserDefault()->flush();
    
    SystemInform *inform = SystemInform::alertLoading(runningScene());
    
    // user might return before event is received    
    this->retain();
    
    int oldNetworkTimeout = PuzzleRPC::getInstance()->getNetworkTimeOut();
    PuzzleRPC::getInstance()->setNetworkTimeOut(600);
    PuzzleRPC::getInstance()->purchaseGolds(PlayerData::getInstance()->getToken(),
                                            targetItem,
                                            serial,
                                            [=](auto_ptr<Player> player,
                                                auto_ptr<VipUpgrade> upgrad,
                                                auto_ptr<Error> e)
    {
        PuzzleRPC::getInstance()->setNetworkTimeOut(oldNetworkTimeout);
        inform->cancel();
        if(e.get())
        {
            if (e->code == PurchaseInvalid)
            {
            }
            else if (e->code == PurchasePending)
            {
                handlePayOurs(serial);
            }
            else if (e->code == PurchaseVerified)
            {
            }
            else
            {
                CCLog("!!! purchase error: %s", e->toStr().c_str());
                CCLog("amount: %d", targetItem.amount);
                CCLog("serial: %s", serial.c_str());
                SystemInform::alertMessage(runningScene(),
                                           (e->toStrCN() + "， 点确定重试").c_str(),
                                           "警告" ,
                                           [=]()
                {
                    handlePayOurs(serial);
                });
            }
            this->release();
            return;
        }

        int deltaGolds = player->golds - PlayerData::getInstance()->player->golds;
        if (deltaGolds)
        {
            SystemInform::alertMessage(runningScene(), "你购买的元宝已到账",
                                       ("你购买的" + toStr(deltaGolds) + "个元宝已到账").c_str(), [](){});
        }

        CCUserDefault::sharedUserDefault()->setStringForKey((keyPrefix+ "serial").c_str(), "");
        CCUserDefault::sharedUserDefault()->setIntegerForKey((keyPrefix+"amount").c_str(), 0);
        CCUserDefault::sharedUserDefault()->flush();
        CCLog("--- purchase success!!");
        PlayerData* data = PlayerData::getInstance();
        data->updatePlayer(player.get());

        if(targetItem.promotion == 2)
        {
            for(StoreItem& item : PlayerData::getInstance()->storeItems)
                if(item.productId == targetItem.productId)
                {
                    item.promotion = 1;
                    break;
                }
        }
        
        // user might return before event is received
        if(this->getParent() && czLayer)
        {
            czLayer->juanzhou->update();
            czLayer->makeItems();
        }
        
        if(upgrad.get())
        {
            data->setNormalStage(upgrad->stages);
            data->setInstanceStage(upgrad->fbs);
            SystemInform::alertVIPLevelUpInform(GameScene::getInstance(), data->player->vipLevel, *upgrad);
        }
        this->release();
    });
}

void GoldItem::buyAnima()
{

}

void GoldItem::blink(cocos2d::CCSprite *target)
{
	CCFiniteTimeAction *fade1 = CCFadeTo::create(0.7 , 100);
	CCFiniteTimeAction *fade2 = CCFadeTo::create(0.7 , 255);
	CCFiniteTimeAction *sequence = CCSequence::create(fade1 , fade2 , NULL);
	CCAction *repeatBlink = CCRepeatForever::create((CCActionInterval*)sequence);

	target->runAction(repeatBlink);
}
//////////////////////////////////////////////
//MonthlyItem
//////////////////////////////////////////////
    
bool MonthlyItem::init(const StoreItem& item)
{
    if(!ContentLayer::create())
        return false;
    
    targetItem = item;
    
    CCSprite* bg = GetSprite("gameui/yuanbaoyueka.png");
    bg->setAnchorPoint(ccp(0.5, 1));
    addChild(bg);
    
    CCLabelBMFont *amountLabel = CCLabelBMFont::create("+320" , "bmfont/Sumo_38_orange.fnt");
    amountLabel->setScale(contentScale());
    amountLabel->setPosition(ccp(-160, -115));
	amountLabel->setAnchorPoint(ccp(1 , 1));
	addChild(amountLabel);
    
    ButtonSprite* purchaseBtn = ButtonSprite::create(createBtn("购买", ButtonType::NORMAL_RED));
    purchaseBtn->setOnClickListener([=](){this->onClick(NULL);});
    purchaseBtn->setPosition(ccp(220, -130));
    addChild(purchaseBtn);
    
    LabelX* priceLabel = LabelX::create("", FONT_NUM, 26);
    if (item.localizePrice.empty())
        priceLabel->setString(("￥"+boost::lexical_cast<string>(item.price)).c_str());
    else
        priceLabel->setString(trim_right_copy_if(trim_right_copy_if(item.localizePrice, is_any_of("0")), is_any_of(".")).c_str());
    priceLabel->setColor(TITLE_YELLOW);
    priceLabel->setPosition(ccp(-135 , -80));
    priceLabel->setAnchorPoint(ccp(0, 0.5));
    addChild(priceLabel);
    
    CCLabelTTF* msgLabel = addLabelTTF(this, "连续登录30天，每天可额外领取30元宝",
                                       FONT_CN, 24, ccp(-150, -100), ccp(0, 1));
    msgLabel->setDimensions(CCSizeMake(300, 0));
    msgLabel->setHorizontalAlignment(CCTextAlignment::kCCTextAlignmentLeft);
    
    return true;
}
    
//////////////////////////////////////////////
//VIPLayer
//////////////////////////////////////////////
bool VIPLayer::init(bool isPushToCharge)
{
    if(!ContentLayer::init())
        return false;
    
    scrollView = NULL;
    
    CCMenu* topMenu = CCMenu::create();
    CCMenuItem* itemLeft = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                        GetSprite("gameui/back_t.png"),
                                        this,
                                        menu_selector(VIPLayer::onBack));
	itemLeft->setAnchorPoint(ccp(0.5, 1));
	topMenu->addChild(itemLeft);
    
    
    {
        CCMenuItem* itemMiddle = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                              GetSprite("gameui/hongchengdi_t_tongyong.png"));
        itemMiddle->setAnchorPoint(ccp(0.5 , 1));
        
        LabelX* name = LabelX::create("VIP特权", FONT_CN, 36);
        name->setColor(ccWHITE);
        name->setAnchorPoint(ccp(0.5, 0.5));
        name->setPosition(ccp( 141 , 35));
        itemMiddle->addChild(name);
        
        topMenu->addChild(itemMiddle);
    }
    
	topMenu->alignItemsHorizontallyWithPadding(8);
	topMenu->setPosition(ccp(-70, 18));
	addChild(topMenu, 2);
    
    juanzhou = VIPJuanZhou::create(VIPJuanZhouType::With_Btn_Charge, isPushToCharge);
    juanzhou->setUserData(this);
    CCSprite* zhou1 = GetSprite("gameui/vip_1_1.png");
    CCSprite* zhou2 = GetSprite("gameui/vip_1_1.png");
    zhou2->setFlipX(true);
    
    jz = JuanZhouLayer::create();
    jz->addSkin(juanzhou, zhou1 ,zhou2);
    jz->setPositionY(-30);
    addChild(jz, 1);
    
    makeBoards();
    
    return true;
}
void VIPLayer::makeBoards()
{
    if(scrollView)
        scrollView->removeFromParentAndCleanup(true);
    
    scrollView = ZqlScrollView::create();
    scrollView->setPositionY(-110);
    scrollView->setViewSize(CCSize(winSize.width , 960) , CCRect(0 , -80 , winSize.width , 470));
    addChild(scrollView);
    
    PlayerData* data = PlayerData::getInstance();
    for(int i = 1; i <= data->vipLevelSpent.size(); i++)
        scrollView->addLayer(makeVIPBoard(i, i == data->player->vipLevel, i>data->player->vipLevel+3));
    
}
                                              
ContentLayer* VIPLayer::makeVIPBoard(int level, bool isSelf, bool isDarkPack)
{
    ContentLayer* layer = ContentLayer::create();
    string path = isSelf ? "b" : "a";
    CCSprite* bg = GetSprite("gameui/vip_bg_"+path+".png");
    bg->setAnchorPoint(ccp(0.5, 1));
    layer->addChild(bg);
    
    ButtonSprite* btn = ButtonSprite::create("gameui/libao_vip.png", -127);
    btn->setPosition(ccp(225, -20));
    if(isDarkPack)
    {
        btn->getSprite()->setColor(ccc3(100, 100, 100));
        btn->getPressedSprite()->setColor(ccc3(100, 100, 100));
    }
    else
    {
        btn->runAction(CCRepeatForever::create(CCSequence::create(CCRotateTo::create(0.2, -10),
                                                                   CCRotateTo::create(0.2, 10),
                                                                   NULL)));
        btn->setOnClickListener([=](){
            GameScene::getInstance()->pushLayerWithDefaultAnimation(VIPPacksLayer::create());
        });
    }
    layer->addChild(btn);
    
    CCSprite* title = GetSprite("gameui/jiagnlivip"+toStr(level)+"_"+path+".png");
    title->setPosition(ccp(-165, -26));
    layer->addChild(title);
    CCPoint pos = ccp(-253, -66);
    int interval = 32;
    if(level == 1)
        LayoutColumn(pos, ccp(1, 0), interval, true,
                     VA(makeItem(layer, "super", 2),
                    makeItem(layer, "reputation", 3),
                    makeItem(layer, "friends", 5),
                    makeItem(layer, "xiwei", 3),
                    makeItem(layer, "coins", 9000),
                    makeItem(layer, "vip", 1),
                    NULL));
    else if(level == 2)
        LayoutColumn(pos, ccp(1, 0), interval, true,
                     VA(makeItem(layer, "super", 1, "0"),
                        makeItem(layer, "huodong", 0, "[color=ff350000]开启招募功能：材料道具十连抽[/color]"),
                        makeItem(layer, "huodong", 0, "[color=ff350000]每日材料道具十连抽+[color=ffff0000]1[/color][/color]"),
                        makeItem(layer, "reputation", 6),
                        makeItem(layer, "friends", 5),
                        makeItem(layer, "xiwei", 6),
                        makeItem(layer, "coins", 45000),
                        makeItem(layer, "vip", 2),
                        NULL));
    else if(level == 3)
        LayoutColumn(pos, ccp(1, 0), interval, true,
                     VA(makeItem(layer, "super", 1),
                        makeItem(layer, "huodong", 0, "[color=ff350000]每日材料道具十连抽+[color=ffff0000]2[/color][/color]"),
                        makeItem(layer, "huodong", 0, "[color=ff350000]黄金岛/玉虚洞活动次数各+[color=ffff0000]1[/color][/color]"),
                        makeItem(layer, "reputation", 9),
                        makeItem(layer, "friends", 10),
                        makeItem(layer, "xiwei", 12),
                        makeItem(layer, "coins", 150000),
                        makeItem(layer, "vip", 3),
                        NULL));
    else if(level == 4)
        LayoutColumn(pos, ccp(1, 0), interval, true,
                     VA(makeItem(layer, "super", 1),
                        makeItem(layer, "huodong", 0, "[color=ff350000]每日材料道具十连抽+[color=ffff0000]3[/color][/color]"),
                        makeItem(layer, "huodong", 0, "[color=ff350000]黄金岛/玉虚洞活动次数各+[color=ffff0000]1[/color][/color]"),
                        makeItem(layer, "huodong", 0, "[color=ff350000]材料活动次数+[color=ffff0000]1[/color][/color]"),
                        makeItem(layer, "reputation", 12),
                        makeItem(layer, "friends", 10),
                        makeItem(layer, "xiwei", 18),
                        makeItem(layer, "coins", 240000),
                        makeItem(layer, "vip", 4),
                        NULL));
    else if(level == 5)
        LayoutColumn(pos, ccp(1, 0), interval, true,
                     VA(makeItem(layer, "super", 1),
                        makeItem(layer, "huodong", 0, "[color=ff350000]每日材料道具十连抽+[color=ffff0000]4[/color][/color]"),
                        makeItem(layer, "huodong", 0, "[color=ff350000]黄金岛/玉虚洞/材料活动次数各+[color=ffff0000]1[/color][/color]"),
                        makeItem(layer, "huodong", 0, "[color=ff350000]侠客副本活动次数+[color=ffff0000]1[/color][/color]"),
                        makeItem(layer, "reputation", 15),
                        makeItem(layer, "friends", 15),
                        makeItem(layer, "xiwei", 24),
                        makeItem(layer, "coins", 600000),
                        makeItem(layer, "vip", 5),
                        NULL));
    else if(level == 6)
        LayoutColumn(pos, ccp(1, 0), interval, true,
                     VA(makeItem(layer, "super", 1),
                        makeItem(layer, "huodong", 0, "[color=ff350000]每日材料道具十连抽+[color=ffff0000]5[/color][/color]"),
                        makeItem(layer, "huodong", 0, "[color=ff350000]黄金岛/玉虚洞/材料活动次数各+[color=ffff0000]2[/color][/color]"),
                        makeItem(layer, "huodong", 0, "[color=ff350000]侠客副本活动次数+[color=ffff0000]1[/color][/color]"),
                        makeItem(layer, "reputation", 21),
                        makeItem(layer, "friends", 15),
                        makeItem(layer, "xiwei", 30),
                        makeItem(layer, "coins", 1200000),
                        makeItem(layer, "vip", 6),
                        NULL));
    else if(level == 7)
        LayoutColumn(pos, ccp(1, 0), interval, true,
                     VA(makeItem(layer, "super", 1),
                        makeItem(layer, "huodong", 0, "[color=ff350000]每日材料道具十连抽+[color=ffff0000]6[/color][/color]"),
                        makeItem(layer, "huodong", 0, "[color=ff350000]黄金岛/玉虚洞/材料活动次数各+[color=ffff0000]2[/color][/color]"),
                        makeItem(layer, "huodong", 0, "[color=ff350000]侠客副本活动次数+[color=ffff0000]2[/color][/color]"),
                        makeItem(layer, "reputation", 27),
                        makeItem(layer, "friends", 20),
                        makeItem(layer, "xiwei", 36),
                        makeItem(layer, "coins", 2000000),
                        makeItem(layer, "vip", 7),
                        NULL));
    else if(level == 8)
        LayoutColumn(pos, ccp(1, 0), interval, true,
                     VA(makeItem(layer, "super", 1),
                        makeItem(layer, "huodong", 0, "[color=ff350000]每日材料道具十连抽+[color=ffff0000]10[/color][/color]"),
                        makeItem(layer, "huodong", 0, "[color=ff350000]黄金岛/玉虚洞活动次数各+[color=ffff0000]3[/color][/color]"),
                        makeItem(layer, "huodong", 0, "[color=ff350000]材料/侠客副本活动次数+[color=ffff0000]2[/color][/color]"),
                        makeItem(layer, "reputation", 33),
                        makeItem(layer, "friends", 20),
                        makeItem(layer, "xiwei", 42),
                        makeItem(layer, "coins", 5000000),
                        makeItem(layer, "vip", 8),
                        NULL));
    else if(level == 9)
        LayoutColumn(pos, ccp(1, 0), interval, true,
                     VA(makeItem(layer, "super", 1),
                        makeItem(layer, "huodong", 0, "[color=ff350000]每日材料道具十连抽+[color=ffff0000]15[/color][/color]"),
                        makeItem(layer, "huodong", 0, "[color=ff350000]黄金岛/玉虚洞/材料活动次数各+[color=ffff0000]3[/color][/color]"),
                        makeItem(layer, "huodong", 0, "[color=ff350000]侠客副本活动次数+[color=ffff0000]3[/color][/color]"),
                        makeItem(layer, "reputation", 39),
                        makeItem(layer, "friends", 25),
                        makeItem(layer, "xiwei", 48),
                        makeItem(layer, "coins", 8000000),
                        makeItem(layer, "vip", 9),
                        NULL));

    
    layer->setContentSize(CCSizeMake(640, 370));
    return layer;
}
                                              
ContentLayer* VIPLayer::makeItem(ContentLayer* parent, const std::string& type, int value1, string value2)
{
    string msg;
    if(type == "super")
    {
        if(value1 == 2)
            msg = "[color=ff350000]免费招绝顶高手由[color=ffff0000]72[/color]小时缩短为[color=ffff0000]48[/color]小时[/color]";
        else if(value1 == 1 && value2 == "0")
            msg = "[color=ff350000]免费招绝顶高手由[color=ffff0000]48[/color]小时缩短为[color=ffff0000]24[/color]小时[/color]";
        else
            msg = "[color=ff350000]每[color=ffff0000]"+toStr(value1*24)+"[/color]小时可免费招绝顶高手[color=ffff0000]1[/color]次[/color]";
    }
    else if(type == "fuben")
        msg = "[color=ff350000]每日限次活动次数+[color=ffff0000]"+toStr(value1)+"[/color][/color]";
    else if(type == "coins")
        msg = "[color=ff350000]赠送[color=ffff0000]"+toStr(value1)+"[/color]铜钱[/color]";
    else if(type == "friends")
        msg = "[color=ff350000]好友上限扩展+[color=ffff0000]"+toStr(value1)+"[/color][/color]";
    else if(type == "xiwei")
        msg = "[color=ff350000]席位上限扩展+[color=ffff0000]"+toStr(value1)+"[/color][/color]";
    else if(type == "reputation")
        msg = "[color=ff350000]威望上限+[color=ffff0000]"+toStr(value1)+"[/color][/color]";
    else if(type == "unlock")
        msg = "[color=ff350000]解锁功能丹药十连抽、道具十连抽[/color]";
    else if(type == "qianghua")
    {
        string s = value1==1 ? "小" : (value1==2? "中": "大");
        msg = "[color=ff350000]"+s+"幅度提升侠客强化成功和超成功几率[/color]";
    }
    else if(type == "chest")
        msg = "[color=ff350000]开启宝箱次数+[color=ffff0000]"+toStr(value1)+"[/color][/color]";
    else if(type == "huodong")
        msg = value2;
    else if(type == "vip")
        msg = "[color=ff350000]可购买VIP[color=ffff0000]"+toStr(value1)+"[/color]特权礼包[/color]";
    
    ContentLayer* layer = ContentLayer::create();
    CCSprite* dot = GetSprite("gameui/vip_dian.png");
    dot->setPosition(ccp(0, 0));
    layer->addChild(dot);
    
    CCRichLabelTTF* text = CCRichLabelTTF::create(msg.c_str(), FONT_CN, 26);
    text->setPosition(ccp(15, 0));
    text->setAnchorPoint(ccp(0, 0.5));
    layer->addChild(text);
    //addLabelTTF(layer, text, msg.c_str(), FONT_CN, 26, ccp(15, 0), ccp(0, 0.5), ccc3(0x55, 0x00, 0x00));
    parent->addChild(layer);
    return layer;
}

void VIPLayer::onEnter()
{
    ContentLayer::onEnter();
    juanzhou->update();
    
    makeBoards();
}
                                              
void VIPLayer::onEnterAnimationDone()
{
    jz->startAnim();
}

void VIPLayer::onBack()
{
    GameScene::getInstance()->popLayer();
    ring("sound/v/button_click.mp3");
}
                                              
//////////////////////////////////////////////
//StoreLayer
//////////////////////////////////////////////
                                                     
bool SpiritLayer::init()
{
    if(!ContentLayer::init())
        return false;

    CCMenu *menu = CCMenu::create();

    CCMenuItem *backItem = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"), GetSprite("gameui/back_t.png"),
                                                    this , menu_selector(SpiritLayer::onBack));
    CCMenuItem *titleItem = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"), GetSprite("gameui/hongchengdi_t_tongyong.png"));
    LabelX* name = LabelX::create("补充体力", FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp( 141 , 34));
    titleItem->addChild(name);

    menu->addChild(backItem);
    menu->addChild(titleItem);

    backItem->setAnchorPoint(ccp(0.5 , 1));
    titleItem->setAnchorPoint(ccp(0.5 , 1));
    menu->alignItemsHorizontallyWithPadding(8);
    menu->setPosition(ccp(-70 , 18));
    addChild(menu);
    
     StoreItem item1;
     item1.productId = "1";
     item1.amount = SpiritPurchaseAmount::Ten;
     item1.price = 20;
     
     StoreItem item2;
     item2.productId = "1";
     item2.amount = SpiritPurchaseAmount::Half;
     item2.price = 40;
     
     StoreItem item3;
     item3.productId = "1";
     item3.amount = SpiritPurchaseAmount::Full;
     item3.price = 60;
    
     GoodItem *jingliItem1 = GoodItem::create(item1, "+10", "gameui/jingliping.png", "gameui/jingliping_2.png", "补充");
     jingliItem1->setPosition(ccp(-155, -150));
     addChild(jingliItem1);
     
     GoodItem *jingliItem2 = GoodItem::create(item2, "50%", "gameui/jingliping.png", "gameui/jingliping_2.png", "补充");
     jingliItem2->setPosition(ccp(155, -150));
     addChild(jingliItem2);
     
     GoodItem *jingliItem3 = GoodItem::create(item3, "加滿", "gameui/jingliping.png", "gameui/jingliping_2.png", "补充");
     jingliItem3->setPosition(ccp(-155, -340));
     addChild(jingliItem3);

    return true;
}
                                                     
void SpiritLayer::onBack()
{
    GameScene::getInstance()->popLayer();
}
 
                                                     
bool XiweiLayer::init()
{
     if(!ContentLayer::init())
         return false;
     
     CCMenu *menu = CCMenu::create();
     
     CCMenuItem *backItem = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"), GetSprite("gameui/back_t.png"),
                                                     this , menu_selector(SpiritLayer::onBack));
     CCMenuItem *titleItem = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"), GetSprite("gameui/hongchengdi_t_tongyong.png"));
     LabelX* name = LabelX::create("增加席位", FONT_CN, 36);
     name->setColor(ccWHITE);
     name->setAnchorPoint(ccp(0.5, 0.5));
     name->setPosition(ccp( 141 , 34));
     titleItem->addChild(name);
     
     menu->addChild(backItem);
     menu->addChild(titleItem);
     
     backItem->setAnchorPoint(ccp(0.5 , 1));
     titleItem->setAnchorPoint(ccp(0.5 , 1));
     menu->alignItemsHorizontallyWithPadding(8);
     menu->setPosition(ccp(-70 , 18));
     addChild(menu);
    
    StoreItem item1;
	item1.productId = "2";
	item1.amount = 6;
	item1.price = 60;
    
    StoreItem item2;
	item2.productId = "2";
	item2.amount = 12;
	item2.price = 120;
    
    StoreItem item3;
	item3.productId = "2";
	item3.amount = 18;
	item3.price = 180;
    
    GoodItem *xiwei1 = GoodItem::create(item1, "+6", "gameui/xiwei.png", "gameui/xiwei_2.png", "扩展");
	xiwei1->setPosition(ccp(-155, -150));
	addChild(xiwei1);
    
    GoodItem *xiwei2 = GoodItem::create(item2, "+12", "gameui/xiwei.png", "gameui/xiwei_2.png", "扩展");
	xiwei2->setPosition(ccp(155, -150));
	addChild(xiwei2);
    
    GoodItem *xiwei3 = GoodItem::create(item3, "+18", "gameui/xiwei.png", "gameui/xiwei_2.png", "扩展");
	xiwei3->setPosition(ccp(-155, -340));
	addChild(xiwei3);
 
 return true;
}
                                                     
                                                     
bool VIPPacksLayer::init()
{
    if(!ContentLayer::init())
     return false;
    scrollView = NULL;

    CCMenu *menu = CCMenu::create();

    CCMenuItem *backItem = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"), GetSprite("gameui/back_t.png"),
                                                 this , menu_selector(SpiritLayer::onBack));
    CCMenuItem *titleItem = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"), GetSprite("gameui/hongchengdi_t_tongyong.png"));
    LabelX* name = LabelX::create("VIP礼包", FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp( 141 , 34));
    titleItem->addChild(name);

    menu->addChild(backItem);
    menu->addChild(titleItem);

    backItem->setAnchorPoint(ccp(0.5 , 1));
    titleItem->setAnchorPoint(ccp(0.5 , 1));
    menu->alignItemsHorizontallyWithPadding(8);
    menu->setPosition(ccp(-70 , 18));
    addChild(menu, 1);

    makeItems();

    return true;
}
                                                     
void VIPPacksLayer::onBack()
{
    GameScene::getInstance()->popLayer();
}
                                                   
void VIPPacksLayer::makeItems()
{
    if(scrollView)
        scrollView->removeFromParentAndCleanup(true);
    
   scrollView = ZqlScrollView::create();
   scrollView->setViewSize(CCSize(winSize.width , 800) ,  CCRect(0, -100, winSize.width, 600));
   scrollView->setPosition(ccp(0, 40));
   this->addChild(scrollView);
   
   std::vector<VIPPack> packs = PlayerData::getInstance()->vipPacks;
   for(const VIPPack& pack : packs)
       if(pack.availableAtLevel <= PlayerData::getInstance()->player->vipLevel+3 &&
          !pack.purchased)
           scrollView->addLayer(createPackItem(pack));
}
                                                   
ContentLayer* VIPPacksLayer::createPackItem(const VIPPack& pack)
{
    ContentLayer* layer = ContentLayer::create();
    std::string bgPath = pack.isHeroesPack() ? "gameui/xiake_bg_libao.png" : "gameui/libao_neirong.png";
    CCSprite* bg = GetSprite(bgPath);
    bg->setAnchorPoint(ccp(0.5, 1));
    layer->addChild(bg);
    
    addLabelBMFont(layer, ("VIP"+toStr(pack.availableAtLevel)).c_str(), "bmfont/Sumo_38_orange.fnt",
                   ccp(-260, -130), ccp(0, 0.5))->setScale(contentScale());
    
    std::string msg = pack.isHeroesPack() ? "侠客礼包" : "特权礼包";
    addLabelBMFont(layer, ("VIP"+toStr(pack.availableAtLevel)+msg).c_str(),
                   "bmfont/Sumo_32_orange.fnt", ccp(-140, -40), ccp(0, 0.5))->setScale(contentScale());
    
    string s = "特别为VIP"+toStr(pack.availableAtLevel)+"准备的至尊礼包，紧限购买一次。";
    CCLabelTTF* label = addLabelTTF(layer, s.c_str(), FONT_CN, 22, ccp(-150, -100), ccp(0, 1));
    label->setDimensions(CCSizeMake(320, 60));
    label->setHorizontalAlignment(CCTextAlignment::kCCTextAlignmentLeft);
    
    ButtonSprite* btn = ButtonSprite::create(createBtn("购买", ButtonType::NORMAL_RED),
                                             createBtn("购买", ButtonType::NORMAL_RED));
    btn->setPosition(ccp(230, -140));
    btn->setOnClickListener([=](){
        SystemInform::alertPurchaseVipPacks(runningScene(), pack, [=](){
            this->makeItems();
        });
    });
    layer->addChild(btn);
    
    addLabelBMFont(layer, toStr(pack.price).c_str(), "bmfont/Sumo_22_yellow.fnt",
                   ccp(-35, -80), ccp(0, 0.5))->setScale(contentScale());
    addLabelBMFont(layer, toStr(pack.showPrice).c_str(), "bmfont/Sumo_22_white_brown.fnt",
                   ccp(175, -80), ccp(0, 0.5))->setScale(contentScale());
    CCScale9Sprite* line = Get9SpriteFromFrame("icon_mask.png", 1, 1, 98, 98);
    line->setContentSize(CCSizeMake(150, 4));
    line->setPosition(ccp(150, -80));
    layer->addChild(line);
    
    layer->setContentSize(CCSizeMake(bg->getContentSize().width, bg->getContentSize().height+10));
    return layer;
}
                                                     
//////////////////////////////////////////////
//StoreLayer
//////////////////////////////////////////////

bool StoreLayer::init(bool hasBackBtn)
{
	if(!ContentLayer::init())return false;

    int adjust = 0;
    tips = NULL;
    
    if (hasBackBtn)
    {
        ButtonSprite* backBtn = ButtonSprite::create("gameui/back_t.png", "gameui/back_t.png");
        backBtn->setPosition(ccp(-220, -20));
        backBtn->setOnClickListener([](){
            GameScene::getInstance()->popLayer();
            ring("sound/v/button_click.mp3");
        });
        addChild(backBtn, 1);
        
        adjust = -60;
        
        isShowBottom = false;
    }
    
    container = ContentLayer::create();
    container->setContentSize(CCSize(winSize.width , 584));
    container->setPositionY(adjust);
	addChild(container);
    
    /*{
        CCMenu* topMenu = CCMenu::create();
        rechargeBtn = CCMenuItemSprite::create(GetSprite("gameui/chongzhi2.png"),
                                                    GetSprite("gameui/chongzhi2.png"),
                                                    this,
                                                    menu_selector(StoreLayer::onChongZhiClick));
        topMenu->addChild(rechargeBtn);
        
        CCMenuItem* vipBtn = CCMenuItemSprite::create(GetSprite("gameui/viptequan.png"),
                                                      GetSprite("gameui/viptequan.png"),
                                                      this,
                                                      menu_selector(StoreLayer::onVIPClick));
        topMenu->addChild(vipBtn);
        topMenu->alignItemsHorizontallyWithPadding(8);
        topMenu->setPosition(ccp(0, -17));
        container->addChild(topMenu, 1);
    }*/
    
    juanzhou = VIPJuanZhou::create();
    juanzhou->setUserData(this);
    CCSprite* zhou1 = GetSprite("gameui/vip_1_1.png");
    CCSprite* zhou2 = GetSprite("gameui/vip_1_1.png");
    zhou2->setFlipX(true);
    
    jz = JuanZhouLayer::create();
    jz->addSkin(juanzhou, zhou1 ,zhou2);
    jz->setPositionY(15);
    container->addChild(jz, 10);

    /*scrollView = ZqlScrollView::create();
	scrollView->setPositionY(-95);
	scrollView->setViewSize(CCSize(winSize.width , 960) , CCRect(0 , -45, winSize.width , 515));
	scrollView->addLayer(makeItems());*/
    ContentLayer* itemsLayer = makeItems();
    itemsLayer->setPositionY(-150);
    container->addChild(itemsLayer);
    
    ContentLayer::setTouchEnabled(true);

	return true;
}
                                              
void StoreLayer::onEnterAnimationDone()
{
    jz->startAnim();
}

void StoreLayer::onEnter()
{
    ContentLayer::onEnter();
    
    if(PlayerData::getInstance()->storeItems.size())
    {
        if(PlayerData::getInstance()->storeItems[0].promotion == 2 && !tips)
        {
            tips = GetSprite("gameui/shouchongliangbei2.png");
            tips->setPosition(ccp(95, 135));
            chargeBtn->addChild(tips);
        }
        else if(PlayerData::getInstance()->storeItems[0].promotion != 2 && tips)
            tips->removeAllChildrenWithCleanup(true);
    }
    
    juanzhou->update();
}
 
ContentLayer* StoreLayer::makeItems()
{
    ContentLayer* layer = ContentLayer::create();
    
	/*StoreItem item1;
	item1.productId = "1";
	item1.amount = SpiritPurchaseAmount::Ten;
	item1.price = 20;

	StoreItem item2;
	item2.productId = "1";
	item2.amount = SpiritPurchaseAmount::Half;
	item2.price = 40;

	StoreItem item3;
	item3.productId = "1";
	item3.amount = SpiritPurchaseAmount::Full;
	item3.price = 60;

	StoreItem item4;
	item4.productId = "2";
	item4.amount = 6;
	item4.price = 60;*/
    
    chargeBtn = ButtonSprite::create("gameui/chongzhi2.png", "gameui/chongzhi2.png",
                                                   kCCMenuHandlerPriority, false, false);
    int btnHeight = chargeBtn->getSprite()->getContentSize().height;
    chargeBtn->setPosition(ccp(-155, -btnHeight*0.5f));
    chargeBtn->setOnClickListener([=](){
        onChongZhiClick();
    });
    layer->addChild(chargeBtn); 
    
    ButtonSprite* vipBtn = ButtonSprite::create("gameui/viptequan.png", "gameui/viptequan.png",
                                                   kCCMenuHandlerPriority, false, false);
    vipBtn->setPosition(ccp(155, -btnHeight*0.5f));
    vipBtn->setOnClickListener([=](){
        onVIPClick();
    });
    layer->addChild(vipBtn);
    
    ButtonSprite* spiritBtn = ButtonSprite::create("gameui/jingli_chong.png", "gameui/jingli_chong.png",
                                                kCCMenuHandlerPriority, false, false);
    spiritBtn->setPosition(ccp(-155, -btnHeight*0.5f-170));
    spiritBtn->setOnClickListener([=](){
        GameScene::getInstance()->pushLayerWithDefaultAnimation(SpiritLayer::create());
        ring("sound/v/button_click.mp3");
    });
    layer->addChild(spiritBtn);
    
    ButtonSprite* xiweiBtn = ButtonSprite::create("gameui/xiwei_chong.png", "gameui/xiwei_chong.png",
                                                   kCCMenuHandlerPriority, false, false);
    xiweiBtn->setPosition(ccp(155, -btnHeight*0.5f-170));
    xiweiBtn->setOnClickListener([=](){
        GameScene::getInstance()->pushLayerWithDefaultAnimation(XiweiLayer::create());
        ring("sound/v/button_click.mp3");
    });
    layer->addChild(xiweiBtn);
    
    ButtonSprite* vipChongBtn = ButtonSprite::create("gameui/libao_chong.png", "gameui/libao_chong.png",
                                                  kCCMenuHandlerPriority, false, false);
    vipChongBtn->setPosition(ccp(-155, -btnHeight*0.5f-340));
    vipChongBtn->setOnClickListener([=](){
        GameScene::getInstance()->pushLayerWithDefaultAnimation(VIPPacksLayer::create());
    });
    layer->addChild(vipChongBtn);
    
	/*GoodItem *jingliItem1 = GoodItem::create(item1, "+10", "gameui/jingliping.png", "gameui/jingliping_2.png", "补充");
    int itemHeight = jingliItem1->getContentSize().height;
	jingliItem1->setPosition(ccp(-155, -btnHeight-10-itemHeight*0.5f));
	layer->addChild(jingliItem1);
	items.push_back(jingliItem1);

	GoodItem *jingliItem2 = GoodItem::create(item2, "50%", "gameui/jingliping.png", "gameui/jingliping_2.png", "补充");
	jingliItem2->setPosition(ccp(155, -btnHeight-10-itemHeight*0.5f));
	layer->addChild(jingliItem2);
	items.push_back(jingliItem2);

	GoodItem *jingliItem3 = GoodItem::create(item3, "加滿", "gameui/jingliping.png", "gameui/jingliping_2.png", "补充");
	jingliItem3->setPosition(ccp(-155, -btnHeight-10-itemHeight*1.5f));
	layer->addChild(jingliItem3);
	items.push_back(jingliItem3);

	GoodItem *xiweiItem = GoodItem::create(item4, "+6", "gameui/xiwei.png", "gameui/xiwei_2.png", "扩展");
	xiweiItem->setPosition(ccp(155, -btnHeight-10-itemHeight*1.5f));
	layer->addChild(xiweiItem);
	items.push_back(xiweiItem);*/

    //layer->setContentSize(CCSizeMake(640, btnHeight+2*itemHeight+10));
    return layer;
}

void StoreLayer::setTouchEnabled(bool value)
{
	ContentLayer::setTouchEnabled(value);
	for(GoodItem *item : items)item->setTouchEnabled(value);
}

void StoreLayer::onChongZhiClick()
{
    GameScene::getInstance()->pushLayerWithDefaultAnimation(ChongZhiLayer::create());
    ring("sound/v/button_click.mp3");
}
                                              
void StoreLayer::onVIPClick()
{
    GameScene::getInstance()->pushLayerWithDefaultAnimation(VIPLayer::create(true));
    ring("sound/v/button_click.mp3");
}

//////////////////////////////////////////////
//ChongZhiLayer
//////////////////////////////////////////////

bool ChongZhiLayer::init(bool inbattle, bool isPushToVIP)
{
	if(!ContentLayer::init())
        return false;

    inBattle =  inbattle;
    container = NULL;
    inBattleReturnCall = [](){};
    if(inBattle)
    {
        CCSprite* darkBg = GetSprite("gameui/1x1_b.png");
        darkBg->setScaleX(640);
        darkBg->setScaleY(960);
        darkBg->setOpacity(150);
        darkBg->setPosition(ccp(0, 0));

        this->addChild(darkBg, -1);
    }   
    
	topMenu = CCMenu::create();
    CCMenuItem* itemLeft = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                                    GetSprite("gameui/back_t.png"),
                                                    this,
                                                    menu_selector(ChongZhiLayer::onBackClick));
	itemLeft->setAnchorPoint(ccp(0.5, 1));
	topMenu->addChild(itemLeft);
    
    
    {
        CCMenuItem* itemMiddle = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                          GetSprite("gameui/hongchengdi_t_tongyong.png"));
        itemMiddle->setAnchorPoint(ccp(0.5 , 1));
        
        LabelX* name = LabelX::create("充值", FONT_CN, 36);
        name->setColor(ccWHITE);
        name->setAnchorPoint(ccp(0.5, 0.5));
        name->setPosition(ccp( 141 , 35));
        itemMiddle->addChild(name);
        
        topMenu->addChild(itemMiddle);
    }
    
    topMenu->alignItemsHorizontallyWithPadding(8);
	topMenu->setPosition(ccp(-70 , 18));
	addChild(topMenu, 1); 

	/*GoldItem *gold = GoldItem::create("gameui/chongzhiGold.png" ,	"gameui/goumai.png" , this , menu_selector(ChongZhiLayer::onItemClick) , 10);
	gold->setPosition(ccp(100 , -200));
	addChild(gold);

	GoldItem *gold2 = GoldItem::create("gameui/chongzhiGold.png" ,	"gameui/goumai.png" , this , menu_selector(ChongZhiLayer::onItemClick) , 10);
	gold2->setPosition(ccp(300 , -200));
	addChild(gold2);*/
    if(inbattle)
    {
        juanzhou = NULL;
        jz = NULL;
    }
    else
    {
        juanzhou = VIPJuanZhou::create(VIPJuanZhouType::With_Btn_Detail, isPushToVIP);
        juanzhou->setUserData(this);
        CCSprite* zhou1 = GetSprite("gameui/vip_1_1.png");
        CCSprite* zhou2 = GetSprite("gameui/vip_1_1.png");
        zhou2->setFlipX(true);
        
        jz = JuanZhouLayer::create();
        jz->addSkin(juanzhou, zhou1 ,zhou2);
        jz->setPositionY(-30);
        addChild(jz, 1);
    }
    
	scrollView = ZqlScrollView::create();
    if(inbattle)
    {
        scrollView->setPosition(ccp(0, -60));
        scrollView->setViewSize(CCSize(winSize.width, 900),
                                CCRect(0, 0, winSize.width, 900));
    }
    else
    {
        scrollView->setPosition(ccp(0, -110));
        scrollView->setViewSize(CCSize(winSize.width, 900),
                                CCRect(0, -80, winSize.width, 470));
    }
	this->addChild(scrollView);

	makeItems();

	return true;
}

void ChongZhiLayer::onEnterAnimationDone()
{
    if(jz)
        jz->startAnim();
}
                                              
void ChongZhiLayer::onEnter()
{
    ContentLayer::onEnter();
    if(juanzhou)
        juanzhou->update();
}
                                              
void ChongZhiLayer::setTouchEnabled(bool value)
{
    ContentLayer::setTouchEnabled(value);
    topMenu->setTouchEnabled(value);
    for(ContentLayer *item : goldItems)
        item->setTouchEnabled(value);
}

void ChongZhiLayer::makeItems()
{
    SystemInform* inform = SystemInform::alertLoading(runningScene());
    PuzzleRPC::getInstance()->getStoreItems(PlayerData::getInstance()->player->token,
                                            [=](std::auto_ptr<Control> control, std::auto_ptr<Error> err)
    {
        inform->cancel();
        if (control.get())
            PlayerData::getInstance()->storeItems.swap(control->store);

        std::vector<StoreItem> storeItems = PlayerData::getInstance()->storeItems;
        
        sort(storeItems.begin(), storeItems.end(), [](const StoreItem& a, const StoreItem& b){return a.price > b.price;});
        
        LOGD("store item count %zu\n", storeItems.size());
        for(const StoreItem& item : PlayerData::getInstance()->storeItems)
        {
            LOGD("item: %s, %d, %d\n", item.channel.c_str(), item.amount, item.price);
        }
        
        if(!container){
            container = ContentLayer::create();
            scrollView->addLayer(container);
        }
        else
        container->removeAllChildrenWithCleanup(true);
        
        /*StoreItem item;
        item.price = 100;
        control->monthly.push_back(item);*/
        int i = 0;
        int py = 0;
        int yStart = 0;
        if(!control->monthly.empty())
        {
            MonthlyItem* monthlyItem = MonthlyItem::create(control->monthly[0]);
            monthlyItem->setPosition(ccp(0,0));
            container->addChild(monthlyItem);
            yStart = -185;
        }
        
        for (const StoreItem& item : storeItems)
        {
            int px = i%2;
            py = int(i/2);
            GoldItem *itemSpr = GoldItem::create(item);
            itemSpr->czLayer = this;
            itemSpr->setPosition(ccp( -155 + 310*px,yStart -90-190*py));
            goldItems.push_back(itemSpr);
            container->addChild(itemSpr);
            i++;
        }
        container->setContentSize(CCSize(winSize.width , 190 * (py+1)-yStart));
    });
}

void ChongZhiLayer::onBackClick(cocos2d::CCObject *target)
{
    if(inBattle)
    {
        inBattleReturnCall();
        this->removeFromParentAndCleanup(true); // this must be the last statement!!!
    }
    else
        GameScene::getInstance()->popLayer();
    
    ring("sound/v/button_click.mp3");
}
}