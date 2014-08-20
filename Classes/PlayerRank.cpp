//
//  PlayerRank.cpp
//  HelloCpp
//
//  Created by zql on 13-10-9.
//
//

#include "PlayerRank.h"
#include "UIComponent.h"
#include "GameScene.h"
#include "SystemInform.h"
#include "PuzzleRPC.h"
#include "PlayerData.h"
#include "HeroIcon.h"
#include "UIPresets.h"
#include "XiaKe.h"

namespace PH
{
    CCSprite* createMedal(int rank)
    {
        CCSprite* medal;
        if(rank <= 3)
            medal = GetSprite(("gameui/pvejiangzhang"+toStr(rank)+".png").c_str());
        else
        {
            medal = GetSprite("gameui/pvejiangzhang_t.png");
            addLabelBMFont(medal, toStr(rank).c_str(), "bmfont/HZ_32_yellow.fnt",
                           ccp(medal->getContentSize().width*0.5-2, medal->getContentSize().height*0.5),
                           ccp(0.5, 0.5))->setScale(contentScale());
        }
        return medal;
    }
    
    bool PlayerRankItem::init(RankingItem& item, int rank, RankingType type, bool isMine)
    {
        if(!ContentLayer::init())
            return false;
        
        rankData = item;
        CCSprite* bg = isMine ?  GetSprite("gameui/tiaomu_huang_bg.png"):
                                (rank <= 3 ? GetSprite("gameui/tiaomu_hong_bg.png") :
                                    GetSprite("gameui/tiaomu_lan_bg.png"));
        bg->setAnchorPoint(ccp(0.5, 1));
        addChild(bg);
        
        if(!item.team.empty() && item.team[0].get())
        {
            ButtonSprite* iconBtn = ButtonSprite::create(HeroIcon::create(item.team[0].get()), -127);
            iconBtn->setOnClickListener([=](){
                std::vector<Hero*> team;
                for(boost::shared_ptr<Hero> h : item.team)
                    team.push_back(h.get());
                GameScene::getInstance()->pushLayerWithDefaultAnimation(XiangQingLayer::create(&team));
            });
            iconBtn->setPosition(ccp(-250, -60));
            addChild(iconBtn);
        }
        
        CCSprite* medal = createMedal(rank);
        medal->setPosition(ccp(-160, -37));
        addChild(medal);
        
        
        addLabelTTF(this, item.owner.c_str(), FONT_CN, 30, ccp(-120, -32), ccp(0, 0.5));
        ccColor3B color = isMine ? ccRED : TITLE_YELLOW;
        if(type == RankingType::Player_Level)
            addLabelTTF(this, ("等级："+toStr(item.hostLevel)).c_str(),
                        FONT_CN, 30, ccp(-180, -70),ccp(0, 1), color);
        else
            addLabelTTF(this, ("战斗力："+toStr(item.score)).c_str(),
                        FONT_CN, 30, ccp(-180, -70), ccp(0, 1), color);
        
        ButtonSprite* checkBtn = ButtonSprite::create(createBtn("查看"),createBtn("查看"));
        checkBtn->setOnClickListener([=](){
            if(!this->isTouchEnabled())
                return;
            std::vector<Hero*> team;
            for(boost::shared_ptr<Hero> h : item.team)
                team.push_back(h.get());
            GameScene::getInstance()->pushLayerWithDefaultAnimation(XiangQingLayer::create(&team));
        });
        checkBtn->setPosition(ccp(240, -85));
        addChild(checkBtn);
        
        setContentSize(CCSizeMake(winSize.width, bg->getContentSize().height+10));
        return true;
    }
    
PlayerRankLayer::~PlayerRankLayer()
{
    std::map<RankingType, ZqlScrollView*>::iterator it;
    for(it = casheLayer.begin(); it != casheLayer.end(); it++)
        it->second->release();
}
    
bool PlayerRankLayer::init()
{
    if(!ContentLayer::init())
        return false;
    
    isShowTop = false;
    rankNode = NULL;
    title = createTitleMenu(this, "战力排行", "", menu_selector(PlayerRankLayer::onBack), NULL, false);
    
    ButtonSprite* switchBtn = ButtonSprite::create("gameui/qiehuan_shouye.png");
    switchBtn->setPosition(ccp(200, -50));
    switchBtn->setOnClickListener([=](){
        this->switchRank();
    });
    addChild(switchBtn, 100);
    
    {
        statusBar = GetSprite("gameui/tiaomu_dangqian.png");
        statusBar->setPosition(ccp(0, -790));
        addChild(statusBar, 1000);
        
        addLabelTTF(statusBar, "我的排名:", FONT_CN, 30, ccp(20, 30), ccp(0, 0.5), TITLE_YELLOW);
        addLabelTTF(statusBar, statusTitleLabel, "战斗力", FONT_CN, 30, ccp(490, 30), ccp(1, 0.5), TITLE_YELLOW);
        addLabelTTF(statusBar, myZhanliLabel, "", FONT_NUM, 28, ccp(490, 30), ccp(0, 0.5));
    }
    
    return true;
}
    
void PlayerRankLayer::onEnterAnimationDone()
{
    ContentLayer::onEnterAnimationDone();
    rankType = 0;
    switchRank();
}
    
void PlayerRankLayer::switchRank()
{
    if(currLayer)
        currLayer->removeFromParentAndCleanup(true);
    currLayer = rankType ? createLevelLayer() : createZhanliLayer();
    currLayer->setPositionY(-120);
    addChildWithAnim(currLayer);
    std::string txt = rankType ? "等级排行" : "战力排行";
    title->setString(txt.c_str());
    rankType = ++rankType%2;
}
    
ContentLayer* PlayerRankLayer::createZhanliLayer()
{
    ContentLayer* layer = ContentLayer::create();
    
    boost::function<void (int i)> createRank = [=](int i){
        if(layer->getChildByTag(1001))
            layer->removeChildByTag(1001, true);
        
        ZqlScrollView* view = createRankList(i);
        view->setPositionY(0);
        view->setViewSize(winSize, CCRectMake(winSize.width, -40, winSize.width , 605));
        view->setTag(1001);
        layer->addChild(view);
    };
    
    RankSwitcher* switcher = RankSwitcher::create();
    switcher->setPosition(ccp(0, 0));
    switcher->setCallBack(createRank);
    switcher->setTouchPriority(-200);
    layer->addChild(switcher, 100);
    
    createRank(0);
    
    return layer;
}
    
ContentLayer* PlayerRankLayer::createLevelLayer()
{
    ContentLayer* layer = ContentLayer::create();
    
    ZqlScrollView* view = createRankList(PlayerRankType::Rank_level);
    view->setPositionY(30);
    view->setViewSize(winSize, CCRectMake(winSize.width, 0, winSize.width , 675));
    layer->addChild(view);
    return layer;
}

ZqlScrollView* PlayerRankLayer::createRankList(int type)
{
    
    if(type == 0)
        rankTypeToServer = RankingType::Team_Mix;
    else if(type == 1)
        rankTypeToServer = RankingType::Team_Water;
    else if(type == 2)
        rankTypeToServer = RankingType::Team_Fire;
    else if(type == 3)
        rankTypeToServer = RankingType::Team_Wood;
    else if(type == 4)
        rankTypeToServer = RankingType::Team_Light;
    else if(type == 5)
        rankTypeToServer = RankingType::Team_Dark;
    else if(type == 6)
        rankTypeToServer = RankingType::Player_Level;
    
    if(casheLayer.count(rankTypeToServer))
    {
        ZqlScrollView* layer = casheLayer[rankTypeToServer];
        layer->scheduleUpdate();
        updateBottomBar();
        return layer;
    }
    
    ZqlScrollView* layer = ZqlScrollView::create();
    casheLayer[rankTypeToServer] = layer;
    layer->retain();
    
    SystemInform* inform = SystemInform::alertLoading(runningScene());
    PuzzleRPC::getInstance()->getRankingList(PlayerData::getInstance()->getToken(),
                                             rankTypeToServer,
                                             [=](std::auto_ptr<RankingList> data, std::auto_ptr<Error> e)
         {
             if(e.get())
             {
                 inform->cancel();
                 e->handleErrorInform();
                 return;
             }
             for(int i = 0; i < data->table.size(); i++)
                 layer->addLayer(PlayerRankItem::create(data->table[i], i+1, rankTypeToServer, i+1 == data->hostRank));
             inform->cancel();
             if(data->table.empty())
                 SystemInform::alertMessage(runningScene(), "当前还没有该排名", "提示", [](){});
             rankMap[rankTypeToServer] = data->hostRank;
             deltaMap[rankTypeToServer] = data->rankDelta;
             zhanliMap[rankTypeToServer] = data->hostScore;
             updateBottomBar();

         });
    
    return layer;
}
    
void PlayerRankLayer::updateBottomBar()
{
    
    int rank = rankMap[rankTypeToServer];
    int score;
   
    if(rankNode)
        rankNode->removeFromParentAndCleanup(true);
    if(rank > 50)
    {
        rankNode = addLabelTTF(statusBar, toStr(rank).c_str(), FONT_CN, 28, ccp(160, 30), ccp(0, 0.5));
        
        score = zhanliMap[rankTypeToServer];
    }
    else if(rank > 0)
    {
        rankNode = createMedal(rank);
        rankNode->setPosition(ccp(190, 30));
        statusBar->addChild(rankNode);
        score = zhanliMap[rankTypeToServer];
    }
    else
    {
        rankNode = addLabelTTF(statusBar, "未上榜", FONT_CN, 28, ccp(160, 30), ccp(0, 0.5));
        score = 0;
    }
    
    std::string rankDeltaPath = "";
    if(deltaMap[rankTypeToServer] > 0)
        rankDeltaPath = "gameui/jiantou_hong.png";
    else if(deltaMap[rankTypeToServer] < 0)
        rankDeltaPath = "gameui/jiantou_lv.png";
    if(statusBar->getChildByTag(10000))
        statusBar->removeChildByTag(10000, true);
    if(!rankDeltaPath.empty() && rank > 0)
    {
        CCSprite* delta = GetSprite(rankDeltaPath.c_str());
        delta->setPosition(ccp(250, 30));
        delta->setTag(10000);
        statusBar->addChild(delta);
    }
    
    if(rankTypeToServer == RankingType::Player_Level)
    {
        statusTitleLabel->setString("等级：");
        myZhanliLabel->setString(toStr(PlayerData::getInstance()->player->level).c_str());
    }
    else
    {
        statusTitleLabel->setString("战斗力：");
        myZhanliLabel->setString(toStr(score).c_str());
    }
    
}
    
void PlayerRankLayer::onBack()
{
    GameScene::getInstance()->popLayer();
}

}