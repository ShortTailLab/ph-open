//
//
//  Created by ZQL on 11.23
//
//


#include "ZhuanSheng.h"
#include "XiaKe.h"
#include "GameScene.h"
#include "HeroProfileLayer.h"
#include "PlayerData.h"
#include <extensions/utils.h>
#include "UIAnimation.h"
#include "CCCallLambda.h"
#include "UIPresets.h"
#include "ZqlScrollView.h"
#include "HeroIcon.h"
#include "network/PuzzleRPC.h"
#include "Common.h"
#include <boost/shared_ptr.hpp>
#include "ZhaoMu.h"

namespace PH
{
    
using namespace std;
using namespace boost::posix_time;
    
bool isTransformable(const Hero* h, const std::vector<Hero*>& storage)
{
    if(h->level < h->profile->maxLevel)
        return false;
    
    const std::vector<int>& requires = h->profile->transformMats;
    
    int count = 0;
    for(int i=0; i<requires.size(); i++)
    {
        int ref = requires[i];
        for(const Hero* h: storage)
        {
            if(h->profile->tableId == ref)
            {
                count++;
                break;
            }
        }
    }
    
    return count == requires.size();
}
    
bool ZhuanShengBoard::init()
{
    displayPlaceholder = false;
	if(!HeroBoard::init(PlayerData::getInstance()->getStorage(), false))
        return false;

	return true;
}

void ZhuanShengBoard::refresh(const std::vector<PH::Hero *>& heroes)
{
	HeroBoard::refresh(heroes);
    
    for (auto h : heroes)
    {
        HeroIcon *icon = getHeroImg(h);
        bool enable = !h->profile->isMat() && (PlayerData::getInstance()->isHeroTransformMatsReady(h) &&
                        h->level >= h->profile->maxLevel);
        icon->setEnabled(enable);
        if(h->profile->transformTargetHeroId == -1 || h->profile->transformMats.empty())
            icon->setBottomMsg(HeroIcon::LabelType::Ultimate);
    }
}

bool ZhuanShengLayer::init()
{
	if(!ContentLayer::init())
        return false;

    heroesOfCurrType = PlayerData::getInstance()->getStorage();
    ContentLayer::setTouchEnabled(true);
    
    title = createTitleMenu(this,
                            "转生",
                            PlayerData::getInstance()->getSortTypeMsg(),
                            menu_selector(ZhuanShengLayer::onBackClick),
                            menu_selector(ZhuanShengLayer::onTitleClick));
    
    switcher = HeroBoardSwitcher::create();
    switcher->setPosition(ccp(0, -80));
    switcher->setCallBack([=](int i){
        scrollView->reset();
        heroesOfCurrType = getHeroesBySortAndSwitchType(switcher->getCurNode());
        board->removeFromParentAndCleanup(true);
        board = ZhuanShengBoard::create();
        board->refresh(heroesOfCurrType);
        scrollView->addLayer(board);
    });
    addChild(switcher, 1);

	scrollView = ZqlScrollView::create();
	scrollView->setViewSize(CCSize(winSize.width , 900) , CCRect(0 , -190 , winSize.width , 520));
	board = ZhuanShengBoard::create();
	scrollView->addLayer(board);
	scrollView->setPositionY( 70 );
	this->addChild(scrollView);
    
	return true;
}

void ZhuanShengLayer::onEnter()
{
    ContentLayer::onEnter();
    heroesOfCurrType = getHeroesBySortAndSwitchType(switcher->getCurNode());
    board->refresh(heroesOfCurrType);
    
    if(GameConfig::shared()->getTutorialProgress() == TutorialTransformHero)
    {
        AlertMessage::create("", "达到最大等级的，收集到对应的转生物品后，就可以转生啦。\t快来看看侠客还需要什么物品！");
        
        GameConfig::shared()->setTutorialProgress(TutorialSkip);
        
        const Hero* targetHero = NULL;
        const std::vector<Hero*>& storage = PlayerData::getInstance()->getStorage();
        
        for(const Hero* h : storage)
        {
            /*
             // only point to transformable heroes
            if(isTransformable(h, storage))
            {
                targetHero = h;
                break;
            }*/
            if(h->level == h->profile->maxLevel &&
               h->profile->transformTargetHeroId != -1)
            {
                targetHero = h;
                break;
            }
        }
        
        // should never be false here.
        if(targetHero)
        {
            HeroIcon* icon = board->getHeroImg(targetHero);
            CCPoint p = icon->getPosition();
            p.y -= 100;
            CCSprite* arrow = createTutorialArrow(p,
                                                   ArrowSize::Medium,
                                                   ArrowDirection::Up,
                                                   true);
            board->addChild(arrow, 1000);
        }
    }
}
    
void ZhuanShengLayer::onExit()
{
    ContentLayer::onExit();
    
    ArrowMask::shared()->clear();
}
    
void ZhuanShengLayer::onSingleClick(cocos2d::CCTouch *touch)
{
    Hero* hero = NULL;
    CCPoint localPoint = scrollView->convertTouchToNodeSpace(touch);
	if(scrollView->boundingBox().containsPoint(localPoint))
        hero = board->getHero(touch);
    if(hero)
    {
		if(PlayerData::getInstance()->isHeroTransable(hero))
        {
            auto layer = ZhuanShengConfirmLayer::create(hero);
            GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
        }
		else
        {
            auto layer = HeroProfileLayer::create(hero);
            GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
        }
	}
}

void ZhuanShengLayer::onLongClick(cocos2d::CCTouch *touch)
{
    Hero* hero = NULL;
    CCPoint localPoint = scrollView->convertTouchToNodeSpace(touch);
	if(scrollView->boundingBox().containsPoint(localPoint))
        hero = board->getHero(touch);
    
    if(hero)
        GameScene::getInstance()->pushLayerWithDefaultAnimation(HeroProfileLayer::create(hero));
}

void ZhuanShengLayer::onTitleClick(cocos2d::CCObject *target)
{
	PlayerData::getInstance()->switchSortType();
	heroesOfCurrType = getHeroesBySortAndSwitchType(switcher->getCurNode());
    board->reOrder(heroesOfCurrType);
	title->setString(PlayerData::getInstance()->getSortTypeMsg());
    
    ring("sound/v/button_click.mp3");
}

void ZhuanShengLayer::onBackClick(CCObject *target)
{
	GameScene::getInstance()->popLayer();
    
    ring("sound/v/button_click.mp3");
}


ZhuanShengJuanZhou::~ZhuanShengJuanZhou()
{
    if(tempHero)
        delete tempHero;
}


bool ZhuanShengJuanZhou::init(Hero *hero)
{
	if(!JuanZhou::init())
        return false;

	viewPort = CCSize(20 , 536);
	heroNum = 1;
    
	CCSprite *bg = GetSprite("gameui/zhuansheng_1_3.png");
	bg->setAnchorPoint(ccp(0.5 , 1));
	addChild(bg);
    
	locations.push_back(CCRectMake(-275 , -150 , 100 , 100));
	heroVec = std::vector<Hero*>(1 , (Hero*)NULL);
	heroImgVec = std::vector<HeroIcon*>(1, (HeroIcon*)NULL);

	tempHero = new Hero("dummy", hero->profile->transformTargetHeroId , 1, 0);
	tempHero->isLock = false;
	addHero(tempHero, 0);

    CCLabelTTF *coinLabel;
    addLabelTTF(this , coinLabel , toStr(hero->profile->transformPrice).c_str() , FONT_NUM, 22 , ccp(260 , -35) , ccp(1 , 0.5));
    addLabelBMFont(this, hpLabel, toStr(int(tempHero->health)).c_str(),
                   "bmfont/Sumo_22_yellow.fnt", ccp(-22 , -70), ccp(0.5,0.5));
    hpLabel->setScale(contentScale());
    addLabelBMFont(this, attackLabel, toStr(int(tempHero->attack)).c_str(),
                   "bmfont/Sumo_22_yellow.fnt", ccp(-22 , -105), ccp(0.5,0.5));
    attackLabel->setScale(contentScale());
    addLabelBMFont(this, regenLabel, toStr(int(tempHero->regen)).c_str(),
                   "bmfont/Sumo_22_yellow.fnt", ccp(-22 , -141), ccp(0.5,0.5));
    regenLabel->setScale(contentScale());
    addLabelBMFont(this, levelLabel, "1",
                   "bmfont/Sumo_22_yellow.fnt", ccp(192 , -69), ccp(0.5,0.5));
    levelLabel->setScale(contentScale());
    addLabelBMFont(this, lpLabel, toStr(tempHero->profile->LP).c_str(),
                   "bmfont/Sumo_22_yellow.fnt", ccp(192 , -104), ccp(0.5,0.5));
    lpLabel->setScale(contentScale());
	addLabelTTF(this, skillLabel, AST(tempHero->profile->activeSkillId).name.c_str(), FONT_CN, 24, ccp(198, -139), ccp(0.5, 0.5) , ccc3(0x96, 0, 0));

	return true;
}

bool TransBoard::init(Hero *hero)
{
	if(!ContentLayer::init())
        return false;

    targetHero = hero;
    targetId = hero->id;
    heroCopy = *hero;
    
	CCSprite *bg = GetSprite("gameui/zhuansheng_bg.png");
	bg->setPosition(ccp(0,0));
	addChild(bg);
    
	locations.push_back( ccp(-4 , -4));
	locations.push_back(ccp(-4 , 217));
	locations.push_back(ccp(215 , 75));
	locations.push_back(ccp(140 , -182));
	locations.push_back(ccp(-146 , -182));
	locations.push_back(ccp(-215 , 75));

	angles.push_back(0);
	angles.push_back(70.2);
	angles.push_back(141);
	angles.push_back(-141);
	angles.push_back(-70.2);

	HeroIcon *icon = HeroIcon::create(hero , false , true);
	icon->setPosition(locations[0]);
	addChild(icon);
	heroIcons.push_back(icon);
    
    if(hero->level < hero->profile->maxLevel)
    {
        icon->setEnabled(false);
        icon->levelBlink();
    }

    auto matHeroes = PlayerData::getInstance()->getUnlockHeroByIds(hero->profile->transformMats);
	for (int i=0;i<hero->profile->transformMats.size();i++)
	{
        int mid = hero->profile->transformMats[i];
        auto heroIt = find_if(matHeroes.begin(), matHeroes.end(), [mid](Hero* h){ return h->heroId == mid; });
        HeroIcon* matIcon = NULL;
        if (heroIt != matHeroes.end())
        {
            matIcon = HeroIcon::create(*heroIt);
            matHeroes.erase(heroIt);
        }
        else
        {
            matIcon = HeroIcon::createWithProfile(const_cast<HeroProfile*>(&HPT(mid)));
            matIcon->setEnabled(false);
        }
		heroIcons.push_back(matIcon);
		
		matIcon->setPosition(locations[i+1]);
		addChild(matIcon);
	}

	return true;
}
    
Hero* TransBoard::getHero(CCTouch *touch)
{
    CCPoint point = convertTouchToNodeSpace(touch);
    for(HeroIcon *icon : heroIcons)
    {
        if(icon)
        {
            CCPoint p = icon->getPosition();
            CCSize s = icon->getContentSize();
            CCRect rect = CCRectMake(p.x - s.width*0.5f, p.y - s.height*0.5f, s.width, s.height);
            if (rect.containsPoint(point))
                return icon->getHero();
        }
    }
    return NULL;
}
    
static CCCallLambda* genRemoveFromParent(CCNode* parent, CCNode* node)
{
    return CCCallLambda::create([=](){ parent->removeChild(node, true); });
}

void TransBoard::transSuccess()
{
	HeroIcon *targetIcon = heroIcons[0];
    
    Hero* newHero = PlayerData::getInstance()->getHero(heroCopy.id);
	transHeroIcon = HeroIcon::create(newHero, false, true);
	transHeroIcon->setPosition(locations[0]);
	transHeroIcon->setVisible(false);
	addChild(transHeroIcon);

    float delay = 0.0f;
	for(int i = 1 ; i < heroIcons.size() ; i++)
    {
		if(heroIcons[i])
        {
            HeroIcon *icon = heroIcons[i];
			
			CCSprite *light = GetSprite("gameui/qiu2_1.png");
			light->setPosition(icon->getPosition());
			addChild(light);

			CCAction *hSeq = CCSequence::create(CCDelayTime::create(delay),
                                                CCScaleTo::create(0.2, 0),
                                                CCCallLambda::create([](){ ring("sound/v/light_bling.mp3"); }),
                                                genRemoveFromParent(this, icon),
                                                NULL);
			icon->runAction(hSeq);

			CCAction *seq = CCSequence::create(CCDelayTime::create(delay),
                                               createHeroDisappearAnim(),
                                               genRemoveFromParent(this, light),
                                               NULL);
			light->runAction(seq);
			
			CCSprite *fire = GetSprite("gameui/tou2.png");
			fire->setPosition(icon->getPosition());
			fire->setVisible(false);
			fire->setRotation(angles[i-1]);
			addChild(fire);

			CCSprite *fire_tail = GetSprite("gameui/trans_2.png");
			fire_tail->setAnchorPoint(ccp(0.5 , 0));
			fire_tail->setPosition(ccp(30 , 30));
			fire_tail->setScaleY(0);
			fire->addChild(fire_tail);

			CCAction *fireBody = CCSequence::create(CCDelayTime::create(0.2+delay),
                                                    CCShow::create(),
                                                    CCMoveTo::create(0.2 , targetIcon->getPosition()),
                                                    genRemoveFromParent(this, fire),
                                                    NULL);
			fire->runAction(fireBody);

			CCAction *fireTail = CCSequence::create(CCDelayTime::create(0.2+delay),
                                                    CCScaleTo::create(0.2, 1, 10),
                                                    NULL);
			fire_tail->runAction(fireTail);
            
            delay += 0.2f;
		}
	}
    
    //change the icon of hero
    CCAction *heroAnim = CCSequence::create(CCDelayTime::create(0.6+delay),
                                            CCScaleTo::create(0.2, 0),
                                            genRemoveFromParent(this, targetIcon),
                                            NULL);
    targetIcon->runAction(heroAnim);
    
    
    CCSprite *light1 = GetSprite("gameui/qiu1_1.png");
    light1->setPosition(targetIcon->getPosition());
    light1->setScale(0.1);
    light1->setVisible(false);
    addChild(light1);
    
    CCAction *seq = CCSequence::create(CCDelayTime::create(0.4+delay),
                                       CCShow::create(),
                                       CCScaleTo::create(0.2, 1),
                                       createHeroAppearAnim(),
                                       genRemoveFromParent(this, light1),
                                       CCCallFunc::create(this, callfunc_selector(TransBoard::success)),
                                       NULL);
    light1->runAction(seq);
    
    CCAction *hSeq = CCSequence::create(CCDelayTime::create(0.8+delay),
                                        CCShow::create(),
                                        CCScaleTo::create(0.3, 1.2),
                                        CCScaleTo::create(0.3, 1),
                                        NULL);
    transHeroIcon->runAction(hSeq);
}

void TransBoard::success()
{
	CCSprite *suc = GetSprite("gameui/chenggong_pic.png");
	suc->setScale(2);
	addChild(suc, 10);

	CCAction *seq = CCSequence::create(CCEaseIn::create(CCScaleTo::create(0.5 , 1.0f , 1.0f), 0.5),
                                       CCFadeOut::create(0.2),
                                       genRemoveFromParent(this, suc),
                                       CCCallFunc::create(this, callfunc_selector(TransBoard::afterSuccess)),
                                       NULL);
	suc->runAction(seq);
    ring("sound/v/evolve_success.mp3");
}

void TransBoard::afterSuccess()
{
    ((ZhuanShengConfirmLayer*)getParent())->onTranSuc(targetId);
}
    
bool SourceBoard::init()
{
    if(!ContentLayer::create())
        return false;
    
    bg = Get9SpriteFromFrame("zhuanshegn_chuchu.png", CCRectMake(80, 50, 14, 44));
    bg->setContentSize(CCSize(180, 200));
    bg->setAnchorPoint(ccp(0.5, 0));
    addChild(bg);
    
    content = addLabelTTF(this, "", FONT_CN, 28, ccp(0, 20), ccp(0.5, 0));
    
    return true;
}
    
void SourceBoard::update(const std::vector<string> &sources)
{
    string ss;
    for(string s : sources)
        ss += s+"\n";
    content->setString(ss.c_str());
    bg->setContentSize(CCSizeMake(185, content->getContentSize().height+70));
}
    
//-----------------------------------
// ZhuanShengConfirmLayer

ZhuanShengConfirmLayer::~ZhuanShengConfirmLayer()
{
    if(transHero)
        delete transHero;
}
    
bool ZhuanShengConfirmLayer::init(Hero *hero)
{
	if(!ContentLayer::init())
        return false;
    
    this->hero = hero;
	isTrans = false;
    heroOnTouch = NULL;
    isShowBottom = false;
    isShowNormal = true;

    ContentLayer::setTouchEnabled(true);
    
    transHero = new Hero("dummy", hero->profile->transformTargetHeroId , 1, 0);
    transHero->eliteLevel = hero->eliteLevel/2;

    container = ContentLayer::create();
    addChild(container);
    
	topMenu = CCMenu::create();
	CCMenuItem *itemLeft = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                                    GetSprite("gameui/back_t.png"),
                                                    this,
                                                    menu_selector(ZhuanShengConfirmLayer::onBackClick));
	itemLeft->setAnchorPoint(ccp(0.5 , 1));
	topMenu->addChild(itemLeft );

