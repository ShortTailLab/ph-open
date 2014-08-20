#include "XiaKe.h"
#include <cocos2d.h>
#include "ZqlScrollView.h"
#include "ZhuanSheng.h"
#include <algorithm>
#include "QiangHua.h"
#include "Sale.h"
#include "network/PuzzleRPC.h"
#include "GameScene.h"
#include "UIPresets.h"
#include "HeroIcon.h"
#include "HeroProfileLayer.h"
#include "PageScrollView.h"
#include "WashRelation.h"
#include "ButtonSprite.h"
#include "PvPUtils.h"
#include "Common.h"
#include "Achievement.h"
#include "CCCallLambda.h"

#include <boost/unordered_set.hpp>

namespace PH
{
    USING_NS_CC;
    
    ////////////////////////////////////////////////
    //BianDuiJuanZhou
    ////////////////////////////////////////////////
    
    bool BianDuiJuanZhou::init(const std::vector<PH::Hero *>& heroVec2)
    {
        if(!JuanZhou::init())
            return false;
        
        isUpdate = false;
        team = heroVec2;
        viewPort = CCSize(20 , 280);
        
        background = GetSprite("gameui/biandui_1_3.png");
        background->setAnchorPoint(ccp(0.5 , 1));
        addChild(background);
        
        heroVec = std::vector<Hero*>(5, (Hero*)NULL);
        heroImgVec = std::vector<HeroIcon*>(5, (HeroIcon*)NULL);
        
        for(int i = 0 ; i < heroNum ; i++)
        {
            if(i == 0)
                locations.push_back(CCRect(-272 , -168 , 100 , 100 ));
            else
                locations.push_back(CCRect(-257+108*i , -168 , 100 , 100 ));
        }
        
        for(int j = 0 ; j < heroNum ; j++)
        {
            if(heroVec2[j] != NULL){
                addHero(heroVec2[j], j, false);
            }
        }
        
        addLabelBMFont(this, hpLabel, "0", "bmfont/Sumo_22_yellow.fnt", ccp(-203, -189));
        hpLabel->setScale(contentScale());
        
        addLabelBMFont(this, attackLabel, "0", "bmfont/Sumo_22_yellow.fnt", ccp(-65, -189));
        attackLabel->setScale(contentScale());
        
        addLabelBMFont(this, regenLabel, "0", "bmfont/Sumo_22_yellow.fnt", ccp(65, -189));
        regenLabel->setScale(contentScale());
        
        addLabelBMFont(this, lpLabel, "0", "bmfont/Sumo_22_yellow.fnt", ccp(213, -189), ccp(1, 1));
        lpLabel->setScale(contentScale());
        
        addLabelBMFont(this, lpMaxLabel, "0", "bmfont/Sumo_22_yellow.fnt", ccp(213, -189), ccp(0, 1));
        lpMaxLabel->setScale(contentScale());
        
        for(int i = 0; i < 5; i++)
        {
            CCLabelBMFont* label;
            addLabelBMFont(this, label, "0", "bmfont/Sumo_22_yellow.fnt", ccp(-208+110*i, -225));
            label->setScale(contentScale());
            resistLabels.push_back(label);
        }
        
        updateLabel();
        
        switcher = BianDuiSwitcher::create();
        switcher->setPosition(ccp(-275, -42));
        switcher->setCallBack([=](int i){
            if(PlayerData::getInstance()->getActiveTeam() != i)
                ((BianDuiLayer*)this->getUserData())->switchActiveTeam(i);
        });
        addChild(switcher);
        
        return true;
    }
    
    void BianDuiJuanZhou::clear()
    {
        JuanZhou::clear();
        updateLabel();
    }
    
    void BianDuiJuanZhou::updateLabel()
    {
        PlayerData *data = PlayerData::getInstance();
        
        int hp = data->teamData.hp;
        int attack = data->teamData.attack[GemUtils::AllColor];
        int regen = data->teamData.regen;
        int lp = data->teamData.lp;
        int maxLp = PlayerData::getInstance()->getPlayer()->maxLP;
        
        hpLabel->setString( toStr(hp).c_str() );
        attackLabel->setString( toStr(attack).c_str() );
        regenLabel->setString( toStr(regen).c_str() );
        lpLabel->setFntFile(lp<=maxLp?"bmfont/Sumo_22_yellow.fnt":"bmfont/Sumo_22_red.fnt");
        lpLabel->setString((toStr(lp).c_str()));
        lpMaxLabel->setString(("/"+toStr(maxLp)).c_str());
        
        std::vector<float> resists = calcResistance(data->getTeam());
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
    
    void BianDuiJuanZhou::updateData()
    {
        PlayerData::getInstance()->updateTeam(heroVec);
        updateLabel();
    }
    
    
	////////////////////////////////////////////////
	//XiangQing
	////////////////////////////////////////////////
    
    
    XiangQingJuanZhou::XiangQingJuanZhou() :
	relations(0), relationBoard(0), rName(0), rHeroes(0),
	rEffect(0), tempRItem(0), lpLabel(0), hpLabel(0),
	regenLabel(0), redAttackLabel(0), blueAttackLabel(0),
	greenAttackLabel(0), purpleAttackLabel(0), yellowAttackLabel(0),
	tempIcon(0) {}
    
    bool XiangQingJuanZhou::init(std::vector<Hero*>* team)
    {
        if(! JuanZhou::init())
            return false;
        
        viewPort = CCSize(20 , 722);
        tempRItem = NULL;
        tempIcon = NULL;
        isShowNormalSkill = true;
        relationsView = NULL;
        if(team)
            for(Hero* h : *team)
                targetTeam.push_back(h);
        
        CCSprite *bg = GetSprite("gameui/biandui_2_1_3.png");
        bg->setAnchorPoint(ccp(0.5 , 1));
        this->addChild(bg);
        
        BianDuiSwitcher* switcher = BianDuiSwitcher::create(team);
        switcher->setPosition(ccp(-275, -44));
        if(!team)
            switcher->setCallBack([=](int i){
                if(PlayerData::getInstance()->getActiveTeam() != i)
                {
                    PlayerData::getInstance()->setActiveTeam(i);
                    this->refresh();
                    this->initHeroRelations();
                }
            });
        else
            switcher->setTouchEnabled(false);
        addChild(switcher);
        
        heroVec = std::vector<Hero*>(5, (Hero*)NULL);
        heroImgVec = std::vector<HeroIcon*>(5, (HeroIcon*)NULL);
        
        for(int i = 0 ; i < 5 ; i++)
        {
            if(i == 0)
            {
                locations.push_back(CCRect(-274 , -214 , 100 , 100 ));
            }
            else
            {
                locations.push_back(CCRect(-258+108*i , -214 , 100 , 100 ));
            }
        }
        
        addLabelBMFont(this, lpLabel, "",
                       "bmfont/Sumo_22_yellow.fnt", ccp(-165 , -314));
        lpLabel->setScale(contentScale());
        addLabelBMFont(this, hpLabel, "", "bmfont/Sumo_22_yellow.fnt", ccp(35,-314));
        hpLabel->setScale(contentScale());
        addLabelBMFont(this, fightLabel, "", "bmfont/Sumo_22_yellow.fnt", ccp(212,-314));
        fightLabel->setScale(contentScale());
        addLabelBMFont(this, regenLabel, "", "bmfont/Sumo_22_yellow.fnt", ccp(-65,-352));
        regenLabel->setScale(contentScale());
        addLabelBMFont(this, redAttackLabel, "",
                       "bmfont/Sumo_22_yellow.fnt", ccp(212 , -352));
        redAttackLabel->setScale(contentScale());
        addLabelBMFont(this, blueAttackLabel, "",
                       "bmfont/Sumo_22_yellow.fnt", ccp(-65 , -390));
        blueAttackLabel->setScale(contentScale());
        addLabelBMFont(this, greenAttackLabel, "",
                       "bmfont/Sumo_22_yellow.fnt", ccp(212 , -390));
        greenAttackLabel->setScale(contentScale());
        addLabelBMFont(this, purpleAttackLabel, "",
                       "bmfont/Sumo_22_yellow.fnt", ccp(-65 , -429));
        purpleAttackLabel->setScale(contentScale());
        addLabelBMFont(this, yellowAttackLabel, "",
                       "bmfont/Sumo_22_yellow.fnt", ccp(212 , -429));
        yellowAttackLabel->setScale(contentScale());
        
        for(int i = 0; i < 5; i++)
        {
            CCLabelBMFont* label;
            addLabelBMFont(this, label, "0", "bmfont/Sumo_22_yellow.fnt", ccp(-208+110*i, -505));
            label->setScale(contentScale());
            resistLabels.push_back(label);
        }
        
        {
            bool pvpOpen = PlayerData::getInstance()->isPvPOpen;
            ButtonSprite* switchSkillBtn = ButtonSprite::create("gameui/jianghuji_d.png", "gameui/leitaiji_d.png",
                                                                kCCMenuHandlerPriority, pvpOpen);
            switchSkillBtn->setPosition(ccp(223, -563));
            switchSkillBtn->setOnClickListener([=](){
                this->switchSkillType();
                this->isShowNormalSkill ? switchSkillBtn->onRelease() : switchSkillBtn->onPressed();
            });
            switchSkillBtn->setTouchEnabled(pvpOpen);
            addChild(switchSkillBtn);
        }
        addLabelTTF(this, skillLabel, "", FONT_CN, 28, ccp(-155 , -567), ccp(0, 0.5), ccc3(108, 46, 0));
        
        descLabel = LabelX::create("", FONT_CN, 28, LabelX::FontEffect::SHADOW, ccp(2, -2));
        descLabel->setPosition(ccp(-253, -605));
        descLabel->setAnchorPoint(ccp(0, 0.5));
        this->addChild(descLabel);
        
        refresh();
        
        return true;
    }
    
    bool XiangQingJuanZhou::addHero(PH::Hero *hero , int id ,bool isUpdate)
    {
        JuanZhou::addHeroImg(HeroIcon::create(hero , false , true) , id , isUpdate);
        
        CCSprite *icon = getImgById(id);
        CCLabelBMFont *lpNum;
        addLabelBMFont(icon, lpNum, toStr(hero->profile->LP).c_str(), "bmfont/Sumo_20_white.fnt" , ccp(75, -32), ccp(0.5, 0.5));
        lpNum->setScale(contentScale());
        return true;
    }
    
    void XiangQingJuanZhou::openDone()
    {
        initHeroRelations();
        setTouchEnabled(true);
    }
    
    void XiangQingJuanZhou::initHeroRelations()
    {
        if(relationsView)
        {
            relationsView->removeFromParentAndCleanup(true);
            relationsView = NULL;
        }
        
        std::vector<HeroRelation> items = calcRelationsInTeam(targetTeam.empty() ?
                                                              PlayerData::getInstance()->getTeam() : targetTeam);
        if(!items.empty())
        {
            relationsView = ScrollRelations::create(items, CCRectMake(-85, 0, 442, 68));
            relationsView->setPosition(ccp(0, -695));
            addChild(relationsView);
        }
    }
    
    void XiangQingJuanZhou::refresh()
    {
        std::vector<Hero*> team = targetTeam.empty() ? PlayerData::getInstance()->getTeam() : targetTeam;
        clear();
        for (int i = 0; i < team.size(); i++)
            if(team[i])
                addHero(team[i], i, false);
        
        teamData.update(team);
        std::string lpStr = !targetTeam.empty() ? "" : "/"+toStr(PlayerData::getInstance()->player->maxLP);
        lpLabel->setString((toStr(teamData.lp)+lpStr).c_str());
        regenLabel->setString(toStr(teamData.regen).c_str());
        hpLabel->setString(toStr(teamData.hp).c_str());

        fightLabel->setString(toStr(teamData.fightSkill).c_str());
        redAttackLabel->setString(toStr(teamData.attack[GemUtils::Fire]).c_str());
        blueAttackLabel->setString(toStr(teamData.attack[GemUtils::Water]).c_str());
        greenAttackLabel->setString(toStr(teamData.attack[GemUtils::Wood]).c_str());
        purpleAttackLabel->setString(toStr(teamData.attack[GemUtils::Dark]).c_str());
        yellowAttackLabel->setString(toStr(teamData.attack[GemUtils::Light]).c_str());
        
        std::vector<float> resists = calcResistance(team);
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
            resistLabels[i]->setString(toStr(int(resists[color]*100+100)).c_str());
        }
        
        updateSkillType();
        
    }
    
