//
//
//  Created by ZQL on 11.29
//
//

#include "HeroProfileLayer.h"
#include "GameScene.h"
#include "UIPresets.h"
#include "HeroIcon.h"
#include <support/user_default/CCUserDefault.h>
#include "PlayerData.h"
#include <extensions/utils.h>
#include "Board/Common.h"

using namespace std;

namespace PH
{
    
bool HeroRelationItem::init(const HeroRelation& _relation)
{
    if(!CCSprite::initWithFile("gameui/yuanfen_hong_heng.png"))
        return false;
    
    relation = _relation;
    
    item2 = GetSprite("gameui/yuanfen_lv_heng.png");
    item2->setPosition(ccp(item2->getContentSize().width*0.5f,item2->getContentSize().height*0.5f));
    item2->setVisible(false);
    addChild(item2);
    
    LabelX *rLabel = LabelX::create(relation.name.c_str(), FONT_CN, 26);
    rLabel->setPosition(ccp(item2->getPosition().x, item2->getPosition().y+8));
    addChild(rLabel);

    if(relation.withHeroes.size() > 0)
    {
        LabelX *rLabel2 = LabelX::create( ("("+HPT(relation.withHeroes[0]).nameCN+")").c_str(), FONT_CN, 16);
        rLabel2->setPosition(ccp(item2->getPosition().x, item2->getPosition().y-12));
        addChild(rLabel2);
    }
    
    return true;
}

void HeroRelationItem::onPress(bool value)
{
    item2->setVisible(value);
    //float s = value ? 0.8 : 1;
    //setScale(s);
}
    

USING_NS_CC;
    
bool ProfileJuanZhou::init(Hero *hero)
{
	if(!JuanZhou::init())
        return false;
    
	heroNum = 1;
	targetHero = hero;
	viewPort = CCSize(20 , 465);
    relationBoard = NULL;
    tempRItem = NULL;
    isShowNormal = true;
    heroType = NULL;
    scheduleFlag = false;
    isHero = !hero->profile->isMat();
    labels = ContentLayer::create();
    addChild(labels);
    
        
    CCSprite *bg = GetSprite("gameui/shuxing_1_3.png");
	bg->setAnchorPoint(ccp(0.5 , 1));
	addChild(bg, -1);
    
    bg2 = GetSprite("gameui/shuxing_1_4.png");
    bg2->setAnchorPoint(ccp(0.5 , 1));
    addChild(bg2, -1);

	locations.push_back(CCRect(-265 , -133 , 100 , 100 ));
	heroVec.push_back(NULL);
	heroImgVec.push_back(NULL);

	addHero(hero , 0);
	  
    {
        addLabelBMFont(this , hpLabel , toStr(hero->health).c_str() , "bmfont/Sumo_22_yellow.fnt",
                       ccp(-85 , -50) , ccp(0 , 0.5));
        hpLabel->setScale(contentScale());
        
        addLabelBMFont(this , attackLabel , toStr(hero->attack).c_str() ,  "bmfont/Sumo_22_yellow.fnt",
                       ccp(-85 , -85) , ccp(0 , 0.5));
        attackLabel->setScale(contentScale());
        
        addLabelBMFont(this , regenLabel , toStr(hero->regen).c_str() ,  "bmfont/Sumo_22_yellow.fnt",
                    ccp(-85 , -120) , ccp(0 , 0.5));
        regenLabel->setScale(contentScale());
        
        addLabelBMFont(this , levelLabel , (toStr(hero->level)+"/"+toStr(hero->profile->maxLevel)).c_str() ,
                        "bmfont/Sumo_22_yellow.fnt",ccp(200 , -50) , ccp(0.5 , 0.5));
        levelLabel->setScale(contentScale());
        
        addLabelBMFont(this , lpLabel , toStr(hero->profile->LP).c_str(),  "bmfont/Sumo_22_yellow.fnt",
                       ccp(200 , -85) , ccp(0.5 , 0.5));
        lpLabel->setScale(contentScale());
        
    }
    
    {
        addLabelBMFont(this,hpPlusLabel,"","bmfont/Sumo_22_purple.fnt", ccp(0,0),ccp(0,0.5));
        hpPlusLabel->setScale(contentScale());
        addLabelBMFont(this,attackPlusLabel,"","bmfont/Sumo_22_purple.fnt", ccp(0,0),ccp(0,0.5));
        attackPlusLabel->setScale(contentScale());
        addLabelBMFont(this,regenPlusLabel,"","bmfont/Sumo_22_purple.fnt", ccp(0,0),ccp(0,0.5));
        regenPlusLabel->setScale(contentScale());
    }
    switchHero(hero);
    
    setTouchEnabled(true);
    
	return true;
}
    
void ProfileJuanZhou::initHeroRelations(const Hero* hero)
{
    if(relations)
    {
        relations->removeAllChildrenWithCleanup(true);
        relations = NULL;
    }
    relationItems.clear();
    tempRItem = NULL;
    
    if(isHero)
    {
        relations = CCLayer::create();
        addChild(relations);
        
        relationBoard = GetSprite("gameui/yuanfen_lv_neirong_chang.png");
        relationBoard->setPositionY(-250);
        relationBoard->setVisible(false);
        relations->addChild(relationBoard , 10);
        
        addLabelTTF(relationBoard, rName, "", FONT_CN, 36, ccp(45 , 195) , ccp(0 , 0.5));
        addLabelTTF(relationBoard, rIsReady, "", FONT_CN, 30, ccp(550 , 152), ccp(1 , 0.5) , ccBLACK);
        addLabelTTF(relationBoard, rEffect, "", FONT_CN, 28, ccp(45 , 50), ccp(0 , 0.5));
        
        rHeroesLayer = CCLayer::create();
        rHeroesLayer->setPosition(ccp(45 , 120));
        relationBoard->addChild(rHeroesLayer);
        
        relats = hero->relations;
        for(int i = 0 ; i < relats.size() ; i++)
        {
            HeroRelationItem* item = HeroRelationItem::create(relats[i]);
            item->setPosition(ccp(-143 + 164*i , -398));
            relations->addChild(item);
            relationItems.push_back(item);
        }
    }

}
    
void ProfileJuanZhou::updateRelationBoard(const HeroRelation* r)
{
    if(r)
    {
        PlayerData *data = PlayerData::getInstance();
        
        relationBoard->setVisible(true);
        rName->setString(r->name.c_str());
        std::vector<int> heroes = r->withHeroes;
        int l = 0;
        bool isReady = true;
        for(int i = 0 ; i < heroes.size() ; i++)
        {
            int id = heroes[i];
            CCLabelTTF *label;
            addLabelTTF(rHeroesLayer, label, HPT(id).nameCN.c_str(), FONT_CN, 28, ccp(l , 0) , ccp(0 , 0.5));
            label->setColor(ccc3(100, 100, 100));
            l += label->getContentSize().width + 10;
            int id2;
            for( id2 = id ; id2 != -1 ; id2 = HPT(id2).transformTargetHeroId)
                if(data->getHeroByHeroId(id2))
                {
                    label->setColor(ccc3(255 , 255 , 255));
                    break;
                }
            if(id2 == -1)isReady = false;

        }
        
        isReady ? rIsReady->setString("") : rIsReady->setString("同时登场触发");
            
        std::string effectStr;
        for(boost::tuple<RelationEffects, double> t : r->effects)
        {
            boost::get<0>(t) == RelationEffects::HP ? effectStr += "血量增加" :
                                (boost::get<0>(t) == RelationEffects::STRENGTH ? effectStr += "攻击增加" : effectStr += "恢复增加");
            
            effectStr += (toStr(int(boost::get<1>(t)*100)) + "% ");
        }
        rEffect->setString(effectStr.c_str());
    }
    else
    {
        relationBoard->setVisible(false);
        rName->setString("");
        rEffect->setString("");
        rHeroesLayer->removeAllChildrenWithCleanup(true);
    }
    
}
    
void ProfileJuanZhou::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    CCPoint point = convertTouchToNodeSpace((CCTouch*)pTouches->anyObject());
    