	CCMenuItem *itemMiddle = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                      GetSprite("gameui/hongchengdi_t_tongyong.png"));
	itemMiddle->setAnchorPoint(ccp(0.5 , 1));
	topMenu->addChild(itemMiddle );

    CCMenuItem* itemRight = CCMenuItemSprite::create(GetSprite("gameui/chengdi_t_tongyong_l.png"),
                                                     GetSprite("gameui/chengdi_t_tongyong_l.png"),
                                                     this,
                                                     menu_selector(ZhuanShengConfirmLayer::onConfirmClick));
    itemRight->setAnchorPoint(ccp(0.5 , 1));
    
    LabelX* detail_label = LabelX::create("确定", FONT_CN, 34);
    detail_label->setPosition(ccp(itemRight->getContentSize().width*0.5,
                                  itemRight->getContentSize().height*0.5f+3));
    detail_label->setColor(TITLE_YELLOW);
    itemRight->addChild(detail_label);
	topMenu->addChild(itemRight);

	topMenu->alignItemsHorizontallyWithPadding(8);
	topMenu->setPosition(ccp(0, 18));
	container->addChild(topMenu, 1);

    LabelX* name = LabelX::create("转生", FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp(itemMiddle->getContentSize().width*0.5f+2,
                          itemMiddle->getContentSize().height*0.5f+2));
    itemMiddle->addChild(name);

	/*transBoard = TransBoard::create(hero);
	transBoard->setPositionY(-320);
	this->addChild(transBoard);

	juanzhou = ZhuanShengJuanZhou::create(hero);
	CCSprite *zhou1 = GetSprite("gameui/zhuansheng_1_1.png");
	CCSprite *zhou2 = GetSprite("gameui/zhuansheng_1_1.png");
    zhou2->setFlipX(true);
	jz = JuanZhouLayer::create();
	jz->addSkin(juanzhou , zhou1 , zhou2);
	jz->setPositionY(-580);
	this->addChild(jz);*/


	ContentLayer* org = createOrgHeroBoard(hero);
    org->setPositionY(-50);
    container->addChild(org);

    ContentLayer* mats = createMatsBoard(hero);
    mats->setPositionY(-200);
    container->addChild(mats, 10);
    
    matSourceBoard = SourceBoard::create();
    matSourceBoard->setVisible(false);
    mats->addChild(matSourceBoard);
    
    ContentLayer* trans = createTargetBoard(transHero);
    trans->setPositionY(-380);
    container->addChild(trans);
    
	return true;
}

