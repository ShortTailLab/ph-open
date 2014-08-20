//
//  HeroSelectionLayer.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 1/5/13.
//
//

#include "HeroSelectionLayer.h"
#include "PlayerData.h"
#include "GameScene.h"
#include "UIPresets.h"
#include "network/PuzzleRPC.h"
#include "ControlExtensions.h"
#include "PageScrollView.h"
#include "Common.h"

namespace PH
{
    bool HeroJuanZhou::init(HeroPtr hero)
    {
        if(!ContentLayer::init())
            return false;
        
        viewPort = CCSize(20 , 240);
        
        CCSprite *bg = GetSprite("gameui/renwuxuanze1_3.png");
        bg->setAnchorPoint(ccp(0.5 , 1));
        addChild(bg);
        
        addLabelTTF(this, pSkillLabel , "" , FONT_CN , 28 ,  ccp(-68,-26) , ccp(0,1) , ccc3(108, 46, 0));
        pSkillDetailLabel = LabelX::create("", FONT_CN, 28, LabelX::FontEffect::SHADOW, ccp(2, -2));
        pSkillDetailLabel->setPosition(ccp(-155,-60));
        pSkillDetailLabel->setAnchorPoint(ccp(0, 1));
        this->addChild(pSkillDetailLabel);
        
        addLabelTTF(this, aSkillLabel, "", FONT_CN , 28 ,  ccp(-68,-94) , ccp(0,1) , ccc3(108, 46, 0));
        aSkillDetailLabel = LabelX::create("", FONT_CN, 28, LabelX::FontEffect::SHADOW, ccp(2, -2));
        aSkillDetailLabel->setPosition(ccp(-155,-126));
        aSkillDetailLabel->setAnchorPoint(ccp(0, 1));
        this->addChild(aSkillDetailLabel);
        
        addLabelBMFont(this, hpLabel ,"" , "bmfont/Sumo_22_yellow.fnt",
                       ccp(-185 , -185) ,ccp(0 , 0.5));
        hpLabel->setScale(contentScale());
        addLabelBMFont(this, attackLabel ,"" , "bmfont/Sumo_22_yellow.fnt",
                       ccp(-5 , -185) ,ccp(0 , 0.5));
        attackLabel->setScale(contentScale());
        addLabelBMFont(this, regenLabel ,"" , "bmfont/Sumo_22_yellow.fnt",
                       ccp(180 , -185) ,ccp(0 , 0.5));
        regenLabel->setScale(contentScale());
        switchHero(hero);
        
        return true;
    }
    
    void HeroJuanZhou::switchHero(HeroPtr hero)
    {
        {
            if(icon)
                icon->removeFromParentAndCleanup(true);
            icon = HeroIcon::create(hero.get());
            icon->setPosition(ccp(-230, -110));
            addChild(icon);
        }
        
        {
            std::string askill = hero->profile->activeSkillId == -1 ? "无" : AST(hero->profile->activeSkillId).name;
            std::string askillDetail = hero->profile->activeSkillId == -1 ? "无技能" : AST(hero->profile->activeSkillId).desc;
            std::string pskill = hero->profile->passiveSkillId == -1 ? "无" : PST(hero->profile->passiveSkillId).name;
            std::string pskillDetail = hero->profile->passiveSkillId == -1 ? "无技能" : PST(hero->profile->passiveSkillId).desc;
            std::string energy = hero->profile->activeSkillId == -1? "" : toStr(AST(hero->profile->activeSkillId).energy);
            std::string coolDown = hero->profile->activeSkillId == -1 ? "" : toStr(AST(hero->profile->activeSkillId).maxCooldown);
            
            pSkillLabel->setString(pskill.c_str());
            pSkillDetailLabel->setString(pskillDetail.c_str());
            aSkillLabel->setString(askill.c_str());
            aSkillDetailLabel->setString(askillDetail.c_str());
        }
        
        {
            hpLabel->setString(toStr(hero->health).c_str());
            attackLabel->setString(toStr(hero->attack).c_str());
            regenLabel->setString(toStr(hero->regen).c_str());
        }
    }
    
