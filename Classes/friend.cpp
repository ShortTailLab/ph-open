#include "friend.h"
#include "network/PuzzleRPC.h"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <auth/Weixin.h>
#include "Utils.h"
#include "GameScene.h"
#include "PlayerData.h"
#include "HeroIcon.h"
#include "HeroProfileLayer.h"
#include "More.h"
#include "Common.h"
#include "UIPresets.h"
#include "ObjectPool.h"
#include <boost/unordered_set.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

#if PH_USE_WEIBO
#include <auth/Weibo.h>
#endif

namespace PH
{
    
USING_NS_CC;
    
std::string getTimeDes(boost::posix_time::ptime time)
{
    std::string timeMsg;
    using namespace boost::date_time;
    using namespace boost::posix_time;
    ptime now = boost::posix_time::second_clock::universal_time();
    
    boost::posix_time::time_duration dur = (now - time);
    float h = dur.hours();
    float m = dur.minutes();
    if(h > 24)
        timeMsg = toStr(std::min(int(h/24) , 10))+"天前";
    else if(h > 0)
        timeMsg = toStr(h)+"小时前";
    else
        timeMsg = toStr(m)+"分钟前";
    return timeMsg;
}
    
bool FriendItem::init(PlayerPtr player)
{
    assert(player && "player must no null!");
    
	if(!ContentLayer::init())
        return false;
    
    this->player = player;
	playerName = player->uid;
    heroImg = NULL;

	CCSprite *bg = GetSprite("gameui/wode_1_2.png");
	bg->setAnchorPoint(ccp(0.5 , 1));
	bg->setPosition(ccp(0 , 0));
	addChild(bg);
	
    Hero* hero = player->team[player->activeTeam*5];
	if(!player->team.empty() &&  hero){
		heroImg = HeroIcon::create(hero , false , true);
		heroImg->setAnchorPoint(ccp(0,1));
		heroImg->setPosition(ccp(-270  , -9));
		addChild(heroImg);
        PlayerData* data = PlayerData::getInstance();
        if(data->getFriSortType() == FriendSortType::Blood_Fri_Sort)
            ((HeroIcon*)heroImg)->setBottomMsg(HeroIcon::LabelType::HP);
        else if(data->getFriSortType() == FriendSortType::Attack_Fri_Sort)
            ((HeroIcon*)heroImg)->setBottomMsg(HeroIcon::LabelType::Attack);
        else if(data->getFriSortType() == FriendSortType::Regen_Fri_Sort)
            ((HeroIcon*)heroImg)->setBottomMsg(HeroIcon::LabelType::Regen);
        else
            ((HeroIcon*)heroImg)->setBottomMsg(HeroIcon::LabelType::Level);
	}
    
    std::string value = trimNameCN(player->name);
	CCLabelTTF *lastTimeLabel;
    
    int posX = 0;
    if(player->vipLevel > 0)
    {
        CCSprite* vip = GetSprite("gameui/vip"+toStr(player->vipLevel)+".png");
        vip->setPosition(ccp(-160, -34));
        vip->setAnchorPoint(ccp(0, 0.5));
        vip->setScale(0.7f);
        addChild(vip);
        posX  = vip->getContentSize().width*0.7f;
    }
    
    LabelX *nameLabel = LabelX::create(value.c_str() , FONT_CN , 28, LabelX::FontEffect::SHADOW, ccp(2, -2));
    nameLabel->setColor(ccWHITE);
    nameLabel->setPosition(ccp(-160+posX, -34));
    nameLabel->setAnchorPoint(ccp(0, 0.5));
    addChild(nameLabel);
    
    LabelX *levelLabel = LabelX::create(toStr(player->level).c_str() , FONT_CN , 22, LabelX::FontEffect::SHADOW, ccp(2, -2));
    levelLabel->setColor(ccWHITE);
    levelLabel->setPosition(ccp(230, -33));
    levelLabel->setAnchorPoint(ccp(0.5, 0.5));
    addChild(levelLabel);
    
	addLabelTTF(this , lastTimeLabel, getTimeDes(boost::posix_time::from_time_t(player->lastLoginTS)).c_str(),
                FONT_CN, 24 , ccp(90 , -82) , ccp(0.5 , 0.5) , ccBLACK);

	menu = CCMenu::create();
	CCMenuItem *deleteBtn = CCMenuItemSprite::create(createBtn("删除"), createBtn("删除"), this, menu_selector(FriendItem::onDelete));
	menu->addChild(deleteBtn);
	menu->setPosition(ccp(210 , -80));
	addChild(menu);

	setContentSize(CCSize(556 , 140));
    setTouchEnabled(true);
	return true;
}
    
void FriendItem::update()
{
    if(!heroImg)
        return;
    PlayerData* data = PlayerData::getInstance();
    if(data->getFriSortType() == FriendSortType::Blood_Fri_Sort)
        ((HeroIcon*)heroImg)->setBottomMsg(HeroIcon::LabelType::HP);
    else if(data->getFriSortType() == FriendSortType::Attack_Fri_Sort)
        ((HeroIcon*)heroImg)->setBottomMsg(HeroIcon::LabelType::Attack);
    else if(data->getFriSortType() == FriendSortType::Regen_Fri_Sort)
        ((HeroIcon*)heroImg)->setBottomMsg(HeroIcon::LabelType::Regen);
    else
        ((HeroIcon*)heroImg)->setBottomMsg(HeroIcon::LabelType::Level);
}
    
void FriendItem::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    isClick = true;
}
void FriendItem::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    CCTouch *touch = (CCTouch *)pTouches->anyObject();
	CCPoint currPoint = touch->getLocationInView();
	CCPoint prevPoint = touch->getPreviousLocationInView();
    
	if( ccpDistance(currPoint, prevPoint) > 5)
        isClick = false;
}
void FriendItem::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    CCPoint point = convertTouchToNodeSpace((CCTouch*)pTouches->anyObject());
    
    if(isClick && heroImg && heroImg->boundingBox().containsPoint(point))
    {
        auto layer = HeroProfileLayer::create(heroImg->getHero() , false);
        GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
        isClick = false;
    }
}

void FriendItem::onDelete(cocos2d::CCObject *target) 
{    
    boost::function<void ()> toDelete = [=]()
    {
        SystemInform *inform = SystemInform::alertLoading(GameScene::getInstance());
        PuzzleRPC::getInstance()->deleteFriend(PlayerData::getInstance()->getToken(),
                                               playerName ,
                                               [=](std::auto_ptr<Error> e)
        {
            inform->cancel();
            if(e.get())
            {
                CCLog("delete friend error: %s", e->toStr().c_str());
                e->handleErrorInform();
                return;
            }
            //GameScene::getInstance()->popLayer();
            //((MyFriendLayer*)this->getUserData())->getPlayers();
            
            std::vector<PlayerPtr> *p = &PlayerData::getInstance()->friends;
            for(std::vector<PlayerPtr>::iterator it = p->begin() ; it != p->end() ; it++)
                if((*it)->uid == player->uid)
                {
                    p->erase(it);
                    break;
                }
            MyFriendLayer* layer = (MyFriendLayer*)this->getUserData();
            layer->scrollView->removeLayer(this);
            layer->initHeroNumTips();
            
        });
    };
    
    SystemInform::alertConfirmPanel(GameScene::getInstance(),
                                    "确定要删除好友吗？",
                                    "警告",
                                    toDelete,
                                    [](){});
    ring("sound/v/button_click.mp3");
}

//////////////////////////////////////////////////
//FriendAskLayer
//////////////////////////////////////////////////

