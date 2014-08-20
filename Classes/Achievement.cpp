//
//  Achievement.cpp
//  HelloCpp
//
//  Created by zql on 13-9-9.
//
//

#include "Achievement.h"
#include "GameScene.h"
#include "Common.h"
#include "PuzzleRPC.h"
#include "PlayerData.h"
#include "SystemInform.h"
#include "QiangHua.h"
#include "ZhuanSheng.h"
#include "XiaKe.h"
#include "Store.h"
#include "JiangHu.h"
#include "WashRelation.h"

namespace PH
{
    bool isHeadToJiangHu(const PlayerTask& task)
    {
        return task.type == PlayerTaskType::LevelClear ||
               task.type == PlayerTaskType::LevelS ||
               task.type == PlayerTaskType::UserLevel ||
               task.type == PlayerTaskType::LevelSCount;
    }
    
    bool isHeadToChongZhi(const PlayerTask& task)
    {
        return task.type == PlayerTaskType::PayGolds ||
               task.type == PlayerTaskType::VIPLevel;
    }
    
    
    bool AchievementItem::init(const PH::PlayerTask& task)
    {
        if(!ContentLayer::init())
            return false;
        
        this->task = task;
        CCSprite* bg = NULL;
        ButtonSprite* btn = NULL;
        if(task.progress >= task.total)
        {
            bg = GetSprite("gameui/yiwanchegn_bg.png");
            btn = ButtonSprite::create(createBtn("领取", ButtonType::SMALL_RED),
                                       createBtn("领取", ButtonType::SMALL_RED));
            btn->setOnClickListener([=](){
                SystemInform* inform = SystemInform::alertLoading(runningScene());
                PuzzleRPC::getInstance()->receivePlayerTaskReward(
                                PlayerData::getInstance()->getToken(),
                                task.tid,
                                [=](std::auto_ptr<Player> player,
                                    std::auto_ptr<Error> e)
                {
                    inform->cancel();
                    if(e.get())
                    {
                        e->handleErrorInform();
                        return;
                    }
                    PlayerData::getInstance()->updatePlayer(player.get());
                    AchievementLayer* layer = ((AchievementLayer*)this->getUserData());
                    
                    std::string msg = task.tid == 5007 ? "怎么我升到50级又送了一个5星侠客？！@群侠转 真的太给力了！都300个侠客了还送！小伙伴们赶快来玩：https://itunes.apple.com/cn/app/qun-xia-zhuan/id669428248" : "";
                    SystemInform::alertRewardTips(runningScene(), "获得奖励", player->rewards,
                          [=](){
                              layer->updateTasks();
                          }, "", msg);
                });
            });
            
        }
        else
        {
            bg = GetSprite("gameui/jinxingzhong_bg.png");
            
            addLabelTTF(this, (toStr(task.progress)+"/"+toStr(task.total)).c_str(),
                        FONT_CN, 28, ccp(180, -95), ccp(1, 0.5), ccGREEN);
            
            if(task.position > 0)
            {
                btn = ButtonSprite::create(createBtn("前往", ButtonType::SMALL_RED),
                                           createBtn("前往", ButtonType::SMALL_RED),
                                           false);
                btn->setOnClickListener([=](){
                    ContentLayer *layer = NULL;
                    if(task.position == 1)
                        GameScene::getInstance()->selectTab(1);
                    else if(task.position == 2)
                        GameScene::getInstance()->selectTab(3);
                    else if(task.position == 3)
                    {
                        layer = ChongZhiLayer::create();
                        layer->isShowBottom = false;
                    }
                    else if(task.position == 4)
                    {
                        GameScene::getInstance()->selectTab(2);
                        layer = QiangHuaLayer::create();
                    }
                    else if(task.position == 5)
                    {
                        GameScene::getInstance()->selectTab(2);
                        layer = ZhuanShengLayer::create();
                    }
                    else if(task.position == 6)
                    {
                        GameScene::getInstance()->selectTab(2);
                        layer = SoulLayer::create();
                    }
                    else if(task.position == 7)
                    {
                        GameScene::getInstance()->selectTab(2);
                        layer = WashRelationLayer::create();
                    }
                    else if(task.position == 8)
                    {
                        GameScene::getInstance()->selectTab(4);
                        layer = SpiritLayer::create();
                    }
                    else if(task.position == 9)
                    {
                        GameScene::getInstance()->selectTab(4);
                        layer = XiweiLayer::create();
                    }
                    else if(task.position == 10)
                    {
                        GameScene::getInstance()->selectTab(1);
                        layer = JiangHu::create(JiangHuType::ZhanYi);
                    }
                    else if(task.position == 11)
                    {
                        GameScene::getInstance()->selectTab(1);
                        layer = JiangHu::create(JiangHuType::HuoDong);
                    }
                    if(layer)
                        GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
                    
                });
            }
            
        }
        bg->setAnchorPoint(ccp(0.5, 1));
        addChild(bg, -1);
        
        if(btn)
        {
            btn->setPosition(ccp(250, -85));
            addChild(btn);
        }
      
        logo = NULL;
        if(task.sample.size() > 1)
        {
            logo = ButtonSprite::create("gameui/libao_vip.png", kCCMenuHandlerPriority, false);
            ((ButtonSprite*)logo)->setOnClickListener([=](){
                SystemInform::alertAchievementPacks(runningScene(), task.sample, [](){});
            });
            logo->runAction(CCRepeatForever::create(CCSequence::create(CCRotateTo::create(0.2, -10),
                                                                      CCRotateTo::create(0.2, 10),
                                                                      NULL)));
        }
        else if(!task.sample.empty() && task.sample[0].type == LoginRewardType::Reward_Gold)
            logo = GetSprite("gameui/yuanbaodui.png");
        else if(!task.sample.empty() && task.sample[0].type == LoginRewardType::Reward_Coins)
            logo = GetSprite("gameui/yiduitongqian.png");
        else if(!task.sample.empty() && task.sample[0].type == LoginRewardType::Reward_Hero)
        {
            HeroProfile p = HPT(task.sample[0].ident);
            if(p.isMat())
                logo = HeroIcon::createWithProfile(&p);
            else if(isSkyHero(p.tableId))
                logo = GetSprite("gameui/denglujiangli_tian.png");
            else
                logo = GetSprite("gameui/denglujiangli_xiake.png");
        }
        else if(!task.sample.empty() && task.sample[0].type == 7)
        {
            logo = GetSprite("gameui/jingliping_100.png");
        }
        
        if(logo)
        {
            logo->setPosition(ccp(-252, -62));
            addChild(logo);
        }
        
        addLabelTTF(this, task.name.c_str(), FONT_CN, 28, ccp(-190, -30), ccp(0, 0.5), TITLE_YELLOW);
        addLabelTTF(this, task.desc.c_str(), FONT_CN, 24, ccp(-190, -60), ccp(0, 0.5), ccWHITE);
        
        setContentSize(CCSizeMake(bg->getContentSize().width, bg->getContentSize().height+10));
        return true;
    }
    
