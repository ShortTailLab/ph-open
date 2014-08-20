//
//  PvPLayer.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 4/26/13.
//
//

#include "PvPLayer.h"
#include "PuzzleRPC.h"
#include "Fonts.h"
#include "UIPresets.h"
#include "PvPMessageHandler.h"
#include "GameScene.h"
#include "PlayerData.h"
#include "ButtonSprite.h"
#include "PvPSearchGame.h"
#include "PvPUtils.h"
#include "HeroIcon.h"
#include "UIComponent.h"
#include "ControlExtensions.h"

namespace PH
{
    CCSprite* createMedal(const int rank, const int reputation)
    {
        std::string medalPath;
        if(rank >=1 && rank <= 3)
            medalPath = "gameui/xunzhang"+toStr(9-rank)+".png";
        else if(rank >=1 && rank <= 10)
            medalPath = "gameui/xunzhang5.png";
        else if(reputation <= 1200)
            medalPath = "gameui/xunzhang1.png";
        else if(reputation <= 1600)
            medalPath = "gameui/xunzhang2.png";
        else if(reputation <= 2000)
            medalPath = "gameui/xunzhang3.png";
        else
            medalPath = "gameui/xunzhang4.png";
        
        return GetSprite(medalPath.c_str());
    }
    
    bool PvPInviteLayer::init()
    {
        if(!ContentLayer::init())
            return false;

        this->setTouchEnabled(true);
        
        // text input field
        textInput = CCTextFieldTTF::textFieldWithPlaceHolder("Player name",
                                                             CCSize(400, 200),
                                                             CCTextAlignment::kCCTextAlignmentLeft,
                                                             FONT_CN,
                                                             30);
        textInput->setPosition(ccp(0, -300));
        textInput->attachWithIME();
        this->addChild(textInput);
        
        CCMenu* menu = CCMenu::create();
        
        // --------------------
        CCMenuItemFont* search = CCMenuItemFont::create("搜索",
                                                        this,
                                                        menu_selector(PvPInviteLayer::search));
        search->setColor(ccBLACK);
        search->setPosition(ccp(160, 0));

        menu->addChild(search);
        menu->setPosition(ccp(0, 0));
        this->addChild(menu);
        
        
        friendsMenu = CCMenu::create();
        friendsMenu->setPosition(ccp(0, -320));
        this->addChild(friendsMenu);
        
        return true;
    }
    
    void PvPInviteLayer::search(CCObject* o)
    {
        SystemInform *inform = SystemInform::alertLoading(GameScene::getInstance());
        
        
        auto handleSearchResult = [=](std::auto_ptr<PlayerList> list,
                                      std::auto_ptr<Error> e)
        {
            inform->cancel();
            if(e.get())
            {
                CCLog("search friend Error: %s", e->toStr().c_str());
                e->handleErrorInform();
                return;
            }
            
            friendsMenu->removeAllChildrenWithCleanup(true);
            
            for(Player* p : list->players)
            {
                CCMenuItemFont* item = CCMenuItemFont::create(p->name.c_str(),
                                                              this,
                                                              menu_selector(PvPInviteLayer::invite));
                item->setColor(ccBLACK);
                item->setUserObject( CCString::create(p->uid) );
                
                friendsMenu->addChild(item);
            }
        };
        
        PuzzleRPC::getInstance()->findFriendByName(PlayerData::getInstance()->getToken(),
                                                   textInput->getString(),
                                                   0,
                                                   20,
                                                   handleSearchResult);
    }
    
    void PvPInviteLayer::invite(CCObject* o)
    {
        CCString* rivalUID = (CCString*)((CCNode*)o)->getUserObject();
        
        PuzzleRPC::getInstance()->getPvP()->sendReset();
        PuzzleRPC::getInstance()->getPvP()->sendChallenge(rivalUID->getCString());
    }
    //----------------------------------
    // PvPTeamConfirmLayer
    //----------------------------------
    