bool FriendAskItem::init(PlayerPtr player)
{
    assert(player && "player must no null!");
    
	if(!ContentLayer::init())return false;

    this->player = player;
	playerName = player->uid;
    heroImg = NULL;

	CCSprite *bg = GetSprite("gameui/qingqiu_1_2.png");
	bg->setAnchorPoint(ccp(0.5 , 1));
	bg->setPosition(ccp(0 , 0));
	addChild(bg);
    
    Hero* hero = player->team[player->activeTeam*5];
	if(player && hero){
		heroImg = HeroIcon::create(hero , false , true);
		heroImg->setAnchorPoint(ccp(0,1));
		heroImg->setPosition(ccp(-270  , -8));
		addChild(heroImg);
	}
    
    std::string value = trimNameCN(player->name);
    
    int posX = 0;
    if(player->vipLevel > 0)
    {
        CCSprite* vip = GetSprite("gameui/vip"+toStr(player->vipLevel)+".png");
        vip->setPosition(ccp(-160, -34));
        vip->setAnchorPoint(ccp(0, 0.5));
        vip->setScale(0.7f);
        addChild(vip);
        posX  = vip->getContentSize().width*0.7f;
    }
    CCLabelTTF *nameLabel = LabelX::create(value.c_str() , FONT_CN , 28, LabelX::FontEffect::SHADOW, ccp(2, -2));
    nameLabel->setColor(ccWHITE);
    nameLabel->setPosition(ccp(-160+posX, -34));
    nameLabel->setAnchorPoint(ccp(0, 0.5));
    addChild(nameLabel);
    
    CCLabelTTF *levelLabel = LabelX::create(toStr(player->level).c_str() , FONT_CN , 22, LabelX::FontEffect::SHADOW, ccp(2, -2));
    levelLabel->setColor(ccWHITE);
    levelLabel->setPosition(ccp(230, -33));
    levelLabel->setAnchorPoint(ccp(0.5, 0.5));
    addChild(levelLabel);

	CCMenu *menu = CCMenu::create();
	CCMenuItem *deleteBtn = CCMenuItemSprite::create(createBtn("拒绝"), createBtn("拒绝"), this, menu_selector(FriendAskItem::onDelete));
	CCMenuItem *acceptBtn = CCMenuItemSprite::create(createBtn("接受"), createBtn("接受"), this, menu_selector(FriendAskItem::onAccept));
	menu->addChild(deleteBtn);
	menu->addChild(acceptBtn);
	menu->alignItemsHorizontallyWithPadding(10);
	menu->setPosition(ccp(160 , -80));
	addChild(menu);

	setContentSize(CCSize(556 , 140));
    setTouchEnabled(true);
	return true;
}

void FriendAskItem::onDelete(cocos2d::CCObject *target)
{
    SystemInform *inform = SystemInform::alertLoading(GameScene::getInstance());
    
	PuzzleRPC::getInstance()->rejectFriendRequest(PlayerData::getInstance()->getToken(),
                                                  playerName,
                                                  [=](std::auto_ptr<Error> e)
    {
        inform->cancel();
		if(e.get())
        {
			CCLog("reject friend error: %s", e->toStr().c_str());
            e->handleErrorInform();
			return;
		}
        FriendAskLayer* layer = (FriendAskLayer*)this->getUserData();
        this->removeFromParentAndCleanup(true);
        
        std::vector<PlayerPtr> *p = &layer->players;
        for(std::vector<PlayerPtr>::iterator it = p->begin() ; it != p->end() ; it++)
            if((*it)->uid == player->uid)
            {
                p->erase(it);
                break;
            }
        layer->makeItems(layer->clipPlayers());
        
        PlayerData::getInstance()->player->pendingFriendRequests = p->size();
	});
    
    ring("sound/v/button_click.mp3");
}

void FriendAskItem::onAccept(cocos2d::CCObject *target)
{
    
    if(PlayerData::getInstance()->friends.size() >= PlayerData::getInstance()->player->maxFriends)
    {
        SystemInform::alertMessage(runningScene(), "好友已满，无法添加更多好友", "警告",[](){});
        return;
    }
    
    SystemInform *inform = SystemInform::alertLoading(GameScene::getInstance());
    
	PuzzleRPC::getInstance()->acceptFriendRequest(PlayerData::getInstance()->getToken(),
                                                  playerName,
                                                  [=](std::auto_ptr<Error> e)
    {
        inform->cancel();
		if(e.get())
        {
			CCLog("add friend error: %s", e->toStr().c_str());
            e->handleErrorInform();
			return;
		}
		
        FriendAskLayer* layer = (FriendAskLayer*)this->getUserData();
        this->removeFromParentAndCleanup(true);
        
        std::vector<PlayerPtr> *p = &layer->players;
        for(std::vector<PlayerPtr>::iterator it = p->begin() ; it != p->end() ; it++)
            if((*it)->uid == player->uid)
            {
                p->erase(it);
                break;
            }
        layer->makeItems(layer->clipPlayers());
        
        PlayerData::getInstance()->player->pendingFriendRequests = p->size();
	});
    
    ring("sound/v/button_click.mp3");
}

bool SearchResultItem::init(const Player& player)
{
    
	if(!ContentLayer::init())return false;

	this->player = player;
    // HACK!!!
    const_cast<Player*>(&player)->storage.clear();
    
    heroImg = NULL;

	CCSprite *bg = GetSprite("gameui/qingqiu_1_2.png");
	bg->setAnchorPoint(ccp(0.5 , 1));
	bg->setPosition(ccp(0 , 0));
	addChild(bg);
    
    Hero* hero = player.team[player.activeTeam*5];
	if(!player.team.empty() && hero)
    {
		heroImg = HeroIcon::create(hero , false , true);
		heroImg->setAnchorPoint(ccp(0,1));
		heroImg->setPosition(ccp(-270  , -8));
		addChild(heroImg);
	}

    std::string value = trimNameCN(player.name);
    int posX = 0;
    if(player.vipLevel > 0)
    {
        CCSprite* vip = GetSprite("gameui/vip"+toStr(player.vipLevel)+".png");
        vip->setPosition(ccp(-160, -34));
        vip->setAnchorPoint(ccp(0, 0.5));
        vip->setScale(0.7f);
        addChild(vip);
        posX  = vip->getContentSize().width*0.7f;
    }
    CCLabelTTF *nameLabel = LabelX::create(value.c_str() , FONT_CN , 28, LabelX::FontEffect::SHADOW, ccp(2, -2));
    nameLabel->setColor(ccWHITE);
    nameLabel->setPosition(ccp(-160+posX, -34));
    nameLabel->setAnchorPoint(ccp(0, 0.5));
    addChild(nameLabel);
    
    CCLabelTTF *levelLabel = LabelX::create(toStr(player.level).c_str() , FONT_CN , 22, LabelX::FontEffect::SHADOW, ccp(2, -2));
    levelLabel->setColor(ccWHITE);
    levelLabel->setPosition(ccp(230, -33));
    levelLabel->setAnchorPoint(ccp(0.5, 0.5));
    addChild(levelLabel);

	CCMenu *menu = CCMenu::create();
	CCMenuItem *addBtn = CCMenuItemSprite::create(createBtn("添加"), createBtn("添加"), this, menu_selector(SearchResultItem::onAdd));
	menu->addChild(addBtn);
	menu->setPosition(ccp(210 , -80));
	addChild(menu);

	setContentSize(CCSize(556 , 140));
    setTouchEnabled(true);
	return true;
}

void SearchResultItem::onAdd(cocos2d::CCObject *target)
{
    if (player.isFriend) {
        SystemInform::alertMessage(GameScene::getInstance(), "对方已经是你的好友", "提示" ,[](){});
        return;
    }
    SystemInform *inform = SystemInform::alertLoading(GameScene::getInstance());
	PuzzleRPC::getInstance()->addFriend(PlayerData::getInstance()->getToken() , player.uid , [=](std::auto_ptr<Error> e){
        inform->cancel();
		if(e.get()){
			std::cout<<"addfriend error:"<<e.get()->msg<<std::endl;
            SystemInform::alertMessage(GameScene::getInstance(), e->toStrCN().c_str(), "提示" , [](){});
			return;
		}
        SystemInform::alertMessage(GameScene::getInstance(), "请求已发送，等待对方确认", "提示" , [](){});
        CCLog("add friend success!!");
	});
    ring("sound/v/button_click.mp3");
}


