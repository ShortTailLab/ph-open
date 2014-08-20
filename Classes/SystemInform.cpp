#include "SystemInform.h"

#include <locale.h>
#include <extensions/utils.h>
#include <extensions/native/CCNative.h>
#include <boost/algorithm/string.hpp>
#include <boost/move/move.hpp>

#include "GameScene.h"
#include "Fonts.h"
#include "WebViewLayer.h"
#include "CCCallLambda.h"
#include "UIPresets.h"
#include "PlayerData.h"
#include "URLHelpers.h"
#include "Common.h"
#include "PuzzleRPC.h"
#include "ZqlScrollView.h"
#include "More.h"
#include "friend.h"

#if PH_USE_WEIBO
#include "extensions/auth/Weibo.h"
#include "extensions/auth/Weixin.h"
#endif

namespace PH
{
    USING_NS_CC;
    
CCSprite* createRewardItemSprite(const Treasure& reward)
{
    std::string bgPath = "";
    std::string path = "";
    std::string msg = toStr(reward.count);
    CCSprite* item = NULL;
    
    if(reward.type == LoginRewardType::Reward_Coins || reward.type == LoginRewardType::Reward_PercentCoins)
    {
        bgPath = "gameui/jiangli_bg.png";
        path = "gameui/yiduitongqian.png";
        msg = "铜钱"+toStr(reward.count)+"枚";
    }
    else if(reward.type == LoginRewardType::Reward_Exp || reward.type == LoginRewardType::Reward_PercentExp)
    {
        bgPath = "gameui/jiangli_bg.png";
        path = "gameui/jingyantubiao.png";
        msg = "经验"+toStr(reward.count)+"点";
    }
    else if(reward.type == LoginRewardType::Reward_Gold)
    {
        bgPath = "gameui/jiangli_bg.png";
        path = "gameui/yuanbaodui.png";
        msg = "元宝"+toStr(reward.count)+"锭";
    }
    else if(reward.type == LoginRewardType::Reward_Hero)
    {
        HeroProfile p = HPT(reward.ident);
        item = HeroIcon::createWithProfile(&p);
        msg = p.nameCN+"*"+toStr(reward.count);
    }
    else if(reward.type == LoginRewardType::Reward_Soul)
    {
        HeroProfile p = HPT(reward.ident);
        item = HeroIcon::createWithProfile(&p, true);
        ((HeroIcon*)item)->showPlusNum(reward.count);
        msg = p.nameCN+"魂魄*"+toStr(reward.count);
    }
    else
    {
        bgPath = "gameui/jiangli_bg.png";
        path = "gameui/jingliping_100.png";
        msg = "精力"+toStr(reward.count);
    }
    
    if(!item)
    {
        item = GetSprite(path);
        item->setScale(0.8);
    }
    
    if(!bgPath.empty())
    {
        CCSprite* bg = GetSprite(bgPath);
        item->setPosition(ccp(bg->getContentSize().width*0.5f, bg->getContentSize().height*0.5f));
        bg->addChild(item);
        item = bg;
    }
    CCSize itemSize = item->getContentSize();
    addLabelTTF(item, msg.c_str(), FONT_CN, 24, ccp(itemSize.width*0.5f, -20), ccp(0.5, 0.5), ccc3(0x55, 0x00, 0x00));
    return item;
}
    
CCSprite* createVIPPackItem(const Treasure& treasure)
{
    CCSprite* icon;
    std::string msg;
    std::string bgPath = "";
    std::string path = "";
    if(treasure.type == LoginRewardType::Reward_Hero || treasure.type == LoginRewardType::Reward_Soul)
    {
        bool isSoul = treasure.type == LoginRewardType::Reward_Soul;
        HeroProfile p = HPT(treasure.ident);
        //certain heroes
        if(treasure.count > 0)
        {
            icon = HeroIcon::createWithProfile(&p, isSoul);
            msg = isSoul ? "魂魄*"+toStr(treasure.count) : p.nameCN+"*"+toStr(treasure.count);
            CCSize itemSize = icon->getContentSize();
            addLabelTTF(icon, msg.c_str(), FONT_CN, 24, ccp(itemSize.width*0.5f, -30), ccp(0.5, 0.5), ccc3(0x55, 0x00, 0x00));
        }
        //random heroes
        else if(treasure.count == 0)
        {
            CCSprite* content;
            int stars = p.star;
            if(isSoul)
            {
                content = GetSprite("gameui/denglujiangli_hun.png");
                msg = "随机"+toStr(stars)+"星魂魄";
            }
            else
            {
                std::string xiakePath = "gameui/denglujiangli_xiake.png";
                std::string des = "侠客";
                for(int i = 0 ; i < sizeof(SuperHeroIds)/sizeof(SuperHeroIds[0]); i++)
                    if(treasure.ident == SuperHeroIds[i])
                    {
                        xiakePath = "gameui/denglujiangli_wujue.png";
                        des = "五绝侠客";
                        break;
                    }
                for(int i = 0 ; i < sizeof(GodHeroIds)/sizeof(GodHeroIds[0]); i++)
                    if(treasure.ident == GodHeroIds[i])
                    {
                        xiakePath = "gameui/denglujiangli_wushen.png";
                        des = "五神侠客";
                        break;
                    }
                
                content = GetSprite(xiakePath);
                msg = toStr(stars)+"星"+des;
            }
            
            icon = GetSprite("gameui/jiangli_bg.png");
            content->setScale(1.2);
            content->setPosition(ccp(icon->getContentSize().width*0.5f, icon->getContentSize().height*0.5f));
            icon->addChild(content);
            
            
            for(int i = 0; i < stars; i++)
            {
                CCSprite* star = GetSprite("gameui/xingxing2.png");
                star->setScale(0.6);
                star->setPosition(ccp(52-18*(stars-1)*0.5+18*i, 90));
                icon->addChild(star);
            }
            CCSize itemSize = icon->getContentSize();
            addLabelTTF(icon, msg.c_str(), FONT_CN, 24, ccp(itemSize.width*0.5f, -23), ccp(0.5, 0.5), ccc3(0x55, 0x00, 0x00));
        }
        return icon;
    }
    else if(treasure.type == LoginRewardType::Reward_Coins || treasure.type == LoginRewardType::Reward_PercentCoins)
    {
        bgPath = "gameui/jiangli_bg.png";
        path = "gameui/yiduitongqian.png";
        msg = "铜钱"+toStr(treasure.count)+"枚";
    }
    else if(treasure.type == LoginRewardType::Reward_Exp || treasure.type == LoginRewardType::Reward_PercentExp)
    {
        bgPath = "gameui/jiangli_bg.png";
        path = "gameui/jingyantubiao.png";
        msg = "经验"+toStr(treasure.count)+"点";
    }
    else if(treasure.type == LoginRewardType::Reward_Gold)
    {
        bgPath = "gameui/jiangli_bg.png";
        path = "gameui/yuanbaodui.png";
        msg = "元宝"+toStr(treasure.count)+"锭";
        
    }

    icon = GetSprite(bgPath);
    CCSprite* content = GetSprite(path);
    content->setScale(0.8);
    content->setPosition(ccp(icon->getContentSize().width*0.5f, icon->getContentSize().height*0.5f));
    icon->addChild(content);
    CCSize itemSize = icon->getContentSize();
    addLabelTTF(icon, msg.c_str(), FONT_CN, 24, ccp(itemSize.width*0.5f, -20), ccp(0.5, 0.5), ccc3(0x55, 0x00, 0x00));
 
    return icon;
}
    
RewardBoard::~RewardBoard()
{
    if(anim)
        anim->release();
}
    
bool RewardBoard::init(SystemInform* inform,
                       const LoginRewards& loginRewards,
                       const std::vector<int>& _path)
{
    if(!CCLayer::init())
        return false;
    
    startPoint = ccp(-173, -201);
    currNode = currLeft = currUp = -1;
    btn = NULL;
    anim = NULL;
    this->rewards = loginRewards;
    //the first element of path is -1 to represent the startpoint.
    this->path = std::vector<int>(1, -1);
    for(int i = 0; i < _path.size(); i++)
        path.push_back(_path[i]); 
    this->inform = inform;
    
    CCSprite* bg = GetSprite("gameui/denglujiangli_bg.png");
    addChild(bg);
    
    routeLayer = CCLayer::create();
    addChild(routeLayer);
    
    arrowLayer = CCLayer::create();
    addChild(arrowLayer, 10);
    
    for(int i = 1; i <= loginRewards.size()-2; i++)
    {
        CCSprite* item;
        if(i<loginRewards.size()-2)
            item = createRewardItem(loginRewards[i-1]);
        else
        {
            item = GetSprite("gameui/denglujiangli_baoxiang.png");
            item->runAction(CCRepeatForever::create(CCSequence::create(CCRotateTo::create(0.2, -10), CCRotateTo::create(0.2, 10), NULL)));
        }
        item->setPosition(ccp(startPoint.x+115*(i%4), startPoint.y+115*(i/4)));
        addChild(item);
        items.push_back(item);
    }
    createRoute(path);
    initCurrPos();
    
    //addLabelTTF(this, msgLabel, "向右或者向上移动可获得每日登陆奖励\n最终可获得包含高星侠客的至尊宝箱\n停止登陆一天将返回起点",
              //  FONT_CN, 26, ccp(-20, -280), ccp(0.5, 1), ccc3(0x55, 0x00, 0x00));
    
    msgLabel = CCRichLabelTTF::create("[color=ff000000]向右或者向上移动可获得每日登录奖励\n最终可获得[color=ffff0000]包含高星侠客的至尊宝箱[/color]\n停止登录一天将返回起点[/color]",FONT_CN, 26);
    msgLabel->setPosition(ccp(-0, -270));
    msgLabel->setAnchorPoint(ccp(0.5, 1));
    msgLabel->setHorizontalAlignment(kCCTextAlignmentLeft);
    addChild(msgLabel);
    
    this->setTouchEnabled(true);
    
    return true;
}
    
void RewardBoard::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,
                                                                            UI_TOUCH_ORDER_MODAL_MENU,
                                                                            true);
}
   
bool RewardBoard::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    isPressed = true;
    return true;
}
    
void RewardBoard::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint currPoint = pTouch->getLocationInView();
	CCPoint prevPoint = pTouch->getPreviousLocationInView();
    
	if(ccpDistance(currPoint, prevPoint) > 5)
    {
        isPressed = false;
    }
}
    
void RewardBoard::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint localPoint = convertTouchToNodeSpace(pTouch);
    if(isPressed)
    {
        if(currLeft != -1 && items[currLeft]->boundingBox().containsPoint(localPoint))
            choose(currLeft);
        else if(currUp != -1 && items[currUp]->boundingBox().containsPoint(localPoint))
            choose(currUp);
        
        if(btn && btn->boundingBox().containsPoint(localPoint))
            inform->cancel();
    }
    
}
    
void RewardBoard::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
}
    
