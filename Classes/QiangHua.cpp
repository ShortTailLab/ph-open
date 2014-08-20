#include "QiangHua.h"
#include "PlayerData.h"
#include "GameScene.h"
#include "HeroIcon.h"
#include "boost/lexical_cast.hpp"
#include "network/PuzzleRPC.h"
#include "UIAnimation.h"
#include "CCCallLambda.h"
#include "UIPresets.h"
#include "ZqlScrollView.h"
#include "Common.h"

namespace PH
{
    USING_NS_CC;
    
QiangHua::QiangHua(void)
{
}   

QiangHua::~QiangHua(void)
{
}


bool QiangHuaJuanZhou::init()
{
	if( !JuanZhou::init())return false;

	viewPort = CCSize(20 , 367);
	heroNum = 6;
	sucBar= NULL;
    levelPlusLabel = NULL;
    hpPlusLabel = NULL;
    attackLabel = NULL;
    regenPlusLabel = NULL;

	background = GetSprite("gameui/qianghua_1_3.png");
	background->setAnchorPoint(ccp(0.5 , 1));
	addChild(background);
    
    heroVec = std::vector<Hero*>(heroNum, (Hero*)NULL);
    heroImgVec = std::vector<HeroIcon*>(heroNum, (HeroIcon*)NULL);
	
	for(int i = 0 ; i < heroNum ; i++)
    {
		if(i == 0)
			locations.push_back(CCRect(-274 , -155, 100 , 100 ));
		else
			locations.push_back(CCRect(-271+110.5*(i-1) , -330 , 100 , 100 ));
	}

    {
        expBar = Get9SpriteFromFrame("qianghua_1_4.png", 3, 3, 200, 4);
//        expBar = CCScale9Sprite::create("gameui/qianghua_1_4.png",
//                                        CCRect(0, 0, 300, 10),
//                                        CCRect(3, 4, 256, 4));
        
        expBar->setAnchorPoint(ccp(0 , 0.5));
        expBar->setPosition(ccp(66 , -70));
        expBar->setVisible(false);
        expBar->setContentSize(CCSize(206, 12));
        
        this->addChild(expBar);
    }
    
    {
        ownCoinsLabel = LabelX::create("", FONT_NUM, 24, LabelX::FontEffect::SHADOW, ccp(2, -2));
        ownCoinsLabel->setPosition(ccp(275, -138));
        ownCoinsLabel->setAnchorPoint(ccp(1, 0.5));
        ownCoinsLabel->setColor(ccWHITE);
        this->addChild(ownCoinsLabel);
        updateOwnCoinsNum();
        
        coinsLabel = LabelX::create("", FONT_NUM, 24, LabelX::FontEffect::SHADOW, ccp(2, -2));
        coinsLabel->setPosition(ccp(270, -214));
        coinsLabel->setAnchorPoint(ccp(1, 0.5));
        coinsLabel->setColor(TITLE_YELLOW);
        this->addChild(coinsLabel);
        
        eliteLabel = LabelX::create("", FONT_NUM, 24, LabelX::FontEffect::SHADOW, ccp(2, -2));
        eliteLabel->setPosition(ccp(275, -103));
        eliteLabel->setAnchorPoint(ccp(1, 0.5));
        eliteLabel->setColor(ccWHITE);
        this->addChild(eliteLabel);
        
        eliteMask = GetSprite("gameui/wufashengneili.png");
        eliteMask->setPosition(ccp(170, -100));
        eliteMask->setVisible(false);
        this->addChild(eliteMask);
    }

    {
        addLabelBMFont(this,levelLabel,"0","bmfont/Sumo_22_yellow.fnt", ccp(-92, -42),ccp(0,0.5));
        levelLabel->setScale(contentScale());   
        
        addLabelBMFont(this,hpLabel,"0","bmfont/Sumo_22_yellow.fnt", ccp(-92,-76),ccp(0,0.5));
        hpLabel->setScale(contentScale());
        
        addLabelBMFont(this,attackLabel,"0","bmfont/Sumo_22_yellow.fnt", ccp(-92,-110),ccp(0,0.5));
        attackLabel->setScale(contentScale());
        
        addLabelBMFont(this,regenLabel,"0","bmfont/Sumo_22_yellow.fnt", ccp(-92, -144),ccp(0,0.5));
        regenLabel->setScale(contentScale());
    }
    
    
    {
        addLabelBMFont(this,levelPlusLabel,"","bmfont/Sumo_22_green.fnt", ccp(0,0),ccp(0,0.5));
        levelPlusLabel->setScale(contentScale());
        addLabelBMFont(this,hpPlusLabel,"","bmfont/Sumo_22_green.fnt", ccp(0,0),ccp(0,0.5));
        hpPlusLabel->setScale(contentScale());
        addLabelBMFont(this,attackPlusLabel,"","bmfont/Sumo_22_green.fnt", ccp(0,0),ccp(0,0.5));
        attackPlusLabel->setScale(contentScale());
        addLabelBMFont(this,regenPlusLabel,"","bmfont/Sumo_22_green.fnt", ccp(0,0),ccp(0,0.5));
        regenPlusLabel->setScale(contentScale());
    }
    
    expNeedLabel = LabelX::create("", FONT_NUM, 24, LabelX::FontEffect::SHADOW, ccp(2, -2));
    expNeedLabel->setPosition(ccp(275, -44));
    expNeedLabel->setAnchorPoint(ccp(1, 0.5));
    expNeedLabel->setColor(ccWHITE);
    this->addChild(expNeedLabel);

    expGainLabel = LabelX::create("", FONT_NUM, 24, LabelX::FontEffect::SHADOW, ccp(2, -2));
    expGainLabel->setPosition(ccp(-5, -214));
    expGainLabel->setAnchorPoint(ccp(1, 0.5));
    expGainLabel->setColor(TITLE_YELLOW);
    this->addChild(expGainLabel);
    
	return true;
}

int QiangHuaJuanZhou::getMatsNum()
{
	int count = 0;
	for(int i = 0 ; i < heroVec.size() ; i++)
	{
		if(i != 0 && heroVec[i])count++;
	}
	return count;
}
    
void QiangHuaJuanZhou::updateOwnCoinsNum()
{
    int coinsNum = PlayerData::getInstance()->player->coins;
    coinsNum >= 1000000 ? ownCoinsLabel->setScale(0.9) : ownCoinsLabel->setScale(1);
    ownCoinsLabel->setString(toStr(coinsNum).c_str());
}

void QiangHuaJuanZhou::updateData()
{
	if(heroVec[0])
    {
		updateUIFromHero(heroVec[0]);
	}
    else if(!heroVec[0])
    {
		updateUIFromHero(NULL);
	}
    updateUIFromHero(heroVec[0]);
}

void QiangHuaJuanZhou::updateExpGain()
{
    int expGain = 0;
    for(int i=1; i<heroVec.size(); ++i)
        if (heroVec[i])
        {
            int baseExp = heroVec[i]->profile->gain.eval(heroVec[i]->level);
            expGain += baseExp * (heroVec[i]->profile->color == heroVec[0]->profile->color ? 1.5 : 1);
        }
    expGainLabel->setString(toStr(expGain).c_str());
    
    Hero* target = heroVec[0];
    if(heroVec[0] && expGain>0)
    {
        const HeroProfile* profile = target->profile;
        int level = target->level+1;
        int exp = target->exp + expGain - target->expCap;
        for( ; exp>=0 ;)
            exp -= profile->exp.eval(level++);
        level = std::min(target->profile->maxLevel, --level);
        
        levelPlusLabel->setFntFile("bmfont/Sumo_22_green.fnt");
        hpPlusLabel->setFntFile("bmfont/Sumo_22_green.fnt");
        attackPlusLabel->setFntFile("bmfont/Sumo_22_green.fnt");
        regenPlusLabel->setFntFile("bmfont/Sumo_22_green.fnt");
        if(level > target->level)
        {
            levelLabel->setString(toStr(heroVec[0]->level).c_str());
            levelPlusLabel->setString(("+"+toStr(level - target->level)).c_str());
            int hpPlus = target->eliteLevel > 0 ? int(profile->eliteHP.eval(target->eliteLevel)) : 0;
            hpPlusLabel->setString(("+"+toStr(int(profile->health.eval(level))+hpPlus-target->health)).c_str());
            int atkPlus = target->eliteLevel > 0 ? int(profile->eliteAttack.eval(target->eliteLevel)) : 0;
            attackPlusLabel->setString(("+"+toStr(int(profile->attack.eval(level))+atkPlus - target->attack)).c_str());
            int regenPlus = target->eliteLevel > 0 ? int(profile->eliteRegen.eval(target->eliteLevel)) : 0;
             regenPlusLabel->setString(("+"+toStr(int(profile->regen.eval(level))+regenPlus-target->regen)).c_str());
        }
        else
        {
            levelLabel->setString(toStr(heroVec[0]->level).c_str());
            levelPlusLabel->setString("+0");
            hpPlusLabel->setString("+0");
            attackPlusLabel->setString("+0");
            regenPlusLabel->setString("+0"); 
        }
        levelPlusLabel->setPosition(ccp(levelLabel->getPosition().x+levelLabel->getContentSize().width*
                                        contentScale()+5,
                                        levelLabel->getPosition().y));
        hpPlusLabel->setPosition(ccp(hpLabel->getPosition().x+hpLabel->getContentSize().width*
                                     contentScale()+5,
                                     hpLabel->getPosition().y));
        attackPlusLabel->setPosition(ccp(attackLabel->getPosition().x+attackLabel->getContentSize().width*
                                         contentScale()+5,
                                         attackLabel->getPosition().y));
        regenPlusLabel->setPosition(ccp(regenLabel->getPosition().x+regenLabel->getContentSize().width*
                                        contentScale()+5,regenLabel->getPosition().y));
    }
    else
    {
        if(heroVec[0]->eliteLevel>0)
        {
            
            int eliteHp = target->profile->eliteHP.eval(target->eliteLevel);
            int eliteAtk = target->profile->eliteAttack.eval(target->eliteLevel);
            int eliteReg = target->profile->eliteRegen.eval(target->eliteLevel);
            
            levelLabel->setString((toStr(target->level)+"/"+toStr(target->profile->maxLevel)).c_str());
            hpLabel->setString(toStr(target->health-eliteHp).c_str());
            attackLabel->setString(toStr(target->attack-eliteAtk).c_str());
            regenLabel->setString(toStr(target->regen-eliteReg).c_str());
            
            levelPlusLabel->setString("");
            hpPlusLabel->setFntFile("bmfont/Sumo_22_purple.fnt");
            hpPlusLabel->setString(("+"+toStr(eliteHp)).c_str());
            
            attackPlusLabel->setFntFile("bmfont/Sumo_22_purple.fnt");
            attackPlusLabel->setString(("+"+toStr(eliteAtk)).c_str());
            
            regenPlusLabel->setFntFile("bmfont/Sumo_22_purple.fnt");
            regenPlusLabel->setString(("+"+toStr(eliteReg)).c_str());
            
            levelPlusLabel->setPosition(ccp(levelLabel->getPosition().x+levelLabel->getContentSize().width*
                                            contentScale()+5,
                                            levelLabel->getPosition().y));
            hpPlusLabel->setPosition(ccp(hpLabel->getPosition().x+hpLabel->getContentSize().width*
                                         contentScale()+5,
                                         hpLabel->getPosition().y));
            attackPlusLabel->setPosition(ccp(attackLabel->getPosition().x+attackLabel->getContentSize().width*
                                             contentScale()+5,
                                             attackLabel->getPosition().y));
            regenPlusLabel->setPosition(ccp(regenLabel->getPosition().x+regenLabel->getContentSize().width*
                                            contentScale()+5,regenLabel->getPosition().y));
        
        }
        else
        {
            levelPlusLabel->setString("");
            hpPlusLabel->setString("");
            attackPlusLabel->setString("");
            regenPlusLabel->setString("");
        }
    }
    
}
    
void QiangHuaJuanZhou::updateUIFromHero(Hero *hero)
{
    //clear the previous train state
	if(sucBar)
    {
		removeChild(sucBar , true);
		sucBar = NULL;
	}
	if(hero)
    {
		levelLabel->setString((toStr(hero->level)+"/"+toStr(hero->profile->maxLevel)).c_str());
		hpLabel->setString(toStr(hero->health).c_str());
		attackLabel->setString(toStr(hero->attack).c_str());
		regenLabel->setString(toStr(hero->regen).c_str());
        
        if(hero->level == hero->profile->maxLevel)
        {
            expNeedLabel->setString("MAX");
        }
        else
        {
            float expDiff = hero->expCap - hero->exp;
            expNeedLabel->setString(toStr(expDiff).c_str());
        }
        
        if(hero->profile->star >= 3)
        {
            eliteLabel->setString((toStr(calElite())+"%").c_str());
            eliteMask->setVisible(false);
        }
        else
            eliteMask->setVisible(true);
        
        //these two were set visible false after train success.
        updateOwnCoinsNum();
		coinsLabel->setString(toStr(hero->trainPrice * getMatsNum()).c_str());

        float scaleX = (float)hero->exp / hero->expCap;
        expBar->setVisible(hero->exp != 0);
        expBar->setContentSize(CCSize(std::max(206 * scaleX, 8.0f), 12));
        updateExpGain();
	}
    else
    {
        levelLabel->setString("0");
		hpLabel->setString("0");
		attackLabel->setString("0");
		regenLabel->setString("0");
		//expNowLabel->setString("0");
		expNeedLabel->setString("0");
        eliteLabel->setString("0");
        eliteMask->setVisible(false);
		coinsLabel->setString("0");
		expBar->setVisible(false);
        expGainLabel->setString("");
        
        levelPlusLabel->setString("");
        hpPlusLabel->setString("");
        attackPlusLabel->setString("");
        regenPlusLabel->setString("");
	}
}
    
void QiangHuaJuanZhou::updateUIFromHeroWithAnim(const Hero& prev, const Hero& curr)
{
    
    levelLabel->setString((toStr(curr.level)+"/"+toStr(curr.profile->maxLevel)).c_str());
    hpLabel->setString(toStr(curr.health).c_str());
    attackLabel->setString(toStr(curr.attack).c_str());
    regenLabel->setString(toStr(curr.regen).c_str());

    if(curr.level == curr.profile->maxLevel)
    {
        expNeedLabel->setString("MAX");
    }
    else
    {
        float expDiff = curr.expCap - curr.exp;
        expNeedLabel->setString(toStr(expDiff).c_str());
        
    }
    
    updateOwnCoinsNum();
	coinsLabel->setString(toStr(curr.trainPrice * getMatsNum()).c_str());
    
    if(curr.profile->star >= 3)
    {
        eliteLabel->setString((toStr(calElite())+"%").c_str());
        eliteMask->setVisible(false);
    }
    else
        eliteMask->setVisible(true);
    
    float levelDiff = curr.level - prev.level;
    
    float from = (float)prev.exp / prev.expCap;
    float to = (float)curr.exp / curr.expCap;

    float distance = (levelDiff * 1.0f - from + to) * 210;
    float duration = std::abs(distance) / 210.0f * 0.5f;
	if(duration != 0)
    {
		expBar->setVisible(true);
		expBar->runAction(CCEaseSineOut::create(CCAnimate9SpriteProgress::create(duration, 8, 206, distance)));
	}

    updateExpGain();
}
    
int QiangHuaJuanZhou::calElite()
{
    float value = 0;
    if(heroVec[0]->eliteLevel >= 5 || heroVec[0]->profile->star <=2)
        return value;
    int count = 0;
    for (int i = 1; i < heroVec.size(); i++)
    {
        //if hero is NULL jump over
        if(!heroVec[i] || heroVec[i]->heroId != heroVec[0]->heroId)
            continue;
        float orgElite = heroVec[0]->eliteLevel;
        float matElite = heroVec[i]->eliteLevel;
        
        if(heroVec[i]->eliteLevel > heroVec[0]->eliteLevel)
            return 100;
        else
        {
            count++;
            value += std::max(1.0f, 40.0f-(orgElite/(orgElite+matElite+1)*40.0f+(orgElite-matElite)*1.75f));
        }
    }
    float pAdjustByCount = 0.15*count + 0.25;
    value *= pAdjustByCount;
    
    return std::min(value, 100.0f);
}

void QiangHuaJuanZhou::onTrainHeroSuccess(const Hero& prev, const Hero& curr)
{
	GameScene::getInstance()->setTouchEnabled(false);
    targetHero = curr;

    float delay = 0.0f;
    
	for(int i = 1; i < heroVec.size(); i++)
    {
		if(heroVec[i])
        {
			HeroIcon* icon = (HeroIcon*)heroImgVec[i];
			
			CCSprite *light = CCSprite::create(); //GetSprite("gameui/qiu2_1.png");
			light->setPosition(icon->getPosition());
			addChild(light);

			CCAction *lightSeq = CCSequence::create(CCDelayTime::create(delay),
                                                    CCCallLambda::create([](){ ring("sound/v/icon_disappear.mp3"); }),
                                                    createHeroDisappearAnim(),
                                                    CCCallLambda::create([=]() { this->removeChild(light, true);}),
                                                    NULL);
			light->runAction(lightSeq);

            auto removeIcon = [=]() { this->removeHero(icon->getHero(), false); };
			CCAction *iconSeq = CCSequence::create(CCDelayTime::create(delay),
                                                   CCScaleTo::create(0.2, 0, 0),
                                                   CCCallLambda::create(removeIcon),
                                                   NULL);
			icon->runAction(iconSeq);
            
            delay += 0.4f;
		}
	}

	HeroIcon* icon = (HeroIcon*)heroImgVec[0];
    
	CCSprite *light = CCSprite::create(); //GetSprite("gameui/qiu1_1.png");
	light->setPosition(icon->getPosition());
	this->addChild(light);
    
	CCAction *seq = CCSequence::create(CCDelayTime::create(0.2 + delay),
                                       CCCallLambda::create([](){ ring("sound/v/light_bling.mp3");}),
                                       createHeroAppearAnim(),
                                       CCCallLambda::create([=](){ this->removeChild(light, true); }),
                                       CCCallLambda::create([=]()
                                       {
                                           this->playSuccessAnim(curr.eliteLevel - prev.eliteLevel);
                                           updateUIFromHeroWithAnim(prev, curr);
                                       }),
                                       NULL);
	light->runAction(seq);
	
	CCAction *iconSeq = CCSequence::create(CCDelayTime::create(0.2 + delay),
                                           CCCallLambda::create([=](){
                                                //update the elite num on heroicon
                                                icon->showElite(true);
                                           }),
                                           CCScaleTo::create(0.3, 1.2, 1.2),
                                           CCScaleTo::create(0.3, 1, 1),
                                           NULL);
    
    
	icon->runAction(iconSeq);
}

void QiangHuaJuanZhou::playSuccessAnim(int neiliUp)
{
    CCParticleSystemQuad *fire = CCParticleSystemQuad::create("particle/firework.plist");
    fire->setPosition(ccp(-200 , -100));
    this->addChild(fire , 100);
    
    
    CCParticleSystemQuad *fire2 = CCParticleSystemQuad::create("particle/firework.plist");
    fire2->setPosition(ccp(200 , -150));
    this->addChild(fire2 , 100);
    
    const char* path;
    const char* path2;
    std::string s1;
	std::string s2;

    if(targetHero.trainRate == 2)
	{
		path = "gameui/chaochenggong_wenzi.png";
        path2 = "gameui/chaochenggong_2bei.png";
		s1 = "超成功";
		s2 = "2倍經驗";
	}
    else if(targetHero.trainRate == 1.5)
    {
        path = "gameui/dachenggong_wenzi.png";
        path2 = "gameui/dachenggong_15bei.png";
        s1 = "大成功";
		s2 = "1.5倍經驗";
    }
    else
    {
        path = "gameui/chenggong_pic.png";
        path2 = "gameui/chenggong-_1bei.png";
        s1 = "成功";
		s2 = "";
    }
    
	CCSprite *suc = GetSprite(path);
	suc->setPosition(ccp(0 , -200));
	suc->setScale(1.8);
	this->addChild(suc, 10);
    
	sucBar = GetSprite(path2);
	sucBar->setAnchorPoint(ccp(1 , 0.5));
    sucBar->setPosition(ccp(270 , -180));
    this->addChild(sucBar , 5);

    CCAction* neiliUpAction = NULL;
    if(neiliUp > 0)
    {
        int currNeili = heroVec[0]->eliteLevel;
        CCLayerColor* bg = CCLayerColor::create(ccc4(0, 0, 0, 103));
        bg->retain();
        
        HeroIcon* icon = HeroIcon::create(heroVec[0]);
        icon->setPosition(ccp(320, 700));
        icon->setScale(1.5);
        bg->addChild(icon);
        CCSprite* neiliFrame = GetSprite("gameui/neili.png");
        neiliFrame->setPosition(icon->getPosition());
        neiliFrame->setScale(10);
        bg->addChild(neiliFrame);
        CCLabelBMFont* num = addLabelBMFont(neiliFrame, toStr(currNeili-neiliUp).c_str(),
                                            "bmfont/Molot_30_yellow_gradient.fnt", ccp(90, 90), ccp(0.5, 0.5));
        num->setScale(contentScale());
        
        CCSprite* neili = GetSprite("gameui/neilitishegn.png");
        neili->setPosition(ccp(320, 400));
        neili->setOpacity(0);
        
        bg->addChild(neili);
        
        neiliUpAction = CCSequence::create(
                           CCCallLambda::create([=]()
                                {
                                    ring("sound/v/level_up_done.mp3"); 
                                    GameScene::getInstance()->addChild(bg, 100);
                                }),
                           CCTargetedAction::create(neiliFrame, CCScaleTo::create(0.5, 1.5)),
                           CCTargetedAction::create(num, CCScaleTo::create(0.2, contentScale()+0.5)),
                           CCCallLambda::create([=](){num->setString(toStr(currNeili).c_str());}),
                           CCTargetedAction::create(num, CCScaleBy::create(0.2, contentScale())),
                           CCCallLambda::create([=]()
                                {
                                    CCParticleSystemQuad *fire3 = CCParticleSystemQuad::create("particle/firework.plist");
                                    fire3->setPosition(ccp(100 , 500));
                                    bg->addChild(fire3 , 100);
                                    
                                    CCParticleSystemQuad *fire4 = CCParticleSystemQuad::create("particle/firework.plist");
                                    fire4->setPosition(ccp(500 , 430));
                                    bg->addChild(fire4 , 100);
                                }),
                           CCTargetedAction::create(neili, CCFadeIn::create(0.5)),
                           CCDelayTime::create(2),
                           CCCallLambda::create([=]()
                                {
                                    bg->release();
                                    bg->removeFromParentAndCleanup(true);
                                    GameScene::getInstance()->setTouchEnabled(true);
                                }),
                           NULL);
        
    }
    else
        neiliUpAction = CCCallLambda::create([=](){GameScene::getInstance()->setTouchEnabled(true);});
    
    
	CCFiniteTimeAction *seq = CCSequence::create(CCCallLambda::create([](){ ring("sound/v/level_up_done.mp3"); }),
                                       CCEaseIn::create(CCScaleTo::create(1.0f, 1.0f, 1.0f), 0.5),
                                       CCFadeOut::create(0.3f),
                                       neiliUpAction,
                                       CCCallLambda::create([=]()
                                              {
                                                  this->removeChild(suc, true);
                                                  this->removeChild(fire, true) ;
                                                  this->removeChild(fire2 , true);
                                              }),
                                       NULL);
    suc->runAction(seq);
    

}
    
//////////////////////////////////////////////////////
//QiangHuaBoard
//////////////////////////////////////////////////////

bool QiangHuaBoard::init(const std::vector<PH::Hero *>&heroVec)
{
    displayPlaceholder = false;
	if(!HeroBoard::init(heroVec, false))
        return false;
	return true;
}

void QiangHuaBoard::refresh(const std::vector<PH::Hero *>& heroVec)
{
	HeroBoard::refresh(heroVec);
	
	std::vector<Hero*> team = PlayerData::getInstance()->getHeroesInAllTeams();
	for(Hero* h : team)
	{
		if(h != NULL)
			((HeroIcon*)getHeroImg(h))->setBottomMsg(HeroIcon::LabelType::InTeam);
	}
    
    for(Hero* hero :  PlayerData::getInstance()->getNewHeroes("qianghua"))
        getHeroImg(hero)->showNew();
}

Hero *QiangHuaBoard::getHero(CCTouch *touch)
{
	Hero *h = HeroBoard::getHero(touch);
	if(!h)
        return NULL;
	HeroIcon *icon = getHeroImg(h);
	if(icon->isClickEnabled())
        return NULL;
	return h;
}

/////////////////////////////////////////////////////////////////////
//QiangHuaLayer
//(0.5,1)
/////////////////////////////////////////////////////////////////////


bool QiangHuaLayer::init()
{
	if(!ContentLayer::init())
		return false;
    
    isShowTop = false;
    
	PlayerData *data = PlayerData::getInstance();

	ContentLayer::setTouchEnabled(true);

	scrollView = ZqlScrollView::create();
	board = QiangHuaBoard::create(data->getStorage());
    board->setClip(0, 480);
	scrollView->addLayer(board);
	scrollView->setPositionY(-430);
	scrollView->setViewSize(CCSize(winSize.width, 500),
                            CCRect(0, -20, winSize.width, 363));
	this->addChild(scrollView);

	topMenu = CCMenu::create();
    itemLeft = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                        GetSprite("gameui/back_t.png"),
                                        this,
                                        menu_selector(QiangHuaLayer::onBackClick));
	itemLeft->setAnchorPoint(ccp(0.5, 1));
	topMenu->addChild(itemLeft);

    
    {
        itemMiddle = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                              GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                              this,
                                              menu_selector(QiangHuaLayer::onTitleClick));
        itemMiddle->setAnchorPoint(ccp(0.5 , 1));
        