/*bool FriendAskLayer::init()
{
	if(!ContentLayer::init())return false;

	scrollView = NULL;

	CCMenu *menu = CCMenu::create();

	CCMenuItem *backItem = CCMenuItemImage::create("gameui/back_t.png" , "gameui/back_t.png", this , menu_selector(FriendAskLayer::onBack));
	CCMenuItem *titleItem = CCMenuItemImage::create("gameui/qingqiu_t.png" , "gameui/qingqiu_t.png");
	menu->addChild(backItem);
	menu->addChild(titleItem);

	menu->alignItemsHorizontallyWithPadding(30);
	menu->setPosition(ccp(-50 , -20));
	addChild(menu);


	return true;
}*/

CCMenuItem *FriendAskLayer::getTitle()
{
    CCMenuItem *titleItem = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"), GetSprite("gameui/hongchengdi_t_tongyong.png"));
    LabelX* name = LabelX::create("好友请求", FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp(141 , 35));
    titleItem->addChild(name);
    
	return titleItem;
}
void FriendAskLayer::getPlayers()
{
    SystemInform *inform = SystemInform::alertLoading(GameScene::getInstance());
	PuzzleRPC::getInstance()->pendingFriends(PlayerData::getInstance()->getToken() ,
                                             [=](std::vector<Player*> pls,
                                                 std::auto_ptr<Error> e)
    {
        inform->cancel();
		if(e.get())
        {
			std::cout<<"pending friends error:"<<e.get()->code<<std::endl;
			return;
		}
        
        std::vector<PlayerPtr> players;
        for(Player* p : pls)
            players.push_back(PlayerPtr(p));
        
        this->players = players;
        
        if (!players.size())
        {
            SystemInform::alertMessage(GameScene::getInstance(), "你当前没有好友请求", "提示",
                                       [](){GameScene::getInstance()->popLayer();});
            return;
        }
        makeItems(clipPlayers());
        PlayerData::getInstance()->player->pendingFriendRequests = players.size();
	});
}
ContentLayer *FriendAskLayer::getItem(PlayerPtr player)
{
	return FriendAskItem::create(player);
}
    
void FriendAskLayer::makeItems(std::vector<PlayerPtr > friends)
{
    if(scrollView)
        scrollView->removeFromParentAndCleanup(true);
    
    scrollView = ZqlScrollView::create();
    ContentLayer *item;
    for(PlayerPtr player : friends){
        item = getItem(player);
        item->setUserData(this);
        scrollView->addLayer(item);
    }
    scrollView->setViewSize(CCSize(winSize.width , 800) ,  CCRect(0, -100, winSize.width, 600));
    scrollView->setPosition(ccp(0, 40));
    this->addChild(scrollView);
    
}
    
std::vector<PlayerPtr> FriendAskLayer::clipPlayers()
{
    std::vector<PlayerPtr>::iterator end = players.size()>20 ? players.begin()+20 : players.end();
    return std::vector<PlayerPtr>(players.begin(), end);
}

//////////////////////////////////////////////////
//MyFriend
//////////////////////////////////////////////////

bool MyFriendLayer::init()
{
	if(!ContentLayer::init())return false;

	scrollView = NULL;

	CCMenu *menu = CCMenu::create();

	CCMenuItem *backItem = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"), GetSprite("gameui/back_t.png"), this, menu_selector(MyFriendLayer::onBack));
	CCMenuItem *titleItem = getTitle();
	menu->addChild(backItem);
	menu->addChild(titleItem);
    
    backItem->setAnchorPoint(ccp(0.5 , 1));
    titleItem->setAnchorPoint(ccp(0.5 , 1));
    menu->alignItemsHorizontallyWithPadding(8);
	menu->setPosition(ccp(-70 , 18));
	addChild(menu , 10);

	getPlayers();

	return true;
}

void MyFriendLayer::getPlayers()
{
    SystemInform *inform  = SystemInform::alertLoading(GameScene::getInstance());
	PuzzleRPC::getInstance()->friends(PlayerData::getInstance()->getToken() ,
                                      [=](std::vector<Player*> pls,
                                          int maxFriends,
                                          std::auto_ptr<Error> e)
    {
        inform->cancel();
		if(e.get())
        {
			std::cout<<"get friends error:"<<e.get()->code<<std::endl;
            e->handleErrorInform();
			return;
		}
        
        if (!pls.size())
        {
            SystemInform::alertMessage(GameScene::getInstance(), "你当前没有好友", "提示",
                                       [](){GameScene::getInstance()->popLayer();});
            return;
        }
        
        std::vector<PlayerPtr> players;
        for(Player* p : pls)
            players.push_back(PlayerPtr(p));
        
		PlayerData::getInstance()->friends = players;
		PlayerData::getInstance()->player->maxFriends = maxFriends;

		makeItems(getFriendsBySortType());
        initHeroNumTips();
	});
}

CCMenuItem *MyFriendLayer::getTitle()
{
    
    CCMenuItem* sortButton = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                      GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                      this,
                                                      menu_selector(MyFriendLayer::onChangeSortType));
    sortButton->setAnchorPoint(ccp(0.5 , 1));
    
    LabelX* name = LabelX::create("好友", FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp(90 , sortButton->getContentSize().height*0.5f+2));
    sortButton->addChild(name);
    
    title = LabelX::create(getSortStr().c_str() , FONT_CN , 20, LabelX::FontEffect::SHADOW, ccp(2, -2));
    title->setColor(ccWHITE);
    title->setPosition(ccp( 170 , sortButton->getContentSize() .height*0.5f));
    title->setAnchorPoint(ccp(0, 0.5));
    sortButton->addChild(title);
    
    CCSprite *xian = GetSprite("gameui/hongchengdi_huang_tongyong.png");
    xian->setPosition(ccp(160 , sortButton->getContentSize().height*0.5f));
    sortButton->addChild(xian);
    
    return sortButton;
}
    
std::string MyFriendLayer::getSortStr()
{
    switch (PlayerData::getInstance()->getFriSortType()) {
        case Nor_Fri_Sort:
            return "默认排序";
        case Time_Fri_Sort:
            return "时间排序";
        case Color_Fri_Sort:
            return "属性排序";
        case Blood_Fri_Sort:
            return "血量排序";
        case Attack_Fri_Sort:
            return "攻击排序";
        case Regen_Fri_Sort:
            return "恢复排序";
        case Level_Fri_Sort:
            return "等级排序";
        default:
            return "默认排序";
    }
}
    
void MyFriendLayer::onChangeSortType()
{
    PlayerData::getInstance()->switchFriSortType();
    makeItems(getFriendsBySortType());
    title->setString(getSortStr().c_str());
    
    ring("sound/v/button_click.mp3");
}
    