ContentLayer* ZhuanShengConfirmLayer::createOrgHeroBoard(Hero* hero)
{
    ContentLayer* layer = ContentLayer::create();
    
    CCSprite* bg = GetSprite("gameui/zhuanshegn_1.png");
    bg->setAnchorPoint(ccp(0.5, 1));
    layer->addChild(bg);
    
    HeroIcon* icon = HeroIcon::create(hero, false, false);
    icon->showElite(true);
    icon->setPosition(ccp(-212, -71));
    icon->setEnabled(!hero->profile->isMat() && (PlayerData::getInstance()->isHeroTransformMatsReady(hero) &&
                                              hero->level >= hero->profile->maxLevel));
    layer->addChild(icon);
    heroIcons.push_back(icon);
    
    addLabel(layer, hero);
    return layer;
    
}
ContentLayer* ZhuanShengConfirmLayer::createMatsBoard(Hero* hero)
{
    ContentLayer* layer = ContentLayer::create();
    
    CCSprite* bg = GetSprite("gameui/zhuanshegn_2_bg.png");
    bg->setAnchorPoint(ccp(0.5, 1));
    layer->addChild(bg);
    
    auto matHeroes = PlayerData::getInstance()->getUnlockHeroByIds(hero->profile->transformMats);
	for (int i=0;i<hero->profile->transformMats.size();i++)
	{
        int mid = hero->profile->transformMats[i];
        auto heroIt = find_if(matHeroes.begin(), matHeroes.end(), [mid](Hero* h){ return h->heroId == mid; });
        HeroIcon* matIcon = NULL;
        if (heroIt != matHeroes.end())
        {
            matIcon = HeroIcon::create(*heroIt);
            matHeroes.erase(heroIt);
        }
        else
        {
            matIcon = HeroIcon::createWithProfile(const_cast<HeroProfile*>(&HPT(mid)));
            matIcon->setEnabled(false);
        }
		mats.push_back(matIcon);
		matIcon->setPosition(ccp(-235+118*i, -102));
		layer->addChild(matIcon);
	}
    
    addLabelBMFont(layer, toStr(hero->profile->transformPrice).c_str(),
                   "bmfont/Sumo_22_yellow.fnt",
                   ccp(290, -30), ccp(1, 0.5))->setScale(contentScale());
    
    return layer;
}
ContentLayer* ZhuanShengConfirmLayer::createTargetBoard(Hero* hero)
{
    ContentLayer* layer = ContentLayer::create();
    
    CCSprite* bg = GetSprite("gameui/zhuanshegn_3.png");
    bg->setAnchorPoint(ccp(0.5, 1));
    layer->addChild(bg);
    
    HeroIcon* icon = HeroIcon::create(hero, false, false);
    icon->showElite(true);
    icon->setPosition(ccp(-212, -71));
    layer->addChild(icon);
    heroIcons.push_back(icon);
    
    addLabel(layer, hero);
    
    if(PlayerData::getInstance()->isPvPOpen)
    {
        normalSkillBtn = ButtonSprite::create("gameui/zjianghuji_b.png", "gameui/zjianghuji_a.png");
        normalSkillBtn->setPosition(ccp(-135, -158));
        normalSkillBtn->onPressed();
        normalSkillBtn->setOnClickListener([=]()
        {
            if(!isShowNormal)
            {
                isShowNormal = true;
                normalSkillBtn->onPressed();
                pvpSkillBtn->onRelease();
                this->updateSkillType(transHero);
            }
        });
        
        pvpSkillBtn = ButtonSprite::create("gameui/zleitaiji_b.png", "gameui/zleitaiji_a.png");
        pvpSkillBtn->setPosition(ccp(135, -158));
        pvpSkillBtn->setOnClickListener([=](){
            if(isShowNormal)
            {
                isShowNormal = false;
                normalSkillBtn->onRelease();
                pvpSkillBtn->onPressed();
                this->updateSkillType(transHero);
            }
        });
        layer->addChild(pvpSkillBtn);
    }
    else
    {
        normalSkillBtn = ButtonSprite::create("gameui/zjianghuji_b.png",
                                              "gameui/zjianghuji_a.png",
                                              kCCMenuHandlerPriority,
                                              false);
        normalSkillBtn->setPosition(ccp(-135, -158));
    }
    layer->addChild(normalSkillBtn);
    
	addLabelTTF(layer, pSkillLabel, "" , FONT_CN , 28 ,  ccp(-150,-191) , ccp(0,1) , ccc3(108, 46, 0));
    pSkillDetailLabel = CCLabelTTF::create("", FONT_CN, 24, CCSizeMake(550, 70), kCCTextAlignmentLeft);
    pSkillDetailLabel->setPosition(ccp(-265,-230));
    pSkillDetailLabel->setAnchorPoint(ccp(0, 1));
    layer->addChild(pSkillDetailLabel);
    
	addLabelTTF(layer, aSkillLabel, "" , FONT_CN , 28 ,  ccp(-150,-297) , ccp(0,1) , ccc3(108, 46, 0));
    aSkillDetailLabel = CCLabelTTF::create("", FONT_CN, 24, CCSizeMake(550, 70), kCCTextAlignmentLeft);
    aSkillDetailLabel->setPosition(ccp(-265,-333));
    aSkillDetailLabel->setAnchorPoint(ccp(0, 1));
    layer->addChild(aSkillDetailLabel);
    
    addLabelTTF(layer,aSkillCoolDown, "", FONT_NUM , 24 ,  ccp(153,-312) , ccp(1,0.5));
    addLabelTTF(layer,aSkillEnergy, "", FONT_NUM , 24 ,  ccp(257,-312) , ccp(1,0.5));
    
    updateSkillType(hero);
    return layer;
}
 