        LabelX* name = LabelX::create("强化", FONT_CN, 36);
        name->setColor(ccWHITE);
        name->setAnchorPoint(ccp(0.5, 0.5));
        name->setPosition(ccp(90 , itemMiddle->getContentSize().height*0.5f));
        itemMiddle->addChild(name);
        
        title = LabelX::create(PlayerData::getInstance()->getSortTypeMsg() , FONT_CN , 20, LabelX::FontEffect::SHADOW, ccp(2, -2));
        title->setColor(ccWHITE);
        title->setPosition(ccp( 170 , itemMiddle->getContentSize().height*0.5f));
        title->setAnchorPoint(ccp(0, 0.5));
        itemMiddle->addChild(title);
        
        CCSprite *xian = GetSprite("gameui/hongchengdi_huang_tongyong.png");
        xian->setPosition(ccp(160 , itemMiddle->getContentSize().height*0.5f));
        itemMiddle->addChild(xian);
        
        topMenu->addChild(itemMiddle);
    }
    
    itemRight = CCMenuItemSprite::create(GetSprite("gameui/chengdi_t_tongyong_l.png"),
                                            GetSprite("gameui/chengdi_t_tongyong_l.png"),
                                            this,
                                            menu_selector(QiangHuaLayer::onStartClick));
    itemRight->setAnchorPoint(ccp(0.5 , 1));
    LabelX* label = LabelX::create("开始", FONT_CN, 34);
    label->setPosition(ccp(itemRight->getContentSize().width*0.5, itemRight->getContentSize().height*0.5f));
    label->setColor(TITLE_YELLOW);
    itemRight->addChild(label);
    
	topMenu->addChild(itemRight);

	topMenu->alignItemsHorizontallyWithPadding(8);
	topMenu->setPosition(ccp(0, -20));
	this->addChild(topMenu, 1);

	juanzhou = QiangHuaJuanZhou::create();
	CCSprite* zhou1 = GetSprite("gameui/qianghua_1_1.png");
	CCSprite* zhou2 = GetSprite("gameui/qianghua_1_1.png");
    zhou2->setFlipX(true);
	jz = JuanZhouLayer::create();
	jz->addSkin(juanzhou, zhou1, zhou2);
	jz->setPositionY(-68);
	this->addChild(jz);
    
	return true;
}
    