    void XiangQingJuanZhou::updateSkillType()
    {
        std::string skillName;
        std::string skillDesc;
        if(isShowNormalSkill)
        {
            skillName = teamData.skill ? teamData.skill->name : "无技能";
            skillDesc = teamData.skill ? teamData.skill->desc : "无技能";
        }
        else
        {
            skillName = teamData.battleSkill ? teamData.battleSkill->name : "无技能";
            skillDesc = teamData.battleSkill ? teamData.battleSkill->desc : "无技能";
        }
        
        if(!heroVec[0])
            skillName = skillDesc = " ";
            
        skillLabel->setString(skillName.c_str());
        descLabel->setString(skillDesc.c_str());
    }
    
    void XiangQingJuanZhou::switchSkillType()
    {
        isShowNormalSkill = !isShowNormalSkill;
        
        updateSkillType();
    }
    
    void XiangQingJuanZhou::onSingleClick(CCTouch* touch)
    {
        //check if touch on the heroIcon
        for (HeroIcon *icon : heroImgVec)
        {
            if(!icon)continue;
            CCPoint localPoint = icon->getParent()->convertTouchToNodeSpace(touch);
            if(icon->boundingBox().containsPoint(localPoint))
            {
                auto* layer = HeroProfileLayer::create(icon->getHero(), false);
                GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
                break;
            }
        }
    }
    
    void XiangQingJuanZhou::onLongClick(cocos2d::CCTouch *touch)
    {
        //check if touch on the heroIcon
        for (HeroIcon *icon : heroImgVec)
        {
            if(!icon)continue;
            CCPoint localPoint = icon->getParent()->convertTouchToNodeSpace(touch);
            if(icon->boundingBox().containsPoint(localPoint))
            {
                auto* layer = HeroProfileLayer::create(icon->getHero(), false);
                GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
                break;
            }
        }
    }
    
    //--------------------
    //
    bool XiangQingLayer::init(std::vector<Hero*>* team)
    {
        if(!ContentLayer::init()){
            return false;
        }
        
        isShowTop = false;
        
        CCMenu *topMenu = CCMenu::create();
        CCMenuItem *itemLeft = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"), GetSprite("gameui/back_t.png"), this , menu_selector(XiangQingLayer::onBackClick));
        itemLeft->setAnchorPoint(ccp(0.5,1));
        topMenu->addChild(itemLeft );
        CCMenuItem *itemMiddle = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png") , GetSprite("gameui/hongchengdi_t_tongyong.png"));
        itemMiddle->setAnchorPoint(ccp(0.5,1));
        topMenu->addChild(itemMiddle );
        
        CCPoint pos(-70, -20);
        if(isSharedAvailable() && !team)
        {
            pos = ccp(0, -20);
            CCMenuItem *itemRight = CCMenuItemSprite::create(createBtn("分享", ButtonType::NORMAL_BLUE, TITLE_YELLOW),
                                                             createBtn("分享", ButtonType::NORMAL_BLUE, TITLE_YELLOW),
                                                             this,
                                                             menu_selector(XiangQingLayer::onShare));
            itemRight->setAnchorPoint(ccp(0.5 , 1));
            topMenu->addChild(itemRight);
        }

        
        topMenu->alignItemsHorizontallyWithPadding(8);
        topMenu->setPosition(pos);
        this->addChild(topMenu, 1);
        
        LabelX* name = LabelX::create("编队详情", FONT_CN, 36);
        name->setColor(ccWHITE);
        name->setAnchorPoint(ccp(0.5, 0.5));
        name->setPosition(ccp( itemMiddle->getContentSize().width*0.5f+2 , itemMiddle->getContentSize().height*0.5f));
        itemMiddle->addChild(name);
        
        
        juanzhou = XiangQingJuanZhou::create(team);
        zhou1 = GetSprite("gameui/biandui_2_1_1.png");
        zhou2 = GetSprite("gameui/biandui_2_1_1.png");
        zhou2->setFlipX(true);
        
        jz = JuanZhouLayer::create();
        jz->addSkin(juanzhou , zhou1 , zhou2);
        jz->setPositionY(-68);
        this->addChild(jz);
        
        return true;
    }
    
    void XiangQingLayer::onEnterAnimationDone()
    {
        jz->startAnim();
    }
    
    void XiangQingLayer::onBackClick(cocos2d::CCObject *target)
    {
        GameScene::getInstance()->popLayer();
        ring("sound/v/button_click.mp3");
    }
    
    void XiangQingLayer::onTitleClick(cocos2d::CCObject *target)
    {
        
    }
    
    void XiangQingLayer::onShare(cocos2d::CCObject *target)
    {
        SystemInform::alertSharePanel(runningScene(),
                                      toStr("小伙伴们快来加我好友，这是我在@群侠转 的强力队伍，加我当援军，刷副本绝对杠杠的！")+                                       "https://itunes.apple.com/cn/app/qun-xia-zhuan/id669428248");
    }
    
    
    /////////////////////////////////////////////////////////////////////
    //BianDuiLayer
    //(0.5,1)
    /////////////////////////////////////////////////////////////////////
    
    bool BianDuiBoard::init(const std::vector<PH::Hero *>& heroVec)
    {
        if(!HeroBoard::init(heroVec, false))
            return false;
        
        
        return true;
    }
    
    void BianDuiBoard::refresh(const std::vector<PH::Hero *>& heroVec)
    {
        HeroBoard::refresh(heroVec);
        
        //the new hero msg will be cleared when biandui was pop
        for(Hero* hero :  PlayerData::getInstance()->getNewHeroes("biandui"))
            getHeroImg(hero)->showNew();
    }
    
    bool BianDuiLayer::init()
    {
        if(!ContentLayer::init())
            return false;
        
        curTouchIcon = NULL;
        isShowTop = false;
        
        isClick = false;
        
        ContentLayer::setTouchEnabled(true);
        
        PlayerData* data = PlayerData::getInstance();
        
        board = BianDuiBoard::create(data->getStorage());
        board->setClip(0, 600);

        scrollView = ZqlScrollView::create();
        scrollView->addLayer(board);
        scrollView->setPositionY(-350);
        scrollView->setViewSize(CCSize(winSize.width, 600),
                                CCRect(winSize.width, -20, winSize.width, 440));
        this->addChild(scrollView); 
        
        topMenu = CCMenu::create();
        
        {
            backButton = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                                  GetSprite("gameui/back_t.png"),
                                                  this,
                                                  menu_selector(BianDuiLayer::onBackClick));
            backButton->setAnchorPoint(ccp(0.5 , 1));
            topMenu->addChild(backButton);
        }
        
        {
            sortButton = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                  GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                  this,
                                                  menu_selector(BianDuiLayer::onBianduiClick));
            sortButton->setAnchorPoint(ccp(0.5 , 1));
            
            LabelX* name = LabelX::create("编队", FONT_CN, 36);
            name->setColor(ccWHITE);
            name->setAnchorPoint(ccp(0.5, 0.5));
            name->setPosition(ccp(90 , sortButton->getContentSize().height*0.5f));
            sortButton->addChild(name);
            
            title = LabelX::create(PlayerData::getInstance()->getSortTypeMsg() , FONT_CN , 20, LabelX::FontEffect::SHADOW, ccp(2, -2));
            title->setColor(ccWHITE);
            title->setPosition(ccp( 170 , sortButton->getContentSize().height*0.5f));
            title->setAnchorPoint(ccp(0, 0.5));
            sortButton->addChild(title);
            
            CCSprite *xian = GetSprite("gameui/hongchengdi_huang_tongyong.png");
            xian->setPosition(ccp(160 , sortButton->getContentSize().height*0.5f));
            sortButton->addChild(xian);
            