std::vector<PlayerPtr>  MyFriendLayer::getFriendsBySortType()
{
    std::vector<PlayerPtr> noMainHero;
    std::vector<PlayerPtr> temp;
    for(PlayerPtr p : PlayerData::getInstance()->friends)
        if(p->getMainHero())
            temp.push_back(p);
        else
            noMainHero.push_back(p);
    
    boost::function<bool (PlayerPtr p1, PlayerPtr p2)> sortFuncDependonTeam0 = [=](PlayerPtr p1, PlayerPtr p2)->bool{
        return p1->getMainHero()==NULL && p2->getMainHero()==NULL ? p1->uid < p2->uid : p1->getMainHero()!=NULL;
    };
    
    
    switch (PlayerData::getInstance()->getFriSortType()) {
        case Nor_Fri_Sort:
            break;
        case Time_Fri_Sort:
            std::sort(temp.begin() , temp.end() , [](PlayerPtr p1, PlayerPtr p2){
                return  p1->lastLoginTS != p2->lastLoginTS ? p1->lastLoginTS > p2->lastLoginTS : p1->getMainHero()->heroId < p2->getMainHero()->heroId;
            });
            break;
        case Color_Fri_Sort:
            std::sort(temp.begin() , temp.end() , [=](PlayerPtr p1, PlayerPtr p2){
                return p1->getMainHero()!=NULL&&p2->getMainHero()!=NULL ? (p1->getMainHero()->profile->color != p2->getMainHero()->profile->color ? p1->getMainHero()->profile->color < p2->getMainHero()->profile->color : p1->getMainHero()->heroId < p2->getMainHero()->heroId) :
                    sortFuncDependonTeam0(p1, p2);
            });
            break;
        case Blood_Fri_Sort:
            std::sort(temp.begin() , temp.end() , [=](PlayerPtr p1, PlayerPtr p2){
                return p1->getMainHero()!=NULL&&p2->getMainHero()!=NULL ? (p1->getMainHero()->health != p2->getMainHero()->health ? p1->getMainHero()->health > p2->getMainHero()->health  : p1->getMainHero()->heroId < p2->getMainHero()->heroId) : sortFuncDependonTeam0(p1, p2);
            });
            break;
        case Attack_Fri_Sort:
            std::sort(temp.begin() , temp.end() , [=](PlayerPtr p1, PlayerPtr p2){
                return p1->getMainHero()!=NULL&&p2->getMainHero()!=NULL ? (p1->getMainHero()->attack != p2->getMainHero()->attack ? p1->getMainHero()->attack > p2->getMainHero()->attack : p1->getMainHero()->heroId < p2->getMainHero()->heroId) : sortFuncDependonTeam0(p1, p2);
            });
            break;
        case Regen_Fri_Sort:
            std::sort(temp.begin() , temp.end() , [=](PlayerPtr p1, PlayerPtr p2){
                return p1->getMainHero()!=NULL&&p2->getMainHero()!=NULL ? (p1->getMainHero()->regen != p2->getMainHero()->regen ? p1->getMainHero()->regen > p2->getMainHero()->regen : p1->getMainHero()->heroId < p2->getMainHero()->heroId) : sortFuncDependonTeam0(p1, p2);
            });
            break;
        case Level_Fri_Sort:
            std::sort(temp.begin() , temp.end() , [=](PlayerPtr p1, PlayerPtr p2){
                return p1->getMainHero()!=NULL&&p2->getMainHero()!=NULL ? (p1->level != p2->level ? p1->level > p2->level : p1->getMainHero()->heroId < p2->getMainHero()->heroId) : sortFuncDependonTeam0(p1, p2);
            });
            break;

        default:
            break;
    }
    
    if(noMainHero.size())
        temp.insert(temp.end(), noMainHero.begin(), noMainHero.end());
    return temp;
}

ContentLayer *MyFriendLayer::getItem(PlayerPtr player)
{
	return FriendItem::create(player);
}

void MyFriendLayer::makeItems(std::vector<PlayerPtr> friends)
{
	if(!scrollView)
    {
        scrollView = ZqlScrollView::create();
        scrollView->setViewSize(CCSize(winSize.width , 800) ,  CCRect(0, -100, winSize.width, 600));
        scrollView->setPosition(ccp(0, 40));
        this->addChild(scrollView);
        
        ContentLayer *item = NULL;
        int posY = 0;
        for(PlayerPtr player : friends){
            item = getItem(player);
            item->setUserData(this);
            scrollView->addLayer(item);
            posY += item->getContentSize().height;
        }
    }
    else
    {
        CCArray* nodes = scrollView->getChildren();
        for(int i = 0; i < friends.size(); i++)
        {
            for(int j = 0; j < nodes->count(); j++)
                if(((FriendItem*)nodes->objectAtIndex(j))->player->uid == friends[i]->uid)
                {
                    ((FriendItem*)nodes->objectAtIndex(j))->update();
                    if(i != j)
                        nodes->exchangeObjectAtIndex(j, i);
                    break;
                }
        }
    }
}

void MyFriendLayer::onBack(cocos2d::CCObject *target)
{
	GameScene::getInstance()->popLayer();
    ring("sound/v/button_click.mp3");
}
    
void MyFriendLayer::initHeroNumTips()
{
    int num = PlayerData::getInstance()->friends.size();
    int maxNum = PlayerData::getInstance()->player->maxFriends;
    
    if(!heroNumBg)
    {
        heroNumBg = GetSprite("gameui/duiwurenshu.png");
        heroNumBg->setAnchorPoint(ccp(1 , 0.5));
        heroNumBg->setPosition(ccp(winSize.width*0.5f , -634));
        //heroNumBg->setOpacity(200);
        addChild(heroNumBg , 1);
        
        addLabelTTF(heroNumBg, numLabel, "", FONT_NUM, 24, ccp(100 , heroNumBg->getContentSize().height*0.5f) , ccp(0.5 , 0.5));
    }
    
    numLabel->setString((toStr(num) + "/" + toStr(maxNum)).c_str());
    
    if(num > maxNum)
    {
        numLabel->setColor(ccRED);
        
        CCFiniteTimeAction *fade1 = CCFadeTo::create(0.3 , 100);
        CCFiniteTimeAction *fade2 = CCFadeTo::create(0.3 , 255);
        CCFiniteTimeAction *sequence = CCSequence::create(fade1 , fade2 , NULL);
        CCAction *repeatBlink = CCRepeatForever::create((CCActionInterval*)sequence);
        
        numLabel->runAction(repeatBlink);
    }
}

//////////////////////////////////////////////////
//searchfriend
//////////////////////////////////////////////////

bool SearchResultLayer::init(PlayerList *list)
{
	if(!MyFriendLayer::init())
        return false;

	makeItems(list->players);

	return true;
}

CCMenuItem *SearchResultLayer::getTitle()
{
    CCMenuItem *titleItem = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                     GetSprite("gameui/hongchengdi_t_tongyong.png"));
    LabelX* name = LabelX::create("搜索结果", FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp( 141 , 35));
    titleItem->addChild(name);
    
    return titleItem;
}
void SearchResultLayer::getPlayers()
{

}

void SearchResultLayer::makeItems(const std::vector<Player*>& players)
{
	if(scrollView)
        this->removeChild(scrollView, true);

	scrollView = ZqlScrollView::create();
	ContentLayer *item;

	for(const Player* player : players)
    {
		item = getItem(*player);
        item->setUserData(this);
		scrollView->addLayer(item);
	}
    scrollView->setViewSize(CCSize(winSize.width , 800) ,  CCRect(0, -100, winSize.width, 600));
	scrollView->setPosition(ccp( 0 , 40));
	this->addChild(scrollView);
}


ContentLayer *SearchResultLayer::getItem(const Player& player)
{
	return SearchResultItem::create(player);
}

void SearchResultLayer::onBack(cocos2d::CCObject *target)
{
	GameScene::getInstance()->popLayer();
    ring("sound/v/button_click.mp3");
}

//////////////////////////////////////////////////
//searchfriend
//////////////////////////////////////////////////


bool SearchJuanZhou::init()
{
	if(!JuanZhou::init())return false;

	viewPort = CCSize(20 , 203);

	bg = GetSprite("gameui/suosou_1_3.png");
	bg->setAnchorPoint(ccp(0.5 , 1));
	addChild(bg);

    std::string value = trimNameCN(PlayerData::getInstance()->player->name);

	menu = CCMenu::create(); 
	CCMenuItem *btn = CCMenuItemSprite::create(createBtn("搜索", ButtonType::NORMAL_RED),
                                               createBtn("搜索", ButtonType::NORMAL_RED),
                                               this,
                                               menu_selector(SearchJuanZhou::onSearch));
	menu->addChild(btn);
	menu->setPosition(ccp( 0 , -145));
	addChild(menu);
    
    //    CCScale9Sprite* sp = CCScale9Sprite::create("gameui/suosou_1_3_k.png", CCRectMake(0, 0, 488, 60), CCRectMake(10, 10, 468, 40));
    CCScale9Sprite* sp = Get9SpriteFromFrame("suosou_1_3_k.png", CCRectMake(10, 10, 468, 40));
    editText = CCEditBox::create(CCSizeMake(484, 50), sp);
    editText->setFontColor(ccWHITE);
    editText->setPlaceholderFontColor(ccc3(0xdc, 0xdc, 0xdc));
    editText->setPlaceHolder("输入好友用户名一起来玩吧！");
    editText->setPosition(ccp(0, -70));
    editText->setAnchorPoint(ccp(0.5, 0.5));
    editText->setInputMode(kEditBoxInputModeAny);
    addChild(editText,100);
    
    JuanZhou::setTouchEnabled(true);

	return true;
}
    