void QiangHuaLayer::onEnter()
{
    ContentLayer::onEnter();
    refreshBoard();
    
    if(GameConfig::shared()->getTutorialProgress() == TutorialQiangHua)
    {
        ArrowMask::shared()->clear();
        // Disable touch for everything first
        for(const Hero* h : PlayerData::getInstance()->getStorage())
        {
            HeroIcon* icon = board->getHeroImg(h);
            icon->setEnabled(false);
        }
        
        itemLeft->setEnabled(false);
        itemMiddle->setEnabled(false);
        itemRight->setEnabled(false);
    }
}
    
void QiangHuaLayer::onEnterAnimationDone()
{
    ContentLayer::onEnterAnimationDone();
    
    jz->startAnim();
    
    // enable touch for initial heroes
    if(GameConfig::shared()->getTutorialProgress() == TutorialQiangHua)
    {
        HeroIcon* targetHero = NULL;
        // find first fire/wood/wood hero
        for(Hero* h : PlayerData::getInstance()->getStorage())
        {
            HeroIcon* icon = board->getHeroImg(h);
            
            if(isInitialHero(h->heroId))
            {
                icon->setEnabled(true);
                icon->setLongClickEnabled(false);
                if(targetHero == NULL)
                    targetHero = icon;
            }
        }
        
        // this must be done after refresh board is called.
        CCPoint p = board->convertToWorldSpace(targetHero->getPosition());
        
        ArrowMask::shared()->setMask(CCRect(0, 140, 640, 845));
        ArrowMask::shared()->setArrow(ccp(p.x, p.y - 70), ArrowDirection::Up);
        
        targetHero->setZOrder(50);
    }
    else if(GameConfig::shared()->getTutorialProgress() == TutorialNeili)
    {
        std::vector<Hero*> dups;
        
        for(const Hero* h : PlayerData::getInstance()->player->storage)
        {
            if(h->profile->star >= 3 &&
               h->profile->tableId >= 10000)
            {
                dups = PlayerData::getInstance()->getHeroesById(h->heroId);
                if(dups.size() > 1)
                    break;
            }
        }
        
        if(dups.size() > 1)
        {
            AlertMessage::create("", "侠客内力：您有两个相同的侠客，强化侠客时使用与被强化的侠客\t一样的侠客，有几率提升内力等级喔！提升内力等级能给您的侠\t客带来三维加成喔！让我们尝试一下提升内力等级吧！");
            
            HeroIcon* icon1 = board->getHeroImg(dups[0]);
            HeroIcon* icon2 = board->getHeroImg(dups[1]);
            
            CCPoint p1 = icon1->getPosition();
            p1.y -= 100;
            CCPoint p2 = icon2->getPosition();
            p2.y -= 100;
            
            CCSprite* arrow1 = createTutorialArrow(p1,
                                                   ArrowSize::Medium,
                                                   ArrowDirection::Up,
                                                   true);
            icon1->setUserObject(arrow1);
            board->addChild(arrow1, 1000);
                                                   
            CCSprite* arrow2 = createTutorialArrow(p2,
                                                   ArrowSize::Medium,
                                                   ArrowDirection::Up,
                                                   true);
            icon2->setUserObject(arrow2);
            board->addChild(arrow2, 1000);
        }
    }
}
    
