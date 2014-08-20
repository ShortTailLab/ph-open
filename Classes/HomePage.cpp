//
//  HomePage.cpp
//  HelloCpp
//
//  Created by zql on 13-10-7.
//
//

#include "HomePage.h"
#include "SystemInform.h"
#include "PlayerData.h"
#include "GameScene.h"
#include "Achievement.h"
#include "Store.h"
#include "JiangHu.h"
#include "PvPLayer.h"
#include "UIComponent.h"
#include "Common.h"
#include "PageScrollView.h"
#include "Store.h"
#include "PlayerRank.h"
#include "PuzzleRPC.h"

namespace PH
{
    
bool HomePage::init()
{
    if(!ContentLayer::init())
        return false;
    teamBg = GetSprite("gameui/sy_duiwu.png");
    teamBg->setPosition(ccp(0, 15));
    teamBg->setAnchorPoint(ccp(0.5, 1));
    addChild(teamBg);
    
    CCSprite* skin = GetSprite("gameui/fangxiang_daohang.png");
    skin->setFlipX(true);
    leftBtn = ButtonSprite::create(skin, GetSprite("gameui/fangxiang_daohang2.png"));
    leftBtn->setPosition(ccp(-300, -teamBg->getContentSize().height*0.5f));
    leftBtn->setOnClickListener([=](){
        int id = std::max(view->getCurrentNodeId()-1, 0);
        view->setMoveTargetId(id);
    });
    addChild(leftBtn, 100);
    CCSprite* skin2 = GetSprite("gameui/fangxiang_daohang2.png");
    skin2->setFlipX(true);
    rightBtn = ButtonSprite::create(GetSprite("gameui/fangxiang_daohang.png"), skin2);
    rightBtn->setOnClickListener([=](){
        int id = std::min(view->getCurrentNodeId()+1, view->size-1);
        view->setMoveTargetId(id);
    });
    rightBtn->setPosition(ccp(300, -teamBg->getContentSize().height*0.5f));
    addChild(rightBtn, 100);
    
    for(int i = 0; i < 5; i++)
    {
        CCSprite* dot = GetSprite("gameui/yuandian_a.png");
        dot->setPosition(ccp(-60+30*i, -335));
        addChild(dot);
        CCSprite* dot2 = GetSprite("gameui/yuandian_b.png");
        dot2->setPosition(ccp(-60+30*i, -335));
        addChild(dot2);
        dots.push_back(dot2);
    }
    
    makeTeamGallery();
    
    {
        ButtonSprite* announcement = ButtonSprite::create("gameui/sy_gonggao.png");
        announcement->setOnClickListener([](){
            SystemInform::alertAnnoucement(runningScene(),
                                           "公告",
                                           PlayerData::getInstance()->annoucement,
                                           [=](){});
        });
        announcement->setPosition(ccp(-220, -410));
        addChild(announcement);
        task = ButtonSprite::create("gameui/sy_renwu.png");
        task->setOnClickListener([=](){
            if(this->tryExit(-1))
                GameScene::getInstance()->pushLayerWithDefaultAnimation(AchievementLayer::create());
        });
        task->setPosition(ccp(-110, -410));
        addChild(task);
        
        ButtonSprite* rank = ButtonSprite::create("gameui/sy_paihang.png");
        rank->setOnClickListener([=](){
            if(tryExit(-3))
                GameScene::getInstance()->pushLayerWithDefaultAnimation(PlayerRankLayer::create());
        });
        rank->setPosition(ccp(0, -412));
        addChild(rank);
        
        ButtonSprite* vip = ButtonSprite::create("gameui/sy_vip.png");
        vip->setOnClickListener([=](){
            if(tryExit(-2))
                GameScene::getInstance()->pushLayerWithDefaultAnimation(VIPLayer::create());
        });
        vip->setPosition(ccp(108, -410));
        addChild(vip);
        if(PlayerData::getInstance()->getPlayer()->vipLevel == 0)
        {
            CCSprite* hot = GetSprite("gameui/sy_re.png");
            hot->setPosition(ccp(80, 80));
            vip->addChild(hot);
            hot->runAction(CCRepeatForever::create(CCSequence::create(CCScaleTo::create(0.3, 1.2),
                                                                      CCScaleTo::create(0.3, 1),
                                                                      NULL)));
        }
        
        ButtonSprite* chongzhi = ButtonSprite::create("gameui/sy_chongzhi.png");
        chongzhi->setOnClickListener([=](){
            if(tryExit(-4))
                GameScene::getInstance()->pushLayerWithDefaultAnimation(ChongZhiLayer::create());
        });
        chongzhi->setPosition(ccp(220, -407));
        addChild(chongzhi);
        if(PlayerData::getInstance()->hasPromotion)
        {
            CCSprite* shuang = GetSprite("gameui/sy_shuang.png");
            shuang->setPosition(ccp(80, 80));
            chongzhi->addChild(shuang);
            shuang->runAction(CCRepeatForever::create(CCSequence::create(CCScaleTo::create(0.3, 1.2),
                                                                      CCScaleTo::create(0.3, 1),
                                                                      NULL)));
        }
    }
    
    {
        ButtonSprite* zhanyi = ButtonSprite::create("gameui/sy_zhanyi.png");
        zhanyi->setOnClickListener([=](){
            if(tryExit(-5))
                GameScene::getInstance()->pushLayerWithDefaultAnimation(JiangHu::create(JiangHuType::ZhanYi));
        });
        zhanyi->setPosition(ccp(-210, -560));
        addChild(zhanyi);
        
        std::string path = PlayerData::getInstance()->player->level < 10 ? "_h" : "";
        huodong = ButtonSprite::create(("gameui/sy_huodong"+path+".png").c_str(),
                                       kCCMenuHandlerPriority,
                                       PlayerData::getInstance()->player->level >= 10);
        huodong->setOnClickListener([=](){
            if(PlayerData::getInstance()->player->level >= 10 && tryExit(-6))
            {
                GameScene::getInstance()->pushLayerWithDefaultAnimation(JiangHu::create(JiangHuType::HuoDong));
            }
        });
        huodong->setPosition(ccp(0, -560));
        addChild(huodong);
        
        if(PlayerData::getInstance()->player->level < 10)
        {
            CCLabelBMFont* label = addLabelBMFont(huodong, "10级可开启", "bmfont/PW_Red_24.fnt",
                                                  ccp(huodong->getContentSize().width*0.5, 130), ccp(0.5, 0.5));
            label->setScale(contentScale());
        }
        
        path = ((PlayerData::getInstance()->isPvPOpen && PlayerData::getInstance()->player->level >= 30)) ? "" : "_h";
        ButtonSprite* leitai = ButtonSprite::create(("gameui/sy_leitai"+path+".png").c_str(),
                                                    kCCMenuHandlerPriority,
                                                    PlayerData::getInstance()->isPvPOpen);
        leitai->setPosition(ccp(210, -560));
        addChild(leitai);
        
        leitaiLabel = addLabelBMFont(leitai, "", "bmfont/PW_24_red.fnt",
                                     ccp(leitai->getContentSize().width*0.5, 130), ccp(0.5, 0.5));
        leitaiLabel->setScale(contentScale());
        if(!PlayerData::getInstance()->isPvPOpen)
        {
            leitaiLabel->setString("尚未开启");
        }
        else if(!PH_PVP_ALWAYS_ENABLED && PlayerData::getInstance()->player->level < 30)
        {
            leitaiLabel = addLabelBMFont(leitai, "30级可开启", "bmfont/PW_24_red.fnt",
                                                  ccp(leitai->getContentSize().width*0.5, 130), ccp(0.5, 0.5));
            leitaiLabel->setScale(contentScale());
        }
        else
        {
            schedule1Sec();
            schedule(schedule_selector(HomePage::schedule1Sec), 1);
            leitai->setOnClickListener([=](){
                if(tryExit(-7))
                {
                    GameScene::getInstance()->pushLayerWithDefaultAnimation(PvPLayer::create());
                }
            });
        }
        

    }
    
    return true;
}
void HomePage::schedule1Sec()
{
    using namespace boost::date_time;
    using namespace boost::posix_time;
    ptime startTime = PlayerData::getInstance()->pvpStartTime;
    ptime endTime = PlayerData::getInstance()->pvpEndTime;
    ptime now = boost::posix_time::second_clock::universal_time() +
    PlayerData::getInstance()->delta_local_server;
    auto toStart = startTime - now;
    auto toEnd = endTime - now;
    
    if(!toStart.is_negative())
    {
        int h = toStart.hours();
        int m = toStart.minutes();
        int s = toStart.seconds();
        
        char ss[128];
        if(h)
            sprintf(ss, "%02d:%02d:%02d后开启", h, m, s);
        else
            sprintf(ss, "%02d:%02d后开启 ", m, s);
        leitaiLabel->setString(ss);
    }
    else if(toEnd.is_negative())
        leitaiLabel->setString("今日已结束");
    else
        leitaiLabel->setString("");
    
}
    
void HomePage::onEnter()
{
    ContentLayer::onEnter();
    makeTeamGallery();
    teamIdOnEnter = PlayerData::getInstance()->getActiveTeam();
    
    CCSprite* tip = (CCSprite*)task->getChildByTag(100);
    if(tip)
        tip->removeFromParentAndCleanup(true);
    if(PlayerData::getInstance()->doneTasksNum > 0)
    {
        tip = makeTips(PlayerData::getInstance()->doneTasksNum);
        tip->setPosition(ccp(80 , 80));
        tip->setTag(100);
        task->addChild(tip);
    }
    
    
    // Tutorial stuff must be done in onEnter.
    // fail scenario if this is done in init:
    //     1. init jianghu, init calls selectTab(x)
    //     2. tab x is pushed to nav stack
    //     3. init jianghu returns, pushed to nav stack
    if(GameConfig::shared()->getTutorialProgress() == TutorialFirstBattle)
    {
        ArrowMask::shared()->setMask(CCRect(0, 140, 640, 685));
    }
    else if(GameConfig::shared()->getTutorialProgress() == TutorialSpendGoldOnHero)
    {
        AlertMessage::create("",
                             "招募侠客：少侠经过了试炼，声名鹊起！现在试试看[color=ffff0000]招募[/color]一名侠客陪你一起出入江湖！");
        ArrowMask::shared()->setMask(CCRect(220, 0, 100, 100));
        ArrowMask::shared()->setArrow(ccp(265, 150), ArrowDirection::Down);
    }
    else if(GameConfig::shared()->getTutorialProgress() == TutorialTeam ||
            GameConfig::shared()->getTutorialProgress() == TutorialQiangHua)
    {
        ArrowMask::shared()->clear();
        GameScene::getInstance()->selectTab(2);
    }
    
    if(GameConfig::shared()->getTutorialProgress() == TutorialSkip &&
       PlayerData::getInstance()->player->level >= UNLOCK_YUANFEN_LEVEL &&
       !GameConfig::shared()->showShuffleRelation)
    {
        bool has3star = false;
        for(const Hero* h : PlayerData::getInstance()->player->storage)
            if(h->profile->star >= 3)
                has3star = true;
        if(has3star)
        {
            GameConfig::shared()->setTutorialProgress(TutorialShuffleRelation);
            GameConfig::shared()->showShuffleRelation = true;
            GameConfig::shared()->save();
            GameScene::getInstance()->selectTab(2);
        }
    }
    
    if(GameConfig::shared()->getTutorialProgress() == TutorialSkip &&
       GameConfig::shared()->firstTimeZhuansheng)
    {
        // hero reaches max level
        for(const Hero* h : PlayerData::getInstance()->getStorage())
        {
            if(h->level == h->profile->maxLevel &&
               h->profile->transformTargetHeroId != -1)
            {
                GameConfig::shared()->setTutorialProgress(TutorialTransformHero);
                GameConfig::shared()->firstTimeZhuansheng = false;
                GameConfig::shared()->save();
                
                GameScene::getInstance()->selectTab(2);
                break;
            }
        }
    }
    
    if(GameConfig::shared()->getTutorialProgress() == TutorialSkip &&
       PlayerData::getInstance()->player->level >= UNLOCK_NEILI_LEVEL &&
       GameConfig::shared()->firstTimeNeili)
    {
        for(const Hero* h : PlayerData::getInstance()->player->storage)
        {
            if(h->profile->star >= 3 &&
               h->profile->tableId >= 10000)
            {
                auto heroes = PlayerData::getInstance()->getHeroesById(h->profile->tableId);
                if(heroes.size() >= 2)
                {
                    GameConfig::shared()->setTutorialProgress(TutorialNeili);
                    GameConfig::shared()->firstTimeNeili = false;
                    GameConfig::shared()->save();
                    
                    GameScene::getInstance()->selectTab(2);
                    break;
                }
            }
        }
    }
    
    // if player level exceeds 10 and we never showed instance stage tutorials, do it.
    if(PlayerData::getInstance()->player->level >= UNLOCK_INSTANCE_STAGE_LEVEL &&
       GameConfig::shared()->getTutorialProgress() == TutorialSkip &&
       !GameConfig::shared()->showInstanceStage)
    {
        ArrowMask::shared()->setMask(CCRect(0, 0, 640, 960));
    }
}
    
void HomePage::onEnterAnimationDone()
{
    if(GameConfig::shared()->getTutorialProgress() == TutorialFirstBattle)
    {
        ArrowMask::shared()->setMask(CCRect(10, 140, 200, 200));
        ArrowMask::shared()->setArrow(ccp(90, 140), ArrowDirection::Up);
        AlertMessage::create("欢迎来到《群侠转》！",
                             "少侠请随我来，闯荡江湖前请先修炼下基本的战斗要领！");
    }
    if(PlayerData::getInstance()->player->level >= UNLOCK_INSTANCE_STAGE_LEVEL &&
       GameConfig::shared()->getTutorialProgress() == TutorialSkip &&
       !GameConfig::shared()->showInstanceStage)
    {
        GameConfig::shared()->showInstanceStage = true;
        GameConfig::shared()->firstTimeShowInstance = true;
        GameConfig::shared()->save();
        
         CCPoint worldPos = inWorldSpace(huodong);
         worldPos.y -= 100;
         ArrowMask::shared()->setArrow(worldPos, ArrowDirection::Up);
         ArrowMask::shared()->setMask(CCRect(215, 140, 210, 200));

         AlertMessage::create("",
                             "恭喜掌门您达到10级，【转珠指引】已经被自动关闭，可在【暂停】界面重新开启。\t 《群侠传》丰富多样的【活动】正等待您的挑战，现在可以进入活动啦！");
         GameConfig::shared()->showDragHint = false;
         GameConfig::shared()->save();
    }
}
    
bool HomePage::tryExit(int msg)
{
    PlayerData* data = PlayerData::getInstance();
    
    std::vector<std::string> teamIds = data->getTeamIds();
    
    if(!data->getTeam()[0])
    {
        SystemInform::alertMessage(GameScene::getInstance(),
                                   "掌门不能为空！",
                                   "警告" ,
                                   [](){});
        return false;
    }
    if(teamIdOnEnter == data->getActiveTeam())
        return true;
    
    SystemInform* alert = SystemInform::alertLoading(GameScene::getInstance());
    
    PuzzleRPC::getInstance()->setFormation(PlayerData::getInstance()->getToken(),
                                           teamIds, data->getActiveTeam(),
                                           [=](std::auto_ptr<Player> p,
                                               std::auto_ptr<Error> e)
   {
       alert->cancel();
       if(e.get())
       {
           LOGD("!!! set team error: %s\n", e->toStr().c_str());
           e->handleErrorInform();
           return;
       }
       
       PlayerData::getInstance()->updatePlayer(p.get());
       teamIdOnEnter = PlayerData::getInstance()->getActiveTeam();
       
       if(msg == -1)
           GameScene::getInstance()->pushLayerWithDefaultAnimation(AchievementLayer::create());
       else if(msg == -2)
           GameScene::getInstance()->pushLayerWithDefaultAnimation(VIPLayer::create());
       else if(msg == -3)
           GameScene::getInstance()->pushLayerWithDefaultAnimation(PlayerRankLayer::create());
       else if(msg == -4)
           GameScene::getInstance()->pushLayerWithDefaultAnimation(ChongZhiLayer::create());
       else if(msg == -5)
           GameScene::getInstance()->pushLayerWithDefaultAnimation(JiangHu::create(JiangHuType::ZhanYi));
       else if(msg == -6)
           GameScene::getInstance()->pushLayerWithDefaultAnimation(JiangHu::create(JiangHuType::HuoDong));
       else if(msg == -7)
           GameScene::getInstance()->pushLayerWithDefaultAnimation(PvPLayer::create());
       else
           GameScene::getInstance()->selectTab(msg);
   });
    return false;
}
    
void HomePage::makeTeamGallery()
{
    cards.clear();
    teamBg->removeAllChildrenWithCleanup(true);
    
    std::vector<Hero*> team = PlayerData::getInstance()->player->team;
    std::vector<CCSprite*> allTeams;
    CCSprite* content;
    for(int i = 0; i < team.size(); i++)
    {
        if(i%5 == 0)
        {
            content = CCSprite::create();
            allTeams.push_back(content);
        }
        if(team[i])
        {
            CCSprite* card = createHeroCard(team[i]->profile);
            card->setPosition(ccp(-222+111.5*(i%5), 192));
            card->setTag(i);
            content->addChild(card);
            cards.push_back(card);
        }
    }
    
    view = PageScrollView::create(allTeams, CCSizeMake(640, 360));
    view->isHomePage = true;
    view->setPositionX(320);
    view->setCallBack([=](CCSprite* s){
        updateBtnState();
        
    });
    view->setCurrentNode(PlayerData::getInstance()->getActiveTeam());
    teamBg->addChild(view);
    

    updateBtnState();
}
    
void HomePage::updateBtnState()
{
    if(view->getCurrentNodeId() == 0)
        leftBtn->onPressed();
    else
        leftBtn->onRelease();
    if(view->getCurrentNodeId() == view->size-1)
        rightBtn->onPressed();
    else
        rightBtn->onRelease();
    
    for(CCSprite* d : dots)
        d->setVisible(true);
    dots[view->getCurrentNodeId()]->setVisible(false);
    
    if(PlayerData::getInstance()->getActiveTeam() != view->getCurrentNodeId())
        PlayerData::getInstance()->setActiveTeam(view->getCurrentNodeId());
}
    
CCSprite* HomePage::createHeroCard(const HeroProfile* profile)
{
    std::string path = "";
    if(profile->color == GemUtils::GemColor::Dark)
        path = "gameui/zi_bg_sy.png";
    else if(profile->color == GemUtils::GemColor::Fire)
        path = "gameui/hong_bg_sy.png";
    else if(profile->color == GemUtils::GemColor::Light)
        path = "gameui/huang_bg_sy.png";
    else if(profile->color == GemUtils::GemColor::Water)
        path = "gameui/lan_bg_sy.png";
    else if(profile->color == GemUtils::GemColor::Wood)
        path = "gameui/lv_bg_sy.png";
    
    CCSprite* bg = GetSprite(path);
    CCSprite* heroBody = GetSprite(profile->bodyPath());
    int clipHeight = 0;
    int py = 0;
    if(profile->indexPageRegion.origin.y > 0)
    {
        py = profile->indexPageRegion.origin.y+145-heroBody->getContentSize().height;
        clipHeight = 290;
    }
    else
    {
        py = 0;
        clipHeight = heroBody->getContentSize().height;
    }
    CCRect rect = profile->indexPageRegion;
    heroBody->setTextureRect(CCRectMake(profile->indexPageRegion.origin.x-50,
                                        py,
                                        100, clipHeight));
    heroBody->setPosition(ccp(bg->getContentSize().width*0.5f, heroBody->getContentSize().height*0.5+5));
    bg->addChild(heroBody);
    //CCLog((profile->nameCN+" "+toStr(py)+" "+toStr(clipHeight)).c_str());
    return bg;
}
    
void HomePage::onSingleClick(CCTouch* touch)
{
    CCRect bound = CCRectMake(-280, -315, 560, 300);
    if(bound.containsPoint(convertTouchToNodeSpace(touch)))
        GameScene::getInstance()->pushLayerWithDefaultAnimation(BianDuiLayer::create());
        
}
    
void HomePage::onLongClick(CCTouch* touch)
{
    for(CCSprite* card : cards)
    {
        CCPoint localPoint = card->getParent()->convertTouchToNodeSpace(touch);
        if(card->boundingBox().containsPoint(localPoint))
        {
            Hero* h = PlayerData::getInstance()->player->team[card->getTag()];
            GameScene::getInstance()->pushLayerWithDefaultAnimation(HeroProfileLayer::create(h));
            return;
        }
    }
}
    
    
}