void ZhuanShengConfirmLayer::updateSkillType(Hero* hero)
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
    
    pSkillLabel->setString(pskill.c_str());
    pSkillDetailLabel->setString(pskillDetail.c_str());
    aSkillLabel->setString(askill.c_str());
    aSkillDetailLabel->setString(askillDetail.c_str());
    aSkillCoolDown->setString(coolDownNum.c_str());
    aSkillEnergy->setString(energy.c_str());
}
    
void ZhuanShengConfirmLayer::addLabel(ContentLayer *layer, Hero* hero)
{

    
    int eliteHp = 0;
    int eliteAtk = 0;
    int eliteReg = 0;
    if(hero->eliteLevel > 0)
    {
        eliteHp = hero->profile->eliteHP.eval(hero->eliteLevel);
        eliteAtk = hero->profile->eliteAttack.eval(hero->eliteLevel);
        eliteReg = hero->profile->eliteRegen.eval(hero->eliteLevel);
    }
    
    addLabelBMFont(layer , (toStr(hero->level)+"/"+toStr(hero->profile->maxLevel)).c_str() , "bmfont/Sumo_22_yellow.fnt",
                   ccp(-37 , -73) , ccp(0.5, 0.5))->setScale(contentScale());
    
    addLabelBMFont(layer , toStr(hero->profile->LP).c_str() , "bmfont/Sumo_22_yellow.fnt",
                   ccp(-37 , -109) , ccp(0.5, 0.5))->setScale(contentScale());
    
    CCLabelBMFont* hpLabel = addLabelBMFont(layer, toStr(hero->health-eliteHp).c_str() , "bmfont/Sumo_22_yellow.fnt",
                                            ccp(122, -37) , ccp(0 , 0.5));
    hpLabel->setScale(contentScale());
    
    CCLabelBMFont* attackLabel = addLabelBMFont(layer, toStr(hero->attack-eliteAtk).c_str() , "bmfont/Sumo_22_yellow.fnt",
                                                ccp(122, -73) , ccp(0 , 0.5));
    attackLabel->setScale(contentScale());
    
    CCLabelBMFont* regenLabel = addLabelBMFont(layer, toStr(hero->regen-eliteReg).c_str() , "bmfont/Sumo_22_yellow.fnt",
                                               ccp(122, -109) , ccp(0 , 0.5));
    regenLabel->setScale(contentScale());
    
    if(hero->eliteLevel)
    {
        addLabelBMFont(layer, ("+"+toStr(eliteHp)).c_str(),
                       "bmfont/Sumo_22_purple.fnt",
                       ccp(hpLabel->getPosition().x+hpLabel->getContentSize().width*contentScale()+5,
                           hpLabel->getPosition().y), ccp(0, 0.5))->setScale(contentScale());
        addLabelBMFont(layer, ("+"+toStr(eliteHp)).c_str(),
                       "bmfont/Sumo_22_purple.fnt",
                       ccp(attackLabel->getPosition().x+attackLabel->getContentSize().width*contentScale()+5,
                           attackLabel->getPosition().y), ccp(0, 0.5))->setScale(contentScale());
        addLabelBMFont(layer, ("+"+toStr(eliteHp)).c_str(),
                       "bmfont/Sumo_22_purple.fnt",
                       ccp(regenLabel->getPosition().x+regenLabel->getContentSize().width*contentScale()+5,
                           regenLabel->getPosition().y), ccp(0, 0.5))->setScale(contentScale());
    }
}
    