void SearchJuanZhou::setTouchEnabled(bool value)
{
    JuanZhou::setTouchEnabled(value);
    menu->setTouchEnabled(value);
}
    
void SearchJuanZhou::onSearch(cocos2d::CCObject *target)
{
    std::string inputText = editText->getText();
    if(inputText.empty())
    {
        SystemInform::alertMessage(GameScene::getInstance(), "搜索名字不能为空", "警告", [](){});
        return;
    }
        
    
	SystemInform *inform = SystemInform::alertLoading(GameScene::getInstance());
	PuzzleRPC::getInstance()->findFriendByName(PlayerData::getInstance()->getToken(), inputText.c_str(), 0 , 20 , [=](std::auto_ptr<PlayerList> list , std::auto_ptr<Error> e)
    {
        inform->cancel();
		if(e.get())
        {
			CCLog("find Friend Error: %s", e->toStr().c_str());
            e->handleErrorInform();
			return;
		}
		CCLog("find friend success!!");
        auto layer = SearchResultLayer::create(list.get());
		GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
    });
    ring("sound/v/button_click.mp3");
}


bool SearchFriendLayer::init()
{
	if(!ContentLayer::init())return false;

	CCMenu *menu = CCMenu::create();

	CCMenuItem *backItem = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"), GetSprite("gameui/back_t.png"),
                                                    this , menu_selector(SearchFriendLayer::onBack));
	CCMenuItem *titleItem = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"), GetSprite("gameui/hongchengdi_t_tongyong.png"));
    LabelX* name = LabelX::create("搜索好友", FONT_CN, 36);
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

    searchBoard = SearchJuanZhou::create();

	CCSprite *zhou1 = GetSprite("gameui/suosou_1_1.png");
	CCSprite *zhou2 = GetSprite("gameui/suosou_1_1.png");

    zhou2->setFlipX(true);
	zj = JuanZhouLayer::create();
	zj->addSkin(searchBoard , zhou1 , zhou2);
	zj->setPositionY(-30);
	addChild(zj);
    
	return true;
}

void SearchFriendLayer::setTouchEnabled(bool value)
{
    ContentLayer::setTouchEnabled(value);
    searchBoard->setTouchEnabled(value);
}


void SearchFriendLayer::onBack(cocos2d::CCObject *target)
{
	GameScene::getInstance()->popLayer();
    ring("sound/v/button_click.mp3");
}

void SearchFriendLayer::onEnterAnimationDone()
{
	zj->startAnim();
}
    
    
bool InviteJuanZhou::init()
{
    if(!JuanZhou::init())return false;
    
    viewPort = CCSize(20 , 600);
    
    bg = GetSprite("gameui/yaoqing_1_3.png");
    bg->setAnchorPoint(ccp(0.5 , 1));
    addChild(bg);
    
    CCLabelTTF* referCodeLabel;
    std::string referCode = PlayerData::getInstance()->player->referCode;
    addLabelTTF(this, referCodeLabel, referCode.c_str()
                , FONT_CN, 34, ccp(-50,-46) ,ccp(0,0.5));
    
    menu = CCMenu::create();
    CCMenuItem *item_sina = CCMenuItemSprite::create(GetSprite("gameui/yaoqing_xinlang.png"), GetSprite("gameui/yaoqing_xinlang.png"), this, menu_selector(InviteJuanZhou::onInvSina));
    
    CCMenuItem* item_tencent = CCMenuItemSprite::create(GetSprite("gameui/yaoqing_tengxun.png"), GetSprite("gameui/yaoqing_tengxun.png"),this,menu_selector(InviteJuanZhou::onInvTencent));
    CCMenuItem* item_wechat = CCMenuItemSprite::create(GetSprite("gameui/yaoqing_weixin.png"), GetSprite("gameui/yaoqing_weixin.png"),this,menu_selector(InviteJuanZhou::onInvWechat));
    
    menu->addChild(item_sina);
    menu->addChild(item_tencent);
    menu->addChild(item_wechat);
    menu->setPosition(ccp( 0 , -123));
    menu->alignItemsHorizontallyWithPadding(5);
    addChild(menu);
    
    JuanZhou::setTouchEnabled(true);
    
    tweets.push_back("还没玩过《群侠转》吗，快跳到《群侠转》里来！笑看金氏群侠间风起云涌，感受虐暴眼球的爱恨情仇！我的邀请码是【"+referCode+"】，通过邀请码进入游戏赢取豪华大礼包，还有每日一抽哦！免费下载地址:https://itunes.apple.com/us/app/zhuan-jiang-hu/id590246890");
    tweets.push_back("传说中的金氏侠客大乱斗《群侠转》，你还没加入吗？输入我的邀请码【"+referCode+"】，和杨过、东方娘娘一起玩《群侠转》，还有豪华礼包，每日一抽侠客元宝送不停！免费下载地址:https://itunes.apple.com/us/app/zhuan-jiang-hu/id590246890");
    tweets.push_back("我在玩《群侠转》哦，app store榜单推荐休闲游戏！输入邀请码【"+referCode+"】加入《群侠转》，赢取豪华礼包，侠客元宝送不停! 免费下载地址:https://itunes.apple.com/us/app/zhuan-jiang-hu/id590246890");
    tweets.push_back("最近痴迷《群侠转》，加入我，和我一起搜集风流倜傥的杨过，妖娆的东方娘娘吧！输入邀请码【"+referCode+"】加入游戏就能获得超豪华大礼包，每日一抽侠客元宝送不停！免费下载地址:https://itunes.apple.com/us/app/zhuan-jiang-hu/id590246890");
    
    return true;
}
    
void InviteJuanZhou::onInvite(cocos2d::CCObject *target)
{
#if PH_USE_WEIBO
    //to be continue....
    auto& sina = Weibo<Sina>::instance();
    if(sina.isAuthorized())
        sina.postTweet("test post tweet in app", [](std::auto_ptr<WBError> error){});
    else
        SystemInform::alertMessage(GameScene::getInstance(), "no!", "tips", [=](){});
#endif
}
                                                        
void InviteJuanZhou::onInvSina(cocos2d::CCObject *target)
{
#if PH_USE_WEIBO
    auto& sina = Weibo<Sina>::instance();
    if(sina.isAuthorized())
        SystemInform::alertConfirmPanel(GameScene::getInstance(), "邀请新浪微博好友一起《群侠转》", "提示",
                                        [=,&sina](){
                                            sina.postTweet(tweets[random()%4], [=](std::auto_ptr<WBError> error){
                                                std::string msg;
                                                if(error.get())
                                                    msg = error->errorDesc;
                                                else
                                                    msg = "发布成功！";
                                                SystemInform::alertMessage(GameScene::getInstance(), msg.c_str(), "提示", [=](){});
                                            });
                                        },
                                        [=](){});
        
    else
        SystemInform::alertConfirmPanel(GameScene::getInstance(), "你还没有绑定新浪微博，要去绑定吗？", "提示",
                                        [=](){
                                            GameScene::getInstance()->pushLayerWithDefaultAnimation(AccountLayer::create());
                                        },
                                        [=](){});
#endif
}