    bool PvPTeamConfirmJuanZhou::init(bool isSwitchEnable)
    {
        if(!JuanZhou::init())
            return false;
        
        viewPort = CCSize(20 , 485);
        effectLayer = NULL;
        relationsView = NULL;
        
        CCSprite *bg = GetSprite("gameui/pvp_1_3.png");
        bg->setAnchorPoint(ccp(0.5 , 1));
        this->addChild(bg);
        
        switcher = BianDuiSwitcher::create(!isSwitchEnable);
        switcher->setPosition(ccp(-275, -42));
        switcher->setCallBack([=](int i){
            if(player->activeTeam != i)
            {
                stopAllActions();
                if(player == PlayerData::getInstance()->getPlayer())
                    PlayerData::getInstance()->setActiveTeam(i);
                player->activeTeam = i;
                this->refresh();
                initHeroRelations();
            }
        });
        addChild(switcher);
        
        heroVec = std::vector<Hero*>(5, (Hero*)NULL);
        heroImgVec = std::vector<HeroIcon*>(5, (HeroIcon*)NULL);
        
        for(int i=0; i<5; i++)
        {
            if(i == 0)
            {
                locations.push_back(CCRect(-274 , -175 , 100 , 100 ));
            }
            else
            {
                locations.push_back(CCRect(-258+108*i , -175 , 100 , 100 ));
            }
        }
        
        addLabelBMFont(this, hpLabel, "", "bmfont/Sumo_22_yellow.fnt", ccp(-120,-193));
        hpLabel->setScale(contentScale());
        addLabelBMFont(this, attackLabel,"", "bmfont/Sumo_22_yellow.fnt", ccp(54,-193));
        attackLabel->setScale(contentScale());
        addLabelBMFont(this, regenLabel, "", "bmfont/Sumo_22_yellow.fnt", ccp(220,-193));
        regenLabel->setScale(contentScale());
        
        for(int i = 0; i < 5; i++)
        {
            CCLabelBMFont* label;
            addLabelBMFont(this, label, "0", "bmfont/Sumo_22_yellow.fnt", ccp(-155+100*i, -240));
            label->setScale(contentScale());
            resistLabels.push_back(label);
        }
        addLabelTTF(this, skillLabel, "", FONT_CN, 28, ccp(-155 , -303), ccp(0, 0.5), ccc3(108, 46, 0));
        descLabel = LabelX::create("", FONT_CN, 28, LabelX::FontEffect::SHADOW, ccp(2, -2));
        descLabel->setPosition(ccp(-253, -342));
        descLabel->setAnchorPoint(ccp(0, 0.5));
        this->addChild(descLabel);
        
        setTouchEnabled(true);
        refresh();
        
        return true;
    }
    
    const std::vector<Hero*> PvPTeamConfirmJuanZhou::getTeam()
    {
        std::vector<Hero*>::const_iterator it = player->team.begin()+player->activeTeam*5;
        return std::vector<Hero*>(it, it+5);
    }
    
    void PvPTeamConfirmJuanZhou::openDone()
    {
        initHeroRelations();
    }
    
    void PvPTeamConfirmJuanZhou::initHeroRelations()
    {
        if(relationsView)
        {
            relationsView->removeFromParentAndCleanup(true);
            relationsView = NULL;
        }
        if(effectLayer)
        {
            effectLayer->removeFromParentAndCleanup(true);
            effectLayer = NULL;
        }
        
        std::vector<HeroRelation> items = calcRelationsInTeam(getTeam());
        if(!items.empty())
        {
            relationsView = ScrollRelations::create(items, CCRectMake(-83, 0, 437, 68));
            relationsView->setPosition(ccp(0, -435));
            addChild(relationsView, 101);
            
            effectLayer = RelationsEffect::create(relationsView, this);
            effectLayer->showEffect();
            addChild(effectLayer, 100);
        }

    }
    
    void PvPTeamConfirmJuanZhou::refresh()
    {
        std::vector<Hero*> confirmTeam = getTeam();
        TeamData teamData;
        teamData.update(confirmTeam);
        
        clear();
        for(int i = 0; i < confirmTeam.size(); i++)
            if(confirmTeam[i])
                addHero(confirmTeam[i], i);
        
        hpLabel->setString(toStr(teamData.hp).c_str());
        attackLabel->setString(toStr(teamData.attack[GemUtils::AllColor]).c_str());
        regenLabel->setString(toStr(teamData.regen).c_str());
        
        if(confirmTeam[0])
        {
            skillLabel->setString(AST(confirmTeam[0]->profile->pvpPassiveSkillId).name.c_str());
            descLabel->setString(AST(confirmTeam[0]->profile->pvpPassiveSkillId).desc.c_str());
        }
        else
        {
            skillLabel->setString("无");
            descLabel->setString("无");
        }
        
        std::vector<float> resists = calcResistance(getTeam());
        for(int i = 0; i < resists.size(); i++)
        {
            int color;
            if(i == 0)
                color = GemUtils::Fire;
            else if(i == 1)
                color = GemUtils::Wood;
            else if(i == 2)
                color = GemUtils::Water;
            else if(i == 3)
                color = GemUtils::Light;
            else
                color = GemUtils::Dark;
            resistLabels[i]->setString(toStr(int((resists[color]+1)*100)).c_str());
        }
    }
    
    void PvPTeamConfirmJuanZhou::onLongClick(CCTouch* touch)
    {
        //check if touch on the heroIcon
        for (HeroIcon *icon : heroImgVec)
        {
            if(!icon)
                continue;
            CCPoint localPoint = icon->getParent()->convertTouchToNodeSpace(touch);
            if(icon->boundingBox().containsPoint(localPoint))
            {
                auto* layer = HeroProfileLayer::create(icon->getHero(), false);
                GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
                break;
            }
        }
    }
    
    
    bool PvPTeamConfirmLayer::init()
    {
        if(!ContentLayer::init())
            return false;
        
        menu = CCMenu::create();
        CCMenuItem *item_back = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                                         GetSprite("gameui/back_t.png"),
                                                         this,
                                                         menu_selector(PvPTeamConfirmLayer::onBack));
        