    for (int i = 0 ; i <  relationItems.size() ; i++)
        if(relationItems[i]->boundingBox().containsPoint(point))
        {
            tempRItem = relationItems[i];
            tempRItem->onPress(true);
            updateRelationBoard(&relats[i]); 
        }
}
void  ProfileJuanZhou::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    CCPoint point = convertTouchToNodeSpace((CCTouch*)pTouches->anyObject());
    if (tempRItem && !tempRItem->boundingBox().containsPoint(point))
    {
        tempRItem->onPress(false);
        tempRItem = NULL;
        updateRelationBoard(NULL);
    }
}
void ProfileJuanZhou::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    if (tempRItem) {
        tempRItem->onPress(false);
        tempRItem = NULL;
        updateRelationBoard(NULL);
    }
}

void ProfileJuanZhou::switchHero(Hero *hero)
{
	PlayerData *data = PlayerData::getInstance();
	std::vector<Hero*> s = data->getStorage();
    isHero = !hero->profile->isMat();
    bg2->setVisible(!isHero);
	switchHeroImg(hero);
	
    int eliteHp = hero->eliteLevel ? hero->profile->eliteHP.eval(hero->eliteLevel): 0;
    int eliteAtk = hero->eliteLevel ? hero->profile->eliteAttack.eval(hero->eliteLevel) : 0;
    int eliteReg = hero->eliteLevel ? hero->profile->eliteRegen.eval(hero->eliteLevel) : 0;
    
	hpLabel->setString(toStr(hero->health - eliteHp).c_str());
	attackLabel->setString(toStr(hero->attack - eliteAtk).c_str());
	regenLabel->setString(toStr(hero->regen - eliteReg).c_str());
	levelLabel->setString((toStr(hero->level)+"/"+toStr(hero->profile->maxLevel)).c_str());
    lpLabel->setString(toStr(hero->profile->LP).c_str());

    if(heroType)
    {
        heroType->removeFromParentAndCleanup(true);
        heroType = NULL;
    }
    
    if(!hero->profile->type.empty())
    {
        heroType = getTypeImg(hero->profile->type);
        if(heroType)
        {
            heroType->setPosition(ccp(206, -118));
            addChild(heroType);
        }
    }
    
    if(hero->eliteLevel)
    {
        hpPlusLabel->setString( ("+"+toStr(eliteHp)).c_str() );
        hpPlusLabel->setPosition(ccp(hpLabel->getPosition().x+hpLabel->getContentSize().width*
                                     contentScale()+5, hpLabel->getPosition().y));
        
        attackPlusLabel->setString( ("+"+toStr(eliteAtk)).c_str() );
        attackPlusLabel->setPosition(ccp(attackLabel->getPosition().x+attackLabel->getContentSize().width*
                                         contentScale()+5,attackLabel->getPosition().y));
        
        regenPlusLabel->setString( ("+"+toStr(eliteReg)).c_str());
        regenPlusLabel->setPosition(ccp(regenLabel->getPosition().x+
                                        regenLabel->getContentSize().width*contentScale()+5,
                                        regenLabel->getPosition().y));
    }
    else
    {
        hpPlusLabel->setString(" ");
        attackPlusLabel->setString(" ");
        regenPlusLabel->setString(" ");
    }

    updateSkillType(hero);
    initHeroRelations(hero);
}
    
    CCSprite* ProfileJuanZhou::getTypeImg(std::string type)
    {
        string path;
        if(type == "攻恢型")
            path = "gameui/gonghui.png";
        else if(type == "攻击型")
            path = "gameui/gongji.png";
        else if(type == "攻血型")
            path = "gameui/gongxue.png";
        else if(type == "恢复型")
            path = "gameui/huifu.png";
        else if(type == "恢血型")
            path = "gameui/huixue.png";
        else if(type == "均衡型")
            path = "gameui/junheng.png";
        else if(type == "强攻型")
            path = "gameui/qianggong.png";
        else if(type == "血量型")
            path = "gameui/xueliang.png";
        else if(type == "转生道具")
            path = "gameui/zhuanshengdaoju.png";
        else if(type == "强化道具")
            path = "gameui/qianghuadaoju.png";
        else if(type == "集气道具")
            path = "gameui/jiqidaoju.png";
        else if(type == "缘分道具")
            path = "gameui/yuanfendaoju.png";
        else
            return NULL;
        
        return GetSprite(path);
    }
    