void ZhuanShengConfirmLayer::setTouchEnabled(bool value)
{
    ContentLayer::setTouchEnabled(value);
    topMenu->setTouchEnabled(value);
}
void ZhuanShengConfirmLayer::onSingleClick(CCTouch* touch)
{
    auto parent = mats[0]->getParent();
    CCPoint localPoint = parent->convertTouchToNodeSpace(touch);
    matSourceBoard->setVisible(false);
    for (HeroIcon* icon : mats)
        if(icon->boundingBox().containsPoint(localPoint))
        {
            auto* layer = HeroProfileLayer::create(icon->getHero() , false);
            GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
            break;
        }
    
}
    
void ZhuanShengConfirmLayer::onLongClick(cocos2d::CCTouch *touch)
{
    CCPoint localPoint = mats[0]->getParent()->convertTouchToNodeSpace(touch);
    Hero* hero = NULL;
    for(HeroIcon* icon : mats)
        if(icon->boundingBox().containsPoint(localPoint))
            hero = icon->getHero();
    
    for(HeroIcon* icon : heroIcons)
    {
        localPoint = icon->getParent()->convertTouchToNodeSpace(touch);
        if(icon->boundingBox().containsPoint(localPoint))
        {
            if(!hero)
                hero = icon->getHero();
            break;
        }
    }
    
    if (hero) {
        GameScene::getInstance()->pushLayerWithDefaultAnimation(HeroProfileLayer::create(hero, false));
    }
}
    