    bool HeroSelectionLayer::init()
    {
        if(!ContentLayer::init())
            return false;
        
        curId = 0;
        
        CCSprite* bg = GetSprite("pvp/pvp_bg.jpg");
        bg->setAnchorPoint(ccp(0, 0));
        bg->setPositionY(150);
        bg->setColor(ccc3(150, 150, 150));
        this->addChild(bg);
        
        addLabelTTF(this, "请选择一位侠客与你共闯江湖吧！", FONT_CN, 34, ccp(320, 930));
        CCSprite* nameBg = GetSprite("gameui/mingzi_bg.png");
        nameBg->setPosition(ccp(320, 820));
        addChild(nameBg, 100);
        CCSize bgSize = nameBg->getContentSize();
        nameLabel = addLabelTTF(nameBg, "小宝", FONT_CN, 38,
                                ccp(bgSize.width*.5f, bgSize.height*0.5f), ccp(0.5, 0.5));
        
        std::vector<cocos2d::CCNode*> heroImgs;
        heroImgs.push_back(GetSprite("gameui/weixiaobao_r.png"));
        heroImgs.push_back(GetSprite("gameui/yuelingshan_r.png"));
        heroImgs.push_back(GetSprite("gameui/chenjialuo_r.png"));
        heroImgs.push_back(GetSprite("gameui/chenglingsu_r.png"));
        heroImgs.push_back(GetSprite("gameui/hufei_r.png"));
        heroImgs.push_back(GetSprite("gameui/yilin_r.png"));
        
        RoundPageScrollView* scrollView = RoundPageScrollView::create(heroImgs, 320);
        scrollView->setPosition(ccp(320, 780));
        scrollView->setCallBack([=](int id)
        {
            juanzhou->switchHero(HeroPtr(new Hero("1", InitialHeroIds[id], 1, 0)));
            curId = id;
            nameLabel->setString(getHeroName(id).c_str());
        });
        addChild(scrollView);
        
        ButtonSprite* btn = ButtonSprite::create(createBtn("选此侠客", ButtonType::FOUR_WORD_RED),
                                                 createBtn("选此侠客", ButtonType::FOUR_WORD_RED));
        btn->setOnClickListener([=](){
            confirmHeroSelection(InitialHeroIds[curId]);
        });
        btn->setPosition(ccp(320, 320));
        addChild(btn);
        
        juanzhou = HeroJuanZhou::create(HeroPtr(new Hero("1", InitialHeroIds[0], 1, 0)));
        CCSprite* zhou1 = GetSprite("gameui/renwuxuanze1_1.png");
        CCSprite* zhou2 = GetSprite("gameui/renwuxuanze1_1.png");
        zhou2->setFlipX(true);
        jz = JuanZhouLayer::create();
        jz->addSkin(juanzhou, zhou1, zhou2);
        jz->setPosition(ccp(320, 270));
        addChild(jz);
        
        return true;
    }
    
    std::string HeroSelectionLayer::getHeroName(int heroId)
    {
        if(heroId == 0)
            return "小宝";
        else if(heroId == 1)
            return "仪琳儿";
        else if(heroId == 2)
            return "胡小刀";
        else if(heroId == 3)
            return "成灵素";
        else if(heroId == 4)
            return "家洛";
        else if(heroId == 5)
            return "小师妹";
        assert("heroid must in range");
        return "";
    }
    
    void HeroSelectionLayer::onEnterAnimationDone()
    {
        jz->startAnim();
    }
    
    void HeroSelectionLayer::confirmHeroSelection(int heroId)
    {
        SystemInform* inform = SystemInform::alertLoading(this);
        
        PuzzleRPC::getInstance()->pickInitialHero(PlayerData::getInstance()->getToken(),
                                                  heroId,
                                                  [=](std::auto_ptr<Hero> h,
                                                      std::auto_ptr<Error> error)
        {
            inform->cancel();
            
            if(error.get())
            {
                LOGD("!!! Pick initial hero error msg: %s", error->toStr().c_str());
                SystemInform::alertMessage(GameScene::getInstance(),
                                           error->toStrCN().c_str(),
                                           "警告" ,
                                           [=](){});
                return;
            }
            
#if DISABLE_TUTORIAL
            if(remoteProgress != TutorialSkip){
                boost::function<void()>* funcPtr = new boost::function<void()>();
                (*funcPtr) = [=]()
                {
                    SystemInform* loading = SystemInform::alertLoading(this);
                    PuzzleRPC::getInstance()->setTutorialProgress(PlayerData::getInstance()->getToken(),
                                                                  0,
                                                                  [=](std::auto_ptr<Error> e)
                    {
                        loading->cancel();
                        GameConfig::shared()->setTutorialProgress(TutorialSkip);
                        
                        clearUIAndReLogin(0);
                        
                        delete funcPtr;
                    });
                };
                (*funcPtr)();
            }
#endif

            Hero* hero = h.release();
            PlayerData::getInstance()->addHero(hero);
            PlayerData::getInstance()->player->team[0] = hero;

            if(GameConfig::shared()->getTutorialProgress())
            {
                // assume server set progress to TutorialFirstBattle after initial pick
                GameConfig::shared()->setTutorialProgress(TutorialFirstBattle);
            }
            
            // init game scene stuff
            GameScene::getInstance()->initBG();
            GameScene::getInstance()->initMsgServer();
            GameScene::getInstance()->initMenuUI();
            GameScene::getInstance()->startMenu();
            GameScene::getInstance()->scheduleRelogin();
            this->removeFromParentAndCleanup(true);
        });
    }
    
    void HeroSelectionLayer::cancelHeroSelection(CCObject* o)
    {
        confirmLayer->removeFromParentAndCleanup(true);
        confirmLayer = NULL;
        
        //heroSelectionMenu->setEnabled(true);
    }
}