void InviteJuanZhou::onInvTencent(cocos2d::CCObject *target)
{
#if PH_USE_WEIBO
    auto&  tencent = Weibo<Tencent>::instance();
    if(tencent.isAuthorized())
        SystemInform::alertConfirmPanel(GameScene::getInstance(), "邀请腾讯微博的好友一起《群侠转》", "提示",
                                        [=,&tencent](){
                                            tencent.postTweet(tweets[random()%4], [=](std::auto_ptr<WBError> error){
                                                std::string msg;
                                                if(error.get())
                                                    msg = error->errorDesc;
                                                else
                                                    msg = "发布成功！";
                                                SystemInform::alertMessage(GameScene::getInstance(), msg.c_str(), "提示", [=](){});
                                            });
                                        },
                                        [=](){});
    else
        SystemInform::alertConfirmPanel(GameScene::getInstance(), "你还没有绑定腾讯微博，要去绑定吗？", "提示",
                                        [=](){
                                            GameScene::getInstance()->pushLayerWithDefaultAnimation(AccountLayer::create());
                                        },
                                        [=](){});
#endif
}

void InviteJuanZhou::onInvWechat(cocos2d::CCObject *target)
{
#if PH_USE_WEIBO
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    boost::shared_ptr<WXTextMessageRequest> req = WXTextMessageRequest::create();
    req->text(tweets[random()%4]);
    //req->thumbData(data_vec);
    //req->webpageURL("www.baidu.com");
    
    WeixinApi::instance().setResponseHandler([=](boost::shared_ptr<WXBaseResp> resp){
        //SystemInform::alertMessage(GameScene::getInstance(), resp->errorString().c_str(), "提示", [=](){});
    });
    WeixinApi::instance().sendRequest(req);
#endif
#endif
}
    
bool InviteFriendLayer::init()
{
    if(!ContentLayer::init())
        return false;
    
    CCMenu* menu = CCMenu::create();
    CCMenuItem *backItem = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"), GetSprite("gameui/back_t.png"),
                                                    this , menu_selector(InviteFriendLayer::onBack));
    CCMenuItem *titleItem = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"), GetSprite("gameui/hongchengdi_t_tongyong.png"));
    LabelX* name = LabelX::create("邀请好友", FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp( 141 , 35));
    titleItem->addChild(name);
    
    menu->addChild(backItem);
    menu->addChild(titleItem); 
    
    backItem->setAnchorPoint(ccp(0.5 , 1));
    titleItem->setAnchorPoint(ccp(0.5 , 1));
    menu->alignItemsHorizontallyWithPadding(8);
    menu->setPosition(ccp(-70 , 18));
    addChild(menu);
    
    inviteJuanZhou = InviteJuanZhou::create();
	CCSprite *zhou1 = GetSprite("gameui/biandui_2_1_1.png");
	CCSprite *zhou2 = GetSprite("gameui/biandui_2_1_1.png");
    zhou2->setFlipX(true);
	zj = JuanZhouLayer::create();
	zj->addSkin(inviteJuanZhou , zhou1 , zhou2);
	zj->setPositionY(-30);
	addChild(zj);
    
    return true;
}
    
void InviteFriendLayer::onEnterAnimationDone()
{
    zj->startAnim();
}

void InviteFriendLayer::onBack(cocos2d::CCObject *target)
{
    GameScene::getInstance()->popLayer();
    ring("sound/v/button_click.mp3");
}

//////////////////////////////////////////////
//MailLayer
//////////////////////////////////////////////
    
bool MailItem::init()
{
    if(!ContentLayer::init())
        return false;
    
    minHeight = 150.0f;
    readLabel = NULL;
    packLogo = NULL;
    openBtn = NULL;
    //make zhou
    {
        CCSprite* zhouTop = GetSprite("gameui/zhanchang_1_1.png");
        zhouTop->setAnchorPoint(ccp(0.5, 1));
        addChild(zhouTop, 10);
        
        zhouBottom = GetSprite("gameui/zhanchang_1_2.png");
        zhouBottom->setAnchorPoint(ccp(0.5, 0));
        addChild(zhouBottom, 10);
        
    }
    //create the content
    {
        content = ScissorLayer::create();
        content->setPositionY(-31);
        addChild(content);
        
        bg = Get9SpriteFromFrame("youjian_3.png", 15, 105, 370, 1);
        bg->setAnchorPoint(ccp(0.5 , 1));
        bg->setContentSize(CCSize(400, 400));
        content->addChild(bg, -10);
        
        addLabelBMFont(content, typeLabel, "收件人", "bmfont/PW_Yellow_24.fnt", ccp(-155, -30), ccp(0.5, 0.5));
        typeLabel->setScale(contentScale());
        addLabelTTF(content, senderLabel, "系统GM", FONT_CN, 26, ccp(-110, -30), ccp(0, 0.5));
        
        zhutiLabel = addLabelBMFont(content, "主题", "bmfont/PW_Yellow_24.fnt", ccp(-155, -65), ccp(0.5, 0.5));
        zhutiLabel->setScale(contentScale());
        addLabelTTF(content, topicLabel, "", FONT_CN, 26, ccp(-110, -65), ccp(0, 0.5));
        hintBody = CCLabelTTF::create("",FONT_CN ,24 ,CCSize(360, 0) ,kCCTextAlignmentLeft);
        hintBody->setAnchorPoint(ccp(0.5, 1));
        hintBody->setPosition(ccp(0, -105));
        hintBody->setColor(ccc3(0, 0, 0));
        content->addChild(hintBody);
        
        addLabelTTF(content, timeLabel, "2天前", FONT_CN, 22, ccp(-130, 0), ccp(0.5, 0.5), ccBLACK);
        
        marginLeft = GetSprite("gameui/zhanchang_2_2.png");
        marginLeft->setAnchorPoint(ccp(1, 1));
        marginLeft->setPosition(ccp(-200, 0));
        content->addChild(marginLeft);
        
        marginRight = GetSprite("gameui/zhanchang_2_2.png");
        marginRight->setAnchorPoint(ccp(0, 1));
        marginRight->setPosition(ccp(199, 0));
        marginRight->setFlipX(true);
        content->addChild(marginRight);
    }
    
    touchRectUp = CCRectMake(-230, -110, 460, 110);
    touchRectDown = zhouBottom->boundingBox();
    
    setTouchEnabled(true);
    
    return true;
}
    
void MailItem::refresh(Mail* mail)
{
    if(openBtn)
    {
        openBtn->removeFromParentAndCleanup(true);
        openBtn = NULL;
        
        packLogo->removeFromParentAndCleanup(true);
        packLogo = NULL;
    }
    
    this->mail = mail;
    isSendMail = (mail->sender == PlayerData::getInstance()->player->uid);
    isOpen = false;
    setContentSize(CCSizeMake(640, minHeight));
    
    std::string s = (isSendMail ? "收件人" : "发件人");
    typeLabel->setString(s.c_str());
    topicLabel->setString(trimNameCN(mail->message, 12).c_str());
    hintBody->setString(mail->message.c_str());
    
    int timeLabelPosY = hintBody->getPositionY()-hintBody->getContentSize().height-30;
    timeLabel->setPositionY(timeLabelPosY);
    timeLabel->setString(getTimeDes(mail->time).c_str());
    
    if(!mail->packageOpened && mail->package.size() > 0)
    {
        packLogo = GetSprite("gameui/libao_vip.png");
        packLogo->setPosition(ccp(199, -45));
        content->addChild(packLogo);
        
        openBtn = ButtonSprite::create(createBtn("获取"), createBtn("获取"));
        openBtn->setOnClickListener([=](){
            if(!isOpen)
                return;
            SystemInform* inform = SystemInform::alertLoading(runningScene());
            PuzzleRPC::getInstance()->openPackage(PlayerData::getInstance()->getToken(), mail->id,
                                                  [=](std::auto_ptr<Player> player, std::auto_ptr<Error> e)
              {
                  inform->cancel();
                  if(e.get())
                  {
                      e->handleErrorInform();
                      return;
                  }
                  mail->packageOpened = true;
                  content->removeChild(openBtn);
                  packLogo->removeFromParentAndCleanup(true);
                  packLogo = NULL;
                  openBtn = NULL;
                  PlayerData::getInstance()->updatePlayer(player.get());
                  
                  SystemInform::alertRewardTips(runningScene(), "获得奖励", mail->package, [=](){});
              });
        });
        openBtn->setPosition(ccp(125, timeLabelPosY));
        content->addChild(openBtn);
    }
    
    maxHeight = std::abs(content->getPositionY())+std::abs(timeLabelPosY)+80;
    bg->setContentSize(CCSize(400, maxHeight-68));
    
    updateReadState();
    reSize();
    scheduleUpdate();
}
    