        CCMenuItem *item_title = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                          GetSprite("gameui/hongchengdi_t_tongyong.png"));
        
        LabelX* name = LabelX::create("确认队伍", FONT_CN, 36);
        name->setColor(ccWHITE);
        name->setAnchorPoint(ccp(0.5, 0.5));
        name->setPosition(ccp( item_title->getContentSize().width*0.5f-2 , item_title->getContentSize().height*0.5f+2));
        item_title->addChild(name);
        
        item_back->setAnchorPoint(ccp(0.5 , 1));
        item_title->setAnchorPoint(ccp(0.5 , 1));
        menu->addChild(item_back);
        menu->addChild(item_title);
        
        menu->alignItemsHorizontallyWithPadding(8);
        menu->setPosition(ccp(-70 , 18));
        addChild(menu , 10);
        
        boost::function<void ()> toBattle = [](){
            GameScene::getInstance()->pushLayerWithDefaultAnimation(PvPSearchGame::create());
            PuzzleRPC::getInstance()->getPvP()->sendFindMatch();
        };
        
        ButtonSprite* battleBtn = ButtonSprite::create(createBtn("立即开战", ButtonType::FOUR_WORD_RED),
                                                       createBtn("立即开战", ButtonType::FOUR_WORD_RED));
        battleBtn->setOnClickListener([=]()
        {
            if(this->teamIdOnEnter != PlayerData::getInstance()->getActiveTeam())
            {
                std::vector<std::string> teamIds = PlayerData::getInstance()->getTeamIds();
                
                if(!PlayerData::getInstance()->getTeam()[0])
                {
                    SystemInform::alertMessage(GameScene::getInstance(),
                                               "掌门不能为空！",
                                               "警告" ,
                                               [](){});
                    return;
                }
                SystemInform* alert = SystemInform::alertLoading(runningScene());
                PuzzleRPC::getInstance()->setFormation(PlayerData::getInstance()->getToken(),
                                                       teamIds, PlayerData::getInstance()->getActiveTeam(),
                                                       [=](std::auto_ptr<Player> p,
                                                           std::auto_ptr<Error> e)
                                                       {
                                                           alert->cancel();
                                                           if(e.get())
                                                           {
                                                               e->handleErrorInform();
                                                               return;
                                                           }
                                                           toBattle();
                                                       });
            }
            else
                toBattle();
            
        });
        battleBtn->setPosition(ccp(0, -600));
        addChild(battleBtn);
        
        PvPTeamConfirmJuanZhou* juanzhou = PvPTeamConfirmJuanZhou::create(PlayerData::getInstance()->getPlayer());
        
        CCSprite *zhou1 = GetSprite("gameui/pvp_1_1.png");
        CCSprite *zhou2 = GetSprite("gameui/pvp_1_1.png");
        zhou2->setFlipX(true);
        
        jz = JuanZhouLayer::create();
        jz->addSkin(juanzhou , zhou1 , zhou2);
        jz->setPosition(ccp(0 , -30));
        addChild(jz);
        
        return true;
    }
    
    void PvPTeamConfirmLayer::onBack()
    {
        GameScene::getInstance()->popLayer();
    }
    
    void PvPTeamConfirmLayer::onEnter()
    {
        ContentLayer::onEnter();
        teamIdOnEnter =  PlayerData::getInstance()->getActiveTeam();
    }
    
    void PvPTeamConfirmLayer::onEnterAnimationDone()
    {
        jz->startAnim();
    }
    
    
    //----------------------------------
    // PvPMsgBoard
    //----------------------------------
    bool PvPMsgBoard::init()
    {
        if(!ContentLayer::init())
            return false;
        
        CCSprite* bg = GetSprite("gameui/pvp_yonghuxinxi.png");
        bg->setAnchorPoint(ccp(0.5, 1));
        addChild(bg);
    
        const Player* player = PlayerData::getInstance()->getPlayer();
        CCSprite* medal = createMedal(player->pvpRank, player->pvpReputation);
        medal->setAnchorPoint(ccp(0.5, 0));
        medal->setPosition(ccp(-235, -103));
        addChild(medal);
        if(player->pvpRank >= 0)
        {
            addLabelBMFont(this, rankLabel, toStr(player->pvpRank).c_str(), "bmfont/Sumo_22_yellow.fnt", ccp(-63, -40), ccp(0.5, 0.5));
            rankLabel->setScale(contentScale());
        }
        
        int repu = player->pvpReputation;
        addLabelBMFont(this, reputationLabel, toStr(repu).c_str(), "bmfont/Sumo_22_yellow.fnt", ccp(-63, -84), ccp(0.5, 0.5));
        reputationLabel->setScale(contentScale());
        
        addLabelTTF(this, getTitleOfRepu(repu).c_str(), FONT_CN, 24, ccp(134, -40), ccp(0.5, 0.5));
        
        addLabelBMFont(this, currencyLabel, toStr(player->pvpScore).c_str(), "bmfont/Sumo_22_yellow.fnt", ccp(134, -84), ccp(0.5, 0.5));
        currencyLabel->setScale(contentScale());
        addLabelBMFont(this, probabilityLabel, (toStr((int)(player->pvpWinRate * 100)) + "%").c_str(), "bmfont/Sumo_22_yellow.fnt", ccp(244, -84), ccp(0.5, 0.5));
        probabilityLabel->setScale(contentScale());
        
        return true;
    }
    
    //----------------------------------
    // PvPRankLayer
    //----------------------------------
    
    bool RankItem::init(Player* player, bool isSelf = false)
    {
        if(!ContentLayer::init())
            return false;
        
        {
            std::string bgPath; 
            if(isSelf)
                bgPath = "pvp_lan_bg.png";
            else if(player->pvpRank > 3)
                bgPath = "gameui/pvp_hong_bg.png";
            else if(player->pvpRank > 2)
                bgPath = "gameui/pvp_tong_bg.png";
            else if(player->pvpRank > 1)
                bgPath = "gameui/pvp_yin_bg.png";
            else
                bgPath = "gameui/pvp_jin_bg.png";
            
            CCSprite* bg = GetSprite(bgPath);
            bg->setAnchorPoint(ccp(0.5, 1));
            addChild(bg);
        }
        
        CCSprite* medal = createMedal(player->pvpRank, player->pvpReputation);
        medal->setAnchorPoint(ccp(0.5,0));
        medal->setPosition(ccp(-223, -114));
        addChild(medal);
        
        addLabelTTF(this, nameLabel, player->name.c_str(), FONT_CN, 36, ccp(-162, -38), ccp(0, 0.5));
        addLabelBMFont(this, levelLabel, toStr(player->level).c_str(), "bmfont/Sumo_22_white.fnt", ccp(235, -40), ccp(0.5, 0.5));
        levelLabel->setScale(contentScale());
        
        std::string reputationFont = player->pvpRank > 3 ? "bmfont/Sumo_22_yellow.fnt" : "bmfont/Sumo_red_22.fnt";
        addLabelBMFont(this, reputationLabel, toStr(player->pvpReputation).c_str(), reputationFont.c_str(), ccp(91, -100), ccp(0.5, 0.5));
        reputationLabel->setScale(contentScale());
        
        if(!getTitleOfRepu(player->pvpReputation).empty())
        {
            LabelX* title = LabelX::create(getTitleOfRepu(player->pvpReputation).c_str(), FONT_CN, 34, LabelX::FontEffect::SHADOW, ccp(-2, -2));
            title->setPosition(ccp(-115, -84));
            title->setAnchorPoint(ccp(0, 0.5));
            title->setColor(ccc3(218, 165, 32));
            addChild(title);
        }
        
        if(player->pvpRank > 3)
        {
            CCLabelBMFont* rankLabel;
            addLabelBMFont(this, rankLabel, toStr(player->pvpRank).c_str(), "bmfont/Sumo_38_blown.fnt", ccp(-150, -84), ccp(0.5, 0.5));
            rankLabel->setScale(contentScale());
        }
        
        teamBtn = ButtonSprite::create(createBtn("队伍", ButtonType::SMALL_RED),
                                                     createBtn("队伍", ButtonType::SMALL_RED));
        teamBtn->setPosition(ccp(215, -84));
        teamBtn->setOnClickListener([=](){
            ((PvPRankLayer*)this->getUserData())->alertTeamBoard(player);
        });
        addChild(teamBtn);
        
        setContentSize(CCSizeMake(winSize.width, 140));
        return true;
    }
    
    void RankItem::setTouchEnabled(bool value)
    {
        ContentLayer::setTouchEnabled(value);
        teamBtn->setTouchEnabled(value);
    }
    
    bool PvPRankLayer::init()
    {
        if(!ContentLayer::init())
            return false;
        
        scrollView = NULL;
        alertLayer = NULL;
        
        topMenu = CCMenu::create();
        CCMenuItem *itemUp = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                                      GetSprite("gameui/back_t.png"),
                                                      this,
                                                      menu_selector(PvPRankLayer::onBack));
        topMenu->addChild(itemUp);
        CCMenuItem *itemMiddle = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                          GetSprite("gameui/hongchengdi_t_tongyong.png"));
        LabelX* name = LabelX::create("擂台排名", FONT_CN, 36);
        name->setColor(ccWHITE);
        name->setAnchorPoint(ccp(0.5, 0.5));
        name->setPosition(ccp( itemMiddle->getContentSize().width*0.5f-2 , itemMiddle->getContentSize().height*0.5f+2));
        itemMiddle->addChild(name);
        
        topMenu->addChild(itemMiddle);
        itemUp->setAnchorPoint(ccp(0.5 , 1));
        itemMiddle->setAnchorPoint(ccp(0.5 , 1));
        topMenu->alignItemsHorizontallyWithPadding(8);
        topMenu->setPosition(ccp(-70 , 18));
        addChild(topMenu, 10);
        
        SystemInform* inform = SystemInform::alertLoading(GameScene::getInstance());
        PuzzleRPC::getInstance()->pvpTopRanks(PlayerData::getInstance()->getToken(),
                                              [=](std::auto_ptr<PlayerRankList> pList, std::auto_ptr<Error> e)
                                              {
                                                  inform->cancel();
                                                   if(e.get())
                                                   {
                                                       e->handleErrorInform();
                                                       return;
                                                   }
                                                  
                                                  playerList = pList;
                                                  makeItems(playerList->players);
                                                  
                                              });
        
        return true;
    }
    
    void PvPRankLayer::makeItems(const std::vector<Player*>& players)
    {
        if(scrollView)
        {
            this->removeChild(scrollView, true);
            scrollView = NULL;
            items.clear();
        }
        
        if(players.empty())
            return;
        
        scrollView = ZqlScrollView::create();
        
        for(Player* p : players)
        {
            RankItem* item = RankItem::create(p);
            //hack!make item can alert team juanzhou above pvpranklayer
            item->setUserData(this);
            items.push_back(item);
            scrollView->addLayer(item);
        }
        
        scrollView->setViewSize(CCSize(winSize.width , 800) ,  CCRect(0, -100, winSize.width, 600));
        scrollView->setPosition(ccp(0, 40));
        addChild(scrollView);
    }
    
    void PvPRankLayer::alertTeamBoard(Player* player)
    {
        setTouchEnabled(false);
        
        if(alertLayer)
            alertLayer->removeFromParentAndCleanup(true);
        
        alertLayer = CCLayer::create();
        addChild(alertLayer, 99);
        
        CCLayerColor* background = CCLayerColor::create(ccc4(0, 0, 0, 100));
        background->setPosition(ccp(-320,-800));
        alertLayer->addChild(background);
        
        PvPTeamConfirmJuanZhou* alertPanel = PvPTeamConfirmJuanZhou::create(player, false);
        alertPanel->setPositionY(-40);
        alertPanel->viewPort = CCSizeMake(winSize.width, 485);
        alertPanel->openDone();
        alertLayer->addChild(alertPanel, 100);
        
        CCSprite* zhou1 = GetSprite("gameui/pvp_1_1.png");
        zhou1->setAnchorPoint(ccp(0 , 1));
        zhou1->setPosition(ccp(-320 , -22));
        alertLayer->addChild(zhou1, 101);
        
        CCSprite* zhou2 = GetSprite("gameui/pvp_1_1.png");
        zhou2->setFlipX(true);
        zhou2->setAnchorPoint(ccp(1 , 1));
        zhou2->setPosition(ccp(320 , -22));
        alertLayer->addChild(zhou2, 101);
        
        CCSprite* teamSigh = GetSprite("gameui/pvp_chakan_duifang.png");
        teamSigh->setPosition(ccp(-200, -15));
        alertLayer->addChild(teamSigh);
        
        ButtonSprite* backBtn = ButtonSprite::create("gameui/pvp_chakan_guanbi.png",
                                                     "gameui/pvp_chakan_guanbi.png",
                                                     kCCMenuHandlerPriority, false);
        backBtn->setPosition(ccp(200, -15));
        backBtn->setOnClickListener([=](){
            this->cancelTeamBoard();
        });
        alertLayer->addChild(backBtn);
    }
    
    void PvPRankLayer::cancelTeamBoard()    
    {
        setTouchEnabled(true);
        if(alertLayer)
        {
            alertLayer->removeFromParentAndCleanup(true);
            alertLayer = NULL;
        }
    }
    
    
    void PvPRankLayer::setTouchEnabled(bool value)
    {
        ContentLayer::setTouchEnabled(value);
        topMenu->setTouchEnabled(value);
        if(scrollView)
            scrollView->setTouchEnabled(value);
        for(RankItem* item : items)
            item->setTouchEnabled(value);
    }
    
    void PvPRankLayer::onBack()
    {
        GameScene::getInstance()->popLayer();
    }
    
    //----------------------------------
    // PvPInformLayer
    //----------------------------------
    
    bool PvPInformLayer::init()
    {
        if(!ContentLayer::init())
            return false;
        
        createTitleMenu(this, "战况书", "", menu_selector(PvPStoreLayer::onBack));
        
        SystemInform* inform = SystemInform::alertLoading(runningScene());
        PuzzleRPC::getInstance()->pvpGameLog(PlayerData::getInstance()->getToken(), 0,
                                             [=](std::auto_ptr<PvPGameLogList> list, std::auto_ptr<Error> e)
        {
            inform->cancel();
            if(e.get())
            {
                e->handleErrorInform();
                return;
            }
            
            if(!list->logs.empty())
            {
                ZqlScrollView* scrollView = ZqlScrollView::create();
                scrollView->setViewSize(CCSize(winSize.width , 800) ,  CCRect(0, -100, winSize.width, 600));
                scrollView->setPosition(ccp(0, 40));
                
                for(const PvPGameLog& log : list->logs)
                {
                    this->makeLog(scrollView, log);
                }
                this->addChild(scrollView);
            }
            else
            {
                SystemInform::alertMessage(runningScene(), "当前没有战况书", "提示", [](){GameScene::getInstance()->popLayer();});
            }
            
        });
        
        return true;
    }
    
    void PvPInformLayer::makeLog(ZqlScrollView* scrollView, const PvPGameLog& log)
    {
        std::string msg = "";
        if(!log.rivalName.empty())
        {
            if(log.result == 1)
                msg = "你在擂台上击败了[color=0xffff4500]"+log.rivalName+"[/color]！表现优异！";
            else if(log.result == 0)
                msg = "你和"+log.rivalName+"过招30回合仍未分出胜负！最终以平局收场！";
            else
                msg = "很遗憾！你被[color=0xffff4500]"+log.rivalName+"[/color]击败了！";
            scrollView->addLayer(createItem(PVPLogLogo::LOGO_NULL, msg));
        }
        if(log.newRank > log.oldRank)
        {
            msg = "恭喜掌门！你的江湖排行提升到第[color=0xff20b2aa]"+toStr(log.newRank)+"[/color]名！";
            scrollView->addLayer(createItem(PVPLogLogo::LOGO_UP, msg));
            int rankStone[7] = {3, 10, 30, 50, 100, 200, 500};
            for(int rank : rankStone)
                if(log.newRank <= rank && log.oldRank > rank)
                {
                    msg = rank == 3 ? "恭喜掌门！你成为江湖榜第[color=0xff20b2aa]"+toStr(rank)+"[/color]名！"
                                    : "恭喜掌门！你进入江湖榜前[color=0xff20b2aa]"+toStr(rank)+"[/color]名！";
                    scrollView->addLayer(createItem(PVPLogLogo::LOGO_UP, msg));
                    break;
                }
        }
        else if(log.newRank < log.oldRank)
        {
            msg = "报告掌门！你的江湖排行下降到第[color=0xff20b2aa]"+toStr(log.newRank)+"[/color]名！";
            scrollView->addLayer(createItem(PVPLogLogo::LOGO_DOWN, msg));
        }
        
        if(log.newRep > log.oldRank)
        {
            int repStones[8] = {2500, 2200, 2000, 1800, 1600, 1400, 1200, 1000};
            for(int rep : repStones)
                if(log.newRep >= rep && log.oldRank < rep)
                {
                    msg = "恭喜掌门！江湖名望达到"+toStr(log.newRep)+"！获得"+getTitleOfRepu(log.newRep)+"称号！";
                    scrollView->addLayer(createItem(PVPLogLogo::LOGO_UP, msg));
                    break;
                }
        }
        
    }
    
    ContentLayer* PvPInformLayer::createItem(PVPLogLogo logoType, std::string msg)
    {
        ContentLayer* layer = ContentLayer::create();
        
        CCSprite* bg = GetSprite("gameui/pvp_zhankuangshu_bg.png");
        bg->setAnchorPoint(ccp(0.5, 1));
        layer->addChild(bg);
        
        std::string path = "gameui/pvp_zks.png";
        if(logoType == PVPLogLogo::LOGO_DOWN)
            path = "gameui/pvp_down.png";
        else if(logoType == PVPLogLogo::LOGO_UP)
            path = "gameui/pvp_up.png";
        
        CCSprite* logo = GetSprite(path.c_str());
        logo->setPosition(ccp(-230, -50));
        layer->addChild(logo);
        
        CCRichLabelTTF* label = CCRichLabelTTF::create(("[color=0xff001340]"+msg+"[/color]").c_str(),
                                                       FONT_CN, 24, CCSizeMake(440, 100),
                                                       CCTextAlignment::kCCTextAlignmentLeft);
        label->setPosition(ccp(-175, -15));
        label->setAnchorPoint(ccp(0, 1));
        layer->addChild(label);
        
        layer->setContentSize(CCSizeMake(640, 115));
        return layer;
    }
    
    void PvPInformLayer::onBack()
    {
        GameScene::getInstance()->popLayer();
    }
    
    //----------------------------------
    // PvPStoreLayer
    //----------------------------------
    
    bool PvPStoreLayer::init()
    {
        if(!ContentLayer::init())
            return false;
        
        createTitleMenu(this, "通宝行", "", menu_selector(PvPStoreLayer::onBack));
        
        CCSprite* currBar = GetSprite("gameui/pvp_jifen.png");
        currBar->setPosition(ccp(0, -75));
        addChild(currBar, 10);
        
        addLabelBMFont(currBar, scoreLabel, "", "bmfont/Sumo_32_orange.fnt",
                       ccp(530, currBar->getContentSize().height*0.5f-4), ccp(1, 0.5));
        scoreLabel->setScale(contentScale());
        
        updateBar();
        if(PlayerData::getInstance()->pvpStoreItems.size() > 0)
            makeItems();
        else
            SystemInform::alertMessage(runningScene(), "当前没有商品!", "提示",
                                       [](){GameScene::getInstance()->popLayer();});
        return true;
    }

    void PvPStoreLayer::updateBar()
    {
        scoreLabel->setString(toStr(PlayerData::getInstance()->player->pvpScore).c_str());
    }
    
    void PvPStoreLayer::makeItems()
    {
        ZqlScrollView* scrollView = ZqlScrollView::create();
        
        
        for(const PvPStoreItem& item : PlayerData::getInstance()->pvpStoreItems)
        {
            scrollView->addLayer(createAItem(item));
        }
        scrollView->setViewSize(CCSize(winSize.width , 800) ,  CCRect(0, -30, winSize.width, 552));
        scrollView->setPosition(ccp(0, -80));
        addChild(scrollView);
    }
    
    std::string PvPStoreLayer::getIconPath(const PvPStoreItemType& type)
    {
        if(type == PvPStoreItemType::SkillPill)
            return HPT(7213).iconPath();
        else if(type == PvPStoreItemType::RelationPill)
            return HPT(7214).iconPath();
        return "";
    }
    
    ContentLayer* PvPStoreLayer::createAItem(const PvPStoreItem& item)
    {
        ContentLayer* layer = ContentLayer::create();
        
        CCSprite* bg = GetSprite("gameui/pvp_tongbao_bg.png");
        bg->setAnchorPoint(ccp(0.5, 1));
        bg->setPosition(ccp(0, 0));
        layer->addChild(bg);
        
        LabelX* name = LabelX::create(item.name.c_str(), FONT_CN, 28);
        name->setPosition(ccp(-268, -26));
        name->setAnchorPoint(ccp(0, 0.5));
        layer->addChild(name);
        
        if(!getIconPath(item.itemType).empty())
        {
            CCSprite* logo = GetSprite(getIconPath(item.itemType));
            logo->setPosition(ccp(-220, -100));
            layer->addChild(logo);
        }
        addLabelBMFont(layer, toStr(item.price).c_str(),  "bmfont/Sumo_22_yellow.fnt", ccp(110, -60))->setScale(contentScale());
        CCLabelTTF* des = addLabelTTF(layer, item.desc.c_str(), FONT_CN, 22, ccp(-170, -95), ccp(0, 1), ccc3(0x00, 0x13, 0x40));
        des->setHorizontalAlignment(kCCTextAlignmentLeft);
        des->setDimensions(CCSizeMake(425, 72));
        
        ButtonSprite* btn = ButtonSprite::create(createBtn("购买", ButtonType::SMALL_RED));
        btn->setPosition(ccp(225, -70));
        btn->setOnClickListener([=](){
            if(item.price > PlayerData::getInstance()->player->pvpScore)
            {
                SystemInform::alertMessage(runningScene(), "你的通宝不足！", "提示", [](){});
                return;
            }
            SystemInform* inform = SystemInform::alertLoading(runningScene());
            PuzzleRPC::getInstance()->purchasePvPItem(PlayerData::getInstance()->getToken(), item.sid,
                                                      [=](std::auto_ptr<Player> p, std::auto_ptr<Error> e){
                                                          inform->cancel();
                                                          if(e.get())
                                                          {
                                                              e->handleErrorInform();
                                                              return;
                                                          }
                                                          
                                                          PlayerData::getInstance()->updatePlayer(p.get());
                                                          updateBar();
                                                          SystemInform::alertMessage(runningScene(), "购买成功", "提示", [](){});
                                                      });
        });
        layer->addChild(btn);
        
        layer->setContentSize(CCSizeMake(bg->getContentSize().width, bg->getContentSize().height+10));
        return layer;
    }
    
    void PvPStoreLayer::onBack()
    {
        GameScene::getInstance()->popLayer();
    }
    
    //----------------------------------
    // PvPLayer
    //----------------------------------
    bool PvPLayer::init()
    {
        if(!ContentLayer::init())
            return false;
        
        t = 0;
        enterBtnPath = "";
        PvPMsgBoard* board = PvPMsgBoard::create();
        board->setPosition(ccp(0,-30));
        addChild(board);
        
        enterBtn = ButtonSprite::create("gameui/pvp_gu.png", "gameui/pvp_gu.png");
        enterBtn->setPosition(ccp(0,-325));
        addChild(enterBtn);
        
        leitaiLabel = addLabelBMFont(this, "", "bmfont/PW_24_red.fnt",
                                     ccp(0, -250), ccp(0.5, 0.5));
        leitaiLabel->setScale(contentScale());
        
        {
            CCSprite* zhantie = GetSprite("gameui/pvp_zhantie.png");
            zhantie->setPosition(ccp(0, -500));
            addChild(zhantie);
            
            CCLabelBMFont* keyLabel;
            addLabelBMFont(zhantie, keyLabel, (toStr(PlayerData::getInstance()->getPlayer()->pvpKeys)+"/10").c_str(),
                           "bmfont/Sumo_38_orange.fnt",
                           ccp(140, 35),
                           ccp(0.5, 0.5));
            keyLabel->setScale(contentScale());
        }
        
        {
            ButtonSprite* detailBtn = ButtonSprite::create("gameui/zcxq_anniu.png");
            detailBtn->setPosition(ccp(280, -500));
            detailBtn->setOnClickListener([=](){SystemInform::alertPvPMsg(runningScene());});
            addChild(detailBtn);
        }

                             
        CCMenu* menu = CCMenu::create();
        CCMenuItem* rankItem = CCMenuItemSprite::create(GetSprite("gameui/pvp_jianghubang.png"),
                                                        GetSprite("gameui/pvp_jianghubang.png"),
                                                        this,
                                                        menu_selector(PvPLayer::onRank));
        menu->addChild(rankItem);
        CCMenuItem* tongbaoItem = CCMenuItemSprite::create(GetSprite("gameui/pvp_tongbaohang.png"),
                                                           GetSprite("gameui/pvp_tongbaohang.png"),
                                                           this,
                                                           menu_selector(PvPLayer::onStore));
        menu->addChild(tongbaoItem);
        CCMenuItem* msgItem = CCMenuItemSprite::create(GetSprite("gameui/pvp_zhankuangshu.png"),
                                                       GetSprite("gameui/pvp_zhankuangshu.png"),
                                                       this,
                                                       menu_selector(PvPLayer::onZhanTie));
        menu->addChild(msgItem);
        
        menu->alignItemsHorizontallyWithPadding(5);
        menu->setPosition(ccp(0,-600));
        addChild(menu);
        
        schedule1Sec();
        schedule(schedule_selector(PvPLayer::schedule1Sec), 1);
        return true;
    }
    
    void PvPLayer::schedule1Sec()
    {
        using namespace boost::date_time;
        using namespace boost::posix_time;
        ptime startTime = PlayerData::getInstance()->pvpStartTime;
        ptime endTime = PlayerData::getInstance()->pvpEndTime;
        ptime now = boost::posix_time::second_clock::universal_time() +
                    PlayerData::getInstance()->delta_local_server;
        auto toStart = startTime - now;
        auto toEnd = endTime - now;
        
        if(!PH_PVP_ALWAYS_ENABLED && !toStart.is_negative())
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
            
            if(enterBtnPath != "gameui/pvp_gu_hui.png")
            {
                enterBtnPath = "gameui/pvp_gu_hui.png";
                enterBtn->setSprite(GetSprite(enterBtnPath.c_str()));
                enterBtn->setOnClickListener([](){});
            }
        }
        else if(!PH_PVP_ALWAYS_ENABLED && toEnd.is_negative())
        {
            leitaiLabel->setString("今日已结束");
            if(enterBtnPath != "gameui/pvp_gu_hui.png")
            {
                enterBtnPath = "gameui/pvp_gu_hui.png";
                enterBtn->setSprite(GetSprite(enterBtnPath.c_str()));
                enterBtn->setOnClickListener([](){});
            }
        }
        else
        {
            leitaiLabel->setString("");
            if(enterBtnPath != "gameui/pvp_gu.png")
            {
                enterBtnPath = "gameui/pvp_gu.png";
                enterBtn->setSprite(GetSprite(enterBtnPath.c_str()));
                enterBtn->setOnClickListener([=]()
                     {
                         if(PH_PVP_ALWAYS_ENABLED || PlayerData::getInstance()->getPlayer()->pvpKeys > 0)
                             GameScene::getInstance()->pushLayerWithDefaultAnimation(PvPTeamConfirmLayer::create());
                         else
                             SystemInform::alertMessage(runningScene(), "你的战贴不足！", "提示 ", [](){});
                     });
            }
        }
        
        t++;
        
    }
    
    void PvPLayer::onRank(cocos2d::CCObject *target)
    {
        GameScene::getInstance()->pushLayerWithDefaultAnimation(PvPRankLayer::create());
    }
    
    void PvPLayer::onStore(cocos2d::CCObject *target)
    {
        GameScene::getInstance()->pushLayerWithDefaultAnimation(PvPStoreLayer::create());
    }
    void PvPLayer::onZhanTie(cocos2d::CCObject *target)
    {
        GameScene::getInstance()->pushLayerWithDefaultAnimation(PvPInformLayer::create());
    }
}