void QiangHuaLayer::onExit()
{
    ContentLayer::onExit();
    
    if(GameConfig::shared()->getTutorialProgress() == TutorialNeili)
    {
        GameConfig::shared()->setTutorialProgress(TutorialSkip);
        GameConfig::shared()->save();
    }
}
    
void QiangHuaLayer::cleanup()
{
    CCNode::cleanup();
    
    PlayerData::getInstance()->clearNewHeroes("qianghua");
}
    
void QiangHuaLayer::refreshBoard()
{
    board->refresh(PlayerData::getInstance()->getStorage());
    
    for (int i=0; i<juanzhou->heroNum; i++)
    {
        const Hero *hero = juanzhou->getHeroById(i);
        if(!hero)
            continue;
        
        if(i > 0 && hero->isLock)
        {
            juanzhou->removeHeroById(i);
            continue;
        }
        board->getHeroImg(hero)->setEnabled(false);
    }
}

void QiangHuaLayer::putHeroToJz(HeroIcon *icon)
{
	const char *msg = "强化素材已满";
	for(int i = 0; i<juanzhou->heroNum; i++)
	{
		if(!juanzhou->getHeroById(i))
        {
			Hero *hero = icon->getHero();
            if(i == 0 && hero->profile->isMat())
            {
                msg = "丹药和转生材料不能被强化！";
                
                break;
            }
			else if(i != 0 && PlayerData::getInstance()->isHeroInTeam(hero))
            { 
				msg = "侠客在队伍中";
				break;
			}
            else if(i != 0 && hero->isLock)
            {
				msg = "侠客已锁定";
				break;
			}
            else
            {
				HeroIcon* temp = HeroIcon::create(hero);
                temp->showElite(true);
				pullHeroIconFrBoard(icon);
				juanzhou->addHeroImg(temp, i);
                
                if(GameConfig::shared()->getTutorialProgress() == TutorialQiangHua)
                {
                    if(juanzhou->getHeroById(1) == NULL)
                    {
                        temp->setEnabled(false);
                        HeroIcon* target = NULL;
                        for(Hero* h : PlayerData::getInstance()->getStorage())
                        {
                            HeroIcon* icon = board->getHeroImg(h);
                            icon->setEnabled(false);
                            
                            // first hero that is not in team, and not a initial hero.
                            if(!isInitialHero(h->heroId) &&
                               target == NULL &&
                               !PlayerData::getInstance()->isHeroInTeam(h) &&
                               h->profile->star < 3)
                                target = icon;
                        }
                        target->setEnabled(true);
                        target->setLongClickEnabled(false);
                        
                        CCPoint p = board->convertToWorldSpace(target->getPosition());
                        ArrowMask::shared()->setArrow(ccp(p.x, p.y-80), ArrowDirection::Up);
                    }
                    else
                    {
                        temp->setEnabled(false);
                        
                        itemRight->setEnabled(true);

                        CCPoint p = itemRight->getParent()->convertToWorldSpace(itemRight->getPosition());
                        ArrowMask::shared()->setArrow(ccp(p.x, p.y-80), ArrowDirection::Up);
                    }
                }
                else if(GameConfig::shared()->getTutorialProgress() == TutorialNeili)
                {
                    CCNode* arrow = (CCNode*)(icon->getUserObject());
                    if(arrow)
                        arrow->removeFromParentAndCleanup(true);
                }
                
				return;
			}
		}
	}

	SystemInform::alertMessage(GameScene::getInstance(),
                               msg,
                               "警告" ,
                               [](){});
}