void ProfileJuanZhou::updateSkillType(Hero* hero)
{
    std::string askill;
    std::string askillDetail;
    std::string pskill;
    std::string pskillDetail;
    std::string coolDownNum;
    std::string energy;
    
    if(isShowNormal)
    {
        askill = hero->profile->activeSkillId == -1 ? "无" : AST(hero->profile->activeSkillId).name;
        askillDetail = hero->profile->activeSkillId == -1 ? "无技能" : AST(hero->profile->activeSkillId).desc;
        pskill = hero->profile->passiveSkillId == -1 ? "无" : PST(hero->profile->passiveSkillId).name;
        pskillDetail = hero->profile->passiveSkillId == -1 ? "无技能" : PST(hero->profile->passiveSkillId).desc;
        coolDownNum =  hero->profile->activeSkillId == -1? "0" : toStr(AST(hero->profile->activeSkillId).maxCooldown);
        energy = hero->profile->activeSkillId == -1? "0" : toStr(AST(hero->profile->activeSkillId).energy+hero->skillBonus);
    }
    else
    {
        askill = hero->profile->pvpActiveSkillId == -1 ? "无" : AST(hero->profile->pvpActiveSkillId).name;
        askillDetail = hero->profile->pvpActiveSkillId == -1 ? "无技能" : AST(hero->profile->pvpActiveSkillId).desc;
        pskill = hero->profile->pvpPassiveSkillId == -1 ? "无" : PST(hero->profile->pvpPassiveSkillId).name;
        pskillDetail = hero->profile->pvpPassiveSkillId == -1 ? "无技能" : PST(hero->profile->pvpPassiveSkillId).desc;
        coolDownNum =  hero->profile->pvpActiveSkillId == -1? "0" : toStr(AST(hero->profile->pvpActiveSkillId).maxCooldown);
        energy = hero->profile->pvpActiveSkillId == -1? "0" : toStr(AST(hero->profile->pvpActiveSkillId).energy+hero->skillBonus);
    }
    
    unschedule(schedule_selector(ProfileJuanZhou::schedule2Sec));
    labels->removeAllChildrenWithCleanup(true);
    if(isHero)
    {
        if(PlayerData::getInstance()->isPvPOpen)
        {
            normalSkillBtn = ButtonSprite::create("gameui/jianghuji_b.png", "gameui/jianghuji_a.png");
            normalSkillBtn->setPosition(ccp(250, -200));
            normalSkillBtn->setOnClickListener([=]()
               {
                   if(!isShowNormal)
                   {
                       isShowNormal = true;
                       normalSkillBtn->onPressed();
                       pvpSkillBtn->onRelease();
                       this->updateSkillType(targetHero);
                   }
               });
            
            pvpSkillBtn = ButtonSprite::create("gameui/leitaiji_b.png", "gameui/leitaiji_a.png");
            pvpSkillBtn->setPosition(ccp(250, -300));
            pvpSkillBtn->setOnClickListener([=](){
                if(isShowNormal)
                {
                    isShowNormal = false;
                    normalSkillBtn->onRelease();
                    pvpSkillBtn->onPressed();
                    this->updateSkillType(targetHero);
                }
            });
            labels->addChild(pvpSkillBtn);
            isShowNormal ? normalSkillBtn->onPressed() : pvpSkillBtn->onPressed();
        }
        else
        {
            normalSkillBtn = ButtonSprite::create("gameui/jianghuji_b.png",
                                                  "gameui/jianghuji_a.png",
                                                  kCCMenuHandlerPriority,
                                                  false);
            normalSkillBtn->setPosition(ccp(250, -200));
        }
        labels->addChild(normalSkillBtn);
        
        addLabelTTF(labels , pSkillLabel , pskill.c_str() , FONT_CN , 28 ,  ccp(-150,-153) ,
                    ccp(0,1) , ccc3(108, 46, 0));
        pSkillDetailLabel = LabelX::create(pskillDetail.c_str(), FONT_CN, 24,
                                           LabelX::FontEffect::SHADOW, ccp(2, -2));
        pSkillDetailLabel->setDimensions(CCSizeMake(460, 70));
        pSkillDetailLabel->setPosition(ccp(-265,-193));
        pSkillDetailLabel->setAnchorPoint(ccp(0, 1));
        labels->addChild(pSkillDetailLabel);
        
        addLabelTTF(labels , aSkillLabel , askill.c_str() , FONT_CN , 28 ,  ccp(-150,-260) , ccp(0,1) , ccc3(108, 46, 0));
        //addLabelTTF(this , aSkillDetailLabel , "" , FONT_CN , 24 ,  ccp(-265,-295) , ccp(0,1));
        //aSkillDetailLabel->setDimensions(CCSizeMake(200, 70));
        aSkillDetailLabel = LabelX::create(askillDetail.c_str(), FONT_CN, 24,
                                           LabelX::FontEffect::SHADOW, ccp(2, -2));
        aSkillDetailLabel->setDimensions(CCSizeMake(460, 70));
        aSkillDetailLabel->setPosition(ccp(-265,-295));
        aSkillDetailLabel->setAnchorPoint(ccp(0, 1));
        labels->addChild(aSkillDetailLabel);
        
        addLabelTTF(labels , aSkillCoolDown, coolDownNum.c_str() , FONT_NUM , 24 ,  ccp(115,-259) , ccp(1,1));
        addLabelTTF(labels , aSkillEnergy, energy.c_str() , FONT_NUM , 24 ,  ccp(220,-259) , ccp(1,1));
        
        scheduleFlag = false;
        if(hero->maxEnergy != 0)
        {
            schedule(schedule_selector(ProfileJuanZhou::schedule2Sec), 2);
        }
    }
    else
    {
        string content = targetHero->profile->dropHint;
        sourceLabel = LabelX::create(content.c_str(), FONT_CN, 24, LabelX::FontEffect::SHADOW, ccp(2, -2));
        sourceLabel->setDimensions(CCSizeMake(460, 70));
        sourceLabel->setPosition(ccp(-265,-193));
        sourceLabel->setAnchorPoint(ccp(0, 1));
        labels->addChild(sourceLabel);
        
        LabelX* usageLabel = LabelX::create(hero->profile->usage.c_str(), FONT_CN, 24,
                                            LabelX::FontEffect::SHADOW, ccp(2, -2));
        usageLabel->setDimensions(CCSizeMake(460, 70));
        usageLabel->setPosition(ccp(-265,-340));
        usageLabel->setAnchorPoint(ccp(0, 1));
        labels->addChild(usageLabel);
    }

}
    