void MailItem::updateReadState()
{
    if(readLabel)
    {
        readLabel->removeFromParentAndCleanup(true);
        readLabel = NULL;
    }
    
    if(isSendMail)
        return;

    if(mail->read)
        readLabel = addLabelBMFont(this, "已读", "bmfont/PW_24_green.fnt", ccp(-185, -20), ccp(0.5, 0.5));
    else
        readLabel = addLabelBMFont(this, "未读", "bmfont/PW_24_red.fnt", ccp(-185, -20), ccp(0.5, 0.5));
    readLabel->setZOrder(20);
    readLabel->setScale(contentScale());
}
    
void MailItem::write()
{

}
    
void MailItem::reSize()
{
    int height = getContentSize().height;
    content->viewPort = CCSizeMake(640, height-68);
    zhouBottom->setPositionY(-height);
    touchRectDown = zhouBottom->boundingBox();
    float contentHeight = content->viewPort.height;
    marginLeft->setScaleY(contentHeight/marginLeft->getContentSize().height);
    marginRight->setScaleY(contentHeight/marginRight->getContentSize().height);
}
    
void MailItem::onSingleClick(cocos2d::CCTouch *touch)
{
    CCPoint localPoint = convertTouchToNodeSpace(touch);
    if(touchRectUp.containsPoint(localPoint) || touchRectDown.containsPoint(localPoint))
    {
        isOpen = !isOpen;
        //make the scrollview scroll to curr item position and close the other items
        if(isOpen)
        {
            if(!mail->read)
            {
                mail->read = true;
                PlayerData::getInstance()->newMailsNum--;
                PuzzleRPC::getInstance()->markAsRead(PlayerData::getInstance()->getToken(), mail->id,
                                                     [](std::auto_ptr<Error> e){});
                updateReadState();
            }
            
            MenuScrollView* scrollView = (MenuScrollView*)getUserData();
            CCArray* items = scrollView->getChildren();
            for(int i = 0; i < items->count(); i++)
                if(items->objectAtIndex(i) == this)
                        scrollView->moveTo(i);
                else
                    ((MailItem*)items->objectAtIndex(i))->isOpen = false;
        }
    }
}

void MailItem::update(float dt)
{
    int height = getContentSize().height;
    
    if(isOpen && height != maxHeight)
    {
        setContentSize(CCSizeMake(640, easing(height, maxHeight, 0.3)));
        reSize();
    }
    else if(!isOpen && height != minHeight)
    {
        setContentSize(CCSizeMake(640, easing(height, minHeight, 0.3)));
        reSize();
    }
}

bool WriteMailItem::init()
{ 
    if(!MailItem::init())
        return false;
    
    minHeight = maxHeight = 500;
    bg->setContentSize(CCSizeMake(400, maxHeight-68));
    setContentSize(CCSizeMake(640, minHeight));
    touchRectUp = CCRectMake(-200, -500, 400, 500);
    inputText = "";
    tailFlag = false;
    
    ButtonSprite* cancelBtn = ButtonSprite::create("gameui/guanbi.png");
    cancelBtn->setPosition(ccp(165, -30));
    cancelBtn->setOnClickListener([=](){
        GameScene::getInstance()->popLayer();
    });
    content->addChild(cancelBtn);
    
    inputLabel = CCLabelTTF::create("", FONT_CN, 24, CCSize(360, 280), kCCTextAlignmentLeft);
    inputLabel->setAnchorPoint(ccp(0.5, 1));
    inputLabel->setPosition(ccp(0, -75));
    inputLabel->setColor(ccc3(0, 0, 0));
    content->addChild(inputLabel);
    
    CCScale9Sprite* sp = Get9SpriteFromFrame("suosou_1_3_k.png", CCRectMake(10, 10, 468, 40));
    hackLabel = CCEditBox::create(CCSizeMake(484, 50), sp);
    hackLabel->setPosition(ccp(0, 10000 ));
    hackLabel->touchDownAction(NULL, CCControlEventTouchUpInside);
    content->addChild(hackLabel);
    
    ButtonSprite* sendBtn = ButtonSprite::create(createBtn("发送"), createBtn("发送"));
    sendBtn->setPositionY(-390);
    sendBtn->setOnClickListener([=](){
        /*if(!strcmp(inputLabel->getString(), ""))
        {
            SystemInform::alertMessage(runningScene(), "请输入要发送的信息", "提示", [](){});
            return;
        }*/
        SystemInform* inform = SystemInform::alertLoading(runningScene());
        PuzzleRPC::getInstance()->sendMailToGM(PlayerData::getInstance()->getToken(), "", inputLabel->getString(),
                                               [=](std::auto_ptr<Mail> mail, std::auto_ptr<Error> cb)
        {
            inform->cancel();
            if(cb.get())
            {
                SystemInform::alertMessage(runningScene(), "发送失败！", "提示", [](){});
                return;
            }
            
            boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
            PlayerData::getInstance()->lastSendMailTime = now;
            
            SystemInform::alertMessage(runningScene(), "发送成功！", "提示",
                                       [=](){GameScene::getInstance()->popLayer();});
        });
    });
    content->addChild(sendBtn);
    
    reSize();
    this->scheduleUpdate();
    this->schedule(schedule_selector(WriteMailItem::schedule1Sec), 0.5);
    
    return true;
}
  
void WriteMailItem::update(float dt)
{
    std::string tail = tailFlag ? "|" : "";
    inputLabel->setString((hackLabel->getText()+tail).c_str());
}
    
void WriteMailItem::schedule1Sec(float dt)
{
    tailFlag = !tailFlag;
}
    
void WriteMailItem::onSingleClick(cocos2d::CCTouch *touch)
{
    CCPoint localPoint = convertTouchToNodeSpace(touch);
    if(touchRectUp.containsPoint(localPoint))
    {
        hackLabel->touchDownAction(NULL, CCControlEventTouchUpInside);
    }
}
    
bool ShareBoard::init(std::string sharedMsg)
{
    if(!MailItem::init())
        return false;
    minHeight = maxHeight = 460;
    bg->setContentSize(CCSizeMake(400, maxHeight-68));
    setContentSize(CCSizeMake(640, minHeight));
    touchRectUp = CCRectMake(-200, -500, 400, 500);
    
    typeLabel->setString("");
    topicLabel->setString("每天首次分享有1w铜钱奖励");
    topicLabel->setAnchorPoint(ccp(0.5, 0.5));;
    topicLabel->setPositionX(0);
    
    senderLabel->setString("发布微博");
    senderLabel->setColor(TITLE_YELLOW);
    senderLabel->setFontSize(32);
    senderLabel->setAnchorPoint(ccp(0.5, 0.5));
    senderLabel->setPositionX(0);
    
    zhutiLabel->setString("");
    
    hintBody->setString(sharedMsg.c_str());
    
    reSize();
    return true;
}
    
    
bool SendMailLayer::init()
{
    if(!ContentLayer::init())
        return false;
    
    isShowTop = false;
    isShowBottom = false;
    
    WriteMailItem* mail = WriteMailItem::create();
    mail->setPositionY(-35);
    addChild(mail);
    
    return true;
}
    
MailLayer::~MailLayer()
{
    ObjectPool<MailItem>::getInstance()->clear();
}