void ZhuanShengConfirmLayer::onBackClick(cocos2d::CCObject *target)
{
	GameScene::getInstance()->popLayer();
    
    ring("sound/v/button_click.mp3");
}

void ZhuanShengConfirmLayer::onConfirmClick(cocos2d::CCObject *target)
{
    PlayerData *data = PlayerData::getInstance();

	boost::function<void ()> toTransform = [=]()
    {
        std::vector<std::string> mats;
        auto heroes = PlayerData::getInstance()->getUnlockHeroByIds(hero->profile->transformMats);
        std::transform(heroes.begin(), heroes.end(), back_inserter(mats), [](Hero* h){return h->id;});

		SystemInform *inform = SystemInform::alertLoading(GameScene::getInstance());
        
		PuzzleRPC::getInstance()->transformHero(PlayerData::getInstance()->getToken() , hero->id , mats , [=](std::auto_ptr<Player> player , std::auto_ptr<Error> e)
		{
            inform->cancel();

			if(e.get())
            {
				std::cout << "!!! zhuansheng error:" << e->msg.c_str() << std::endl;

				SystemInform::alertMessage(GameScene::getInstance(),
										   e->toStrCN().c_str(),
                                           "警告" ,
										   [](){});
				return;
			}
            this->setTouchEnabled(false);
			PlayerData::getInstance()->updatePlayer(player.get());

            GameScene::getInstance()->showTop(false);
            container->removeFromParentAndCleanup(true);
            transBoard = TransBoard::create(hero);
            transBoard->setPositionY(-460);
            this->addChild(transBoard, 100);
            
			transBoard->transSuccess();
		});
	};
    
    std::string msg = "";
    if(data->isHeroTransformMatsReady(hero))
    {
        if(hero->level < hero->profile->maxLevel)
        {
            msg = "侠客等级不够";
        }
        else if(data->player->coins < hero->profile->transformPrice)
        {
            msg = "铜钱不够了";
        }
        else
        {
            msg = "确定转生吗？";
            int lpGap = HPT(hero->profile->transformTargetHeroId).LP - hero->profile->LP;
            if(data->isHeroInActiveTeam(hero) && lpGap+data->teamData.lp > data->teamData.maxLp)
                msg = "侠客转生后会导致当前队伍的总威望超过上限，确定转生吗？";
            SystemInform::alertConfirmPanel(GameScene::getInstance(),
                                            msg.c_str(),
                                            "警告",
                                            toTransform,
                                            [=](){});
            return;
        }
        if(!msg.empty())
            SystemInform::alertMessage(GameScene::getInstance(),
                                       msg.c_str(),
                                       "警告" ,
                                       [](){});
    }
    else
        SystemInform::alertConfirmPanel(runningScene(),
                                        "侠客转生素材不足\n立即充值成为VIP2后\n可通过招募获得大量转生道具",
                                        "警告",
                                        [](){
                                            GameScene::getInstance()->selectTab(3);
                                        }, [](){}, false, "招募");
    
    ring("sound/v/button_click.mp3");
}
    
void ZhuanShengConfirmLayer::onTranSuc(std::string targetId)
{
    Hero *hero = PlayerData::getInstance()->getHero(targetId);
    auto* layer = HeroProfileLayer::create(hero , false);
    GameScene::getInstance()->popLayer();
    GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
}

void ZhuanShengConfirmLayer::onEnterAnimationDone()
{
	//jz->startAnim();
}

}