void ProfileJuanZhou::schedule2Sec()
{
    if(isHero)
    {
        if(!scheduleFlag)
        {
            int value = targetHero->skillBonus;
            string plus = value >= 0 ? "+" : "";
            aSkillEnergy->setString((plus+toStr(value)).c_str());
            aSkillEnergy->setColor(ccGREEN);
        }
        else
        {
            aSkillEnergy->setString(toStr(targetHero->maxEnergy+targetHero->skillBonus).c_str());
            aSkillEnergy->setColor(ccWHITE);
        }
        scheduleFlag = !scheduleFlag;
    }
}
    
void ProfileJuanZhou::switchHeroImg(Hero *hero)
{
	targetHero = hero;
	removeHeroById(0);
	addHero(hero , 0);
}

void ProfileJuanZhou::addHero(PH::Hero *hero , int id)
{
	HeroIcon *icon = HeroIcon::create(hero, true);
	addHeroImg(icon, id, true);
}


bool HeroProfileLayer::init(Hero *hero , bool isShowStorage , bool isShowSelect)
{
	if(!initCommon(hero->profile , isShowStorage , isShowSelect))
        return false;

	targetHero = hero;
    
    if(isShowStorage){
        initScrollArrow();
    }

	std::vector<Hero*> heroes = isShowStorage ? PlayerData::getInstance()->getStorage() : std::vector<Hero*>(1 , hero);
    int index = isShowStorage ? PlayerData::getInstance()->getHeroIndex(hero) : 0 ;
	initScrollViewWithHeroes(heroes, index);
    initHeroStasScroll(ProfileJuanZhou::create(hero));
	
	PlayerData::getInstance()->registerStorageCallBack(this , [=](){

		for(int i = 0 ; i < heroBodys.size() ; i++)
        {
			heroBodys[i]->setUserData(PlayerData::getInstance()->getStorage()[i]);
		}
	});

	return true;
}
    