bool MailLayer::init()
{
    if(!ContentLayer::init())
        return false;
    scrollView = NULL;
    
    CCMenu* menu = CCMenu::create();
    CCMenuItem *backItem = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                                    GetSprite("gameui/back_t.png"),
                                                    this ,
                                                    menu_selector(InviteFriendLayer::onBack));
    CCMenuItem *titleItem = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                     GetSprite("gameui/hongchengdi_t_tongyong.png"));
    LabelX* name = LabelX::create("邮件", FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp( 141 , 35));
    titleItem->addChild(name);
    
    menu->addChild(backItem);
    menu->addChild(titleItem);
    
    backItem->setAnchorPoint(ccp(0.5 , 1));
    titleItem->setAnchorPoint(ccp(0.5 , 1));
    menu->alignItemsHorizontallyWithPadding(8);
    menu->setPosition(ccp(-70 , 18));
    addChild(menu, 11);
    
    /*switcher = MailSwitcher::create();
    switcher->setPosition(0, -85);
    switcher->setAnchorPoint(ccp(0.5, 0.5));
    switcher->setCallBack([=](int i){
        makeItems(i);
    });
    addChild(switcher, 10);*/
    
    
    return true;
}
    
void MailLayer::makeItems(int type)
{
    if(scrollView)
    {
        scrollView->removeFromParentAndCleanup(true);
        ObjectPool<MailItem>::getInstance()->collect();
    }
    
    scrollView = MenuScrollView::create();
	scrollView->setViewSize(CCSize(winSize.width, 700), CCRect(0, -55, winSize.width, 595));
    scrollView->scrollBarAdjust = 0;
    addChild(scrollView);
    
    std::vector<Mail>* mails = NULL;
    if(type == 0)
        mails = &received;
    else if(type == 1)
    {
        CCSprite* img = GetSprite("gameui/caidan_1_ty.png");
        img->addChild(createTitleLabel("寄信", ccp(264, 60)));
        CCSprite* img2 = GetSprite("gameui/caidan_1_ty.png");
        img2->addChild(createTitleLabel("寄信", ccp(264, 60)));
        ButtonSprite* sendItem = ButtonSprite::create(img, img2, kCCMenuHandlerPriority, false, false);
        sendItem->setOnClickListener([=](){
            boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
            auto interval = now - PlayerData::getInstance()->lastSendMailTime;
            if(interval.total_seconds() <= 180)
            {
                SystemInform::alertMessage(runningScene(), "你发送的邮件过于频繁，请稍后再发。", "提示", [](){});
                return;
            }
            GameScene::getInstance()->pushLayer(SendMailLayer::create());
        });
        scrollView->addLayer(sendItem);
        mails = &sent;
    }
    
    if(mails)
        for(int i = 0; i < mails->size(); i++)
        {
            MailItem* item = ObjectPool<MailItem>::getInstance()->getObject();
            item->refresh(&mails->at(i));
            item->setUserData(scrollView);
            scrollView->addLayer(item);
        }
}

void MailLayer::onEnter()
{
    CCLayer::onEnter();
    
    SystemInform* inform = SystemInform::alertLoading(runningScene());
    PuzzleRPC::getInstance()->checkMail(PlayerData::getInstance()->getToken(),
                                        [=](std::auto_ptr<MailBox> box,std::auto_ptr<Error> e)
                                        {
                                            inform->cancel();
                                            if(e.get())
                                            {
                                                e->handleErrorInform();
                                                return;
                                            }
                                            received.clear();
                                            sent.clear();
                                            int count = 0;
                                            for(int i = box->mails.size()-1; i >=0; i--)
                                            {
                                                if(box->mails[i].sender == PlayerData::getInstance()->player->uid)
                                                    sent.push_back(box->mails[i]);
                                                else
                                                {
                                                    received.push_back(box->mails[i]);
                                                    if(!box->mails[i].read)
                                                        count++;
                                                }
                                                    
                                            }
                                            if(received.size())
                                                makeItems(0);
                                            else
                                                SystemInform::alertMessage(runningScene(), "你当前没有邮件!", "提示",
                                                                           [](){GameScene::getInstance()->popLayer();});
                                            PlayerData::getInstance()->newMailsNum = count;
                                        });
}
    
void MailLayer::onBack()
{
    GameScene::getInstance()->popLayer();
    ring("sound/v/button_click.mp3");
}
    
//////////////////////////////////////////////
//FriendLayer
//////////////////////////////////////////////

bool FriendLayer::init()
{

	if(!ContentLayer::init())return false;
    
    tips = NULL;
    friReqTips = mailTips = NULL;

	menu = CCMenu::create();
	CCMenuItem *myFriendBtn = CCMenuItemSprite::create(GetSprite("gameui/caidan_1_ty.png"),
                                                       GetSprite("gameui/caidan_1_ty.png"),
                                                    this, menu_selector(FriendLayer::onMyFriend));
	myFriendBtn->addChild(createTitleLabel("我的好友", ccp(264, 60)));
	menu->addChild(myFriendBtn);
    
	friendAskBtn = CCMenuItemSprite::create(GetSprite("gameui/caidan_1_ty.png"), GetSprite("gameui/caidan_1_ty.png"),
                                                      this, menu_selector(FriendLayer::onFriendAsk));
    friendAskBtn->addChild(createTitleLabel("好友请求", ccp(264, 60)));
	menu->addChild(friendAskBtn);
    
	CCMenuItem *searchFriendBtn = CCMenuItemSprite::create(GetSprite("gameui/caidan_1_ty.png"),
                                                           GetSprite("gameui/caidan_1_ty.png"),
                                                     this, menu_selector(FriendLayer::onSearchFriend));
    searchFriendBtn->addChild(createTitleLabel("搜索好友", ccp(264, 60)));
	menu->addChild(searchFriendBtn);
    
    msgBtn = CCMenuItemSprite::create(GetSprite("gameui/caidan_1_ty.png"),
                                                           GetSprite("gameui/caidan_1_ty.png"),
                                                           this, menu_selector(FriendLayer::onMail));
    msgBtn->addChild(createTitleLabel("邮件", ccp(264, 60)));
	menu->addChild(msgBtn);
    
	menu->alignItemsVerticallyWithPadding(10);
    menu->setPosition(ccp(0, -250));
	addChild(menu);
    
	return true;
}
void FriendLayer::setTouchEnabled(bool value)
{
	ContentLayer::setTouchEnabled(value);
	menu->setTouchEnabled(value);
}
    
void FriendLayer::onEnter()
{
    ContentLayer::onEnter();
    
    const int friReqs = PlayerData::getInstance()->player->pendingFriendRequests;
    const int mails = PlayerData::getInstance()->newMailsNum;
    
    if(friReqTips)
    {
        friReqTips->removeFromParentAndCleanup(true);
        friReqTips = NULL;
    }
    
    if(friReqs > 0)
    {
        friReqTips = makeTips(friReqs);
        friReqTips->setPosition(ccp(460 , 100));
        friendAskBtn->addChild(friReqTips);
    }
    
    if(mailTips)
    {
        mailTips->removeFromParentAndCleanup(true);
        mailTips = NULL;
    }
    
    if(mails > 0)
    {
        mailTips = makeTips(mails);
        mailTips->setPosition(ccp(460 , 100));
        msgBtn->addChild(mailTips);
    }
    
}


void FriendLayer::onMyFriend(CCObject *Target)
{
	GameScene::getInstance()->pushLayerWithDefaultAnimation(MyFriendLayer::create());
    ring("sound/v/button_click.mp3");
}
void FriendLayer::onFriendAsk(CCObject *Target)
{
	GameScene::getInstance()->pushLayerWithDefaultAnimation(FriendAskLayer::create());
    ring("sound/v/button_click.mp3");
}
void FriendLayer::onSearchFriend(CCObject *Target)
{
    auto layer = SearchFriendLayer::create();
	GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
    ring("sound/v/button_click.mp3");
}
    
void FriendLayer::onInviteFriend(cocos2d::CCObject *Target)
{
    GameScene::getInstance()->pushLayerWithDefaultAnimation(InviteFriendLayer::create());
    ring("sound/v/button_click.mp3");
}

void FriendLayer::onMail(CCObject *Target)
{
    GameScene::getInstance()->pushLayerWithDefaultAnimation(MailLayer::create());
    ring("sound/v/button_click.mp3");
}

}