    bool AchievementLayer::init()
    {
        if(!ContentLayer::init())
            return false;
        isFirstEnter = true;
        createTitleMenu(this, "任务", "", menu_selector(AchievementLayer::onBack));
        
        switcher = AchievementSwitcher::create();
        switcher->setPosition(ccp(0, -80));
        switcher->setCallBack([=](int i){
            makeItems();
        });
        addChild(switcher, 1);
        tipsLayer = CCLayer::create();
        addChild(tipsLayer, 100);
        
        scrollView = ZqlScrollView::create();
        scrollView->setViewSize(CCSize(winSize.width , 800) ,  CCRect(0, -40, winSize.width, 540));
        scrollView->setPosition(ccp(0, -80));
        this->addChild(scrollView);
        
        updateTasks();
        
        return true;
    }
    
    void AchievementLayer::updateTasks()
    {
        SystemInform* inform = SystemInform::alertLoading(runningScene());
        PuzzleRPC::getInstance()->getTasks(PlayerData::getInstance()->getToken(),
               [=](std::auto_ptr<PlayerTaskVec> tasksPtr,
                   std::auto_ptr<Error> e)
               {
                   inform->cancel();
                   if(e.get())
                   {
                       e->handleErrorInform();
                       return;
                   }
                   if(!tasksPtr->size())
                   {
                       SystemInform::alertMessage(runningScene(), "你当前没有任务", "提示",
                                                  [](){GameScene::getInstance()->popLayer();});
                       return;
                   }
                   std::sort(tasksPtr->begin(), tasksPtr->end(), [](const PlayerTask& t1, const PlayerTask& t2){
                       return t1.tid < t2.tid;
                   });
                   
                   doneTasks = std::vector<int>(4, 0);
                   PlayerTaskVec finished;
                   for(PlayerTaskVec::iterator it = tasksPtr->begin(); it != tasksPtr->end();)
                       if(it->progress >= it->total)
                       {
                           doneTasks[int(it->category)]++;
                           finished.push_back(*it);
                           it = tasksPtr->erase(it);
                       }
                       else
                           it++;
                   PlayerData::getInstance()->doneTasksNum = finished.size();
                   tasksPtr->insert(tasksPtr->begin(), finished.begin(), finished.end());
                   curTasks = tasksPtr;
                   if(isFirstEnter)
                   {
                       std::vector<int> numOfPerCatogory = std::vector<int>(4, 0);
                       for(const PlayerTask& task : *curTasks)
                           numOfPerCatogory[int(task.category)]++;
                       
                       int switchCurNode[4] = {0,1,2,3};
                       for(int id : switchCurNode)
                           if(numOfPerCatogory[id]>0)
                           {
                               switcher->setCurNode(id);
                               break;
                           }
                       isFirstEnter = false;
                   }
                   
                   initTips();
                   makeItems();
               });
    }
    void AchievementLayer::initTips()
    {
        tipsLayer->removeAllChildrenWithCleanup(true);

        for(int i = 0; i < doneTasks.size(); i++)
            if(doneTasks[i] > 0)
            {
                CCSprite* tip = makeTips(doneTasks[i]);
                tip->setPosition(ccp(-160+150*i, -60));
                tipsLayer->addChild(tip);
            }
        
    }
    
    void AchievementLayer::makeItems()
    {
        scrollView->reset();
        scrollView->removeAllLayers();
        
        if (!curTasks.get())
            return;
        
        for(const PlayerTask& task : *curTasks)
        {
            if( int(task.category) == switcher->getCurNode())
            {
                AchievementItem* item = AchievementItem::create(task);
                item->setUserData(this);
                scrollView->addLayer(item);
            }
        }
        
    }
    
    void AchievementLayer::onBack()
    {
        GameScene::getInstance()->popLayer();
        ring("sound/v/button_click.mp3");
    }
    
}