void QiangHuaLayer::putBackHeroToBoard(Hero *hero)
{
    HeroIcon *icon = board->getHeroImg(hero);
    if(icon)
        icon->setEnabled(true);
}

void QiangHuaLayer::initTargetHero(PH::Hero *hero)
{
    //juanzhou->clear();
    //putHeroToJz(board->getHeroImg(hero));
	board->getHeroImg(hero)->setEnabled(false);
}

void QiangHuaLayer::onStartClick(cocos2d::CCObject *target)
{
	const std::vector<Hero*>& heroes = juanzhou->heroVec;
    
	if(!heroes[0])
        return;
    
	std::vector<std::string> mats;
	for(int i = 1 ; i < heroes.size() ; i++)
    {
		if(heroes[i])
            mats.push_back(heroes[i]->id);
	}
    
	if(mats.size() == 0)
    {
		SystemInform::alertMessage(GameScene::getInstance(),
                                   "没有选择素材",
                                   "警告",
                                   [](){} );
		return;
	}
        
    int trainingCost = heroes[0]->trainPrice;
    
	boost::function<void ()> toTrain = [=]()
    {
		SystemInform* inform = SystemInform::alertLoading(GameScene::getInstance());

        bool forceSuccess = GameConfig::shared()->getTutorialProgress() == TutorialNeili;
        
		PuzzleRPC::getInstance()->trainHero(PlayerData::getInstance()->getToken(),
											heroes[0]->id,
											mats,
                                            forceSuccess,
											[=](std::auto_ptr<Hero> hero,
                                                std::auto_ptr<Error> e)
		{
            inform->cancel();

			if(e.get())
			{
				LOGD("!!! trainHero error: %s\n", e->toStr().c_str());
				e->handleErrorInform();
				return;
			}
            
            if(GameConfig::shared()->getTutorialProgress() == TutorialQiangHua)
            {
                GameConfig::shared()->setTutorialProgress(TutorialJustFinished);
                
                itemLeft->setEnabled(true);
                itemRight->setEnabled(false);
                
                CCPoint p = itemLeft->getParent()->convertToWorldSpace(itemLeft->getPosition());
                ArrowMask::shared()->setArrow(ccp(p.x, p.y-80), ArrowDirection::Up);
            }
            else if(GameConfig::shared()->getTutorialProgress() == TutorialNeili)
            {
                ArrowMask::shared()->clear();
            }
            
			PlayerData *playerData = PlayerData::getInstance();
            Hero* h = playerData->getHero(hero->id);
            ((QiangHuaJuanZhou*)juanzhou)->onTrainHeroSuccess(*h, *hero);
            
			for(int i = 0; i < mats.size(); i++)
				playerData->removeHero(mats[i]);
		
			playerData->updateHero(hero.get());

			playerData->setCoins(playerData->player->coins - trainingCost*mats.size());

			((QiangHuaBoard*)board)->refresh(playerData->getStorage());
			initTargetHero(hero.get());
		});
	};

    int extraHint = false;
    
    for(int i = 1 ; i < heroes.size() ; i++)
    {
		if(heroes[i] && (heroes[i]->profile->star>=3 || heroes[i]->eliteLevel > 3))
        {
            extraHint = true;
            break;
        }
	}

    if(extraHint)
    {
        SystemInform::alertConfirmPanel(GameScene::getInstance(),
                                        "强化中有稀有素材\n是否继续？",
                                        "警告",
                                        toTrain,
                                        [](){});
    }
    else
    {
        toTrain();
    }
}
    