CCSprite* RewardBoard::createRewardItem(const Treasure& reward)
{
    std::string path = "";
    std::string msg = toStr(reward.count);
    float scale = 0.8;
    CCSprite* itemBg = NULL;
    
    if(reward.type == LoginRewardType::Reward_Coins)
        path = "gameui/yiduitongqian.png";
    else if(reward.type == LoginRewardType::Reward_PercentCoins)
    {
        path = "gameui/yiduitongqian.png";
        msg = toStr(reward.count);
    }
    else if(reward.type == LoginRewardType::Reward_Exp)
        path = "gameui/jingyantubiao.png";
    else if(reward.type == LoginRewardType::Reward_PercentExp)
    {
        path = "gameui/jingyantubiao.png";
        msg = toStr(reward.count);
    }
    else if(reward.type == LoginRewardType::Reward_Gold)
        path = "gameui/yuanbaodui.png";
    else if(reward.type == LoginRewardType::Reward_Hero)
    {
        HeroProfile p = HPT(reward.ident);
        scale = 1.1;
        if(p.isMat())
        {
            itemBg = HeroIcon::createWithProfile(&p);
            scale = 0.8;
        }
        else
            path = "gameui/denglujiangli_xiake.png";
        
        msg = "";
    }
    else if(reward.type == LoginRewardType::Reward_Soul)
    {
        path = "gameui/denglujiangli_hun.png";
        scale = 1.1;
    }
        
    
    if(!itemBg)
    {
        itemBg = GetSprite(path);
    }
    itemBg->setScale(scale);
    if(!msg.empty())
        addLabelBMFont(itemBg, msg.c_str(), "bmfont/Sumo_22_yellow.fnt", ccp(60, 30))->setScale(contentScale());
    return itemBg;
}
    
std::string RewardBoard::getRewardTitle(const Treasure& reward)
{
    if(reward.type == LoginRewardType::Reward_Coins)
        return "铜钱"+toStr(reward.count)+"个";
    else if(reward.type == LoginRewardType::Reward_PercentCoins)
        return "铜钱";
    else if(reward.type == LoginRewardType::Reward_Exp)
        return "经验"+toStr(reward.count);
    else if(reward.type == LoginRewardType::Reward_PercentExp)
        return "经验";
    else if(reward.type == LoginRewardType::Reward_Gold)
        return "元宝"+toStr(reward.count)+"个";
    else if(reward.type == LoginRewardType::Reward_Hero)
        return HPT(reward.ident).nameCN;
    else if(reward.type == LoginRewardType::Reward_Soul)
        return HPT(reward.ident).nameCN+"魂魄"+toStr(reward.count)+"个";
    return "";
}
    
void RewardBoard::initCurrPos()
{
    currNode = path[path.size()-1];

    CCSprite* target;
    CCPoint standPoint = currNode!=-1 ? items[currNode]->getPosition() : startPoint;;
    if(currNode%4 != 2)
    {
        currLeft = currNode+1;
        target = items[currLeft];
        CCSprite* blinkFrame = GetSprite("gameui/denglujiangli_biankuang.png");
        blinkFrame->setPosition(target->getPosition());
        arrowLayer->addChild(blinkFrame);
        blink(blinkFrame);
        connect(arrowLayer, standPoint, target->getPosition(), false);
    }
    
    if(currNode+4 < items.size())
    {
        currUp = currNode+4;
        target = items[currUp];
        CCSprite* blinkFrame = GetSprite("gameui/denglujiangli_biankuang.png");
        blinkFrame->setPosition(target->getPosition());
        arrowLayer->addChild(blinkFrame);
        blink(blinkFrame);
        connect(arrowLayer, standPoint, target->getPosition(), false);
    }

    anim = CCAnimation::create();
    anim->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("loading_anim2.png"));
    anim->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("loading_anim3.png"));
    anim->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("loading_anim3.png"));
    anim->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("loading_anim1.png"));
    anim->setDelayPerUnit(0.15);
    anim->retain();
    
    man = GetSprite("loading_anim1.png");
    //man->runAction(CCRepeatForever::create(CCAnimate::create(anim)));
    man->setScale(0.8);
    man->setFlipX(true);
    man->setAnchorPoint(ccp(0.5, 0));
    man->setPosition(getManPos(standPoint));
    addChild(man);
}
    
void RewardBoard::blink(CCSprite* node)
{
    node->runAction(CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.8, 100),
                                                               CCFadeTo::create(0.8, 255),
                                                               NULL)));
}
    
void RewardBoard::choose(int index)
{
    
    arrowLayer->removeFromParentAndCleanup(true);
    path.push_back(index);
    
    man->runAction(CCSpawn::create(CCAnimate::create(anim),
                                   CCSequence::create(CCMoveTo::create(0.5, getManPos(items[index]->getPosition())),
                                                      CCCallLambda::create([=]()
                                                                           {
                                                                               msgLabel->setString("[color=0xff000000]正在确认奖励......[/color]");
                                                                               this->createRoute(path);
                                                                               this->connectServer(index);
                                                                           }),
                                                      NULL),
                                   NULL)
                   );

    currLeft = currUp = -1;
}
    
void RewardBoard::connectServer(int index)
{
    Treasure reward = rewards[index];
    boost::function<void ()> chooseCall;
    if(index < 18)
        chooseCall = [=]()
        {
            LoginRewards rewards;
            rewards.push_back(reward);
            inform->cancel();
            SystemInform::alertRewardTips(GameScene::getInstance(),
                                          "恭喜获得",
                                          rewards,
                                          [](){},"[color=#ff550000]再登录[color=0xffff0000]"+toStr(8-path.size())+"[/color]天就可以获得至尊宝箱！[/color]");
        };
    else
        chooseCall = [=]()
        {
            
            std::vector<Treasure> rewards(PlayerData::getInstance()->loginRewards.end()-3,
                                          PlayerData::getInstance()->loginRewards.end());
            inform->cancel();
            SystemInform::alertRewardTips(GameScene::getInstance(),
                                          "恭喜获得登录大奖",
                                          rewards,
                                          [](){});
        };
    
    PuzzleRPC::getInstance()->chooseLoginReward(PlayerData::getInstance()->getToken(),
                                                index,
                                                [=](std::auto_ptr<Player> player,
                                                    std::auto_ptr<Error> e)
    {
        if(e.get())
        {
            SystemInform::alertMessage(runningScene(),
                                       "网络错误，请重新连接",
                                       "提示",
                                       [=](){connectServer(index);});
            return;
        }
        PlayerData::getInstance()->updatePlayer(player.get());
        chooseCall();
    });
}
    
void RewardBoard::createRoute(const std::vector<int>& path)
{
    if(path.size() < 2)
        return;
    routeLayer->removeAllChildrenWithCleanup(true);
    connect(routeLayer, startPoint, items[path[1]]->getPosition());
    for(int id = 2; id < path.size(); id++)
    {
        items[path[id-1]]->setColor(ccc3(100, 100, 100));
        connect(path[id-1], path[id]);
    }
}
    
void RewardBoard::connect(int dot1, int dot2)
{
    CCPoint p1 = items[dot1]->getPosition();
    CCPoint p2 = items[dot2]->getPosition();
    connect(routeLayer, p1, p2);
}
    
CCSprite* RewardBoard::connect(CCLayer* parent, const CCPoint& p1, const CCPoint& p2,  bool isRoad)
{
    CCSprite* road;
    if(isRoad)
        road = GetSprite("gameui/honglujing.png");
    else
    {
        road = GetSprite("gameui/denglujiangli_jiantou.png");
        road->runAction(CCRepeatForever::create(CCSequence::create(
                                                   CCPlace::create(ccp((p1.x+p2.x)*0.5f, (p1.y+p2.y)*0.5f)),
                                                   CCMoveBy::create(0.8, ccp((p2.x-p1.x)*0.5f,(p2.y-p1.y)*0.5f)),
                                                   NULL)));
    }
    if(p1.x == p2.x)
        road->setRotation(-90);
    road->setPosition(ccp((p1.x+p2.x)*0.5f, (p1.y+p2.y)*0.5f));
    parent->addChild(road);
    return road;
}
   
    
CCPoint RewardBoard::getManPos(const CCPoint& pos)
{
    return ccp(pos.x-15, pos.y - 50);
}
//////////////////////////////////////////
//SystemInform
//////////////////////////////////////////
    
SystemInform* SystemInform::announceInform = NULL;
SystemInform* SystemInform::landRewardInform = NULL;
    
void SystemInform::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,
                                                                            UI_TOUCH_ORDER_MODAL,
                                                                            true);
}
    