void HeroProfileLayer::onExit()
{
    ContentLayer::onExit();
    
    PlayerData::getInstance()->unRegisterStorageCallBack(this);
    
    if(GameConfig::shared()->getTutorialProgress() == TutorialProfile)
    {
        ArrowMask::shared()->clear();
        GameConfig::shared()->setTutorialProgress(TutorialProfileDone);
    }
}
    
bool HeroProfileLayer::init(const HeroProfile *heroProfile)
{
    if(!initCommon(heroProfile , false))
        return false;
    
    targetHero = NULL;
    
    // TODO: get rid of this fake hero
    fakeHero.reset(new Hero("", heroProfile->tableId, 1, 0));
    fakeHero->isLock = false;
    std::vector<Hero*> heroes;
    heroes.push_back(fakeHero.get());
    initScrollViewWithHeroes(heroes, 0);
    initHeroStasScroll(ProfileJuanZhou::create(fakeHero.get()));
    
    return true;
}
    
bool HeroProfileLayer::initCommon(const HeroProfile *heroProfile , bool isShowStorage , bool isShowSelect)
{
    if( !ContentLayer::init())
        return false;
    
    arrow1 = arrow2 = NULL;
    titleImg = NULL;
    canLock = true;
    this->isShowStorage = isShowStorage;
    selectCall = [](){};
    
    CCSprite *bg = GetSprite("gameui/jiuguan.png");
	bg->setAnchorPoint(ccp(0.5 , 1));
	addChild(bg);
    
	CCSprite *bg2 = GetSprite("gameui/1x1_b.png");
	bg2->setAnchorPoint(ccp(0.5 , 0));
	bg2->setTextureRect(CCRectMake(0 , 0 , 640 , 413));
	bg2->setPosition(ccp(0 , -960));
	addChild(bg2);
    
	topMenu = CCMenu::create();
	itemLeft = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                        GetSprite("gameui/back_t.png"),
                                        this,
                                        menu_selector(HeroProfileLayer::onBackClick));
	topMenu->addChild(itemLeft);
    
	CCMenuItem *itemMiddle = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                      GetSprite("gameui/hongchengdi_t_tongyong.png"));
    
	heroName = LabelX::create(heroProfile->nameCN.c_str(), FONT_CN, 36);
    heroName->setColor(ccWHITE);
	heroName->setAnchorPoint(ccp(0.5 , 0.5));
	heroName->setPosition(ccp(itemMiddle->getContentSize().width/2 + 2,
                              itemMiddle->getContentSize().height/2 + 2));
	itemMiddle->addChild(heroName);
	topMenu->addChild(itemMiddle);
    
    if(isShowStorage)
    {
        itemRight = CCMenuItemSprite::create(GetSprite("gameui/chengdi_t_tongyong_l.png"),
                                             GetSprite("gameui/chengdi_t_tongyong_l.png"),
                                             this,
                                             menu_selector(HeroProfileLayer::onLockClick));
        
    }
    else if(isShowSelect)
    {
        itemRight = CCMenuItemSprite::create(createBtn("选择", ButtonType::NORMAL_BLUE,TITLE_YELLOW),
                                             createBtn("选择",ButtonType::NORMAL_BLUE, TITLE_YELLOW),
                                             this,
                                             menu_selector(HeroProfileLayer::onSelect));
    }
    else
    {
        itemRight = CCMenuItemSprite::create(GetSprite("gameui/chengdi_t_tongyong_l.png"),
                                             GetSprite("gameui/chengdi_t_tongyong_l.png"));
        itemRight->setVisible(false);
    }
    topMenu->addChild(itemRight);
	topMenu->setPosition(ccp(0, -50));
    topMenu->alignItemsHorizontallyWithPadding(20);
	addChild(topMenu);
    
    {
        //add hero number 
        addLabelTTF(this, numLabel,("NO . "+toStr(HPTN(heroProfile)+1)).c_str(), FONT_NUM, 24, ccp(-220, -100),ccp(0.5, 0.5));
        addLabelTTF(this, fightValueLabel, "", FONT_CN, 24, ccp(220, -100), ccp(0.5, 0.5));
    }
    
    {
		star = CCLayer::create();
		star->setPositionY(-100);
		addChild(star);
	}

	showStar(heroProfile);
    
	isShowBottom = false;
    isShowTop = false;
    
    return true;
}
    