void QiangHuaLayer::onTitleClick(cocos2d::CCObject *target)
{
    PlayerData::getInstance()->switchSortType();
	board->reOrder(PlayerData::getInstance()->getStorage());
	title->setString(PlayerData::getInstance()->getSortTypeMsg());
    ring("sound/v/button_click.mp3");
}

void QiangHuaLayer::onBackClick(cocos2d::CCObject *target)
{
    if(GameConfig::shared()->getTutorialProgress() == TutorialJustFinished)
    {
        GameScene::getInstance()->popAllLayers();
        
        ArrowMask::shared()->clear();
        AlertMessage::create("",
                             "做得真好！你已基本掌握了闯荡江湖的技巧！以后的路还有更多的挑战等着你！",
                             true,
                             [=]()
        {
            boost::function<void()>* funcPtr = new boost::function<void()>();
            (*funcPtr) = [=]()
            {
                SystemInform* loading = SystemInform::alertLoading(runningScene());
                PuzzleRPC::getInstance()->setTutorialProgress(PlayerData::getInstance()->getToken(),
                                                              0,
                                                              [=](std::auto_ptr<Error> e)
                {
                    loading->cancel();
                    if(e.get())
                    {
                        std::cout << "!!! set tutorial progress error: " << e->toStr() << std::endl;
                        SystemInform::alertMessage(GameScene::getInstance(),
                                                   "无法连接服务器，请稍后重试。",
                                                   "提示" ,
                                                   [funcPtr](){ (*funcPtr)(); });
                        return;
                    }
                
                    LOGD("set tutorial progress to 0 on Qianghua");
                    GameConfig::shared()->setTutorialProgress(TutorialSkip);

                    GameScene::getInstance()->clearUIAndReLogin(0);
                    
                    delete funcPtr;
                });
            };
            (*funcPtr)();
        });
    }
    else
        GameScene::getInstance()->popLayer();
    
    ring("sound/v/button_click.mp3");
}

}