            topMenu->addChild(sortButton);
        }
        
        {
            detailButton = CCMenuItemSprite::create(GetSprite("gameui/chengdi_t_tongyong_l.png"),
                                                    GetSprite("gameui/chengdi_t_tongyong_l.png"),
                                                    this,
                                                    menu_selector(BianDuiLayer::onDetailClick));
            detailButton->setAnchorPoint(ccp(0.5 , 1));
            LabelX* detail_label = LabelX::create("详情", FONT_CN, 34);
            detail_label->setPosition(ccp(detailButton->getContentSize().width*0.5, detailButton->getContentSize().height*0.5f));
            detail_label->setColor(TITLE_YELLOW);
            detailButton->addChild(detail_label);
            
            topMenu->addChild(detailButton);
            topMenu->alignItemsHorizontallyWithPadding(8);
            topMenu->setPosition(ccp(0, -20));
            addChild(topMenu, 1);
        }
        
        
        
        juanzhou = BianDuiJuanZhou::create(data->getTeam());
        juanzhou->setUserData(this);
        CCSprite* zhou1 = GetSprite("gameui/biandui_1_1.png");
        CCSprite* zhou2 = GetSprite("gameui/biandui_1_1.png");
        zhou2->setFlipX(true);
        
        jz = JuanZhouLayer::create();
        jz->addSkin(juanzhou, zhou1 ,zhou2);
        jz->setPositionY(-68);
        this->addChild(jz);
        
        disableOverLPHeroes();
        
        PlayerData::getInstance()->registerStorageCallBack(this , [=]()
        {
            PlayerData *data = PlayerData::getInstance();
            board->refresh(data->getStorage());
            juanzhou->clear();
            
            std::vector<Hero*> team = data->getTeam();
            
            for(int k = 0 ; k < team.size() ; k++)
            {
                if(team[k])
                    juanzhou->addHero(team[k] , k);
            }
        });
        
        // Tutorial steps
        tutInit = [=]()
        {
            ArrowMask::shared()->clear();
            
            // disable hero icons on juanzhou
            for(CCSprite* hs : juanzhou->heroImgVec)
            {
                if(hs)
                {
                    HeroIcon* hh = (HeroIcon*)hs;
                    hh->setEnabled(false);
                    hh->setDark(false);
                }
            }
            
            // disable touch for all hero icons
            for(Hero* h : PlayerData::getInstance()->getStorage())
            {
                HeroIcon* hh = board->getHeroImg(h);
                hh->setEnabled(false);
            }
            
            ArrowMask::shared()->setMask(CCRect(0, 140, 640, 960));
            
            sortButton->setEnabled(false);
            backButton->setEnabled(false);
            detailButton->setEnabled(false);
        };
        
        tutGroupFirstHero = [=]()
        {
            // find the first dark hero.
            Hero* hero = *(PlayerData::getInstance()->player->storage.rbegin());
            
            // enable touch only for the targeted hero
            HeroIcon* iconInBianDui = board->getHeroImg(hero);
            iconInBianDui->setEnabled(true);
            
            // point arrow at targeted hero
            CCPoint p = inWorldSpace(iconInBianDui);
            ArrowMask::shared()->setArrow(ccp(p.x, p.y - 80), ArrowDirection::Up);
        };
        
        tutShowBackButton = [=]()
        {
            backButton->setEnabled(true);
            CCPoint p = backButton->getParent()->convertToWorldSpace(backButton->getPosition());
            ArrowMask::shared()->setArrow(ccp(p.x, p.y-90), ArrowDirection::Up);
            
            for(const Hero* h : PlayerData::getInstance()->player->storage)
            {
                HeroIcon* hicon = board->getHeroImg(h);
                hicon->setEnabled(false);
            }
            GameConfig::shared()->setTutorialProgress(TutorialProfileDone);
        };
        
        return true;
    }
    
    void BianDuiLayer::onEnter()
    {
        ContentLayer::onEnter();
        
        board->refresh(PlayerData::getInstance()->getStorage());
        disableOverLPHeroes();
        if(idsOnEnter.size() == 0)
        {
            idsOnEnter = PlayerData::getInstance()->getTeamIds();
            activeTeamOnEnter = PlayerData::getInstance()->getActiveTeam();
        }
        
        int activeTeam = PlayerData::getInstance()->getActiveTeam();
        if(((BianDuiJuanZhou*)juanzhou)->switcher->getCurNode() != activeTeam)
        {
            ((BianDuiJuanZhou*)juanzhou)->switcher->setCurNode(activeTeam);
            switchActiveTeam(activeTeam);
        }
        
        if(GameConfig::shared()->getTutorialProgress() == TutorialTeam)
        {
            tutInit();
        }
        else if(GameConfig::shared()->getTutorialProgress() == TutorialProfileDone)
        {
            tutInit();
            tutShowBackButton();
        }

    }
    
    void BianDuiLayer::onEnterAnimationDone()
    {
        ContentLayer::onEnterAnimationDone();
        jz->startAnim();
        
        if(GameConfig::shared()->getTutorialProgress() == TutorialTeam)
        {
            tutGroupFirstHero();
        }
    }
    
    void BianDuiLayer::onExit()
    {
        ContentLayer::onExit();
        
        if(GameConfig::shared()->getTutorialProgress() != TutorialSkip)
        {
            ArrowMask::shared()->clear();
        }
    }
    
    void BianDuiLayer::cleanup()
    {
        CCNode::cleanup();
        //hack,for that it must be called when it is poped
        PlayerData::getInstance()->unRegisterStorageCallBack(this);
        PlayerData::getInstance()->clearNewHeroes("biandui");
    }
    
    bool BianDuiLayer::tryExit(int msg)
    {
        PlayerData* data = PlayerData::getInstance();
        
        std::vector<std::string> teamIds = data->getTeamIds();
        
        bool isEmptyHead = false;
        for(int i = 0; i < 25; i+=5)
            if(!data->player->team[i])
            {
                for(int j = i+1; j < i+5; j++)
                    if(data->player->team[j])
                    {
                        isEmptyHead = true;
                        break;
                    }
                if(isEmptyHead)
                    break;
            }
                
        if(!data->getTeam()[0] || isEmptyHead)
        {
            SystemInform::alertMessage(GameScene::getInstance(),
                                       "掌门不能为空！",
                                       "警告" ,
                                       [](){});
            return false;
        }
        
        bool teamUnchanged = true;
        for(int i=0; i<teamIds.size(); ++i)
        {
            if(teamIds[i] != idsOnEnter[i])
            {
                teamUnchanged = false;
                break;
            }
        }
        int cur = data->getActiveTeam();
        
        if(teamUnchanged && activeTeamOnEnter == cur)
            return true ;
        
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
            idsOnEnter = PlayerData::getInstance()->getTeamIds();
            activeTeamOnEnter = PlayerData::getInstance()->getActiveTeam();
            
            // tutorial stuff
            if(GameConfig::shared()->getTutorialProgress() == TutorialProfileDone)
            {
                GameConfig::shared()->setTutorialProgress(TutorialQiangHua);
                GameScene::getInstance()->selectTab(2);
                return;
            }
            
            if(msg == -1)
                GameScene::getInstance()->popLayer();
            else if(msg == -2)
            {
                auto layer = XiangQingLayer::create();
                GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
            }
            else
                GameScene::getInstance()->selectTab(msg);
        });
        
        return false;
    }
    
    void BianDuiLayer::onSingleClick(CCTouch* touch)
    {
        if(GameConfig::shared()->getTutorialProgress() == TutorialProfile)
        {
            AlertMessage::create("", "请【长按】指定的图标。");
        }

        CCPoint localPoint = convertTouchToNodeSpace(touch);
        CCPoint worldPoint = convertToWorldSpace(localPoint);
        
        // handle touch on juanzhou first.
        CCPoint localToJuanzhou = juanzhou->convertTouchToNodeSpace(touch);
        CCPoint localToScrollView = scrollView->convertTouchToNodeSpace(touch);
        
        if(juanzhou->background->boundingBox().containsPoint(localToJuanzhou))
        {
            HeroIcon *icon = (HeroIcon*)juanzhou->getImgByPoint(worldPoint);
            if(icon && icon->isClickEnabled())
                putBackHeroFJzToBoard(icon);
        }
        else if(scrollView->boundingBox().containsPoint(localToScrollView))
        {
            // then try touch on board
            HeroIcon* icon = board->getHeroImgByPoint(worldPoint); 
            
            if(icon && icon->isClickEnabled())
                putHeroToJz(icon);
        }
    }
    
    void BianDuiLayer::onLongClick(cocos2d::CCTouch *touch)
    {
        if(GameConfig::shared()->getTutorialProgress() == TutorialQiangHua)
            return;
        
        CCPoint localPoint = convertTouchToNodeSpace(touch);
        CCPoint worldPoint = convertToWorldSpace(localPoint);
        
        // first send touch to juanzhou
        CCPoint localToJuanzhou = juanzhou->convertTouchToNodeSpace(touch);
        CCPoint localToScrollView = scrollView->convertTouchToNodeSpace(touch);
        
        if(juanzhou->background->boundingBox().containsPoint(localToJuanzhou))
            curTouchIcon = (HeroIcon*)juanzhou->getImgByPoint(worldPoint);
        else if(scrollView->boundingBox().containsPoint(localToScrollView))
            curTouchIcon = board->getHeroImgByPoint(worldPoint);
        
        if(curTouchIcon && curTouchIcon->isLongClickEnabled())
        {
            bool isShowStorage =  !(GameConfig::shared()->getTutorialProgress() == TutorialProfile);
            auto* layer = HeroProfileLayer::create(curTouchIcon->getHero(), isShowStorage);
            GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
            curTouchIcon = NULL;
        }
    }
    
    void BianDuiLayer::putHeroToJz(HeroIcon *icon)
    {
        int i;
        for(i = 0; i < juanzhou->heroNum; i++)
        {
            if(!juanzhou->getHeroById(i))
            {
                HeroIcon* dupIcon = HeroIcon::create(icon->getHero());
                pullHeroIconFrBoard(icon);
                juanzhou->addHeroImg(dupIcon, i);
                
                if(GameConfig::shared()->getTutorialProgress() == TutorialTeam)
                {
                    dupIcon->setEnabled(false);
                    dupIcon->setDark(false);
                }
                break;
            }
        }
        
        if(i == juanzhou->heroNum)
        {
            SystemInform::alertMessage(GameScene::getInstance(),
                                       "队伍已满.",
                                       "提示" ,
                                       [](){});
        }
        
        disableOverLPHeroes();
        
        if(GameConfig::shared()->getTutorialProgress() == TutorialTeam)
        {
            // disable touch for all hero icons
            for(Hero* h : PlayerData::getInstance()->getStorage())
            {
                HeroIcon* hh = board->getHeroImg(h);
                hh->setEnabled(false);
            }
            tutShowBackButton();
        }
    }
    
    void BianDuiLayer::putBackHeroFJzToBoard(HeroIcon *icon)
    {
        if(icon)
        {
            Hero *hero = icon->getHero();
            juanzhou->removeHero(hero);
            putBackHeroToBoard(hero);
        }
    }
    
    void BianDuiLayer::putBackHeroToBoard(Hero *hero)
    {
        HeroIcon *icon = board->getHeroImg(hero);
        icon->setEnabled(true);
        
        disableOverLPHeroes();
    }
    
    void BianDuiLayer::pullHeroIconFrBoard(HeroIcon *icon)
    {
        icon->setEnabled(false);
    }
    
    void BianDuiLayer::disableOverLPHeroes()
    {
        PlayerData *data = PlayerData::getInstance();
        for(Hero *hero : data->getStorage())
        {
            HeroIcon *icon = board->getHeroImg(hero);
            if(!data->isHeroInActiveTeam(hero))
            {
                if((data->teamData.lp + hero->profile->LP) > data->player->maxLP)
                {
                    icon->setBottomMsg(HeroIcon::LabelType::LpLack);
                    icon->setEnabled(false);
                }
                else
                {
                    icon->defaultBottmMsg();
                    icon->setEnabled(true);
                }
            }
            else
            {
                icon->defaultBottmMsg();
                icon->setEnabled(false);
            }
        }
    }
    
    void BianDuiLayer::switchActiveTeam(int i)
    {
        //enable the pre team icons
        std::vector<Hero*> preTeam = PlayerData::getInstance()->getTeam();
        for(Hero* h : preTeam)
            if(h != NULL)
                board->getHeroImg(h)->setEnabled(true);
        
        PlayerData::getInstance()->setActiveTeam(i);
        std::vector<Hero*> team = PlayerData::getInstance()->getTeam();
        
        juanzhou->clear();
        for (int i = 0; i < team.size(); i++)
            if(team[i])
                juanzhou->addHero(team[i], i);
            
        board->reOrder(PlayerData::getInstance()->getStorage());
        for(Hero* h : team)
            if(h != NULL)
                board->getHeroImg(h)->setEnabled(false);
        
        disableOverLPHeroes();
    }
    
    
    void BianDuiLayer::setTouchEnabled(bool value)
    {
        ContentLayer::setTouchEnabled(value);
        scrollView->setTouchEnabled(value);
        topMenu->setTouchEnabled(value);
        juanzhou->setTouchEnabled(value);
    }
    
    void BianDuiLayer::onBackClick(cocos2d::CCObject *target)
    {
        ring("sound/v/button_click.mp3");
        
        if(tryExit())
            GameScene::getInstance()->popLayer();
        
    }
    
    void BianDuiLayer::onDetailClick(cocos2d::CCObject *target)
    {
        
        if(!juanzhou->heroVec[0])
        {
            SystemInform::alertMessage(GameScene::getInstance(),
                                       "掌门不能为空！",
                                       "警告" ,
                                       [](){});
            return;
        }
        if(tryExit(-2))
        {
            auto layer = XiangQingLayer::create();
            GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
        }
        
        ring("sound/v/button_click.mp3");
    }
    
    void BianDuiLayer::onBianduiClick(cocos2d::CCObject *target)
    {
        PlayerData::getInstance()->switchSortType();
        board->reOrder(PlayerData::getInstance()->getStorage());
        disableOverLPHeroes();
        title->setString(PlayerData::getInstance()->getSortTypeMsg());
        ring("sound/v/button_click.mp3");
    } 
    
    /////////////////////////////////////////////////////////////////////
    //WUJIANGLAYER
    //(0.5,1)
    /////////////////////////////////////////////////////////////////////
    HeroPoolBoard::HeroPoolBoard()
    {
        enlargeBtn = NULL;
    }
    
    void HeroPoolBoard::refresh(const std::vector<PH::Hero *>& heroVec)
    {
        HeroBoard::refresh(heroVec);
        for(Hero* hero :  PlayerData::getInstance()->getNewHeroes("heropool"))
        {
            HeroIcon* icon = getHeroImg(hero);
            if(icon)
                icon->showNew();
        }
        
        if(enlargeBtn)
            removeChild(enlargeBtn, true);
        int addSeatBtnLocation = std::max(capacity, heroVec.size());
        enlargeBtn = GetSprite("gameui/kuozhan_xiakezhong.png");
        enlargeBtn->retain();
        placeHolder.push_back(enlargeBtn);
        addHero(enlargeBtn, addSeatBtnLocation);
    }
    bool HeroPoolBoard::isEnlargeClick(cocos2d::CCTouch *touch)
    {
        CCPoint localPoint = convertTouchToNodeSpace(touch);
        return enlargeBtn->boundingBox().containsPoint(localPoint);
    }
    
    
    bool HeroPoolLayer::init()
    {
        if(!ContentLayer::init())
            return false;
        
        heroNumBg = NULL;
        numLabel = NULL;
        heroesOfCurrType = PlayerData::getInstance()->getStorage();
        
        title = createTitleMenu(this, "弟子", PlayerData::getInstance()->getSortTypeMsg(),
                                menu_selector(HeroPoolLayer::onBackClick), menu_selector(HeroPoolLayer::onTitleClick));
        
        switcher = HeroBoardSwitcher::create();
        switcher->setPosition(ccp(0, -80));
        switcher->setCallBack([=](int i){
            scrollView->reset();
            currSwitchType = i;
            board->displayPlaceholder = (currSwitchType==0);
            heroesOfCurrType = getHeroesBySortAndSwitchType(currSwitchType);
            board->refresh(heroesOfCurrType);
        });
        addChild(switcher, 1);
        
        scrollView = ZqlScrollView::create();
        scrollView->setViewSize(CCSize(winSize.width, 900),
                                CCRect(0, -180, winSize.width, 530));
        
        board = HeroPoolBoard::create(PlayerData::getInstance()->getStorage());
        scrollView->addLayer(board);
        scrollView->setPositionY(70); 
        this->addChild(scrollView);
        
        
        initHeroNumTips();
        
        PlayerData::getInstance()->registerStorageCallBack(this , [=](){
            heroesOfCurrType = getHeroesBySortAndSwitchType(currSwitchType);
        });
        
        setTouchEnabled(true);
        
        return true;
    }
    
    void HeroPoolLayer::onEnter()
    {
        ContentLayer::onEnter();
        initHeroNumTips();
        heroesOfCurrType = getHeroesBySortAndSwitchType(currSwitchType);
        board->refresh(heroesOfCurrType);
    }
    
    void HeroPoolLayer::onExit()
    {
        ContentLayer::onExit();
        
    }
    
    void HeroPoolLayer::cleanup()
    {
        CCNode::cleanup();
        PlayerData::getInstance()->clearNewHeroes("heropool");
        PlayerData::getInstance()->unRegisterStorageCallBack(this);
    }
    
    void HeroPoolLayer::initHeroNumTips()
    {
        int num = PlayerData::getInstance()->getStorage().size();
        int maxNum = PlayerData::getInstance()->player->storageLimit;
        
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
        else{
            numLabel->setColor(ccWHITE);
            numLabel->stopAllActions();
        }
    }
    
    void HeroPoolLayer::onSingleClick(CCTouch* touch)
    {
        CCPoint localPoint = scrollView->convertTouchToNodeSpace(touch);
        Hero* hero = NULL;
        if(scrollView->boundingBox().containsPoint(localPoint))
            hero = board->getHero(touch);
        
        if(hero) 
            GameScene::getInstance()->pushLayerWithDefaultAnimation(HeroProfileLayer::create(hero));
        else if(board->isEnlargeClick(touch))
        {
            GameScene::getInstance()->pushLayerWithDefaultAnimation(XiweiLayer::create());
            
        }
    }
    void HeroPoolLayer::onLongClick(CCTouch* touch)
    {
        CCPoint localPoint = scrollView->convertTouchToNodeSpace(touch);
        Hero* hero = NULL;
        if(scrollView->boundingBox().containsPoint(localPoint))
            hero = board->getHero(touch);
        
        if(hero)
            GameScene::getInstance()->pushLayerWithDefaultAnimation(HeroProfileLayer::create(hero));
    }
    
    void HeroPoolLayer::onBackClick(cocos2d::CCObject *target)
    {
        GameScene::getInstance()->popLayer();
        
        ring("sound/v/button_click.mp3");
    }
    
    void HeroPoolLayer::onTitleClick(cocos2d::CCObject *target)
    {
        PlayerData *data = PlayerData::getInstance();
        data->switchSortType();
        title->setString(data->getSortTypeMsg());
        heroesOfCurrType = getHeroesBySortAndSwitchType(switcher->getCurNode());
        board->reOrder(heroesOfCurrType);
        ring("sound/v/button_click.mp3");
    }
    
    
    ////////////////////////////////////////////////
    //SoulLayer
    ////////////////////////////////////////////////
    
    
    bool SoulItem::init(HeroProfile hProfile , int num, bool isNew)
    {
        
        if(!ContentLayer::init())
            return false;
        
        this->hProfile = hProfile;
        ghostsNum = num;
        
        CCSprite *bg = GetSprite("gameui/hunpo_bg.png");
        bg->setAnchorPoint(ccp(0.5 , 1));
        bg->setPosition(ccp(0 , 0));
        addChild(bg);
        
        CCMenu* menu = CCMenu::create();
        btnSpr = createBtn("兑换");
        if(num < hProfile.costSouls)
        {
            btnSpr->setColor(ccc3(100, 100, 100));
            //hack! setcolor the label on btn
            ((CCLabelTTF*)btnSpr->getChildren()->objectAtIndex(0))->setColor(ccc3(100, 100, 100));
        }
        
        CCMenuItem *addBtn = CCMenuItemSprite::create(btnSpr, btnSpr, this, menu_selector(SoulItem::onChange));
        menu->addChild(addBtn);
        menu->setPosition(ccp(210 , -33));
        addChild(menu);
        
        heroImg = HeroIcon::createWithProfile(&hProfile, true);
        heroImg->setAnchorPoint(ccp(0,1));
        heroImg->setPosition(ccp(-270  , -9));
        if(isNew)
            heroImg->showNew();
        addChild(heroImg);
        
        CCLabelTTF *nameLabel = LabelX::create(hProfile.nameCN.c_str() , FONT_CN , 28, LabelX::FontEffect::SHADOW, ccp(2, -2));
        nameLabel->setColor(ccWHITE);
        nameLabel->setPosition(ccp(-160, -34));
        nameLabel->setAnchorPoint(ccp(0, 0.5));
        addChild(nameLabel);
        
        for(int i = 0; i < hProfile.star; i++)
        {
            CCSprite* star = GetSprite("gameui/xingxing2.png");
            star->setPosition(ccp(30*i, -33));
            addChild(star);
        }
        
        float scale = float(num)/hProfile.costSouls<1?std::max(float(num)/hProfile.costSouls,(float)0.03):1;
//        CCScale9Sprite* numBar = CCScale9Sprite::create("gameui/hunpo_jingyantiao.png", CCRectMake(0,0,336,23),CCRectMake(4, 4, 328, 15));
        
        numBar = Get9SpriteFromFrame("hunpo_jingyantiao.png",
                                                     CCRectMake(4, 4, 328, 15));
        
        numBar->setPosition(ccp(-78, -82));
        numBar->setAnchorPoint(ccp(0, 0.5f));
        numBar->setContentSize(CCSizeMake(int(336*scale), 24));
        addChild(numBar);
        
        std::string numText = toStr(num) + "/" + toStr(hProfile.costSouls);
        addLabelBMFont(this, numLabel, numText.c_str(), "bmfont/Sumo_22_white.fnt", ccp(100,-82), ccp(0.5,0.5));
        numLabel->setScale(contentScale());
        
        setContentSize(CCSize(556 , 140));
        
        return true;
    }
    
    void SoulItem::onChange(CCObject* target)
    {
        boost::function<void ()> toChange = [=](){
            PuzzleRPC::getInstance()->redeemSoul(PlayerData::getInstance()->getToken(), hProfile.tableId,
                                                 [=](std::auto_ptr<Hero> heroPtr, std::auto_ptr<Error> e){
                                                     if (e.get()) {
                                                         e->handleErrorInform();
                                                         return;
                                                     }
                                                     PlayerData::getInstance()->player->heroSouls[hProfile.tableId] -= hProfile.costSouls;
                                                     PlayerData::getInstance()->addHero(heroPtr.release());
                                                     SystemInform::alertTips(GameScene::getInstance(),
                                                                                "兑换成功！",
                                                                                "提示");
                                                     soulLayer->initSouls();
                                                 });
        };
        
        if(PlayerData::getInstance()->player->heroSouls[hProfile.tableId] >= hProfile.costSouls)
            SystemInform::alertConfirmPanel(GameScene::getInstance(), "确定要兑换魂魄吗？", "提示", toChange, [](){});
    }
    void SoulItem::onSingleClick(CCTouch* touch)
    {
        CCPoint localPoint = convertTouchToNodeSpace(touch);
        if(heroImg->boundingBox().containsPoint(localPoint))
        {
            GameScene::getInstance()->pushLayerWithDefaultAnimation(HeroProfileLayer::create(&hProfile));
        }
    }
    
    void SoulItem::onLongClick(CCTouch* touch)
    {
        CCPoint localPoint = this->convertTouchToNodeSpace(touch);
        if(heroImg && heroImg->boundingBox().containsPoint(localPoint))
        {
            auto* layer = HeroProfileLayer::create(&hProfile);
            GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
        }
    }
    
    void SoulItem::update()
    {
        int num = PlayerData::getInstance()->player->heroSouls[hProfile.tableId];
        float scale = float(num)/hProfile.costSouls<1?std::max(float(num)/hProfile.costSouls,(float)0.03):1;
        numBar->setContentSize(CCSizeMake(int(336*scale), 24));
        numLabel->setString((toStr(num) + "/" + toStr(hProfile.costSouls)).c_str());
        if(num < hProfile.costSouls)
        {
            btnSpr->setColor(ccc3(100, 100, 100));
            //hack! setcolor the label on btn
            ((CCLabelTTF*)btnSpr->getChildren()->objectAtIndex(0))->setColor(ccc3(100, 100, 100));
        }
    }
    
    bool SoulLayer::init()
    {
        if(!ContentLayer::init())
            return false;
        
        title = createTitleMenu(this, "魂魄", getSortStr(),
                                menu_selector(SoulLayer::onBack),
                                menu_selector(SoulLayer::onChangeSortType));
        
        scrollView = ZqlScrollView::create();
        scrollView->setViewSize(CCSize(winSize.width , 800) ,  CCRect(0, -100, winSize.width, 600));
        scrollView->setPosition(ccp( 0 , 40));
        this->addChild(scrollView);
        
        newSouls = PlayerData::getInstance()->newSouls;
        PlayerData::getInstance()->newSouls.clear();
        
        return true;
    }
    
    void SoulLayer::initSouls()
    {
        soulsVec.clear();
        std::map<int,int> souls = PlayerData::getInstance()->player->heroSouls;
        for (std::map<int,int>::iterator it = souls.begin(); it != souls.end(); it++)
            if(it->second > 0)
                soulsVec.push_back(std::make_pair(it->first, it->second));
        
        makeItems(getSoulsBySortType());
    }
    
    void SoulLayer::onEnter()
    {
        ContentLayer::onEnter();
        
        initSouls();
        
    }
    
    void SoulLayer::onExit()
    {
        ContentLayer::onExit();
    }
    
    void SoulLayer::makeItems(std::vector<std::pair<int , int>> souls)
    {
        if(!souls.size())
        {
            SystemInform::alertMessage(GameScene::getInstance(), "你当前没有获得魂魄", "提示", [](){});
            return;
        }
        
        if(scrollView)
            scrollView->removeAllLayers();
        
        for (std::vector<std::pair<int, int>>::iterator it = souls.begin(); it != souls.end(); it++)
        {
            SoulItem* item = SoulItem::create(HPT(it->first),
                                              it->second,
                                              std::find(newSouls.begin(), newSouls.end(), it->first) != newSouls.end());
            item->soulLayer = this;
            scrollView->addLayer(item);
        }
        
    }
    
    void SoulLayer::reOrder()
    {
        PlayerData::getInstance()->switchSoulSortType();
        std::vector<std::pair<int , int>> souls = getSoulsBySortType();
        CCArray* items = scrollView->getChildren();
        for(int i=0; i<souls.size(); i++)
            for(int j=0; j<items->count(); j++)
                if(((SoulItem*)items->objectAtIndex(j))->hProfile.tableId == souls[i].first)
                {
                    if(i != j)
                        items->exchangeObjectAtIndex(i, j);
                    break;
                }
        
    }
    
    void SoulLayer::onBack()
    {
        GameScene::getInstance()->popLayer();
    }
    
    void SoulLayer::onChangeSortType(cocos2d::CCObject *target)
    {
        reOrder();
        title->setString(getSortStr().c_str());
    }
    
    std::string SoulLayer::getSortStr()
    {
        switch (PlayerData::getInstance()->getSoulSortType()) {
            case Nor_Soul_Sort:
                return "默认排序";
            case Color_Soul_Sort:
                return "属性排序";
            case Star_Soul_Sort:
                return "星级排序";
            case Left_Soul_Sort:
                return "魂魄排序";
            default:
                return "默认排序";
        }
    }
    
    std::vector<std::pair<int , int>>  SoulLayer::getSoulsBySortType()
    {
        std::vector<std::pair<int, int>> temp = soulsVec;
        switch (PlayerData::getInstance()->getSoulSortType()) {
            case Nor_Soul_Sort:
                break;
            case Color_Soul_Sort:
                std::sort(temp.begin() , temp.end() , [](std::pair<int, int> p1, std::pair<int , int> p2){
                    return HPT(p1.first).color != HPT(p2.first).color ? HPT(p1.first).color < HPT(p2.first).color : HPT(p1.first).tableId < HPT(p2.first).tableId;
                });
                break;
            case Star_Soul_Sort:
                std::sort(temp.begin() , temp.end() , [](std::pair<int, int> p1, std::pair<int , int> p2){
                    return HPT(p1.first).star != HPT(p2.first).star ? HPT(p1.first).star > HPT(p2.first).star : HPT(p1.first).tableId < HPT(p2.first).tableId;
                });
                break;
            case Left_Soul_Sort:
                std::sort(temp.begin() , temp.end() , [](std::pair<int, int> p1, std::pair<int , int> p2){
                    return HPT(p1.first).costSouls-p1.second != HPT(p2.first).costSouls-p2.second ? HPT(p1.first).costSouls-p1.second < HPT(p2.first).costSouls-p2.second : HPT(p1.first).tableId < HPT(p2.first).tableId;
                });
                break;
            default:
                break;
        }
        return temp;
    }
    //------------------
    // JIQILayer
    //------------------
    ccColor3B getColorOfBonus(int bonus)
    {
        ccColor3B color = ccBLACK;
        if(bonus == -5 || bonus == -4)
            color = ccORANGE;
        else if(bonus >=-3 && bonus <= -1)
            color = ccc3(160, 32, 240);
        else if(bonus >=0 && bonus <= 2)
            color = ccBLUE;
        else
            color = ccGREEN;
        return color;
    }
    
    std::string getColorStrOfBonus(int bonus)
    {
        if(bonus == -5 || bonus == -4)
            return "0xffff7f00";
        else if(bonus >=-3 && bonus <= -1)
            return "0xffa020f0";
        else if(bonus >=0 && bonus <= 2)
            return "0xff0000ff";
        else if(bonus >= 3 && bonus <= 5)
            return "0xff00ff00";
        return "0xff000000";
    }
    
    bool JiQiJuanZhou::init()
    {
        if(!JuanZhou::init())
            return false;
        
        viewPort = CCSize(20 , 310);
        heroNum = 1;
        heroVec = std::vector<Hero*>(1, (Hero*)NULL);
        heroImgVec = std::vector<HeroIcon*>(1, (HeroIcon*)NULL);
        isUseGas = true;
        
        CCSprite* bg = GetSprite("gameui/zhuansheng_3.png");
        bg->setAnchorPoint(ccp(0.5, 1));
        addChild(bg, -10);
        
        locations.push_back(CCRectMake(-280, -130, 100, 100));
        addLabelTTF(this, "*集气丹不足可使用元宝重置", FONT_CN, 20, ccp(-270, -150), ccp(0, 0.5));
        
        skillName = addLabelTTF(this, "", FONT_CN, 28, ccp(-50, -50), ccp(0, 0.5) , ccc3(0x96, 0, 0));
        skillDes = addLabelTTF(this, "", FONT_CN, 24, ccp(-165, -71), ccp(0, 1));
        skillDes->setDimensions(CCSizeMake(440, 0));
        skillDes->setHorizontalAlignment(kCCTextAlignmentLeft);
        
        orgGasNum = CCRichLabelTTF::create("", FONT_CN, 20);
        orgGasNum->setPosition(ccp(-185, -195));
        addChild(orgGasNum);
        desGasNum = CCRichLabelTTF::create("", FONT_CN, 20);
        desGasNum->setPosition(ccp(-60, -195));
        addChild(desGasNum);
        
        sucProb = CCRichLabelTTF::create("", FONT_CN, 20);
        sucProb->setPosition(ccp(-105, -230));
        addChild(sucProb);
        
        gasRange = addLabelBMFont(this, "", "bmfont/Sumo_22_yellow.fnt", ccp(-105, -262), ccp(0.5, 0.5));
        gasRange->setScale(contentScale());
        
        {
            addLabelTTF(this, gasTotal, toStr(PlayerData::getInstance()->getNumOfHero(HeroIdOfType::Hero_GasPill)).c_str(), FONT_CN, 22, ccp(38, -238), ccp(0, 0.5), TITLE_YELLOW);
            addLabelTTF(this, goldTotal, toStr(PlayerData::getInstance()->player->golds).c_str(), FONT_CN, 22, ccp(38, -265), ccp(0, 0.5), TITLE_YELLOW);
        }
       
        {
            CCSprite* gasBg = GetSprite("gameui/xuanzhong.png");
            CCSprite* gas = GetSprite("gameui/jiqidan.png");
            gas->setPosition(ccp(gasBg->getContentSize().width*0.5, gasBg->getContentSize().height*0.5));
            gasBg->addChild(gas);
            CCSprite* dot = GetSprite("gameui/shezhi_xuanzhong.png");
            dot->setPosition(ccp(-24, gasBg->getContentSize().height*0.5+2));
            gasBg->addChild(dot);
            gasBtn = ButtonSprite::create(gasBg, GetSprite("gameui/jiqidan.png"),
                                          kCCMenuHandlerPriority, false);
            gasBtn->setOnClickListener([=](){
                int ownPotions = PlayerData::getInstance()->getNumOfHero(HeroIdOfType::Hero_GasPill);
                if(heroVec[0] &&  heroVec[0]->profile->potions >  ownPotions)
                {
                    if(PlayerData::getInstance()->player->golds >= heroVec[0]->profile->potions*60)
                        SystemInform::alertMessage(runningScene(), "集气丹数量不足，请使用元宝重置", "提示", [](){});
                    else
                        SystemInform::alertConfirmPanel(runningScene(), "你的道具和元宝不足, 是否充值？", "提示",
                                                        [](){GameScene::getInstance()->pushLayerWithDefaultAnimation(ChongZhiLayer::create());},
                                                        [](){});
                    return;
                }
                selectPayChanel(true);
            });
            gasBtn->setPosition(ccp(83, -175));
            addChild(gasBtn);
            gasCost = addLabelBMFont(gasBtn, "", "bmfont/Sumo_22_yellow.fnt", ccp(50, 15), ccp(0.5, 0.5));
            gasCost->setScale(contentScale());
            
            CCSprite* goldBg = GetSprite("gameui/xuanzhong.png");
            CCSprite* gold = GetSprite("gameui/yuanbao_dan.png");
            gold->setPosition(ccp(goldBg->getContentSize().width*0.5f, goldBg->getContentSize().height*0.5f));
            goldBg->addChild(gold);
            CCSprite* dot2 = GetSprite("gameui/shezhi_xuanzhong.png");
            dot2->setPosition(ccp(-22, gasBg->getContentSize().height*0.5+2));
            goldBg->addChild(dot2);
            goldBtn = ButtonSprite::create(goldBg, GetSprite("gameui/yuanbao_dan.png"),
                                           kCCMenuHandlerPriority, false);
            goldBtn->setOnClickListener([=](){
                if(heroVec[0] && heroVec[0]->profile->potions*60 > PlayerData::getInstance()->player->golds)
                {
                    if(heroVec[0]->profile->potions <= PlayerData::getInstance()->getNumOfHero(HeroIdOfType::Hero_GasPill))
                        SystemInform::alertMessage(runningScene(), "元宝数量不足，请使用集气丹重置", "提示", [](){});
                    else
                        SystemInform::alertConfirmPanel(runningScene(), "你的道具和元宝不足, 是否充值？", "提示",
                                                    [](){GameScene::getInstance()->pushLayerWithDefaultAnimation(ChongZhiLayer::create());},
                                                    [](){});
                    return;
                }
                selectPayChanel(false);
            });
            goldBtn->setPosition(ccp(232, -175));
            addChild(goldBtn);
            goldCost = addLabelBMFont(goldBtn, "", "bmfont/Sumo_22_yellow.fnt", ccp(50, 15), ccp(0.5, 0.5));
            goldCost->setScale(contentScale());
        }
        
        btn = ButtonSprite::create(createBtn("重置", ButtonType::NORMAL_RED), kCCMenuHandlerPriority, false);
        btn->setOnClickListener([=](){
            
            if(heroVec[0])
            {
                if(heroVec[0]->skillBonus <= -5)
                {
                    SystemInform::alertMessage(runningScene(), "侠客已经是最低集气值了", "提示", [](){});
                    return;
                }
                if(isUseGas && heroVec[0]->profile->potions > PlayerData::getInstance()->getNumOfHero(HeroIdOfType::Hero_GasPill))
                {
                    SystemInform::alertMessage(runningScene(), "你的集气丹不足！", "提示",
                                                    [](){});
                    return;
                }
                
                if(!isUseGas && heroVec[0]->profile->potions*60 > PlayerData::getInstance()->player->golds)
                {
                    SystemInform::alertConfirmPanel(runningScene(), "你的元宝不足, 是否充值？", "提示",
                                                    [](){GameScene::getInstance()->pushLayerWithDefaultAnimation(ChongZhiLayer::create());},
                                                    [](){});
                    return;
                }
                
                SkillBonusChangeType type = isUseGas ? SkillBonusChangeType::Potions : SkillBonusChangeType::Golds;
                SystemInform* inform = SystemInform::alertLoading(runningScene());
                PuzzleRPC::getInstance()->changeSkillBonus(PlayerData::getInstance()->getToken(),
                                                           heroVec[0]->id,
                                                           type,
                                                           [=](std::auto_ptr<BoolWrapper> b,std::auto_ptr<Error> e)
               {
                   inform->cancel();
                   if(e.get())
                   {
                       e->handleErrorInform();
                       return;
                   }
                   
                   Hero* target = heroVec[0];
                   if(b->ok)
                   {
                       playSuccessAnim();
                       target->skillBonus--;
                       target->chance = 0;
                       
                   }
                   else
                   {
                       this->playFailAnim();
                       target->chance += std::pow((float)(target->skillBonus+5), 0.7f)+(target->skillBonus+5)/2;
                   }
                   
                   int num = heroVec[0]->profile->potions;
                   if(type == SkillBonusChangeType::Potions)
                   {
                       
                       PlayerData::getInstance()->removeNumOfHero(HeroIdOfType::Hero_GasPill, num);
                       gasTotal->setString(toStr(PlayerData::getInstance()->getNumOfHero(HeroIdOfType::Hero_GasPill)).c_str());
                   }
                   else
                   {
                       PlayerData::getInstance()->setGolds(PlayerData::getInstance()->player->golds - num*60);
                       goldTotal->setString(toStr(PlayerData::getInstance()->player->golds).c_str());
                   }
                   updateMsg();
                   if(b->ok)
                   {
                       orgGasNum->setString(toStr(heroVec[0]->maxEnergy+heroVec[0]->skillBonus+1).c_str());
                       desGasNum->setString(toStr(heroVec[0]->maxEnergy+heroVec[0]->skillBonus).c_str());
                   }
               });
            }
            else
                SystemInform::alertMessage(runningScene(), "请选择要重置的侠客", "提示", [](){});
            
        });
        btn->setPosition(ccp(210, -252));
        addChild(btn);
        
        setTouchEnabled(true);
        selectPayChanel(true);
        return true;
    }
    
    void JiQiJuanZhou::selectPayChanel(bool isGas)
    {
        isUseGas = isGas;
        if(isGas)
        {
            gasBtn->onRelease();
            goldBtn->onPressed();
        }
        else
        {
            goldBtn->onRelease();
            gasBtn->onPressed();
        }
    }
    
    void JiQiJuanZhou::updateMsg()
    {
        if(heroVec[0])
        {
            skillName->setString(AST(heroVec[0]->profile->activeSkillId).name.c_str());
            skillDes->setString(AST(heroVec[0]->profile->activeSkillId).desc.c_str());
            
            orgGasNum->setString(toStr(heroVec[0]->maxEnergy+heroVec[0]->skillBonus).c_str());
            desGasNum->setString(" ");
            
            int bonus = heroVec[0]->skillBonus;
            int prob =  std::min((bonus+5)*(bonus+5)+std::abs(bonus), 100);
            sucProb->setString(("[color=0xff000000]"+toStr(prob)+"%[color=0xffff0000]+"
                                +toStr(heroVec[0]->chance)+"%[/color][/color]").c_str());
            gasRange->setString((toStr(heroVec[0]->maxEnergy-5)+"~"+toStr(heroVec[0]->maxEnergy+5)).c_str());
            int pCost = heroVec[0]->profile->potions;
            gasCost->setString(toStr(pCost).c_str());
            goldCost->setString(toStr(pCost*60).c_str());
            
        }
        else
        {
            skillName->setString("");
            skillDes->setString("");
            orgGasNum->setString(" ");
            desGasNum->setString(" ");
            sucProb->setString(" ");
            gasRange->setString("");
            gasCost->setString("");
            goldCost->setString("");
        }
    }
    
    void JiQiJuanZhou::onSingleClick(CCTouch* touch)
    {
        CCPoint localPoint = convertTouchToNodeSpace(touch);
        JiQiLayer* layer = (JiQiLayer*)getUserData();
        if(heroImgVec[0] && heroImgVec[0]->boundingBox().containsPoint(localPoint))
        {
            removeHeroById(0);
            updateMsg();
            layer->choose(NULL);
        }
        
    }
    void JiQiJuanZhou::onLongClick(CCTouch* touch)
    {
        CCPoint localPoint = convertTouchToNodeSpace(touch);
        if(heroImgVec[0] && heroImgVec[0]->boundingBox().containsPoint(localPoint))
        {
            GameScene::getInstance()->pushLayerWithDefaultAnimation(HeroProfileLayer::create(heroVec[0]));
        }
    }
    
    void JiQiJuanZhou::setTouchEnabled(bool val)
    {
        
        JuanZhou::setTouchEnabled(val);
        gasBtn->setTouchEnabled(val);
        goldBtn->setTouchEnabled(val);
        btn->setTouchEnabled(val);
        
    }
    
    void JiQiJuanZhou::playSuccessAnim()
    {
        this->setTouchEnabled(false);
        ((JiQiLayer*)getUserData())->setTouchEnabled(false);
        CCParticleSystemQuad *fire = CCParticleSystemQuad::create("particle/firework.plist");
        fire->setPosition(ccp(-200 , -100));
        fire->setVisible(false);
        fire->unscheduleUpdate();
        this->addChild(fire , 100);
        
        
        CCParticleSystemQuad *fire2 = CCParticleSystemQuad::create("particle/firework.plist");
        fire2->setPosition(ccp(200 , -150));
        fire2->setVisible(false);
        fire2->unscheduleUpdate();
        this->addChild(fire2 , 100);
        
        CCSprite* light = GetSprite("gameui/gaung_huang.png");
        light->setPosition(heroImgVec[0]->getPosition());
        light->runAction(CCSequence::create(CCRotateBy::create(1, 180),
                                            CCCallLambda::create([=](){this->removeChild(light);}),
                                            NULL));
        addChild(light, -1);
        
        CCSprite *suc = GetSprite("gameui/chenggong_pic.png");
        suc->setPosition(ccp(0 , -200));
        suc->setScale(1.8);
        suc->setVisible(false);
        this->addChild(suc, 100);
        
        CCFiniteTimeAction *seq = CCSequence::create(CCCallLambda::create([=]()
                                                                          {
                                                                              ring("sound/v/level_up_done.mp3");
                                                                          }),
                                                     CCDelayTime::create(1),
                                                     CCCallLambda::create([=]()
                                                                          {
                                                                              suc->setVisible(true);
                                                                              fire->setVisible(true);
                                                                              fire->scheduleUpdate();
                                                                              fire2->setVisible(true);
                                                                              fire2->scheduleUpdate();
                                                                          }),
                                                     CCEaseIn::create(CCScaleTo::create(1.0f, 1.0f, 1.0f), 0.5),
                                                     CCFadeOut::create(0.3),
                                                     CCCallLambda::create([=]()
                                                                          {
                                                                              this->removeChild(suc, true);
                                                                              this->removeChild(fire, true) ;
                                                                              this->removeChild(fire2 , true);
                                                                              this->setTouchEnabled(true);
                                                                              ((JiQiLayer*)this->getUserData())->setTouchEnabled(true);
                                                                          }),
                                                     NULL);
        suc->runAction(seq);
        
       
    }

    void JiQiJuanZhou::playFailAnim()
    {
        this->setTouchEnabled(false);
        ((JiQiLayer*)getUserData())->setTouchEnabled(false);
        CCSprite* light = GetSprite("gameui/guang_bai.png");
        light->setPosition(heroImgVec[0]->getPosition());
        light->runAction(CCSequence::create(CCRotateBy::create(1, 180),
                                            CCCallLambda::create([=](){this->removeChild(light);}),
                                            NULL));
        addChild(light, -1);
        
        CCSprite *fail = GetSprite("gameui/shibai_da.png");
        fail->setPosition(ccp(0 , -200));
        fail->setScale(1.8);
        fail->setVisible(false);
        this->addChild(fail, 100);
        
        CCFiniteTimeAction *seq = CCSequence::create(CCCallLambda::create([=]()
                                                                          {
                                                                              ring("sound/v/level_up_done.mp3");
                                                                          }),
                                                     CCDelayTime::create(1),
                                                     CCShow::create(),
                                                     CCEaseIn::create(CCScaleTo::create(1.0f, 1.0f, 1.0f), 0.5),
                                                     CCFadeOut::create(0.3),
                                                     CCCallLambda::create([=]()
                                                                          {
                                                                              this->removeChild(fail, true);
                                                                              this->setTouchEnabled(true);
                                                                              ((JiQiLayer*)getUserData())->setTouchEnabled(true);
                                                                          }),
                                                     NULL);
        fail->runAction(seq);
    }

    bool JiQiBoard::init()
    {
        displayPlaceholder = false;
        if(!HeroBoard::init(PlayerData::getInstance()->getStorage(), false))
            return false;
        
        return true;
    }
    
    void JiQiBoard::refresh(const std::vector<PH::Hero *>& heroVec)
    {
        std::vector<PH::Hero*> heroesRemoveMats;
        for(Hero* h : heroVec)
            if(h->profile->tableId >= 10000 && h->maxEnergy > 0)
                heroesRemoveMats.push_back(h);
        
        HeroBoard::refresh(heroesRemoveMats);
    }
    
    void JiQiBoard::reOrder(const std::vector<PH::Hero *>& heroVec)
    {
        std::vector<PH::Hero*> heroesRemoveMats;
        for(Hero* h : heroVec)
            if(h->profile->tableId >= 10000 && h->maxEnergy > 0)
                heroesRemoveMats.push_back(h);
        
        HeroBoard::reOrder(heroesRemoveMats);
    }
    
    bool JiQiLayer::init()
    {
        if(!ContentLayer::init())
            return false;
        isShowTop = false;
        chosenHero = NULL;
        title = createTitleMenu(this, "集气", PlayerData::getInstance()->getSortTypeMsg(),
                                menu_selector(JiQiLayer::onBack),menu_selector(JiQiLayer::onTitleClick),
                        false);
        
        juanzhou = JiQiJuanZhou::create();
        juanzhou->setUserData(this);
        jz = JuanZhouLayer::create();
        CCSprite* zhouRight = GetSprite("gameui/jiqi_1.png");
        zhouRight->setFlipX(true);
        jz->addSkin(juanzhou, GetSprite("gameui/jiqi_1.png"), zhouRight);
        jz->setPosition(ccp(0, -68));
        addChild(jz, 10);
        
        scrollView = ZqlScrollView::create();
        board = JiQiBoard::create();
        scrollView->addLayer(board);
        scrollView->setPositionY(-280);
        scrollView->setViewSize(CCSize(winSize.width, 900),
                                CCRect(0, -126, winSize.width, 410));
        this->addChild(scrollView);
        
        return true;
    }
    
    void JiQiLayer::onEnterAnimationDone()
    {
        jz->startAnim();
    }
    
    void JiQiLayer::choose(Hero* h)
    {
        if(h == chosenHero)
            return;
        
        if(h)
        {
            juanzhou->addHero(h, 0);
            juanzhou->updateMsg();
            board->getHeroImg(h)->setEnabled(false);
        }
        if(chosenHero)
            board->getHeroImg(chosenHero)->setEnabled(true);
        chosenHero = h;
    }
    
    void JiQiLayer::onSingleClick(CCTouch* touch)
    {
        CCPoint localPoint = scrollView->convertTouchToNodeSpace(touch);
        if(scrollView->boundingBox().containsPoint(localPoint))
        {
            Hero* hero = board->getHero(touch);
            if(hero)
                choose(hero);
        }
    }
    void JiQiLayer::onLongClick(CCTouch* touch)
    {
        CCPoint localPoint = scrollView->convertTouchToNodeSpace(touch);
        Hero* hero = NULL;
        if(scrollView->boundingBox().containsPoint(localPoint))
            hero = board->getHero(touch);
        
        if(hero)
            GameScene::getInstance()->pushLayerWithDefaultAnimation(HeroProfileLayer::create(hero));
    }
    
    void JiQiLayer::onTitleClick()
    {
        PlayerData::getInstance()->switchSortType();
        board->reOrder(PlayerData::getInstance()->getStorage());
        title->setString(PlayerData::getInstance()->getSortTypeMsg());
        ring("sound/v/button_click.mp3");
    }
    void JiQiLayer::onBack()
    {
        GameScene::getInstance()->popLayer();
        ring("sound/v/button_click.mp3");
    }
    
    //------------------
    // XiaKe
    //------------------
    enum class ButtonID : int
    {
        BianDui,
        XiaKe,
    };
    
    bool XiaKe::init()
    {
        if(!ContentLayer::init())
            return false;
        
        
        CCPoint pos = ccp(264, 60);
        
        scrollView = ZqlScrollView::create();
        scrollView->setViewSize(CCSizeMake(winSize.width, 900), CCRectMake(0, -50, winSize.width, 670));
        scrollView->setPosition(ccp(0, 70));
        addChild(scrollView);
        
        biandui = GetSprite("gameui/caidan_1_ty.png");
        biandui->addChild(createTitleLabel("编队", pos));
        biandui->setUserData((void*)BianDuiLayer::create);
        biandui->setTag(1); //use to controll touch enable
        items.push_back(biandui);
        scrollView->addLayer(biandui);
        
        qianghua = GetSprite("gameui/caidan_1_ty.png");
        qianghua->addChild(createTitleLabel("强化", pos));
        qianghua->setUserData((void*)QiangHuaLayer::create);
        qianghua->setTag(1);
        items.push_back(qianghua);
        scrollView->addLayer(qianghua);
        
        zhuansheng = GetSprite("gameui/caidan_1_ty.png");
        zhuansheng->setUserData((void*)ZhuanShengLayer::create);
        zhuansheng->setTag(1);
        items.push_back(zhuansheng);
        zhuansheng->addChild(createTitleLabel("转生", pos));
        scrollView->addLayer(zhuansheng);
        
        jiqi = GetSprite("gameui/caidan_1_ty.png");
        jiqi->addChild(createTitleLabel("集气", pos));
        jiqi->setUserData((void*)JiQiLayer::create);
        jiqi->setTag(1);
        items.push_back(jiqi);
        scrollView->addLayer(jiqi);
        
        relChange = NULL;
        if(GameConfig::shared()->showShuffleRelation || 1)
        {
            relChange = GetSprite("gameui/caidan_1_ty.png");
            items.push_back(relChange);
            relChange->addChild(createTitleLabel("缘分", pos));
            relChange->setUserData((void*)WashRelationLayer::create);
            relChange->setTag(1);
            scrollView->addLayer(relChange);
        }

        hunpo = GetSprite("gameui/caidan_1_ty.png");
        items.push_back(hunpo);
        hunpo->addChild(createTitleLabel("魂魄", pos));
        hunpo->setUserData((void*)SoulLayer::create);
        hunpo->setTag(1);
        scrollView->addLayer(hunpo);
        
        chushou = GetSprite("gameui/caidan_1_ty.png");
        chushou->setUserData((void*)SaleLayer::create);
        chushou->setTag(1);
        items.push_back(chushou);
        chushou->addChild(createTitleLabel("出售", pos));
        scrollView->addLayer(chushou);
        
        xiake = GetSprite("gameui/caidan_1_ty.png");
        xiake->setUserData((void*)HeroPoolLayer::create);
        xiake->setTag(1);
        items.push_back(xiake);
        addLabelBMFont(xiake, heroesNum, "", "bmfont/Sumo_22_white.fnt", ccp(422, 115));
        heroesNum->setScale(contentScale());
        xiake->addChild(createTitleLabel("弟子", pos));
        scrollView->addLayer(xiake);

        

        if(GameConfig::shared()->getTutorialProgress() == TutorialTeam)
        {
            ArrowMask::shared()->setMask(CCRect(0, 0, 640, 960));
            
            scrollView->setScrollEnabled(false);
            
            disableAllButtons();
            biandui->setTag(1);
         
            AlertMessage::create("", "编队：让慕名而来的绝顶高手进入你的队伍吧！");
        }
        else if(GameConfig::shared()->getTutorialProgress() == TutorialQiangHua)
        {
            ArrowMask::shared()->setMask(CCRect(0, 0, 640, 960));

            scrollView->setScrollEnabled(false);
            disableAllButtons();
            qianghua->setTag(1);
            AlertMessage::create("", "侠客强化：想让追随你的侠客迅速成长，试试强化吧！");
        }
        else if(GameConfig::shared()->getTutorialProgress() == TutorialTransformHero)
        {
            ArrowMask::shared()->setMask(CCRect(0, 0, 640, 960));
            
            scrollView->setScrollEnabled(false);
            disableAllButtons();
            zhuansheng->setTag(1);
            
            AlertMessage::create("", "侠客转生：掌门！你旗下一名侠客已满级，可以通过[color=ffff0000]转生[/color]来提高侠客星级，快来试试吧！");
        }
        else if(GameConfig::shared()->getTutorialProgress() == TutorialShuffleRelation)
        {
            ArrowMask::shared()->setMask(CCRect(0, 0, 640, 960));
            scrollView->setScrollEnabled(false);
            disableAllButtons();
            relChange->setTag(1);
        }
        else if(GameConfig::shared()->getTutorialProgress() == TutorialNeili)
        {
            ArrowMask::shared()->setMask(CCRect(0, 0, 640, 960));
            scrollView->setScrollEnabled(false);
            AlertMessage::create("", "内力：同类型侠客相互强化会奇特的效果，快到强化里面试试看。");

            disableAllButtons();
            qianghua->setTag(1);
        }
        else if(GameConfig::shared()->getTutorialProgress() == TutorialShuffleRelation)
        {
            ArrowMask::shared()->setMask(CCRect(0, 0, 640, 960));
        }
        
        
        return true;
    }
    
    void XiaKe::setTouchEnabled(bool value)
    {
        ContentLayer::setTouchEnabled(value);
        //menu->setTouchEnabled(value);
    }
    
    void XiaKe::onEnter()
    {
        ContentLayer::onEnter();
        const Player* player = PlayerData::getInstance()->player;
        heroesNum->setString((toStr(player->storage.size())+"/"+toStr(player->storageLimit)).c_str());
        
        if(relChange)
        {
            CCSprite* tip2 = (CCSprite*)relChange->getChildByTag(200);
            if(tip2)
                tip2->removeFromParentAndCleanup(true);
            if(PlayerData::getInstance()->freeRelationTimes > 0)
            {
                tip2 = makeTips(PlayerData::getInstance()->freeRelationTimes);
                tip2->setPosition(ccp(460 , 100));
                tip2->setTag(200);
                relChange->addChild(tip2);
            }
        }
        
    }
    
    void XiaKe::onEnterAnimationDone()
    {
        ContentLayer::onEnterAnimationDone();
        
        if(GameConfig::shared()->getTutorialProgress() == TutorialTeam)
        {
            CCPoint p = biandui->getParent()->convertToWorldSpace(biandui->getPosition());
            ArrowMask::shared()->setArrow(ccp(p.x-200, p.y-60));
            ArrowMask::shared()->setMask(CCRect(0, 140, 640, 685));
        }
        else if(GameConfig::shared()->getTutorialProgress() == TutorialQiangHua)
        {
            CCPoint p = qianghua->getParent()->convertToWorldSpace(qianghua->getPosition());
            ArrowMask::shared()->setArrow(ccp(p.x-200, p.y-60));
            ArrowMask::shared()->setMask(CCRect(0, 140, 640, 685));
        }
        else if(GameConfig::shared()->getTutorialProgress() == TutorialTransformHero)
        {
            CCPoint p = zhuansheng->getParent()->convertToWorldSpace(zhuansheng->getPosition());
            ArrowMask::shared()->setArrow(ccp(p.x-200, p.y-60));
            ArrowMask::shared()->setMask(CCRect(0, 140, 640, 685));
        }

        else if(GameConfig::shared()->getTutorialProgress() == TutorialShuffleRelation)
        {
            CCPoint p = relChange->getParent()->convertToWorldSpace(relChange->getPosition());
            ArrowMask::shared()->setArrow(ccp(p.x-200, p.y-60));
            ArrowMask::shared()->setMask(CCRect(0, 140, 640, 685));
            
            AlertMessage::create("", "侠客缘分：恭喜！掌门你已提升至20级！【缘分重置】功能开启！");
        }
        else if(GameConfig::shared()->getTutorialProgress() == TutorialNeili)
        {
            CCPoint p = qianghua->getParent()->convertToWorldSpace(qianghua->getPosition());
            ArrowMask::shared()->setArrow(ccp(p.x-200, p.y-60));
            ArrowMask::shared()->setMask(CCRect(0, 140, 640, 685));
        }
        else if(GameConfig::shared()->getTutorialProgress() == TutorialTransformHero)
        {
            CCPoint p = qianghua->getParent()->convertToWorldSpace(zhuansheng->getPosition());
            ArrowMask::shared()->setArrow(ccp(p.x-200, p.y-60));
            ArrowMask::shared()->setMask(CCRect(0, 140, 640, 685));
        }
    }
    
    void XiaKe::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
    {
        isClick = true;
    }
    
    void XiaKe::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
    {
        CCTouch *touch = (CCTouch *)pTouches->anyObject();
        CCPoint currPoint = touch->getLocationInView();
        CCPoint prevPoint = touch->getPreviousLocationInView();
        
        if( ccpDistance(currPoint, prevPoint) > 5)
            isClick = false;
    }
    
    void XiaKe::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
    {
        CCTouch *touch = (CCTouch *)pTouches->anyObject();
        if(!isClick)
            return;

        for (int i = 0; i < items.size(); i++)
        {
            CCPoint localPoint = items[i]->getParent()->convertTouchToNodeSpace(touch);
            if(items[i]->boundingBox().containsPoint(localPoint) &&
               items[i]->getTag())
            {
                this->enableAllButtons();
                ArrowMask::shared()->clear();
                scrollView->setScrollEnabled(true);
                
                auto cLayer = ((ContentCreatorFunc)items[i]->getUserData())();
                GameScene::getInstance()->pushLayerWithDefaultAnimation(cLayer);
                ring("sound/v/button_click.mp3");
            }
        }
        isClick = false;
    }
    
}