void HeroProfileLayer::initScrollViewWithHeroes(const std::vector<Hero*>& heroes, int current)
{
    /*for(Hero* h : heroes)
    {
		CCSprite *img = makeHeroImg(h);
		img->setAnchorPoint(ccp(0.5, 0));
		heroBodys.push_back(img);
	}*/
    
	sv = HeroPageScrollView::create(heroes , CCSize(winSize.width/2, 400));
	sv->setPosition(ccp(0 , -465));
	sv->setCurrentNode(current);
	sv->setCallBack( [=](CCSprite* node)
    {
		Hero *h = (Hero*)node->getUserData();
		targetHero = h;
		juanzhou->switchHero(h);
		heroName->setString(h->profile->nameCN.c_str());
        numLabel->setString(("NO . "+toStr(HPTN(h->profile)+1)).c_str());
        fightValueLabel->setString(("战力："+toStr(h->getFightSkill())).c_str());
        
        if(titleImg)
            titleImg->removeFromParentAndCleanup(true);
        titleImg = this->getTitleImg(h->profile->title);
        if(titleImg)
        {
            titleImg->setPosition(ccp(-220, -145));
            this->addChild(titleImg);
        }

		if(isShowStorage)
            lock(h->isLock);
        
		showStar(h->profile);
	});
	this->addChild(sv);
}
    