// static factory methods
void SystemInform::alertMessage(CCNode *parent,
                                const char *msg,
                                const char* title,
                                boost::function<void()> ycall)
{
    CCSize winSize = CCEGLView::sharedOpenGLView()->getVisibleSize();
    CCPoint p = ccp(winSize.width * 0.5, winSize.height * 0.5);
    
    SystemInform* o = new SystemInform();
    if(o && o->initWithMessage(msg, title , p, true, boost::move(ycall)))
    {
        o->autorelease();
        parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(o);
}
    
void SystemInform::alertMessage(CCNode* parent,
                                const char* msg,
                                const char* title,
                                CCPoint center,
                                bool useBg,
                                boost::function<void()> ycall)
{
    SystemInform* o = new SystemInform();
    if(o && o->initWithMessage(msg, title , center, useBg, boost::move(ycall)))
    {
        o->autorelease();
        parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(o);
}

    
bool SystemInform::initWithMessage(const char* message,
                                   const char* title,
                                   CCPoint p,
                                   bool useBg,
                                   boost::function<void()> ycall)
{
    if(! CCLayer::init() )
        return false;
    
    this->setTouchEnabled(true);
    
    this->yesCall = boost::move(ycall);
    
    // background with solid color
    if(useBg)
    {
        this->background = CCLayerColor::create(ccc4(0, 0, 0, 100));
        this->addChild(background, -1);
    }
    else
        this->background = NULL;
    
    // set a max text width but let label auto calculate the height
    const float MAX_TEXT_WIDTH = 360.0f;

    CCLabelTTF *t = CCLabelTTF::create(title,
                                       FONT_CN,
                                       36,
                                       CCSize(MAX_TEXT_WIDTH, 0),
                                       kCCTextAlignmentCenter);

    CCLabelTTF *label = CCLabelTTF::create(message,
                                           FONT_CN,
                                           30,
                                           CCSizeMake(MAX_TEXT_WIDTH, 0),
                                           CCTextAlignment::kCCTextAlignmentCenter);
    
    float titleHeight = t->getContentSize().height;
    float maxWindowHeight = titleHeight + label->getContentSize().height + 140.0f; // with padding

    float MAX_WINDOW_WIDTH = MAX_TEXT_WIDTH + 60.0f;
    
    t->setColor(TITLE_YELLOW);
    t->setAnchorPoint(ccp(0.5 , 1.0));
    t->setPosition(ccp(MAX_WINDOW_WIDTH * 0.5f, maxWindowHeight - 20.0f));
    
    // anchor top center and pad -30 from top
    label->setAnchorPoint(ccp(0.5, 1.0));
    label->setPosition(ccp(MAX_WINDOW_WIDTH * 0.5f, maxWindowHeight - 30.0f - titleHeight));
    
    CCMenu* menu = CCMenu::create();
    menu->setTouchPriority(UI_TOUCH_ORDER_MODAL_MENU);

    CCMenuItem *item = CCMenuItemSprite::create(createBtn("确定", ButtonType::NORMAL_RED),
                                                createBtn("确定", ButtonType::NORMAL_RED),
                                                this,
                                                menu_selector(SystemInform::onConfirm));
    menu->setPosition(MAX_WINDOW_WIDTH * 0.5f, 60.0f);  // offset on x is because asset has shadow
    menu->addChild(item);

    // create window content
    CCScale9Sprite* window = Get9SpriteFromFrame("dialog_bg.png",
                                                 CCRectMake(50, 50, 208, 112));
    window->setPosition(p);
    window->setContentSize(CCSize(MAX_WINDOW_WIDTH, maxWindowHeight));
    
    window->addChild(t);
    window->addChild(label);
    window->addChild(menu);

    // this will change bounding box size, so it should only run at the end
    window->setScale(0.0f);
    window->runAction(CCEaseSineOut::create(CCScaleTo::create(0.2, 1.0f)));
    
    this->addChild(window);
    return true;
}


    void SystemInform::alertTips(CCNode *parent,
                                 const char *msg,
                                 const char* title)
    {
        CCSize winSize = CCEGLView::sharedOpenGLView()->getVisibleSize();
        CCPoint p = ccp(winSize.width * 0.5, winSize.height * 0.5);
        
        SystemInform* o = new SystemInform();
        if(o && o->initWithTips(msg, title, p))
        {
            o->autorelease();
            parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
        }
        else
            CC_SAFE_DELETE(o);
    }
    
    bool SystemInform::initWithTips(const char* message,
                                    const char* title,
                                    CCPoint p)
    {
        if(! CCLayer::init() )
            return false;
        
        this->setTouchEnabled(true);
        
        // set a max text width but let label auto calculate the height
        const float MAX_TEXT_WIDTH = 360.0f;
        
        CCLabelTTF *t = CCLabelTTF::create(title,
                                           FONT_CN,
                                           36,
                                           CCSize(MAX_TEXT_WIDTH, 0),
                                           kCCTextAlignmentCenter);
        
        
        CCLabelTTF *label = CCLabelTTF::create(message,
                                               FONT_CN,
                                               30,
                                               CCSizeMake(MAX_TEXT_WIDTH, 0), // auto calculate height
                                               CCTextAlignment::kCCTextAlignmentCenter);
        
        float titleHeight = t->getContentSize().height;
        float maxWindowHeight = titleHeight + label->getContentSize().height + 140.0f; // with padding
        
        t->setColor(TITLE_YELLOW);
        t->setAnchorPoint(ccp(0.5 , 1.0));
        t->setPosition(ccp(MAX_TEXT_WIDTH * 0.5f + 20.0f, maxWindowHeight - 20.0f));
        
        // anchor top center and pad -30 from top
        label->setAnchorPoint(ccp(0.5, 1.0));
        label->setPosition(ccp(MAX_TEXT_WIDTH * 0.5f + 20.0f, maxWindowHeight - 30.0f - titleHeight));
        
        
        // create window content
        CCScale9Sprite* window = Get9SpriteFromFrame("dialog_bg.png",
                                                     CCRectMake(50, 50, 208, 112));
        window->setPosition(p);
        window->setContentSize(CCSize(MAX_TEXT_WIDTH+40, maxWindowHeight));
        
        window->addChild(t);
        window->addChild(label);
        
        
        // this will change bounding box size, so it should only run at the end
        
        this->runAction(CCSequence::create(CCSpawn::create(CCTargetedAction::create(window, CCFadeOut::create(2)),
                                                            CCTargetedAction::create(t, CCFadeOut::create(2)),
                                                            CCTargetedAction::create(label, CCFadeOut::create(2)),
                                                            NULL),
                                           CCCallLambda::create([=](){cancel();}),
                                           NULL));
        
        this->addChild(window);
        
        return true;
    }

SystemInform* SystemInform::alertLoading(CCNode* parent)
{
    SystemInform* o = new SystemInform();
    
    if(o && o->initWithLoading())
    {
        o->autorelease();
        parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
        return o;
    }
    
    CC_SAFE_DELETE(o);
    return NULL;
}

static CCAnimation* mLoadingAnim = NULL;
    
bool SystemInform::initWithLoading()
{
    if(! CCLayer::init() )
        return false;
    
    this->setTouchEnabled(true);
    
	this->yesCall = [](){};
	this->noCall = [](){};
    
    {
        this->background = CCLayerColor::create(ccc4(0, 0, 0, 100));
        this->addChild(background, -1);
    }
    
    //------------------
    // create bakcground
    CCScale9Sprite* bg = Get9SpriteFromFrame("dialog_bg.png", CCRectMake(50, 50, 208, 112));
    bg->setContentSize(CCSize(450, 210));
    bg->setOpacity(200);

    CCSize winSize = CCEGLView::sharedOpenGLView()->getVisibleSize();
    CCPoint p = ccp(winSize.width * 0.5, winSize.height * 0.5);
    bg->setPosition(p);
    
    //-------------
    // create label
    CCLabelTTF *label = CCLabelTTF::create("网络连接中...", FONT_CN, 30);
    label->setAnchorPoint(ccp(0.5f, 1.0f));
    label->setPosition(ccp(300, 170));
    bg->addChild(label);
    
    CCLabelTTF* sub = CCLabelTTF::create("请在稳定的网络环境\n下进行游戏！", FONT_CN, 24);
    sub->setAnchorPoint(ccp(0.5, 1.0));
    sub->setPosition(ccp(300, 100));
    sub->setColor(TITLE_YELLOW);
    bg->addChild(sub);

    if(!mLoadingAnim)
    {
        mLoadingAnim = CCAnimation::create();
        mLoadingAnim->retain();
        
        int numFrames = 4;
        for(int i=1; i<=numFrames; ++i)
        {
            char name[128];
            sprintf(name, "loading_anim%d.png", i);
            CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
            
            assert(frame != NULL && "cannot find frames");
            
            mLoadingAnim->addSpriteFrame(frame);
        }
        
        mLoadingAnim->setDelayPerUnit(0.18);
    }
    
    CCSprite* jumpAnim = CCSprite::create();
    jumpAnim->runAction(CCRepeatForever::create( CCAnimate::create(mLoadingAnim)) );
    jumpAnim->setPosition(ccp(115, 24 + 140));
    bg->addChild(jumpAnim);
    
    this->addChild(bg);
    
    return true;
}
    
void SystemInform::alertAnnoucement(CCNode* parent,
                                    const std::string& title,
                                    const std::string& annoucement,
                                    boost::function<void()> ycall)
{
    if(announceInform)
        announceInform->cancel();
    announceInform = new SystemInform;
    if(announceInform && announceInform->initWithAnnoucement(title, annoucement, true, boost::move(ycall)))
    {
        announceInform->autorelease();
        parent->addChild(announceInform, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(announceInform);
}

bool SystemInform::initWithAnnoucement(const std::string& title,
                                       const std::string& annoucement,
                                       bool useBg,
                                       boost::function<void()> ycall)
{
    if(! CCLayer::init() )
        return false;
    
    this->setTouchEnabled(true);
    
    this->yesCall = boost::move(ycall);
    
    // background with solid color
    if(useBg)
    {
        this->background = CCLayerColor::create(ccc4(0, 0, 0, 100));
        this->addChild(background, -1);
    }
    else
        this->background = NULL;
    
    CCSprite* messageBoardBG = GetSprite("gameui/gonggao_bg.png");
    messageBoardBG->setPosition(ccp(320, 480));

    CCLabelTTF* titleLabel = CCLabelTTF::create(title.c_str(), FONT_CN, 46);
    titleLabel->setColor(ccc3(67, 2, 0));
    titleLabel->setAnchorPoint(ccp(0.5f, 0.0f));
    titleLabel->setPosition(ccp(320, 745));
    messageBoardBG->addChild(titleLabel);
    
    WebViewLayer* webview = WebViewLayer::create(CCRect(70, 130, 498, 630));
    webview->opaque(false);
    webview->loadHTML(annoucement);

    webview->beforeStartLoadRequest = [](CCHttpRequest& req)->bool
    {
        if (boost::algorithm::istarts_with(req.getUrl(), "http"))
        {
            cocos2d::extension::CCNative::openURL(req.getUrl());
            return false;
        }
        return true;
    };
    messageBoardBG->addChild(webview);
    
    CCMenu* menu = CCMenu::create();
    menu->setTouchPriority(UI_TOUCH_ORDER_MODAL_MENU);
    
    CCMenuItem *item = CCMenuItemSprite::create(createBtn("确定", ButtonType::NORMAL_RED),
                                                createBtn("确定", ButtonType::NORMAL_RED),
                                                this,
                                                menu_selector(SystemInform::onConfirm));
    menu->setPosition(320, 80.0f);  // offset on x is because asset has shadow
    menu->addChild(item);
    messageBoardBG->addChild(menu);
    
    // this will change bounding box size, so it should only run at the end
    messageBoardBG->setScale(0.0f);
    messageBoardBG->runAction(CCEaseSineOut::create(CCScaleTo::create(0.2, 1.0f)));

    this->addChild(messageBoardBG);

    return true;
}

// Alert Panel
void SystemInform::alertConfirmPanel(CCNode* parent,
                                     const char* msg,
                                     const char* title,
                                     boost::function<void()> yesCall,
                                     boost::function<void()> noCall,
                                     bool hasSchedule,
                                     std::string btnLabel,
                                     std::string btnLabel2)
{
    
    SystemInform* o = new SystemInform();
    
    // auto center on screen
    CCSize winSize = CCEGLView::sharedOpenGLView()->getVisibleSize();
    CCPoint p = ccp(winSize.width * 0.5f , winSize.height * 0.5f);
    
    if(o && o->initWithPanel(msg, title , p, boost::move(yesCall), boost::move(noCall), hasSchedule, btnLabel, btnLabel2))
    {
        o->autorelease();
        parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(o);
}
    
void SystemInform::alertConfirmPanel(CCNode* parent,
                                     const char* msg,
                                     const char* title,
                                     CCPoint p,
                                     boost::function<void()> yesCall,
                                     boost::function<void()> noCall)
{
    
    SystemInform* o = new SystemInform();
    if(o && o->initWithPanel(msg, title , p, boost::move(yesCall), boost::move(noCall)))
    {
        o->autorelease();
        parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(o);
}
    
void SystemInform::alertConfirmPanel2(CCNode* parent,
                               const char* msg,
                               const char* title,
                               boost::function<void()> call1,
                               boost::function<void()> call2,
                               boost::function<void()> call3,
                               boost::function<void()> call4)
{
    SystemInform* o = new SystemInform();
    
    CCSize winSize = CCEGLView::sharedOpenGLView()->getVisibleSize();
    CCPoint p = ccp(winSize.width * 0.5f , winSize.height * 0.5f);
    if(o && o->initWithPanel2(msg, title , p, boost::move(call1), boost::move(call2) ,
                              boost::move(call3),boost::move(call4)))
    {
        o->autorelease();
        parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(o);
}
    
    
bool SystemInform::initWithPanel(const char* msg,
                                 const char* title,
                                 CCPoint p,
                                 boost::function<void()> yesCall,
                                 boost::function<void()> noCall,
                                 bool hasSchedule,
                                 std::string btnLabel,
                                 std::string btnLabel2)
{
	if(! CCLayer::init() )
        return false;
    
    this->setTouchEnabled(true);

	this->yesCall = boost::move(yesCall);
	this->noCall = boost::move(noCall);
    
    {
        this->background = CCLayerColor::create(ccc4(0, 0, 0, 100));
        this->addChild(background, -1);
    }
    
    const float MaxTextWidth = 400;
    
    CCLabelTTF *t = CCLabelTTF::create(title,
                                       FONT_CN,
                                       36,
                                       CCSize(MaxTextWidth, 0),
                                       kCCTextAlignmentCenter);
    t->setColor(TITLE_YELLOW);
    
    //-------------
    // create label
    CCLabelTTF *label = CCLabelTTF::create(msg,
                                           FONT_CN,
                                           30,
                                           CCSize(MaxTextWidth, 0),
                                           kCCTextAlignmentCenter);
    
    float titleHeight = t->getContentSize().height;
    float textLabelHeight = label->getContentSize().height;
    float backgroundHeight =titleHeight + textLabelHeight + 130;
    
    
    //------------------
    // create bakcground
    CCScale9Sprite* bg = Get9SpriteFromFrame("dialog_bg.png", CCRectMake(50, 50, 208, 112));
    bg->setContentSize(CCSize(MaxTextWidth+40, backgroundHeight));
    bg->setOpacity(200);
    bg->addChild(t);
    bg->addChild(label);

    //------------------------------------
    // now set label according to bg size
    t->setAnchorPoint(ccp(0.5f, 1.0f));
    t->setPosition(ccp(MaxTextWidth * 0.5f+20, backgroundHeight - 20.0f));
    
    label->setAnchorPoint(ccp(0.5f, 1.0f));
    label->setPosition(ccp(MaxTextWidth * 0.5f+20, backgroundHeight - 30.0f - titleHeight));
    
    {
        CCMenu* menu = CCMenu::create();
        menu->setTouchPriority(UI_TOUCH_ORDER_MODAL_MENU);
        
        CCMenuItem* confirmItem = CCMenuItemSprite::create(createBtn(btnLabel, ButtonType::NORMAL_RED),
                                                           createBtn(btnLabel, ButtonType::NORMAL_RED),
                                                           this,
                                                           menu_selector(SystemInform::onConfirm));
        menu->addChild(confirmItem);

        CCMenuItem* cancelItem = CCMenuItemSprite::create(createBtn(btnLabel2, ButtonType::NORMAL_RED),
                                                          createBtn(btnLabel2, ButtonType::NORMAL_RED),
                                                          this,
                                                          menu_selector(SystemInform::onCancel));
        menu->addChild(cancelItem);
        
        menu->alignItemsHorizontallyWithPadding(10);
        menu->setPosition(ccp(MaxTextWidth * 0.5f+20, 60));
        bg->addChild(menu);
        
        //this
        if(hasSchedule)
        {
            timeCount = 10;
            CCLabelBMFont* count = addLabelBMFont(bg, toStr(timeCount).c_str(),
                                                  "bmfont/Molot_30_red.fnt",
                                                  ccp(MaxTextWidth * 0.5f-50, 60),
                                                  ccp(0.5,0.5));
            
            confirmItem->setEnabled(false);
            
            auto* seq = CCSequence::create(CCRepeat::create(CCSequence::create(CCDelayTime::create(1.0f),
                                                                               CCCallLambda::create([=](){count->setString(toStr(--timeCount).c_str());}),
                                                                               NULL),
                                                            timeCount),
                                           CCCallLambda::create([=]()
                                                                {
                                                                    confirmItem->setEnabled(true);
                                                                    count->removeFromParentAndCleanup(true);
                                                                }),
                                           NULL);
            count->runAction(seq);
        }
        
    }
    
    bg->setPosition(p);
    
    this->addChild(bg);

	return true;
}
    
bool SystemInform::initWithPanel2(const char* msg,
                const char* title,
                CCPoint p,
                boost::function<void()> call1,
                boost::function<void()> call2,
                boost::function<void()> call3,
                boost::function<void()> call4)
{
    if(! CCLayer::init() )
        return false;
    
    this->setTouchEnabled(true);
    
    {
        this->background = CCLayerColor::create(ccc4(0, 0, 0, 100));
        this->addChild(background, -1);
    }
    
    const float MaxTextWidth = 460;
    
    CCLabelTTF *t = CCLabelTTF::create(title,
                                       FONT_CN,
                                       36,
                                       CCSize(MaxTextWidth, 0),
                                       kCCTextAlignmentCenter);
    t->setColor(TITLE_YELLOW);
    
    //-------------
    // create label
    CCLabelTTF *label = CCLabelTTF::create(msg,
                                           FONT_CN,
                                           30,
                                           CCSize(MaxTextWidth, 0),
                                           kCCTextAlignmentCenter);
    
    float titleHeight = t->getContentSize().height;
    float textLabelHeight = label->getContentSize().height;
    float backgroundHeight =titleHeight + textLabelHeight + 180;
    
    
    //------------------
    // create bakcground
    /*
    CCScale9Sprite *bg = CCScale9Sprite::create("gameui/jinengtishi_bg.png",
                                                CCRectMake(0, 0, 308, 212),
                                                CCRectMake(50, 50, 208, 112));
     */
    CCScale9Sprite* bg = Get9SpriteFromFrame("dialog_bg.png", CCRectMake(50, 50, 208, 112));
    bg->setContentSize(CCSize(MaxTextWidth+40, backgroundHeight));
    bg->setOpacity(200);
    bg->addChild(t);
    bg->addChild(label);
    
    //------------------------------------
    // now set label according to bg size
    t->setAnchorPoint(ccp(0.5f, 1.0f));
    t->setPosition(ccp(MaxTextWidth * 0.5f+20, backgroundHeight - 20.0f));
    
    label->setAnchorPoint(ccp(0.5f, 1.0f));
    label->setPosition(ccp(MaxTextWidth * 0.5f+20, backgroundHeight - 30.0f - titleHeight));
    
    {
        ButtonSprite* btn1 = ButtonSprite::create(createBtn("扩充", ButtonType::NORMAL_RED),
                                                  createBtn("扩充", ButtonType::NORMAL_RED),
                                                  UI_TOUCH_ORDER_MODAL_MENU,
                                                  false);
        btn1->setOnClickListener([=](){
            call1();
            this->cancel();
        });
        btn1->setPosition(ccp(MaxTextWidth*0.5f-135, 110));
        bg->addChild(btn1);
        
        ButtonSprite* btn2 = ButtonSprite::create(createBtn("强化", ButtonType::NORMAL_RED),
                                                  createBtn("强化", ButtonType::NORMAL_RED),
                                                  UI_TOUCH_ORDER_MODAL_MENU,
                                                  false);
        btn2->setPosition(ccp(MaxTextWidth*0.5f+15, 110));
        btn2->setOnClickListener([=](){
            call2();
            this->cancel();
        });
        bg->addChild(btn2);
        ButtonSprite* btn3 = ButtonSprite::create(createBtn("出售", ButtonType::NORMAL_RED),
                                                  createBtn("出售", ButtonType::NORMAL_RED),
                                                  UI_TOUCH_ORDER_MODAL_MENU,
                                                  false);
        btn3->setPosition(ccp(MaxTextWidth*0.5f+165, 110));
        btn3->setOnClickListener([=](){
            call3();
            this->cancel();
        });
        bg->addChild(btn3);
        ButtonSprite* btn4 = ButtonSprite::create(createBtn("VIP", ButtonType::NORMAL_RED),
                                                  createBtn("VIP", ButtonType::NORMAL_RED),
                                                  UI_TOUCH_ORDER_MODAL_MENU,
                                                  false);
        btn4->setPosition(ccp(MaxTextWidth*0.5f+15, 45));
        btn4->setOnClickListener([=](){
            call4();
            this->cancel();
        });
        bg->addChild(btn4);
    }
    
    bg->setPosition(p);
    
    this->addChild(bg);
    
	return true;
}

void SystemInform::alertSkill(CCNode *parent,
                              const std::string& title,
                              const std::string& detail,
                              const std::string& energy,
                              CCPoint p,
                              boost::function<void ()> ycall,
                              boost::function<void ()> ncall)
{
    SystemInform* o = new SystemInform();
    if(o && o->initWithSkill(title, detail, energy, p, boost::move(ycall), boost::move(ncall)))
    {
        o->autorelease();
        parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(o);
}
 

bool SystemInform::initWithSkill(const std::string& title,
                                 const std::string& detail,
                                 const std::string& energy,
                                 CCPoint p,
                                 boost::function<void()> ycall,
                                 boost::function<void()> ncall)
{
    if(! CCLayer::init() )
        return false;
    
    this->setTouchEnabled(true);
    
    this->yesCall = boost::move(ycall);
    this->noCall = boost::move(ncall);
    
    this->background = NULL;
    
    const float MaxTextWidth = 400;
    
    //-------------
    // create label
    CCLabelTTF* titleLabel = CCLabelTTF::create(title.c_str(), FONT_CN, 36, CCSize(0, 0),
                                                kCCTextAlignmentCenter);
    titleLabel->setColor(TITLE_YELLOW);

    std::string detailStr = detail + "\n" + "需要集气：" + energy;
    CCLabelTTF *label = CCLabelTTF::create(detailStr.c_str(),
                                           FONT_CN,
                                           24,
                                           CCSize(MaxTextWidth, 0),
                                           kCCTextAlignmentCenter);
    
    std::string energyStr = "确定发动技能吗？";
    CCLabelTTF* energyLabel = CCLabelTTF::create(energyStr.c_str(), FONT_CN, 24);
    
    
    float textLabelHeight = label->getContentSize().height;
    float backgroundHeight = textLabelHeight + 260;
    
    //------------------
    // create bakcground
    CCScale9Sprite* bg = Get9SpriteFromFrame("dialog_bg.png",
                                             CCRectMake(50, 50, 208, 112));
    bg->setContentSize(CCSize(MaxTextWidth + 30, backgroundHeight));
    bg->setOpacity(200);
    bg->addChild(titleLabel);
    
    bg->addChild(label);
    
    bg->addChild(energyLabel);

    //------------------------------------
    // now set label according to bg size
    titleLabel->setAnchorPoint(ccp(0.5, 1.0f));
    titleLabel->setPosition(ccp(MaxTextWidth * 0.5f + 15, backgroundHeight - 40.0f));
    
    label->setAnchorPoint(ccp(0.5f, 1.0f));
    label->setPosition(ccp(MaxTextWidth * 0.5f + 15, backgroundHeight - 100.0f));
    
    energyLabel->setAnchorPoint(ccp(0.5f, 1.0f));
    energyLabel->setPosition(ccp(MaxTextWidth * 0.5f + 15, 120.0f));

    {
        CCMenu* menu = CCMenu::create();
        menu->setTouchPriority(UI_TOUCH_ORDER_MODAL_MENU);
        
        CCMenuItem* confirmItem = CCMenuItemSprite::create(createBtn("确定", ButtonType::NORMAL_RED),
                                                           createBtn("确定", ButtonType::NORMAL_RED),
                                                           this,
                                                           menu_selector(SystemInform::onConfirm));
        menu->addChild(confirmItem);
        
        CCMenuItem* cancelItem = CCMenuItemSprite::create(createBtn("取消", ButtonType::NORMAL_RED),
                                                          createBtn("取消", ButtonType::NORMAL_RED),
                                                          this,
                                                          menu_selector(SystemInform::onCancel));
        menu->addChild(cancelItem);
        
        menu->alignItemsHorizontallyWithPadding(10);
        menu->setPosition(ccp(MaxTextWidth * 0.5f + 15, 50));
        bg->addChild(menu);
    }
    
    bg->setPosition(p);
    
    this->addChild(bg);
    
    return true;
}
    
void SystemInform::alertBattleExitMenu(CCNode *parent,
                                       const std::string& stageName,
                                       const std::string& levelName,
                                       int round,
                                       int maxRound,
                                       int heroDrop,
                                       int soulDrop,
                                       boost::function<void ()> yes)
{
    SystemInform* o = new SystemInform();
    if(o && o->initBattleExitMenu(stageName, levelName,
                                  round, maxRound,
                                  heroDrop, soulDrop, boost::move(yes)))
    {
        o->autorelease();
        parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(o);
}

bool SystemInform::initBattleExitMenu(const std::string& stageName,
                                      const std::string& levelName,
                                      int round,
                                      int maxRound,
                                      int heroDrop,
                                      int soulDrop,
                                      boost::function<void()> yes)
{
    if(! CCLayer::init())
        return false;
    
    this->setTouchEnabled(true);
    
    this->yesCall = boost::move(yes);
    this->noCall = [](){};
    
    {
        this->background = CCLayerColor::create(ccc4(0, 0, 0, 200));
        this->addChild(background, -1);
    }
    
    CCSprite * bg = GetSprite("ui/zanting_bg.png");
    bg->setAnchorPoint(ccp(0.5f, 0.5f));
    bg->setPosition(ccp(320, 480));
    this->addChild(bg);
    
    std::string title = stageName + "之" + levelName;
    CCLabelTTF* titleText = CCLabelTTF::create(title.c_str(), FONT_CN, 40);
    titleText->setAnchorPoint(ccp(0.5f, 0.5f));
    titleText->setPosition(ccp(bg->getContentSize().width/2, 485));
    bg->addChild(titleText);
    
    std::string text = toStr(round) + "/" + toStr(maxRound);
    CCLabelTTF* roundText = CCLabelTTF::create(text.c_str(), FONT_NUM, 40);
    roundText->setAnchorPoint(ccp(0.f, 0.f));
    roundText->setPosition(ccp(290, 410));
    bg->addChild(roundText);
    
    CCLabelTTF * heroText = CCLabelTTF::create(toStr(heroDrop).c_str(), FONT_NUM, 32);
    heroText->setColor(ccc3(55,55,55));
    heroText->setAnchorPoint(ccp(0.5f, 0.5f));
    heroText->setPosition(ccp(200, 385));
    bg->addChild(heroText);
    
    CCLabelTTF * soulText = CCLabelTTF::create(toStr(soulDrop).c_str(), FONT_NUM, 32);
    soulText->setColor(ccc3(55,55,55));
    soulText->setAnchorPoint(ccp(0.5f, 0.5f));
    soulText->setPosition(ccp(463, 385));
    bg->addChild(soulText);
    
    for(int i=0; i<8; i++)
    {
        
    }
    
    {
        CCMenu* menu = CCMenu::create();
        menu->setTouchPriority(UI_TOUCH_ORDER_MODAL_MENU);
        
        CCMenuItem* confirmItem = CCMenuItemSprite::create(createBtn("逃跑", ButtonType::NORMAL_RED),
                                                           createBtn("逃跑", ButtonType::NORMAL_RED),
                                                           this,
                                                           menu_selector(SystemInform::onConfirm));
        menu->addChild(confirmItem);
        
        CCMenuItem* cancelItem = CCMenuItemSprite::create(createBtn("返回", ButtonType::NORMAL_BLUE),
                                                          createBtn("返回", ButtonType::NORMAL_BLUE),
                                                          this,
                                                          menu_selector(SystemInform::onCancel));
        menu->addChild(cancelItem);
        
        //menu->alignItemsVerticallyWithPadding(50);
        menu->alignItemsHorizontallyWithPadding(50);
        menu->setPosition(ccp(320, 263));
        this->addChild(menu);
    }
    
    return true;
}

void SystemInform::alertHint(CCNode *parent,
                             const std::string& hint,
                             boost::function<void ()> yes)
{
    SystemInform* o = new SystemInform();
    if(o && o->initWithHint(hint, boost::move(yes)))
    {
        o->autorelease();
        parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(o);
}
    
bool SystemInform::initWithHint(const std::string& str,
                                boost::function<void()> yes)
{
    if(! CCLayer::init() )
        return false;
    
    this->setTouchEnabled(true);
    
    this->yesCall = boost::move(yes);
    this->noCall = [=](){};
    
    {
        CCSprite* bg = GetSprite("ui/tishi_huang.png");
        bg->setAnchorPoint(ccp(0.5, 1.0));
        bg->setPosition(ccp(320, 530));
        this->addChild(bg);

        CCLabelTTF* title = CCLabelTTF::create("小提示", FONT_CN, 50);
        title->setAnchorPoint(ccp(0.5f, 0.0f));
        title->setPosition(ccp(320, 437));
        title->setColor(ccc3(100, 0, 0));
        this->addChild(title);
        
        CCLabelTTF* hintBody = CCLabelTTF::create(str.c_str(),
                                                  FONT_CN,
                                                  34,
                                                  CCSize(458, 286),
                                                  kCCTextAlignmentCenter,
                                                  kCCVerticalTextAlignmentCenter);
        hintBody->setAnchorPoint(ccp(0.5, 0.5));
        hintBody->setPosition(ccp(320, 280));
        hintBody->setColor(ccc3(0, 0, 0));
        hintBody->setDimensions(CCSize(458, 286));
        this->addChild(hintBody);
        
        CCMenu* menu = CCMenu::create();
        menu->setTouchPriority(UI_TOUCH_ORDER_MODAL_MENU);
        
        CCMenuItem *item = CCMenuItemSprite::create(createBtn("确定", ButtonType::NORMAL_RED),
                                                    createBtn("确定", ButtonType::NORMAL_RED),
                                                    this,
                                                    menu_selector(SystemInform::onConfirm));
        menu->setPosition(320.0f, 73.0f);  // offset on x is because asset has shadow
        menu->alignItemsHorizontally();
        menu->addChild(item);
        this->addChild(menu);
    }
    
    return true;
}
    
void SystemInform::alertVIPLevelUpInform(CCNode* parent,
                           int levelTo, const VipUpgrade& vipMsg)
{
    SystemInform* o = new SystemInform();
    if(o && o->initVIPLevelUpInform(levelTo, vipMsg))
    {
        o->autorelease();
        parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(o);
}
    
bool SystemInform::initVIPLevelUpInform(int level, const VipUpgrade& vipMsg)
{
    if(! CCLayer::init() )
        return false;
    
    this->setTouchEnabled(true);
    
    this->yesCall = [=](){};
    this->noCall = [=](){};
    
    {
        this->background = CCLayerColor::create(ccc4(0, 0, 0, 200));
        this->addChild(background, -1);
    }
    
    {
        CCSprite* bg = GetSprite("ui/tishi_huang.png");
        bg->setAnchorPoint(ccp(0.5, 0.5));
        bg->setPosition(ccp(320, 480));
        this->addChild(bg);
        
        CCSprite* tips = GetSprite("gameui/viptisheng.png");
        tips->setPosition(ccp(320, 672));
        this->addChild(tips);
        
        CCSprite* yb = GetSprite("gameui/yiduitongqian.png");
        yb->setPosition(ccp(320, 525));
        addChild(yb);
        
        addLabelBMFont(this, ("+"+toStr(vipMsg.coins)).c_str(), "bmfont/Sumo_38_orange.fnt", ccp(320, 450))->setScale(contentScale());
        
        /*CCSprite* tb = GetSprite("gameui/tongqianjiangli.png");
        tb->setPosition(ccp(320, 525));
        addChild(tb);
        
        CCSprite* zt = GetSprite("gameui/zhantiejiangli.png");
        zt->setPosition(ccp(480, 525));
        addChild(zt);*/
        
        std::string msg = "恭喜你成为VIP"+toStr(level)+"!";
        CCLabelTTF* hintBody = CCLabelTTF::create(msg.c_str(),
                                                  FONT_CN,
                                                  34,
                                                  CCSize(458, 40),
                                                  kCCTextAlignmentCenter,
                                                  kCCVerticalTextAlignmentCenter);
        hintBody->setAnchorPoint(ccp(0.5, 0.5));
        hintBody->setPosition(ccp(320, 365));
        hintBody->setColor(ccc3(0, 0, 0));
        this->addChild(hintBody);
        
        CCMenu* menu = CCMenu::create();
        menu->setTouchPriority(UI_TOUCH_ORDER_MODAL_MENU);
        
        CCMenuItem *item = CCMenuItemSprite::create(createBtn("确定", ButtonType::NORMAL_RED),
                                                    createBtn("确定", ButtonType::NORMAL_RED),
                                                    this,
                                                    menu_selector(SystemInform::onConfirm));
        menu->setPosition(320.0f, 286.0f);  // offset on x is because asset has shadow
        menu->alignItemsHorizontally();
        menu->addChild(item);
        this->addChild(menu);
    }
    
    return true;
}
    
void SystemInform::alertPurchaseVipPacks(CCNode* parent, const VIPPack& pack, boost::function<void()> purchaseSucCall)
{
    SystemInform* o = new SystemInform();
    if(o && o->initPurchaseVipPacks(pack, purchaseSucCall))
    {
        o->autorelease();
        parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(o);
}
    
void SystemInform::alertVipPacks(CCNode* parent, const VIPPack& pack, boost::function<void()> purchaseSucCall)
{
    SystemInform* o = new SystemInform();
    if(o && o->initPurchaseVipPacks(pack, purchaseSucCall, false))
    {
        o->autorelease();
        parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(o);
}
    
bool SystemInform::initPurchaseVipPacks(const VIPPack& pack, boost::function<void()> purchaseSucCall, bool isPurchase)
{
    if(!CCLayer::init())
        return false;
    
    this->setTouchEnabled(true);
    
    yesCall = [=]()
    {
        auto purchaseCall = [=](std::auto_ptr<Player> p, std::vector<Treasure> treasure, std::auto_ptr<Error> e)
        {
            if(e.get())
            {
                e->handleErrorInform();
                return;
            }
            
            PlayerData::getInstance()->updatePlayer(p.get());
            std::vector<VIPPack>* packs = &PlayerData::getInstance()->vipPacks;
            int i = 0;
            for(; i < packs->size(); i++)
            {
                if(packs->at(i).pid == pack.pid)
                {
                    packs->at(i).items = treasure;
                    packs->at(i).purchased = true;
                    break;
                }
            }
            
            SystemInform::alertVipPacks(runningScene(), packs->at(i), [](){});
            purchaseSucCall();
        };
        
        if(pack.purchased)
            SystemInform::alertMessage(runningScene(), "你已经购买过这个礼包了！", "提示", [](){});
        else if(PlayerData::getInstance()->player->vipLevel >= pack.availableAtLevel)
        {
            if(!pack.isHeroesPack())
                PuzzleRPC::getInstance()->purchaseVIPPack(PlayerData::getInstance()->getToken(),
                                                          pack.availableAtLevel,
                                                          purchaseCall);
            else
                PuzzleRPC::getInstance()->purchaseVIPOtherPack(PlayerData::getInstance()->getToken(),
                                                               pack.pid,
                                                               purchaseCall);
        }
        
        else
            SystemInform::alertMessage(runningScene(), "你的vip等级不足！", "提示", [](){});
    };
    this->noCall = [=](){};
    
    {
        this->background = CCLayerColor::create(ccc4(0, 0, 0, 100));
        this->addChild(background, -1);
    }
    
    CCSprite* bg = GetSprite("gameui/libao_tankuang.png");
    bg->setAnchorPoint(ccp(0.5, 0.5));
    bg->setPosition(ccp(320, 480));
    this->addChild(bg);
    
    std::string title = isPurchase ? "VIP"+toStr(pack.availableAtLevel)+"礼包内容" : "购买成功";
    addLabelTTF(this, title.c_str(), FONT_CN,
                44, ccp(320, 820), ccp(0.5, 0.5), ccc3(0x55, 0x00, 0x00));
    
    ZqlScrollView* scrollView = ZqlScrollView::create();
    ContentLayer* content = ContentLayer::create();
    for(int i = 0; i < pack.items.size(); i++)
    {
        CCSprite* reward = isPurchase ? createVIPPackItem(pack.items[i]) : createRewardItemSprite(pack.items[i]);
        reward->setPosition(ccp(-160+160*(i%3), -55-150*(i/3)));
        content->addChild(reward);
    }
    content->setContentSize(CCSizeMake(640, 150*(int((pack.items.size()-0.1)/3)+1)));
    scrollView->addLayer(content);
    scrollView->setPosition(ccp(320, 778));
    scrollView->setViewSize(CCSizeMake(640, 554));
    scrollView->setTouchMode(kCCTouchesOneByOne);
    scrollView->setTouchPriority(UI_TOUCH_ORDER_MODAL_MENU);
    scrollView->setTouchEnabled(pack.items.size()>9);
    addChild(scrollView);
    
    CCSprite* priceNow = GetSprite("gameui/tejia.png");
    priceNow->setAnchorPoint(ccp(1, 0.5));
    priceNow->setPosition(ccp(250, 195));
    addChild(priceNow);
    addLabelBMFont(this, toStr(pack.price).c_str(), "bmfont/Sumo_22_yellow.fnt",
                   priceNow->getPosition(), ccp(0, 0.5))->setScale(contentScale());
    
    CCSprite* priceOrg = GetSprite("gameui/yuanjia.png");
    priceOrg->setAnchorPoint(ccp(1, 0.5));
    priceOrg->setPosition(ccp(450, 195));
    addChild(priceOrg);
    addLabelBMFont(this, toStr(pack.showPrice).c_str(), "bmfont/Sumo_22_white_brown.fnt",
                   priceOrg->getPosition(), ccp(0, 0.5))->setScale(contentScale());
    CCScale9Sprite* line = Get9SpriteFromFrame("icon_mask.png", 1, 1, 98, 98);
    line->setContentSize(CCSizeMake(150, 4));
    line->setPosition(ccp(420, 195));
    addChild(line);
    
    CCMenu* menu = CCMenu::create();
    menu->setTouchPriority(UI_TOUCH_ORDER_MODAL_MENU);
    
    if(isPurchase)
    {
        CCMenuItem *item = CCMenuItemSprite::create(createBtn("购买", ButtonType::NORMAL_RED),
                                                    createBtn("购买", ButtonType::NORMAL_RED),
                                                    this,
                                                    menu_selector(SystemInform::onConfirm));
        
        CCMenuItem *item2 = CCMenuItemSprite::create(createBtn("取消", ButtonType::NORMAL_BLUE),
                                                     createBtn("取消", ButtonType::NORMAL_BLUE),
                                                     this,
                                                     menu_selector(SystemInform::onCancel));
        menu->addChild(item);
        menu->addChild(item2);
    }
    else
    {
        CCMenuItem *item2 = CCMenuItemSprite::create(createBtn("确定", ButtonType::NORMAL_RED),
                                                     createBtn("确定", ButtonType::NORMAL_RED),
                                                     this,
                                                     menu_selector(SystemInform::onCancel));
        menu->addChild(item2);
    }
    
    menu->alignItemsHorizontallyWithPadding(70);

    menu->setPosition(320.0f, 140.0f);  // offset on x is because asset has shadow
    this->addChild(menu);

    return true;
}
    
void SystemInform::alertLandReward(CCNode* parent,
                                   const LoginRewards& loginRewards,
                                   const std::vector<int>& path,
                                   boost::function<void()> yes)
{
    if(landRewardInform)
        landRewardInform->cancel();
    landRewardInform = new SystemInform();
    if(landRewardInform && landRewardInform->initWithLandReward(loginRewards, path, yes))
    {
        landRewardInform->autorelease();
        parent->addChild(landRewardInform, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(landRewardInform);
}
    
bool SystemInform::initWithLandReward(const LoginRewards& loginRewards,
                                      const std::vector<int>& path,
                                      boost::function<void ()> yes)
{
    if(!CCLayer::init())
        return false;
    
    this->setTouchEnabled(true);
    
    this->yesCall = yes;
    this->noCall = [=](){};
    
    {
        this->background = CCLayerColor::create(ccc4(0, 0, 0, 100));
        this->addChild(background, -1);
    }
    
    rewardBoard = RewardBoard::create(this, loginRewards, path);
    rewardBoard->setPosition(320, 480);
    this->addChild(rewardBoard);
    
    return true;
}
    
void SystemInform::alertRewardTips(cocos2d::CCNode *parent,
                                   std::string title,
                                   const LoginRewards &loginRewards,
                                   boost::function<void ()> yes,
                                   std::string hintTips,
                                   std::string showShareMsg)
{
    SystemInform* o = new SystemInform();
    if(o && o->initWithRewardTips(title, loginRewards, yes, hintTips, false, showShareMsg))
    {
        o->autorelease();
        parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(o);
}
    
void SystemInform::alertAchievementPacks(cocos2d::CCNode *parent,
                                         const LoginRewards &loginRewards,
                                         boost::function<void ()> yes)
{
    SystemInform* o = new SystemInform();
    if(o && o->initWithRewardTips("奖励", loginRewards, yes, "", true, ""))
    {
        o->autorelease();
        parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(o);
}

bool SystemInform::initWithRewardTips(std::string title,
                                      const LoginRewards& loginRewards,
                                      boost::function<void ()> yes,
                                      std::string hintTips,
                                      bool isAchievement,
                                      std::string shareMsg)
{
    if(!CCLayer::init())
        return false;
    
    this->setTouchEnabled(true);
    
    this->yesCall = yes;
    this->noCall = [=](){};
    
    {
        this->background = CCLayerColor::create(ccc4(0, 0, 0, 100));
        this->addChild(background, -1);
    }
    
    CCSprite* bg = GetSprite("ui/tishi_huang.png");
    bg->setAnchorPoint(ccp(0.5, 0.5));
    bg->setPosition(ccp(320, 480));
    this->addChild(bg);
    
    addLabelTTF(this, title.c_str(), FONT_CN,
                44, ccp(320, 672), ccp(0.5, 0.5), ccc3(0x55, 0x00, 0x00));
    
    int size = loginRewards.size();
    int padding = size < 3 ? 178 : 160;
    int px = size < 3 ? 320-padding*(size-1)*0.5 : padding;
    int py = size < 3 ? 520 : 570;
    float scale = size < 3 ? 1.2 : 1;
    for(int i = 0; i < size; i++)
    {
        CCSprite* reward = isAchievement ? createVIPPackItem(loginRewards[size-1-i]) :
                                            createRewardItemSprite(loginRewards[size-1-i]);
        reward->setPosition(ccp(px+padding*(i%3), py-150*(i/3)));
        reward->setScale(scale);
        addChild(reward);
    }
    
    CCMenu* menu = CCMenu::create();
    menu->setTouchPriority(UI_TOUCH_ORDER_MODAL_MENU);
    
    CCMenuItem *item = CCMenuItemSprite::create(createBtn("确定", ButtonType::NORMAL_RED),
                                                createBtn("确定", ButtonType::NORMAL_RED),
                                                this,
                                                menu_selector(SystemInform::onConfirm));
    menu->addChild(item);
    if(!shareMsg.empty())
    {
        call1 = [=](){yes();SystemInform::alertSharePanel(runningScene(), shareMsg);};
        
        CCMenuItem *item2 = CCMenuItemSprite::create(createBtn("分享", ButtonType::NORMAL_BLUE),
                                                    createBtn("分享", ButtonType::    NORMAL_BLUE),
                                                    this,
                                                    menu_selector(SystemInform::onCall1));
        menu->addChild(item2);
        menu->alignItemsHorizontallyWithPadding(10);
    }
    menu->setPosition(320.0f, 286.0f);  // offset on x is because asset has shadow
    this->addChild(menu);
    
    if(!hintTips.empty())
    {
        CCRichLabelTTF* label = CCRichLabelTTF::create(hintTips.c_str(),FONT_CN, 24);
        label->setPosition(ccp(320, 360));
        label->setAnchorPoint(ccp(0.5, 0.5));
        label->setHorizontalAlignment(kCCTextAlignmentLeft);
        addChild(label);
    }
    
    return true;
}
    
void SystemInform::alertSharePanel(CCNode* parent, std::string msg)
{
    SystemInform* o = new SystemInform();
    if(o && o->initWithSharePanel(msg))
    {
        o->autorelease();
        parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(o);
}
    
bool SystemInform::initWithSharePanel(std::string msg)
{
    if(!CCLayer::init())
        return false;
    
    this->setTouchEnabled(true);
    
    this->yesCall = [=](){};
    this->noCall = [=](){};
    
    {
        this->background = CCLayerColor::create(ccc4(0, 0, 0, 100));
        this->addChild(background, -1);
    }
    
    CCSprite* bg = GetSprite("fenxiang_bg.png");
    bg->setAnchorPoint(ccp(0.5, 0.5));
    bg->setPosition(ccp(320, 485));
    bg->setScaleY(1.2);
    this->addChild(bg);
    
    addLabelTTF(this, "分享有礼", FONT_CN, 34, ccp(320, 650));
    
    ButtonSprite* btn = ButtonSprite::create("gameui/guanbi.png", UI_TOUCH_ORDER_MODAL_MENU, false);
    btn->setPosition(ccp(400, 275));
    btn->setOnClickListener([=](){cancel();});
    bg->addChild(btn);
#if PH_USE_WEIBO
    const char* title[4] = {"新浪微博", "腾讯微博", "微信好友", "朋友圈"};
    for(int i = 0; i < 4; i++)
    {
        ButtonSprite* btn = ButtonSprite::create(createBtn(title[i], ButtonType::FOUR_WORD_RED),
                                                 UI_TOUCH_ORDER_MODAL_MENU, false);
        btn->setPosition(ccp(320, 555-68*i));
        addChild(btn);
        if(i == 0)
            btn->setOnClickListener([=](){
                auto& sina = Weibo<Sina>::instance();
                if(sina.isAuthorized())
                {
                    this->cancel();
                    SystemInform::alertShareConfirmPanel(runningScene(), 0, msg);
                    
                }
                else
                    SystemInform::alertConfirmPanel(runningScene(), "你还没有绑定新浪微博，要去绑定吗？", "提示",
                            [=](){
                                this->cancel();
                                GameScene::getInstance()->pushLayerWithDefaultAnimation(AccountLayer::create());
                            }, [](){});
            });
        else if (i == 1)
            btn->setOnClickListener([=](){
                auto&  tencent = Weibo<Tencent>::instance();
                if(tencent.isAuthorized())
                {
                    this->cancel();
                    SystemInform::alertShareConfirmPanel(runningScene(), 1, msg);
                }
                else
                    SystemInform::alertConfirmPanel(runningScene(), "你还没有绑定腾讯微博，要去绑定吗？", "提示",
                        [=](){
                            this->cancel();
                            GameScene::getInstance()->pushLayerWithDefaultAnimation(AccountLayer::create());
                        }, [](){});
            });
        else
            btn->setOnClickListener([=](){
                boost::shared_ptr<WXTextMessageRequest> req = WXTextMessageRequest::create();
                req->text(msg);
                req->sendToTimeline(i!=2);
                
                WeixinApi::instance().setResponseHandler([=](boost::shared_ptr<WXBaseResp> resp){
                    this->cancel();
                    if(resp->errorCode() == 0)
                        SystemInform::alertMessage(GameScene::getInstance(),
                                               "发布成功！", "提示", [=](){});
                });
                WeixinApi::instance().sendRequest(req);
            });
    }
#endif
    
    return true;
}
    
    void SystemInform::alertShareConfirmPanel(CCNode* parent, int type, std::string msg)
    {
        SystemInform* o = new SystemInform();
        if(o && o->initWithShareConfirmPanel(type, msg))
        {
            o->autorelease();
            parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
        }
        else
            CC_SAFE_DELETE(o);
    }
    
    bool SystemInform::initWithShareConfirmPanel(int type, std::string msg)
    {
        if(!CCLayer::init())
            return false;
        
        this->setTouchEnabled(true);
        
        this->yesCall = [=](){
#if PH_USE_WEIBO
            SystemInform* inform = SystemInform::alertLoading(runningScene());
            if(type == 0)
            {
                auto& sina = Weibo<Sina>::instance();
                sina.postTweet(msg.c_str(), [=](std::auto_ptr<WBError> error){
                    inform->cancel();
                    std::string msg;
                    if(error.get())
                        msg = "发布失败！";
                    else
                    {
                        msg = "发布成功！";
                        PuzzleRPC::getInstance()->shareMsgSucceed(PlayerData::getInstance()->getToken(), [](std::auto_ptr<Error> e){});
                    }
                    SystemInform::alertMessage(GameScene::getInstance(), msg.c_str(), "提示", [=](){});
                });
            }
            else
            {
                auto&  tencent = Weibo<Tencent>::instance();
                tencent.postTweet(msg.c_str(), [=](std::auto_ptr<WBError> error){
                    inform->cancel();
                    std::string msg;
                    if(error.get())
                        msg = "发布失败！";
                    else
                    {
                        msg = "发布成功！";
                        PuzzleRPC::getInstance()->shareMsgSucceed(PlayerData::getInstance()->getToken(), [](std::auto_ptr<Error> e){});
                    }
                    SystemInform::alertMessage(GameScene::getInstance(), msg.c_str(), "提示", [=](){});
                });
            }
#endif
        };
        this->noCall = [=](){};
        
        {
            this->background = CCLayerColor::create(ccc4(0, 0, 0, 100));
            this->addChild(background, -1);
        }
        
        ShareBoard* board = ShareBoard::create(msg);
        board->setPosition(ccp(320, 800));
        this->addChild(board);
        
        CCMenu* menu = CCMenu::create();
        menu->setTouchPriority(UI_TOUCH_ORDER_MODAL_MENU);
        
        CCMenuItem *item = CCMenuItemSprite::create(createBtn("分享", ButtonType::NORMAL_RED),
                                                    createBtn("分享", ButtonType::NORMAL_RED),
                                                    this,
                                                    menu_selector(SystemInform::onConfirm));
        CCMenuItem *item2 = CCMenuItemSprite::create(createBtn("取消", ButtonType::NORMAL_BLUE),
                                                    createBtn("取消", ButtonType::NORMAL_BLUE),
                                                    this,
                                                     menu_selector(SystemInform::onCancel));
        menu->setPosition(0.0f, -370.0f);  // offset on x is because asset has shadow
        menu->addChild(item);
        menu->addChild(item2);
        menu->alignItemsHorizontallyWithPadding(20);
        board->addChild(menu);

        return true;
    }
    
void SystemInform::alertPvPMsg(CCNode* parent)
{
    SystemInform* o = new SystemInform();
    if(o && o->initWithPvPMsg())
    {
        o->autorelease();
        parent->addChild(o, UI_DISPLAY_ORDER_MODAL);
    }
    else
        CC_SAFE_DELETE(o);
}
    
bool SystemInform::initWithPvPMsg()
{
    if(!CCLayer::init())
        return false;
    
    this->setTouchEnabled(true);
    
    this->yesCall = [=](){};
    this->noCall = [=](){};
    
    {
        this->background = CCLayerColor::create(ccc4(0, 0, 0, 100));
        this->addChild(background, -1);
    }
    
    CCScale9Sprite* bg = Get9SpriteFromFrame("fenxiang_bg.png",
                                             CCRectMake(40, 80, 35, 170));
    bg->setPosition(ccp(320, 505));
    bg->setContentSize(CCSizeMake(480, 680));
    addChild(bg);
    
    addLabelTTF(this, "擂台详情", FONT_CN, 32, ccp(320, 830));
    
    CCRichLabelTTF* label = CCRichLabelTTF::create("", FONT_CN, 27, CCSizeMake(430, 0),
                                                   CCTextAlignment::kCCTextAlignmentLeft);
    label->setString("[color=0xffffff00]◆擂台时间[/color]\n每周二周四晚20:00~21:00擂台开启，30级以上掌门可以持系统赠予的掌门贴进入，擂台中你将遇到跟你实力相近的掌门同场对决。\n[color=0xffffff00]◆擂台规则[/color]\n进入擂台后，掌门生命值与掌门等级和弟子实力成正比。双方掌门轮流过招，30回合之内某方生命值降至0，则对方胜出。若在30回合内无法分出胜负，则以平局结束。\n[color=0xffffff00]◆擂台奖励[/color]\n擂台胜出可以获得名望，名望越高的掌门将可以进入江湖排行榜，获得称号和勋章。参与擂台无论胜利与否，都将获得通宝奖励，可以在通宝行中购买药材。");
    label->setAnchorPoint(ccp(0.5, 1));
    label->setPosition(ccp(320, 770));
    addChild(label);
    
    ButtonSprite* btn = ButtonSprite::create("gameui/guanbi.png", UI_TOUCH_ORDER_MODAL_MENU, false);
    btn->setPosition(ccp(500, 810));
    btn->setOnClickListener([=](){this->cancel();});
    addChild(btn);
    
    return true;
}

void SystemInform::cancel()
{
    this->removeFromParentAndCleanup(true);
    announceInform = NULL;
    landRewardInform = NULL;
}

void SystemInform::onConfirm(cocos2d::CCObject *target)
{
	yesCall();
    cancel();
    ring("sound/v/button_click.mp3");
}
void SystemInform::onCancel(cocos2d::CCObject *target)
{
	noCall();
    cancel();
    ring("sound/v/button_click.mp3");
}
    

void SystemInform::onCall1(CCObject *target)
{
    call1();
    cancel();
    ring("sound/v/button_click.mp3");
}
void SystemInform::onCall2(CCObject *target)
{
    call2();
    cancel();
    ring("sound/v/button_click.mp3");
}
void SystemInform::onCall3(CCObject *target)
{
    call3();
    cancel();
    ring("sound/v/button_click.mp3");
}
    
bool SystemInform::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    return true;
}
    
    //-------------------------
    // AlertMessage
    //-------------------------
    bool AlertMessage::init(CCNode* parent,
                            const std::string& title,
                            const std::string& message,
                            bool useDarkBG,
                            boost::function<void()> click)
    {
        if(!CCLayer::init())
            return false;
        
        this->setTouchEnabled(true);
        
        // background with solid color
        if(useDarkBG)
        {
            CCLayerColor* background = CCLayerColor::create(ccc4(0, 0, 0, 100));
            this->addChild(background, -1);
        }
        
        nowIndex = 0;
        pageIndex = 0;
        waitingForInput = false;
        delta = 0.f;
        
        textBG = GetSprite("gameui/jieshao_bg.png");
        textBG->setPosition(ccp(320, 280));
        this->addChild(textBG);
        
        std::wstring wmsg = URLHelper::str2wstr(message.c_str());
        boost::split(pages, wmsg, boost::is_any_of(std::wstring(URLHelper::str2wstr("\t"))));

        // topleft: 300, 166, bottom right, 595 38, width 303x127
        //textLabel = CCLabelTTF::create(URLHelper::wstr2str(pages[pageIndex]).c_str(),
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        textLabel = CCRichLabelTTF::create("……",
                                       "",
                                       28,
                                       CCSize(303, 0),
                                       kCCTextAlignmentLeft);
        textLabel->setPosition(ccp(302, 178));
#else
        textLabel = CCRichLabelTTF::create("……",
                                       FONT_CN,
                                       28,
                                       CCSize(303, 0),
                                       kCCTextAlignmentLeft);
        textLabel->setPosition(ccp(302, 176));
#endif
        
        //textLabel->setColor(ccc3(0x5b, 0x00, 0x00));
        textLabel->setAnchorPoint(ccp(0.0f, 1.0f));

        textBG->addChild(textLabel);
        
        cursor = GetSprite("gameui/sanjiao.png");
        cursor->setPosition(ccp(571, 50));
        textBG->addChild(cursor);
        
        parent->addChild(this, UI_DISPLAY_ORDER_MODAL);
        
        lastClick = click;
        
        textBG->setScale(0.0f);
        textBG->runAction(CCScaleTo::create(0.2, 1.0f));
        
        cursor->runAction(CCBlink::create(1000, 1000));
        cursor->setOpacity(0);
        
        //this->update(0.10f);
        
        this->scheduleUpdate();
        return true;
    }
    
    void AlertMessage::attachImg(CCSprite* img)
    {
        img->setAnchorPoint(ccp(0.5, 1));
        img->setPosition(ccp(307, 176));
        textBG->addChild(img);
        int adjust = img->getContentSize().width*0.5+5;
        textLabel->setPositionX(302+adjust);
        textLabel->setDimensions(CCSizeMake(303-adjust, 0));
    }
    
    void AlertMessage::update(float dt)
    {
        if(this->waitingForInput) return;
        
        int prevIndex = this->nowIndex;
        delta += dt;
        //printf(" %f\n",delta);
        while(delta >= 0.05f)
        {
            delta -= 0.05f;
            this->nowIndex += 1;
        }
        
        if(prevIndex != this->nowIndex){
            int begin = this->pages[pageIndex].find_first_of(URLHelper::str2wstr("["));
            int end = this->pages[pageIndex].find_last_of(URLHelper::str2wstr("]"));
            if(nowIndex >= begin && nowIndex <= end)
                nowIndex = end+1;
            int tmp = std::min(nowIndex, (int)this->pages[pageIndex].length());
            
            std::wstring wstr = this->pages[pageIndex].substr(0, tmp);
            std::string content = URLHelper::wstr2str(wstr);
            this->textLabel->setString(("[color=ff5b0000]"+content+"[/color]").c_str());
        }
        
        if(this->nowIndex >= this->pages[pageIndex].length())
        {
            this->cursor->setOpacity(255);
            this->waitingForInput =true;
        }
    }
    
    void AlertMessage::registerWithTouchDispatcher()
    {
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, UI_TOUCH_ORDER_MODAL, true);
    }
    
    bool AlertMessage::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
    {
        return true;
    }
    
    void AlertMessage::ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event)
    {
        
        if(!this->waitingForInput) return;
        
        if(pageIndex < this->pages.size()-1)
        {
            pageIndex ++;
            this->textLabel->setString("……");
            nowIndex = 0;
            cursor->setOpacity(0);
            waitingForInput = false;
        }
        else
        {
            lastClick();
            this->removeFromParentAndCleanup(true);
        }
    }
    
    //------------------
    // BattleExitMenu
    //------------------
    bool BattleExitMenu::init(CCNode * parent,
                              const std::string &stageName, const std::string &levelName,
                              int round, int maxRound,
                              int heroDrop, int soulDrop,
                              boost::function<void ()> yes,
                              boost::function<void ()> no)
    {
        if(! CCLayer::init())
            return false;
        
        this->setTouchEnabled(true);
        
        {
            CCLayerColor * background = CCLayerColor::create(ccc4(0, 0, 0, 200));
            this->addChild(background, -1);
        }
        
        CCSprite * bg = GetSprite("ui/zanting_bg.png");
        bg->setAnchorPoint(ccp(0.5f, 0.5f));
        bg->setPosition(ccp(320, 480));
        this->addChild(bg);
        
        std::string title = stageName + "之" + levelName;
        CCLabelTTF* titleText = CCLabelTTF::create(title.c_str(), FONT_CN, 40);
        titleText->setAnchorPoint(ccp(0.5f, 0.5f));
        titleText->setPosition(ccp(bg->getContentSize().width/2, 488));
        bg->addChild(titleText);
        
        std::string text = toStr(round) + "/" + toStr(maxRound);
        CCLabelTTF* roundText = CCLabelTTF::create(text.c_str(), FONT_NUM, 40);
        roundText->setAnchorPoint(ccp(0.f, 0.f));
        roundText->setPosition(ccp(290, 410));
        bg->addChild(roundText);
        
        CCLabelTTF * heroText = CCLabelTTF::create(toStr(heroDrop).c_str(), FONT_NUM, 32);
        heroText->setColor(ccc3(55,55,55));
        heroText->setAnchorPoint(ccp(0.5f, 0.5f));
        heroText->setPosition(ccp(200, 385));
        bg->addChild(heroText);
        
        CCLabelTTF * soulText = CCLabelTTF::create(toStr(soulDrop).c_str(), FONT_NUM, 32);
        soulText->setColor(ccc3(55,55,55));
        soulText->setAnchorPoint(ccp(0.5f, 0.5f));
        soulText->setPosition(ccp(463, 385));
        bg->addChild(soulText);
        
        int y = 398;
        for(int i=0; i<8; i++)
        {
            int x;
            if(i%2 == 0)
            {
                x = 241;
                y -= 70;
            }else{
                x = 421;
            }
            
            CCSprite * circle = GetSprite("gameui/shezhi_xuanzhong.png");
            circle->setAnchorPoint(ccp(0.5f, 0.5f));
            circle->setPosition(ccp(x,y));
            bg->addChild(circle);
            
            circles.push_back(circle);
            areas.push_back(CCRect(CCRectMake(x-15, y  + 185, 140, 50)));
        }
        {
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
            
            if(GameConfig::shared()->showDragHint)
            {
                circles[4]->setVisible(true);
                circles[5]->setVisible(false);
            }
            else
            {
                circles[5]->setVisible(true);
                circles[4]->setVisible(false);
            }
            
            if(GameConfig::shared()->showAttrHint)
            {
                circles[6]->setVisible(true);
                circles[7]->setVisible(false);
            }else
            {
                circles[7]->setVisible(true);
                circles[6]->setVisible(false);
            }
        }
        
        {
            CCMenu* menu = CCMenu::create();
            menu->setTouchPriority(UI_TOUCH_ORDER_MODAL_MENU);
            
            CCMenuItem* confirmItem = CCMenuItemSprite::create(createBtn("逃跑", ButtonType::NORMAL_RED),
                                                               createBtn("逃跑", ButtonType::NORMAL_RED),
                                                               this,
                                                               menu_selector(BattleExitMenu::onConfirm));
            menu->addChild(confirmItem);
            
            CCMenuItem* cancelItem = CCMenuItemSprite::create(createBtn("返回", ButtonType::NORMAL_BLUE),
                                                              createBtn("返回", ButtonType::NORMAL_BLUE),
                                                              this,
                                                              menu_selector(BattleExitMenu::onCancel));
            menu->addChild(cancelItem);
            
            //menu->alignItemsVerticallyWithPadding(50);
            menu->alignItemsHorizontallyWithPadding(50);
            menu->setPosition(ccp(320, 263));
            this->addChild(menu);
        }
        
        parent->addChild(this, UI_DISPLAY_ORDER_MODAL);
        this->yesCall = boost::move(yes);
        this->noCall = boost::move(no);
        
        return true;
    }
    
    void BattleExitMenu::onConfirm()
    {
        if(this->yesCall)
            this->yesCall();
    }
    
    void BattleExitMenu::onCancel()
    {
        if(this->noCall) this->noCall();
        this->removeFromParentAndCleanup(true);
    }
    
    
    
    void BattleExitMenu::registerWithTouchDispatcher()
    {
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,
                                                                                UI_TOUCH_ORDER_MODAL,
                                                                                true);
    }
    
    bool BattleExitMenu::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
    {
        return true;
    }
    
    void BattleExitMenu::ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event)
    {
        CCPoint localPoint = convertTouchToNodeSpace(touch);
        for(int i = 0; i < areas.size(); i++)
            if(areas[i].containsPoint(localPoint))
            {
                switch(i)
                {
                    case 0:
                        circles[0]->setVisible(true);
                        circles[1]->setVisible(false);
                        
                        GameConfig::shared()->setBGMEnabled(true);
                        playBG("sound/bg/ui.mp3");
                        ring("sound/v/button_click.mp3");
                        break;
                    case 1:
                        circles[1]->setVisible(true);
                        circles[0]->setVisible(false);
                        
                        GameConfig::shared()->setBGMEnabled(false);
                        stopBgMusic();
                        
                        ring("sound/v/button_click.mp3");
                        break;
                    case 2:
                        circles[2]->setVisible(true);
                        circles[3]->setVisible(false);
                        
                        GameConfig::shared()->setSFXEnabled(true);
                        ring("sound/v/button_click.mp3");
                        break;
                    case 3:
                        circles[3]->setVisible(true);
                        circles[2]->setVisible(false);
                        
                        GameConfig::shared()->setSFXEnabled(false);
                        ring("sound/v/button_click.mp3");
                        break;
                    case 4:
                        circles[4]->setVisible(true);
                        circles[5]->setVisible(false);
                        
                        GameConfig::shared()->showDragHint = true;
                        ring("sound/v/button_click.mp3");
                        break;
                    case 5:
                        circles[5]->setVisible(true);
                        circles[4]->setVisible(false);
                        
                        GameConfig::shared()->showDragHint = false;
                        ring("sound/v/button_click.mp3");
                        break;
                    case 6:
                        circles[6]->setVisible(true);
                        circles[7]->setVisible(false);
                        
                        GameConfig::shared()->showAttrHint = true;
                        ring("sound/v/button_click.mp3");
                        break;
                    case 7:
                        circles[7]->setVisible(true);
                        circles[6]->setVisible(false);
                        
                        GameConfig::shared()->showAttrHint = false;
                        ring("sound/v/button_click.mp3");
                        break;
                }
            }
        
        GameConfig::shared()->save();
    }
    
    //-------------
    // ModalBase
    //-------------
    bool ModalBase::init(CCNode* parent, bool enableBG)
    {
        if(!CCLayer::init())
            return false;
        
        // add to parent.
        parent->addChild(this, UI_DISPLAY_ORDER_MODAL);
        
        this->setTouchEnabled(true);
        
        if(enableBG)
        {
            CCLayerColor* mBG = CCLayerColor::create(ccc4(0, 0, 0, 100));
            this->addChild(mBG, -1);
        }
        
        return true;
    }
    
    void ModalBase::registerWithTouchDispatcher()
    {
        // intercept all touch events less prior than ORDER_MODAL
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, UI_TOUCH_ORDER_MODAL, true);
    }
    
    bool ModalBase::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
    {
        return true;
    }
    
    //------------------
    // LoadingDialog
    //------------------
    bool LoadingDialog::init(CCNode* parent,
                             const std::string& title,
                             const std::string& message)
    {
        if(!ModalBase::init(parent, true))
            return false;
        
        // create bakcground
        CCScale9Sprite* bg = Get9SpriteFromFrame("dialog_bg.png", CCRectMake(50, 50, 208, 112));
        bg->setContentSize(CCSize(450, 210));
        bg->setOpacity(200);
        
        CCSize winSize = CCEGLView::sharedOpenGLView()->getVisibleSize();
        CCPoint p = ccp(winSize.width * 0.5, winSize.height * 0.5);
        bg->setPosition(p);
        
        //-------------
        // create label
        CCLabelTTF *label = CCLabelTTF::create(title.c_str(), FONT_CN, 30);
        label->setAnchorPoint(ccp(0.5f, 1.0f));
        label->setPosition(ccp(300, 170));
        bg->addChild(label);
        
        mMsgLabel = CCLabelTTF::create(message.c_str(), FONT_CN, 24);
        mMsgLabel->setAnchorPoint(ccp(0.5, 1.0));
        mMsgLabel->setPosition(ccp(300, 100));
        mMsgLabel->setColor(TITLE_YELLOW);
        bg->addChild(mMsgLabel);
        
        if(!mLoadingAnim)
        {
            mLoadingAnim = CCAnimation::create();
            mLoadingAnim->retain();
            
            int numFrames = 4;
            for(int i=1; i<=numFrames; ++i)
            {
                char name[128];
                sprintf(name, "loading_anim%d.png", i);
                CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
                
                assert(frame != NULL && "cannot find frames");
                
                mLoadingAnim->addSpriteFrame(frame);
            }
            
            mLoadingAnim->setDelayPerUnit(0.18);
        }
        
        CCSprite* jumpAnim = CCSprite::create();
        jumpAnim->runAction(CCRepeatForever::create( CCAnimate::create(mLoadingAnim)) );
        jumpAnim->setPosition(ccp(115, 24 + 140));
        bg->addChild(jumpAnim);
        
        this->addChild(bg);

        return true;
    }
    
    void LoadingDialog::setMessage(const std::string& message)
    {
        mMsgLabel->setString(message.c_str());
    }
    
    void LoadingDialog::cancel()
    {
        this->removeFromParent();
    }
}