CCSprite* HeroProfileLayer::getTitleImg(std::string title)
{
    string path;
    if(title == "博学")
        path = "gameui/boxueliu.png";
    else if(title == "刀剑")
        path = "gameui/daojianliu.png";
    else if(title == "内功")
        path = "gameui/neigongliu.png";
    else if(title == "拳掌")
        path = "gameui/quanzhangliu.png";
    else if(title == "阴阳")
        path = "gameui/yinyangliu.png";
    else
        return NULL;
    
    return GetSprite(path);
}
    
void HeroProfileLayer::initHeroStasScroll(ProfileJuanZhou* statsView)
{
    juanzhou = statsView;
    
    CCSprite *zhou1 = GetSprite("gameui/shuxing_1_1.png");
    
	CCSprite *zhou2 = GetSprite("gameui/shuxing_1_1.png");
    zhou2->setFlipX(true);
    
	jz = JuanZhouLayer::create();
	jz->addSkin(statsView , zhou1 , zhou2);
	jz->setPositionY(-460);
	this->addChild(jz);
}
    
void HeroProfileLayer::initScrollArrow()
{
    arrow1 = GetSprite("gameui/jiantou_xiake_shuxing.png");
    arrow1->setPosition(ccp(250 , -300));
    addChild(arrow1);
    
    arrow2 = GetSprite("gameui/jiantou_xiake_shuxing.png");
    arrow2->setFlipX(true);
    arrow2->setPosition(ccp(-250 , -300));
    addChild(arrow2);
}

void HeroProfileLayer::showStar(const HeroProfile* profile)
{
	star->removeAllChildrenWithCleanup(true);
    int starNum = profile->star;
    int maxStar = profile->getMaxStar();
    
	int l = 33;
	int startX = -l * (maxStar-1) * 0.5;

	for(int i = 0 ; i < maxStar ; i++)
    {
        std::string path = i < starNum ? "gameui/xingxing2.png" : "gameui/xingxing3.png";
        CCSprite *s = GetSprite(path);
        s->setPosition(ccp( startX + l*i , 0));
        star->addChild(s , 3);
	}
}

void HeroProfileLayer::onBackClick(CCObject *target)
{
    if(GameConfig::shared()->getTutorialProgress() == TutorialTeam)
        GameScene::getInstance()->selectTab(2);
    else
    {
        GameScene::getInstance()->popLayer();
        ring("sound/v/button_click.mp3");
    }
}

void HeroProfileLayer::onEnterAnimationDone()
{
	jz->startAnim();
    
    if(arrow1 && arrow2)
    {
        CCFiniteTimeAction *fade = CCFadeOut::create(1.5);
        CCFiniteTimeAction *fade2 = CCFadeOut::create(1.5);
        arrow1->runAction(fade);
        arrow2->runAction(fade2);
    }
    if(GameConfig::shared()->getTutorialProgress() == TutorialTeam)
    {
        ArrowMask::shared()->setArrow(ccp(100, 860), ArrowDirection::Up);
    }
    else if(GameConfig::shared()->getTutorialProgress() == TutorialProfile)
    {
        CCPoint p = inWorldSpace(itemLeft);
        p.y -= 40;
        ArrowMask::shared()->setArrow(p, ArrowDirection::Up);
        
        canLock = false;
    }
}

void HeroProfileLayer::onLockClick(CCObject *object)
{
    if (!targetHero || !canLock)
        return;
    
	targetHero->isLock = !targetHero->isLock;

	lock(targetHero->isLock);
	juanzhou->switchHeroImg(targetHero);
	CCUserDefault::sharedUserDefault()->setBoolForKey(("locked_"+targetHero->id).c_str(), targetHero->isLock);
	CCUserDefault::sharedUserDefault()->flush();
    
    //std::cout<<sv->retainCount()<<std::endl;
   // std::cout<<"this"<<this->retainCount()<<std::endl;
   // std::cout<<"content"<<sv->content->retainCount()<<std::endl;
    ring("sound/v/button_click.mp3");
}
    
void HeroProfileLayer::setSelectCall(boost::function<void ()> call)
{
    selectCall = call;
}
    
void HeroProfileLayer::onSelect(cocos2d::CCObject *target)
{
    selectCall();
}

void HeroProfileLayer::disableLock(bool value)
{
    canLock = !value;
}
    
void HeroProfileLayer::lock(bool value)
{
    if (!targetHero)
        return;

	CCSprite *tempImg;
    LabelX* title = LabelX::create("", FONT_CN, 34);
	if(value){
		tempImg = GetSprite("gameui/chengdi_t_tongyong_l.png");
        title->setString("解锁");
	}else{
		tempImg = GetSprite("gameui/chengdi_t_tongyong_h.png");
        title->setString("加锁");
	}
    title->setColor(ccWHITE);
    title->setPosition(ccp(tempImg->getContentSize().width*0.5, tempImg->getContentSize().height*0.5f+3));
    tempImg->addChild(title);
    
    if(itemRight){
        itemRight->setNormalImage(tempImg);
        itemRight->setSelectedImage(tempImg);
    }
}


}