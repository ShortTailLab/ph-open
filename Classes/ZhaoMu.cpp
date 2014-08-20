#include "ZhaoMu.h"
#include "PlayerData.h"
#include "SystemInform.h"
#include "Store.h"
#include "HeroProfileLayer.h"
#include "HeroIcon.h"
#include "Board/Utils.h"
#include "CCCallLambda.h"
#include "UIPresets.h"
#include "Fonts.h"
#include "GameScene.h"
#include "CCNative.h"
#include "Common.h"
#include "network/PuzzleRPC.h"
#include "DebugMisc.h"
#include "Tutorial.h"

#include <extensions/utils.h>
#include <boost/make_shared.hpp>

namespace PH
{
    USING_NS_CC;
    USING_NS_CC_EXT;
    using namespace std;

static const char* CARD_PATH = "gameui/kapai_8_f.png";
static const char* CARD_BACK_PATH = "gameui/kapai_8_back.png";
    
static const char* DRAW_FREE_FONT = "bmfont/PW_24_green.fnt";
static const char* DRAW_NORMAL_FONT = "bmfont/PW_24_white.fnt";
static const char* DRAW_RED_FONT = "bmfont/PW_24_red.fnt";
    
HeroCard* runPickCardAnim(CCLayer* par , CCSprite *card, Hero *hero, boost::function<void ()> afterCall)
{
    HeroCard *heroCard;
    
    {
        card->setAnchorPoint(ccp(0.5, 0.5));
        CCAction *cardAction = CCSequence::create(CCEaseIn::create(CCScaleTo::create(0.5, 0, 1), 0.5),
                                                  CCCallLambda::create([=](){card->removeFromParentAndCleanup(true);}),
                                                  NULL);
        card->runAction(cardAction);
    }
    
    {
        heroCard = HeroCard::create(CARD_PATH, hero);
        heroCard->setPosition(card->getPosition());
        heroCard->setScaleX(0.01);
        heroCard->setVisible(false);
        heroCard->hideHero(true);
        
        par->addChild(heroCard, 1);  
        
        //bind each HeroCard to card
        card->setUserData(heroCard);
    
        CCFiniteTimeAction *seq_rotate = CCSequence::create(CCHide::create(),
                                                     CCDelayTime::create(0.5),
                                                     CCShow::create(),
                                                     CCEaseOut::create(CCScaleTo::create(0.8, 1, 1), 1),
                                                     NULL);
        
        CCAction *seq_alert = CCSequence::create(CCMoveTo::create(0.2 , ccp(0 , -350)),
                                                 CCEaseElasticOut::create(CCScaleTo::create(0.5, 1.8f, 1.8f), 1),
                                                 CCCallLambda::create([=](){afterCall();heroCard->hideHero(false);}),
                                                 NULL);
        
        CCFiniteTimeAction *seq = CCSequence::create(seq_rotate,
                                                     CCDelayTime::create(0.5),
                                                     CCCallLambda::create([=](){heroCard->setAnchorPoint(ccp(0.5, 0.5));}),
                                                     seq_alert,
                                                     NULL);
        heroCard->runAction(seq);
    }
    
    {
        CCSprite* light = GetSprite("gameui/card_light.png");
        light->setPosition(card->getPosition());
        light->setScale(0);
        par->addChild(light, 1);
        
        CCAction *lSeq = CCSequence::create(CCDelayTime::create(0.5),
                                            CCSpawn::create(CCScaleTo::create(1.3, 1, 1),
                                                            CCRotateBy::create(1.3, 60),
                                                            NULL),
                                            CCCallLambda::create([=](){light->removeFromParentAndCleanup(true);}),
                                            NULL);
        light->runAction(lSeq);
    }
    
    return heroCard;
}
    
JuanZhou* createMenu(Hero* hero ,const char* msg)
{
    JuanZhou* jz = JuanZhou::create(); 
    jz->viewPort = CCSizeMake(20, 324);
    
    CCSprite* bg = GetSprite("gameui/tishi_juanzhou_choujiang1.png");
    bg->setAnchorPoint(ccp(0.5, 1));
    jz->addChild(bg);
    
	/*CCScale9Sprite *bg = CCScale9Sprite::create("gameui/jinengtishi_bg.png" ,
                                                CCRectMake(0,0,308 , 212) ,
                                                CCRectMake(50,50,208,112));
	bg->setContentSize(CCSizeMake(500 , 220));
	alertMenu->addChild(bg);*/
    CCSprite* huode = GetSprite("gameui/huodexiake_wenzi.png");
    huode->setPosition(ccp(-186, -40));
    jz->addChild(huode);
    
	HeroIcon *heroIcon = HeroIcon::create(hero);
	heroIcon->setPosition(ccp(-186 , -108));
	jz->addChild(heroIcon);
    
    CCLabelTTF *label = CCLabelTTF::create(msg,
                                           FONT_CN, 26,
                                           CCSizeMake(370, 150),
                                           CCTextAlignment::kCCTextAlignmentLeft);
	label->setPosition(ccp(-120, -50));
    label->setAnchorPoint(ccp(0, 1));
	jz->addChild(label);
    
    HeroSoulDrop drop = hero->soul;
    if (drop.heroId != 0)
    {
        CCSprite* soulLabel = GetSprite("gameui/zengsonghunpo_wenzi.png");
        soulLabel->setPosition(ccp(-186, -182));
        jz->addChild(soulLabel);
        
        HeroIcon* ghostIcon = HeroIcon::createWithProfile(&HPT(drop.heroId), true);
        ghostIcon->setPosition(ccp(-186, -250));
        ghostIcon->showPlusNum(drop.count);
        jz->addChild(ghostIcon);
    }
    

    
    return jz;
}

void AlertMenu::update(float dt)
{
    if(isOpen)
    {
        easing(zhou1 , -285);
        if(easing(zhou2 , 285))
        {
            juanzhou->openDone();
            isOpen = false;
        }
        float w = zhou2->getPositionX();
        juanzhou->viewPort.width = 2*(w-10);
    }
}
    
void AlertMenu::onEnterAnimationDone()
{
    ContentLayer::onEnterAnimationDone();
    startAnim();
}


////////////////////////////////////////////////
//HeroCard
////////////////////////////////////////////////
bool HeroCard::init(const char* cardPath , Hero *hero)
{
	if( !CCSprite::init())
        return false;

	isShowHero = false;
	count = 0;
	this->hero = hero;

	CCSprite *bg = GetSprite(cardPath);
	bg->setScale(0.5);
	bg->setPositionY(-5);
	addChild(bg , 0);

    {
        front = makeHeroImg(hero);
        float h = front->getContentSize().height;
        if(h > 180)
        {
            float scale = 180/h;
            front->setScale(scale);
        }
        front->setPosition(ccp(0 , 0));
        this->addChild(front , 2);
    }

	scheduleUpdate();
	
	//setAnchorPoint(ccp(0.5f , 0.5f));
	return true;
}

void HeroCard::hideHero(bool value)
{
	if(value){
		front->setColor(ccc3(0,0,0));
	}else{
		isShowHero = true;
		CCSprite *light = GetSprite("gameui/guang.png");
		light->setScale(0.5);
		addChild(light,0);
		CCActionInterval *action = CCRotateBy::create(2 , 180);
		CCAction *seq = CCSequence::create(action ,CCCallFuncND::create(this , callfuncND_selector( HeroCard::onAnimaDone) , light) , NULL); 
		light->runAction(seq);
		
	}
}

void HeroCard::showStar(bool withAnima)
{
    for(CCSprite* star : stars)
    {
        star->removeFromParentAndCleanup(true);
    }
    stars.clear();
    
    int starCount = hero->profile->star;
    int maxStar = hero->profile->getMaxStar();
    
	int l = 30;
	int startX = -l*(maxStar-1) * 0.5;

	for(int i = 0; i < maxStar; i++)
    {
		CCSprite *star = GetSprite((i < starCount) ? "gameui/xingxing2.png" : "gameui/xingxing3.png");
		star->setPosition(ccp(startX + l*i, 82));
        stars.push_back(star);
		addChild(star, 3);
        
        if(!withAnima)
            continue;
        
        star->setScale(0);
		CCDelayTime *a1 = CCDelayTime::create(0.2*i);
		CCActionInterval *a2 = CCScaleTo::create(0.5f, 0.8f, 0.8f);
		CCActionInterval *a3 = CCEaseElasticOut::create(a2, 0.5f);
		CCAction *a;
		if(i != maxStar-1)
            a = CCSequence::create(a1,
                                   a3,
                                   NULL);
		else
            a = CCSequence::create(a1,
                                   a3,
                                   CCCallFunc::create(this,
                                                      callfunc_selector(HeroCard::showDes)),
                                   NULL);
		star->runAction(a);
	}
}

void HeroCard::showDes()
{
	const char* path;
	switch(hero->profile->star)
    {
	case 1:
        path = "gameui/star_title_1.png";
		break;
	case 2:
        path = "gameui/star_title_2.png";
		break;
	case 3:
        path = "gameui/star_title_3.png";
		break;
	case 4:
        path = "gameui/star_title_4.png";
		break;
	case 5:
        path = "gameui/star_title_5.png";
		break;
	case 6:
    default:
        path = "gameui/star_title_6.png";
		break;
	}

	CCSprite *des = GetSprite(path);
	des->setScale(1.5f);
	des->setPositionY(-80);
	addChild(des , 3);

	CCActionInterval *a1 = CCScaleTo::create(0.5f , 0.6f , 0.6f);
	CCActionInterval *a2 = CCEaseInOut::create(a1 , 0.8f);
	des->runAction(a2);
}

void HeroCard::update(float dt)
{
	if(isShowHero){
		count += 5;
		ccColor3B color = ccc3(count , count , count);
		if(count >= 255){
			color = ccc3(255,255,255);
			showStar();
			isShowHero = false;
		}
		front->setColor(color);
	}
}

void HeroCard::onAnimaDone(cocos2d::CCNode *target , void *light)
{
	removeChild( (CCNode*)light , true);
}

////////////////////////////////////////////////
//InformLayer
////////////////////////////////////////////////

bool InformLayer::init(const InformType& type)
{
	if(!ContentLayer::init())
        return false;
    
	this->type = type;
    juedingFreeTime = NULL;
    transTimes = NULL;
    expTimes = NULL;
    
    const char* titleName = (type == InformType::NiceDraw) ? "招绝顶高手" : "招材料道具";

	topMenu = CCMenu::create();
	CCMenuItem *itemUp = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                                  GetSprite("gameui/back_t.png"),
                                                  this,
                                                  menu_selector(InformLayer::onBack));
	topMenu->addChild(itemUp);
	CCMenuItem *itemMiddle = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                      GetSprite("gameui/hongchengdi_t_tongyong.png"));
	topMenu->addChild(itemMiddle);
    itemUp->setAnchorPoint(ccp(0.5, 1));
    itemMiddle->setAnchorPoint(ccp(0.5, 1));
    topMenu->alignItemsHorizontallyWithPadding(8);
	topMenu->setPosition(ccp(-70, 18));
	addChild(topMenu, 10);
    
    LabelX* name = LabelX::create(titleName, FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp(itemMiddle->getContentSize().width*0.5f-2,
                          itemMiddle->getContentSize().height*0.5f+2));
    itemMiddle->addChild(name);

    {
        scrollView = ZqlScrollView::create();
        scrollView->setViewSize(CCSize(winSize.width , 800),
                                CCRect(0, -100, winSize.width, 600));
        scrollView->setPosition(ccp(0, 40));
        scrollView->unscheduleUpdate();
        scrollView->setTouchEnabled(false);
        this->addChild(scrollView);
        
        if(type == InformType::NiceDraw)
        {
            scrollView->addLayer(createItem(ZhaoMuItemType::JueDing));
            //if(PlayerData::getInstance()->hasDrawJueDing)
                scrollView->addLayer(createItem(ZhaoMuItemType::JueDing10));
        }
        else
        {
            scrollView->addLayer(createItem(ZhaoMuItemType::DanYao10));
            scrollView->addLayer(createItem(ZhaoMuItemType::Tool10));
        }
    }

    if(GameConfig::shared()->getTutorialProgress() == TutorialSpendGoldOnHero)
    {
        itemUp->setVisible(false);
        
        ArrowMask::shared()->clear();
        ArrowMask::shared()->setMask(CCRect(0, 0, 640, 960));
    }
    else if(GameConfig::shared()->openJueDing10)
    {
        ArrowMask::shared()->clear();
        ArrowMask::shared()->setMask(CCRect(0, 0, 640, 960));
    }

	return true;
}
    
void InformLayer::onEnterAnimationDone()
{
    ContentLayer::onEnterAnimationDone();

    if(GameConfig::shared()->getTutorialProgress() == TutorialSpendGoldOnHero)
    {
        ArrowMask::shared()->setMask(CCRect(20, 520, 600, 230));
        ArrowMask::shared()->setArrow(ccp(510, 560), ArrowDirection::Up);
    }
    else if(GameConfig::shared()->openJueDing10)
    {
        ArrowMask::shared()->setMask(CCRect(20, 307, 600, 210));
        AlertMessage::create("",
                             "绝顶十连抽开启！必定可以获得一个5星侠客，掌门不妨试试使用这个招募噢！",
                             false,
                             [=](){ArrowMask::shared()->clear();});
        ArrowMask::shared()->setArrow(ccp(510, 500), ArrowDirection::Down);
        GameConfig::shared()->openJueDing10 = false;
        GameConfig::shared()->save();
    }
}
    
void InformLayer::onEnter()
{
    ContentLayer::onEnter();
    updateTimer(0);
}
    
void InformLayer::setTouchEnabled(bool value)
{
    ContentLayer::setTouchEnabled(value);
    topMenu->setTouchEnabled(value);
}
    
void InformLayer::updateTimer(float dt)
{
    if(transTimes)
        transTimes->setString(("今日剩余次数："+toStr(PlayerData::getInstance()->drawTransLeft)).c_str());
    
    if(expTimes)
        expTimes->setString(("今日剩余次数："+toStr(PlayerData::getInstance()->drawExpLeft)).c_str());
    
    if(!juedingFreeTime)
        return;
    
    using namespace boost::date_time;
    using namespace boost::posix_time;
    ptime juedingPTime = from_time_t(PlayerData::getInstance()->drawFreeTime[1]);
    ptime now = boost::posix_time::second_clock::universal_time() +
    PlayerData::getInstance()->delta_local_server;
    
    auto juedingPInterval = juedingPTime - now;
    if(juedingPInterval.is_negative())
    {
        juedingFreeTime->setFntFile(DRAW_FREE_FONT);
        juedingFreeTime->setString("1次免费抽");
    }
    else
    {
        int h = juedingPInterval.hours();
        int m = juedingPInterval.minutes();
        int s = juedingPInterval.seconds();
        char ss[100];
        if(h)
            sprintf(ss, "%02d:%02d:%02d", h, m, s);
        else
            sprintf(ss, "%02d:%02d", m, s);
        juedingFreeTime->setFntFile(DRAW_NORMAL_FONT);
        juedingFreeTime->setString((string(ss)+"后免费").c_str());
    }
}
    
ContentLayer* InformLayer::createItem(const ZhaoMuItemType& type)
{
    ContentLayer* layer = ContentLayer::create();
    layer->setContentSize(CCSizeMake(640, 230));
    
    CCSprite* bg = GetSprite("gameui/zhaomudantiao2_bg.png");
    bg->setAnchorPoint(ccp(0.5, 1));
    layer->addChild(bg);
    
    ButtonSprite* btn;
    string msg1 = "";
    string msg2 = "";
    int price = 0;
    if(type == ZhaoMuItemType::JueDing)
    {
        if(!PlayerData::getInstance()->hasDrawJueDing)
        {
            CCSprite* tips = GetSprite("gameui/vipshouchou.png");
            tips->setAnchorPoint(ccp(0, 0.5));
            tips->cocos2d::CCNode::setPosition(-265, -15);
            layer->addChild(tips);
        }
        price = 210;
        msg1 = "获得3星以上侠客";
        msg2 = "升级VIP可减少招募冷却时间";
        
        if(!(GameConfig::shared()->getTutorialProgress() == TutorialSpendGoldOnHero))
        {
            addLabelBMFont(layer, juedingFreeTime, "", DRAW_NORMAL_FONT,
                        ccp(250, -25), ccp(1, 0.5));
            juedingFreeTime->setScale(contentScale());
            schedule(schedule_selector(InformLayer::updateTimer) , 1);
        }
        
        btn = ButtonSprite::create("gameui/juedign.png", "gameui/juedign.png");
        btn->setOnClickListener([=]()
        {
            if(PlayerData::getInstance()->isDrawFree(1) ||
               PlayerData::getInstance()->player->golds >= price)
            {
                GameScene::getInstance()->pushLayer(PickSuperHeroLayer::create());
                if(GameConfig::shared()->getTutorialProgress() == TutorialSpendGoldOnHero)
                {
                    ArrowMask::shared()->clear();
                }
            }
            else
                SystemInform::alertMessage(GameScene::getInstance(),
                                           "你的元宝不足", "提示",
                                           [](){
                                               ContentLayer *layer = ChongZhiLayer::create();
                                               layer->isShowBottom = false;
                                               GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
                                           });
        });
    }
    else if(type == ZhaoMuItemType::JueDing10)
    {
        if(!PlayerData::getInstance()->hasTenDraw)
        {
            CCSprite* tips = GetSprite("gameui/detiaming.png");
            tips->setAnchorPoint(ccp(0, 0.5));
            tips->setPosition(ccp(-265, -15));
            layer->addChild(tips);
        }
        price = 2000;
        msg1 = "十连抽赠送一抽\n所出侠客必含5星";
        msg2 = "几率抽出五绝五神";
        btn = ButtonSprite::create("gameui/shilian.png", "gameui/shilian.png");
        btn->setOnClickListener([=]()
        {
            if(PlayerData::getInstance()->player->golds >= price)
            {
                InformType type = InformType::Hire10Draw;
                GameScene::getInstance()->pushLayer(Hire10Layer::create(type));
            }
            else
                SystemInform::alertMessage(GameScene::getInstance(),
                                           "你的元宝不足", "提示",
                                           [](){
                                               ContentLayer *layer = ChongZhiLayer::create();
                                               layer->isShowBottom = false;
                                               GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
                                           });
            
        });
    }
    else if(type == ZhaoMuItemType::DanYao10)
    {
        price = 250;
        msg1 = "获得强化侠客丹药";
        msg2 = "必出高级丹药";
        expTimes = addLabelBMFont(layer, ("今日剩余次数："+toStr(PlayerData::getInstance()->drawExpLeft)).c_str(),
                                  DRAW_NORMAL_FONT, ccp(250, -25), ccp(1, 0.5));
        expTimes->setScale(contentScale());
        
        btn = ButtonSprite::create("gameui/danyao.png", "gameui/danyao.png");
        btn->setOnClickListener([=](){
            if(PlayerData::getInstance()->player->golds >= price && PlayerData::getInstance()->drawExpLeft > 0)
            {
                InformType type = InformType::DanYaoDraw;
                GameScene::getInstance()->pushLayer(Hire10Layer::create(type));
                return;
            }
            else if(PlayerData::getInstance()->player->golds < price)
                SystemInform::alertMessage(GameScene::getInstance(),
                                          "你的元宝不足", "提示",
                                           [](){
                                               ContentLayer *layer = ChongZhiLayer::create();
                                               layer->isShowBottom = false;
                                               GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
                                           });
            else
                SystemInform::alertConfirmPanel(runningScene(),
                                                "今日招募次数已经用完，升级VIP可获得更多的次数", "提示",
                                                [](){},
                                                [](){GameScene::getInstance()->pushLayerWithDefaultAnimation(VIPLayer::create());},
                                                false,
                                                "确定",
                                                "VIP");
        });
    }
    else if(type == ZhaoMuItemType::Tool10)
    {
        price = 250;
        msg1 = "获得转生材料";
        transTimes = addLabelBMFont(layer, ("今日剩余次数："+toStr(PlayerData::getInstance()->drawTransLeft)).c_str(),
                                 DRAW_NORMAL_FONT, ccp(250, -25), ccp(1, 0.5));
        transTimes->setScale(contentScale());
        
        btn = ButtonSprite::create("gameui/daoju_b.png", "gameui/daoju_b.png");
        btn->setOnClickListener([=](){
            if(PlayerData::getInstance()->player->golds >= price &&
               PlayerData::getInstance()->drawTransLeft > 0)
            {
                InformType type = InformType::ToolDraw;
                GameScene::getInstance()->pushLayer(Hire10Layer::create(type));
                return;
            }
            else if(PlayerData::getInstance()->player->golds < price)
                SystemInform::alertMessage(GameScene::getInstance(),
                                           "你的元宝不足", "提示",
                                           [](){
                                               ContentLayer *layer = ChongZhiLayer::create();
                                               layer->isShowBottom = false;
                                               GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
                                           });                      
            else
                SystemInform::alertConfirmPanel(runningScene(),
                                                "今日招募次数已经用完，升级VIP可获得更多的次数", "提示",
                                                [](){},
                                                [](){GameScene::getInstance()->pushLayerWithDefaultAnimation(VIPLayer::create());},
                                                false,
                                                "确定",
                                                "VIP");
            
        });
    }
    
    CCLabelTTF* t1 = NULL;
    if(!msg1.empty())
    {
        //addSprite(layer, GetSprite("gameui/vip_dian.png"), ccp(-255, posY));
        t1 = addLabelTTF(layer, msg1.c_str(), FONT_CN, 26, ccp(-220, -40), ccp(0, 1));
        t1->setDimensions(CCSizeMake(315, 0));
        t1->setHorizontalAlignment(CCTextAlignment::kCCTextAlignmentLeft);
    }
    if(!msg2.empty())
    {
        int posY = t1 ? t1->getPositionY()-t1->getContentSize().height : -40;
        CCLabelTTF* t = addLabelTTF(layer, msg2.c_str(), FONT_CN, 26, ccp(-220, posY), ccp(0, 1),
                    ccc3(0x60, 0xff, 0x00));
        t->setDimensions(CCSizeMake(355, 0));
        t->setHorizontalAlignment(CCTextAlignment::kCCTextAlignmentLeft);
    }
    
    CCSprite* gold = GetSprite("gameui/yuanbao_x.png");
    gold->setAnchorPoint(ccp(1, 0.5));
    gold->setPosition(ccp(-60, -165));
    layer->addChild(gold);
    addLabelBMFont(layer, toStr(price).c_str(), "bmfont/Sumo_24_orange.fnt", ccp(-60, -165),
                   ccp(0, 0.5))->setScale(contentScale());
    btn->setPosition(ccp(190, -110));
    layer->addChild(btn);
    return layer;
}
    
void InformLayer::onBack(cocos2d::CCObject *target)
{
	GameScene::getInstance()->popLayer();
    ring("sound/v/button_click.mp3");
}

void InformLayer::onConfirm(cocos2d::CCObject *target)
{
    ring("sound/v/button_click.mp3");
    
    if(!PlayerData::getInstance()->isDrawFree(type) && times<1)
    {
        string money = type ? "元宝" : "铜钱";

        SystemInform::alertConfirmPanel(GameScene::getInstance(),
                                   ("你的" + money + "不足，是否充值？").c_str(),"警告",
                                        [=](){
                                            ChongZhiLayer *layer = ChongZhiLayer::create();
                                            layer->isShowBottom = false;
                                            GameScene::getInstance()->pushLayer(layer);
                                        }, [=](){});
        return;
    }
    
    if(GameConfig::shared()->getTutorialProgress() == TutorialSpendGoldOnHero)
    {
        ArrowMask::shared()->clear();
    }
    
    // for tutorial to work, do not pop self.
    //GameScene::getInstance()->popLayer();
    if(type == InformType::NormalDraw)  
        GameScene::getInstance()->pushLayerWithDefaultAnimation(PickNorHeroLayer::create());
    else if(type == InformType::NiceDraw)
        GameScene::getInstance()->pushLayer(PickSuperHeroLayer::create());
    else 
        GameScene::getInstance()->pushLayer(Hire10Layer::create(type));
    
    
}


////////////////////////////////////////////////
//PickNorHeroLayer
////////////////////////////////////////////////
    
bool PickNorHeroLayer::init()
{
    if(!ContentLayer::init())
        return false;
    
    isShowTop = false;
    isShowBottom = false;
    
    card = GetSprite(CARD_BACK_PATH);
    card->setPosition(ccp(0 , -winSize.height*0.5f));
    addChild(card);
    
    PlayerData::getInstance()->registerStorageCallBack(this, [=]()
                                                       {
                                                           if(targetHero)
                                                               targetHero = HeroPtr(new Hero(*PlayerData::getInstance()->getHeroByHeroId(targetHero->heroId)));
                                                       });
    return true;
}
    
void PickNorHeroLayer::onEnter()
{
    ContentLayer::onEnter();
    deleteMenu();
    makeMenu();
}

void PickNorHeroLayer::onExit()
{
    ContentLayer::onExit();
    PlayerData::getInstance()->unRegisterStorageCallBack(this);
}
    
void PickNorHeroLayer::pickACard(cocos2d::CCSprite *card)
{
    setTouchEnabled(false);
    
    SystemInform* inform = SystemInform::alertLoading(GameScene::getInstance());
    
    PuzzleRPC::getInstance()->drawHero(PlayerData::getInstance()->getToken(),
                                       [=](auto_ptr<Hero> autohero,
                                           auto_ptr<Error> e)
   {
       inform->cancel();
       if(e.get())
       {
           LOGD("!!! Draw hero error %s\n", e->toStr().c_str());
           e->handleErrorInform();
           GameScene::getInstance()->popLayer();
           return;
       }
       
       HeroPtr hero = HeroPtr(autohero.release());
       PlayerData* data = PlayerData::getInstance();
       data->addHero(new Hero(*hero)); // pass a copy to playerData
       data->setDrawFreeTime(0,hero->nextFreeDraw);
       data->drawFreeLeft--;
       
       CCLog("add Hero");
       
       targetHero = hero;
       
       runPickCardAnim(this, card, targetHero.get() ,[=](){this->makeMenu();});
   });
}
    
void PickNorHeroLayer::makeMenu()
{
    if(!targetHero)
        return;
    
    string msg = "";
    CCSprite *itemL = NULL;
    CCSprite *itemR = NULL;
    
    msg = targetHero->profile->nameCN + "已经加入了你的队伍\n";
    itemL = createBtn("查看", ButtonType::NORMAL_RED);
    itemR = createBtn("返回", ButtonType::NORMAL_BLUE);
    
    JuanZhou* jz = createMenu(targetHero.get(), msg.c_str());
    
	CCMenu *menu = CCMenu::create();
    CCMenuItem *itemMiddle = CCMenuItemSprite::create(itemL , itemL , this, menu_selector(PickNorHeroLayer::onGoOn));
    menu->addChild(itemMiddle);
    
	CCMenuItem *itemDown = CCMenuItemSprite::create(itemR , itemR , this, menu_selector(PickNorHeroLayer::onGiveUp));
	menu->addChild(itemDown );
    
	menu->alignItemsHorizontallyWithPadding(40);
	menu->setPosition(ccp(0,-240));
	jz->addChild(menu , 1);
    
    CCSprite* zhou = GetSprite("gameui/tongyong_1_1.png");
    zhou->setFlipX(true);
    alertMenu = AlertMenu::create();
    alertMenu->addSkin(jz,  GetSprite("gameui/tongyong_1_1.png"),  zhou);
    
	alertMenu->setPosition(ccp(0, -1500));
	alertMenu->_setZOrder(10);
    
    this->addChildWithAnim(alertMenu, true,
                           CCSequence::create(CCEaseSineOut::create(CCMoveTo::create(0.5 , ccp(0, -580))),
                                              CCCallLambda::create([=]() { alertMenu->startAnim();}),
                                              NULL));
}
    
void PickNorHeroLayer::deleteMenu()
{
    this->removeChild(alertMenu, true);
}
    
void PickNorHeroLayer::onGoOn()
{
    auto* layer = HeroProfileLayer::create(PlayerData::getInstance()->getHeroByHeroId(targetHero->heroId));
    GameScene::getInstance()->popLayer();
    GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
}

void PickNorHeroLayer::onGiveUp()
{
    // return to top level ZhaoMu menu
    GameScene::getInstance()->popLayer();
    
    ring("sound/v/button_click.mp3");
}
    
    
void PickNorHeroLayer::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    CCTouch *touch = (CCTouch*)pTouches->anyObject();
	CCPoint point = convertTouchToNodeSpace(touch);
    
    if(card->boundingBox().containsPoint(point))
        pickACard(card);
}
    

////////////////////////////////////////////////
//Pick Super Hero Layer
////////////////////////////////////////////////
    
bool PickSuperHeroLayer::init()
{
    if(!ContentLayer::init())
        return false;
    
	pickHeroCount = 0;
	isShowTop = false;
    isShowBottom = false;
    targetHero.reset();
    isAllowPickCard = false;
    pickHeroCount = 0;
    drawTimes = 0;
    
    //init the touch state after this layer pushed in.
    //used to solve the bug in low band.
	setTouchEnabled(false);
    
    drawPrice = PlayerData::getInstance()->drawNiceHeroPrices;
    
    for(int i = 0 ; i < 8 ; i ++)
    {
        int px = -160 + (i%2)*320;
        int py = -120 - int(i/2)*240;
        locations.push_back(ccp(px , py));
        
        CCSprite *card = GetSprite(CARD_BACK_PATH);
        card->setAnchorPoint(ccp(0.5,0.5));
        card->setPosition(ccp(px , py));
        addChild(card);
        cards.push_back(card);
    }

    SystemInform* inform = SystemInform::alertLoading(GameScene::getInstance());
    
    PuzzleRPC::getInstance()->drawHero2(PlayerData::getInstance()->getToken(),
                                        pickHeroCount,
                                        [=](auto_ptr<Hero> autohero,
                                            auto_ptr<Error> e)
    {
        inform->cancel();
        if(e.get())
        {
            LOGD("!!! Draw hero error %s\n", e->toStr().c_str());
            e->handleErrorInform();
            GameScene::getInstance()->popLayer();
            return;
        }
        
        // notify tutorial progress
        if(GameConfig::shared()->getTutorialProgress() == TutorialSpendGoldOnHero)
            GameConfig::shared()->setTutorialProgress(TutorialTeam);
        
        HeroPtr hero = HeroPtr(autohero.release());
        
        herosToDraw.push_back(hero);
        vector<int> heroIDs = hero->next8Draws;
        
        for(int id : heroIDs)
        {
            herosToDraw.push_back(boost::make_shared<Hero>("hero"+toStr(id), id, 1, 0));
        }
        
        if(!PlayerData::getInstance()->isDrawFree(1))
        {
            int golds = PlayerData::getInstance()->getPlayer()->golds;
            PlayerData* data = PlayerData::getInstance();
            data->setGolds(golds-drawPrice[pickHeroCount]);
            
            if(data->player->vipLevel > 0)
            {
                if(!data->hasDrawJueDing)
                {
                    GameConfig::shared()->openJueDing10 = true;
                    GameConfig::shared()->save();
                }
                
                data->hasDrawJueDing = true;
            }
        }
        
        Hero* heroGot = new Hero(*hero);
        PlayerData::getInstance()->addHero(heroGot);
        PlayerData::getInstance()->setDrawFreeTime(1,hero->nextFreeDraw);
        
        if(hero->soul.heroId != 0)
            PlayerData::getInstance()->addHeroSoul(hero->soul);
        
        targetHero = hero;
        showAllCards(false);
                            
    });

    
    PlayerData::getInstance()->registerStorageCallBack(this, [=]()
    {
        if(targetHero)
            targetHero = boost::make_shared<Hero>(*PlayerData::getInstance()->getHeroByHeroId(targetHero->heroId));
    });
    
	return true;
}
    
void PickSuperHeroLayer::onEnter()
{
    ContentLayer::onEnter();
    deleteMenu();
    makeMenu();
}

void PickSuperHeroLayer::onExit()
{
    ContentLayer::onExit();
    PlayerData::getInstance()->unRegisterStorageCallBack(this);
}
    
void PickSuperHeroLayer::calDrawTimes()
{
    if(pickHeroCount >=3 )
        return;
    
    int has = PlayerData::getInstance()->getPlayer()->golds;
    int cost = PlayerData::getInstance()->drawNiceHeroPrices[pickHeroCount];
    drawTimes = int(has/cost);
}
    
void PickSuperHeroLayer::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	if( !isAllowPickCard)
    {
        hideAllCards();
        isAllowPickCard = true;
	}
    else
    {
        CCTouch *touch = (CCTouch*)pTouches->anyObject();
        CCPoint point = convertTouchToNodeSpace(touch);
        
        CCSprite *targetCard = getCard(point);
        if(targetCard)
            pickACard(targetCard);
    }
}
    
void PickSuperHeroLayer::pickACard(CCSprite* card)
{
    setTouchEnabled(false);
    
    vector<CCSprite*>::iterator it = find(cards.begin() , cards.end() , card);
    if(it != cards.end())
        cards.erase(it);
    
    if(pickHeroCount == 0)
    {
        phassert(herosToDraw.size() == 8 && "make sure herostodraw contains 8 heroes");
        currentHeroCard = runPickCardAnim(this ,card ,targetHero.get() ,[=](){this->makeMenu();});
        pickHeroCount++;
        herosToDraw.erase(herosToDraw.begin());
        return;
    }
    
    SystemInform* inform = SystemInform::alertLoading(GameScene::getInstance());
    PuzzleRPC::getInstance()->drawHero2(PlayerData::getInstance()->getToken(),
                                        pickHeroCount,
                                        [=](auto_ptr<Hero> autohero,
                                            auto_ptr<Error> e)
    {
        inform->cancel();
        if(e.get())
        {
            LOGD("!!! Draw hero 2 error %s\n", e->toStr().c_str());
            e->handleErrorInform();
            this->setTouchEnabled(true);
            GameScene::getInstance()->popLayer();
            return;
        }
        PlayerData* data = PlayerData::getInstance();
        if(!data->isDrawFree(1))
        {
            int golds = data->getPlayer()->golds;
            data->setGolds(golds-drawPrice[pickHeroCount]);
        }
        
        HeroPtr hero = HeroPtr(autohero.release());
        
        herosToDraw.erase(find_if(herosToDraw.begin(),
                                  herosToDraw.end(),
                                  [=](HeroPtr h){
                                      return h->profile->tableId == hero->profile->tableId;
                                  })
                          );
        
        phassert(herosToDraw.size() == 7-pickHeroCount);
        phassert(herosToDraw.size() == cards.size());
        
        data->addHero(new Hero(*hero));
        
        if(hero->soul.heroId != 0)
            PlayerData::getInstance()->addHeroSoul(hero->soul);
        
        targetHero = hero;
        currentHeroCard = runPickCardAnim(this ,card, hero.get() ,[=](){this->makeMenu();});
        
        pickHeroCount++;
    });
}
    
void PickSuperHeroLayer::showAllCards(bool isGoHideCard)
{
    setTouchEnabled(false);
    vector<HeroPtr> heros = herosToDraw;
    std::random_shuffle(heros.begin(), heros.end());
    
    CCArray* allCardsAnim = CCArray::create();
    for(int i=0; i<cards.size(); i++)
    {
        CCFiniteTimeAction* flipCard = createCardFlippingAnim(cards[i], heros[i].get());
        allCardsAnim->addObject(flipCard);
    }
    
    if(isGoHideCard)
    {
        this->runAction(CCSequence::create(CCSpawn::create(allCardsAnim),
                                           CCDelayTime::create(2),
                                           CCCallLambda::create([=](){this->hideAllCards();}),
                                           NULL));
    }
    else
    {
        this->runAction(CCSequence::create(CCSpawn::create(allCardsAnim),
                                           CCCallLambda::create([=](){this->setTouchEnabled(true);}),
                                           NULL));
    }
}

void PickSuperHeroLayer::hideAllCards()
{
    setTouchEnabled(false);
    CCArray* allCardsAnim = CCArray::create();
    for(int i = 0 ; i < cards.size() ; i++)
    {
        CCFiniteTimeAction* anim = createCardFaceDownAnim(cards[i], (HeroCard*)cards[i]->getUserData());
        allCardsAnim->addObject(anim);
    }
    
    CCFiniteTimeAction* seq = CCSequence::create(CCSpawn::create(allCardsAnim),
                                                 CCCallLambda::create([=]() { this->reshuffleCards(); }),
                                                 NULL);
    this->runAction(seq);
}

void PickSuperHeroLayer::reshuffleCards()
{
    CCArray* allCardsAnimPhase1 = CCArray::create();
    for(int i =0; i < cards.size(); i++)
    {
        CCFiniteTimeAction* anim = CCSequence::create(CCDelayTime::create(i*0.1f),
                                                      CCEaseIn::create(CCMoveTo::create(0.3, ccp(0, -winSize.height*0.5f)), 0.5f),
                                                      NULL);
        allCardsAnimPhase1->addObject(CCTargetedAction::create(cards[i], anim));
    }
    
    CCArray* allCardsAnimPhase2 = CCArray::create();
    for(int i = 0 ; i < cards.size() ; i++)
    {
        CCFiniteTimeAction* anim = CCSequence::create(CCDelayTime::create((cards.size() - 1 - i)*0.1),
                                                      CCEaseIn::create(CCMoveTo::create(0.3, locations[i]), 0.5),
                                                      NULL);
        allCardsAnimPhase2->addObject(CCTargetedAction::create(cards[i], anim));
    }
    
    
    CCFiniteTimeAction* seq = CCSequence::create(CCSpawn::create(allCardsAnimPhase1),
                                                 CCSpawn::create(allCardsAnimPhase2),
                                                 CCCallLambda::create([=](){this->setTouchEnabled(true);}),
                                                 NULL);
    
    this->runAction(seq);
}

void PickSuperHeroLayer::dropCard()
{
    CCFiniteTimeAction *spawn = CCSpawn::create(CCMoveBy::create(0.5, ccp(winSize.width, winSize.height)),
                                                CCRotateBy::create(0.3 , 90),
                                                NULL);
    CCAction *seq = CCSequence::create(spawn,
                                       CCCallLambda::create([=](){
                                            currentHeroCard->removeFromParentAndCleanup(true);
                                        }),
                                       NULL);
    currentHeroCard->runAction(seq);
}
    
CCFiniteTimeAction* PickSuperHeroLayer::createCardFlippingAnim(CCSprite* card, Hero* hero)
{
    card->setAnchorPoint(ccp(0.5, 0.5));
    CCFiniteTimeAction *cardSeq = CCEaseIn::create(CCScaleTo::create(0.5, 0, 1), 0.5);
    
    HeroCard *heroCard = HeroCard::create(CARD_PATH, hero);
    heroCard->showStar(false);
    heroCard->setPosition(card->getPosition());
    heroCard->setScaleX(0.01);
    heroCard->setVisible(false);
    this->addChild(heroCard, 1);
    
    //bind each HeroCard to card
    card->setUserData(heroCard);
    
    CCFiniteTimeAction *seq = CCSequence::create(CCHide::create(),
                                                 CCDelayTime::create(0.5),
                                                 CCShow::create(),
                                                 CCEaseOut::create(CCScaleTo::create(0.8, 1, 1), 1),
                                                 NULL);
    
    return CCSpawn::create(CCTargetedAction::create(card, cardSeq),
                           CCTargetedAction::create(heroCard, seq),
                           NULL); 
}
    
CCFiniteTimeAction* PickSuperHeroLayer::createCardFaceDownAnim(CCSprite *cardBack , HeroCard *cardFront)
{
    CCFiniteTimeAction* frontSeq = CCSequence::create(CCScaleTo::create(0.5 , 0.01 , 1),
                                                      CCCallLambda::create([=]()
                                                                           {
                                                                               cardFront->removeFromParentAndCleanup(true);
                                                                           }),
                                                      NULL);
    cardFront->runAction(frontSeq);
    
    CCFiniteTimeAction* backSeq = CCSequence::create(CCDelayTime::create(0.5),
                                                     CCScaleTo::create(0.5, 1, 1),
                                                     NULL);
    cardBack->runAction(backSeq);
    
    return CCSpawn::create(CCTargetedAction::create(cardFront, frontSeq),
                           CCTargetedAction::create(cardBack, backSeq),
                           NULL);
}
    
CCSprite *PickSuperHeroLayer::getCard(CCPoint point)
{
    for(CCSprite *card : cards)
    {
        CCSize size = CCSizeMake(card->getContentSize().width,
                                 card->getContentSize().height);
        
        CCRect rect = CCRectMake(card->getPositionX() - size.width*0.5f,
                                 card->getPositionY() - size.height*0.5f,
                                 size.width,
                                 size.height);
        
        if(rect.containsPoint(point))
            return card;
    }
    return NULL;
}
    
void PickSuperHeroLayer::makeMenu()
{
    if(!targetHero)
        return;
    
    string msg = "";
    CCSprite *itemL = NULL;
    CCSprite *itemR = NULL;
    
    calDrawTimes();
    
    string soulMsg = "";
    int menuPadding = 30;
    CCPoint menuPos = ccp(0,-240);
    if (targetHero->soul.heroId != 0)
    {
        HeroProfile p = HPT(targetHero->soul.heroId);
        soulMsg = "并获赠"+p.nameCN+"魂魄"+toStr(targetHero->soul.count)+"个\n";
        
        menuPadding = 10;
        menuPos = ccp(50,-260);
    }

    if(GameConfig::shared()->getTutorialProgress() == TutorialTeam)
    {
        msg = targetHero->profile->nameCN + "已经加入了你的队伍\n" + soulMsg + "\n快去看看你刚获得的侠客吧！";
        itemR = createBtn("返回", ButtonType::NORMAL_BLUE);
    }
    else
    {
        if(pickHeroCount < 3)
        {
            if(drawTimes > 0)
            {
                msg = targetHero->profile->nameCN + "已经加入了你的队伍\n"+soulMsg+"你已经招募了"
                +toStr(pickHeroCount)+"次\n\n是否花费" + toStr(drawPrice[pickHeroCount])
                +"个元宝再抽一次";
                itemL = createBtn("再抽", ButtonType::NORMAL_RED);
                itemR = createBtn("返回", ButtonType::NORMAL_BLUE);
            }
            else
            {
                msg = targetHero->profile->nameCN + "已经加入了你的队伍\n"+soulMsg+"\n你的元宝不足，充值可追抽本次招募其余的侠客";
                itemL = createBtn("充值", ButtonType::NORMAL_RED);
                itemR = createBtn("返回", ButtonType::NORMAL_BLUE);
            }
        }
        else {
            msg = targetHero->profile->nameCN + "已经加入了你的队伍\n"+soulMsg+"你已经抽满了3次\n\n赶快去查看你的新侠客吧！";
            itemL = createBtn("查看", ButtonType::NORMAL_BLUE);
            itemR = createBtn("返回", ButtonType::NORMAL_BLUE);
        }
    }
    
    JuanZhou* jz = createMenu(targetHero.get(), msg.c_str());
    
	CCMenu *menu = CCMenu::create();
    if(isSharedAvailable() && targetHero->profile->star >= 5)
    {
        CCMenuItem *share = CCMenuItemSprite::create(createBtn("分享", ButtonType::NORMAL_BLUE, TITLE_YELLOW),
                                                     createBtn("分享", ButtonType::NORMAL_BLUE, TITLE_YELLOW),
                                                     this,
                                                     menu_selector(PickSuperHeroLayer::onShare));
        menu->addChild(share);
        if (targetHero->soul.heroId != 0)
        {
            menuPadding = 0;
            menuPos = ccp(60,-260);
        }
        else
        {
            menuPadding = 15;
            menuPos = ccp(0,-260);
        }
    }
    
    CCMenuItem *itemMiddle = CCMenuItemSprite::create(itemL , itemL , this, menu_selector(PickSuperHeroLayer::onGoOn));
    menu->addChild(itemMiddle );
    
	CCMenuItem *itemDown = CCMenuItemSprite::create(itemR , itemR , this, menu_selector(PickSuperHeroLayer::onGiveUp));
	menu->addChild(itemDown );
    
	menu->alignItemsHorizontallyWithPadding(menuPadding); 
	menu->setPosition(menuPos);
	jz->addChild(menu , 1);
    
	CCSprite* zhou = GetSprite("gameui/tongyong_1_1.png");
    zhou->setFlipX(true);
    alertMenu = AlertMenu::create();
    alertMenu->addSkin(jz,  GetSprite("gameui/tongyong_1_1.png"),  zhou);
    
	alertMenu->setPosition(ccp(0, -1500));
	alertMenu->_setZOrder(10);
    
    this->addChildWithAnim(alertMenu, true,
                           CCSequence::create(CCEaseSineOut::create(CCMoveTo::create(0.5 , ccp(0, -580))),
                                              CCCallLambda::create([=]() { alertMenu->startAnim();}),
                                              NULL));
}
    
void PickSuperHeroLayer::deleteMenu()
{
    this->removeChild(alertMenu, true);
}
    
void PickSuperHeroLayer::onShare()
{
    SystemInform::alertSharePanel(runningScene(),
                                  toStr("我在@群侠转 一下就抽到5星侠客！用绝世侠客玩消除，除了群侠转没有更好玩的了！")+
                                  "https://itunes.apple.com/cn/app/qun-xia-zhuan/id669428248");
}
    
void PickSuperHeroLayer::onGoOn()
{
    //zhaomu over 3 times
	if(pickHeroCount>=3)
	{
        // make a copy. pop layer will delete this layer and free targetHero
        string id = targetHero->id;
        GameScene::getInstance()->popLayer();
        GameScene::getInstance()->popLayer();
        
        HeroProfileLayer* layer = HeroProfileLayer::create(PlayerData::getInstance()->getHero(id));
        GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
        return;
	}
    
    //when coins or gold is not enough
    if(!PlayerData::getInstance()->isDrawFree(1) && drawTimes <= 0)
    {
        ChongZhiLayer *layer = ChongZhiLayer::create();
        layer->isShowBottom = false;
		GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
        return;
    }

    dropCard();
    showAllCards(true);
    deleteMenu();
    
    ring("sound/v/button_click.mp3");
}

void PickSuperHeroLayer::onGiveUp()
{
    if(pickHeroCount >= 3)
    {
        GameScene::getInstance()->popLayer();
        GameScene::getInstance()->popLayer();
        return;
    }
    
    auto* layer = HeroProfileLayer::create(PlayerData::getInstance()->getHero(targetHero->id));
    if(GameConfig::shared()->getTutorialProgress() == TutorialTeam)
    {
        layer->disableLock(true);
        layer->sv->setTouchEnabled(false);
    }   
    // return to top level ZhaoMu menu
    GameScene::getInstance()->popLayer();
    GameScene::getInstance()->popLayer();
    
    GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
    
    ring("sound/v/button_click.mp3");
}

    
bool Hire10Layer::init(InformType& type)
{
    if(!ContentLayer::init())
        return false;
    
    isShowTop = false;
    isShowBottom = false;
    this->type = type;
    
    bg = GetSprite("gameui/10lian_bg.png");
    bg->setAnchorPoint(ccp(0.5,1));
    bg->setPositionY(4);
    addChild(bg);
    
    topMenu = CCMenu::create();
	CCMenuItem *itemUp = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"),
                                                  GetSprite("gameui/back_t.png"),
                                                  this,
                                                  menu_selector(Hire10Layer::onBack));
	topMenu->addChild(itemUp);
	CCMenuItem *itemMiddle = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                      GetSprite("gameui/hongchengdi_t_tongyong.png"));
	topMenu->addChild(itemMiddle );
    itemUp->setAnchorPoint(ccp(0.5 , 1));
    itemMiddle->setAnchorPoint(ccp(0.5 , 1));
    topMenu->alignItemsHorizontallyWithPadding(8);
	topMenu->setPosition(ccp(-70 , -30));
    topMenu->setVisible(false);
	addChild(topMenu);
    
    LabelX* name = LabelX::create("招募结果", FONT_CN, 36);
    name->setColor(ccWHITE);
    name->setAnchorPoint(ccp(0.5, 0.5));
    name->setPosition(ccp( itemMiddle->getContentSize().width*0.5f-2 , itemMiddle->getContentSize().height*0.5f+2));
    itemMiddle->addChild(name);
    
    goOnMenu = CCMenu::create();
    CCMenuItem *item = CCMenuItemSprite::create(createBtn("继续连招" ,ButtonType::FOUR_WORD_RED),
                                                  createBtn("继续连招", ButtonType::FOUR_WORD_RED),
                                                  this,
                                                menu_selector(Hire10Layer::onConfirm));
     goOnMenu->addChild(item);
    if(isSharedAvailable())
    {
        CCMenuItem* item2 = CCMenuItemSprite::create(createBtn("分享" ,ButtonType::NORMAL_BLUE),
                                                     createBtn("分享", ButtonType::NORMAL_BLUE),
                                                     this,
                                                     menu_selector(Hire10Layer::onShare));
        
        goOnMenu->addChild(item2);
    }
    goOnMenu->alignItemsVerticallyWithPadding(10);
    goOnMenu->setPosition(ccp(0,-570));
    goOnMenu->setVisible(false);
    addChild(goOnMenu,2);
    
    for(int i = 0; i < 4 ; i++)
        addBox(i);
    boxes[0]->setVisible(true);
    setTouchEnabled(false);
    
    SystemInform* inform = SystemInform::alertLoading(GameScene::getInstance());
    auto drawCall = [=](std::auto_ptr<Hero10Draw> heroes, std::auto_ptr<Error> error, int cost)
    {
        inform->cancel();
        if(error.get())
        {
            if(error->code == ErrorCode::QuotaExceeded)
                SystemInform::alertConfirmPanel(runningScene(),
                                                "今日招募次数已经用完，升级VIP可获得更多的次数", "提示",
                                                [](){},
                                                [](){GameScene::getInstance()->pushLayerWithDefaultAnimation(VIPLayer::create());},
                                                false,
                                                "确定",
                                                "VIP");
            else
                error->handleErrorInform();
            GameScene::getInstance()->popLayer();
            return;
        }
        
        int golds = PlayerData::getInstance()->getPlayer()->golds;
        PlayerData::getInstance()->setGolds(golds-cost);
        
        this->heroesDraw = *heroes;
        //move the 5 stars heroes to the end 
        star5HeroesLine = heroesDraw.heroes.size();
        for(int i = 0; i < star5HeroesLine;)
            if(heroesDraw.heroes[i]->profile->star >= 5)
            {
                --star5HeroesLine;
                Hero* h = heroesDraw.heroes[i];
                heroesDraw.heroes[i] = heroesDraw.heroes[star5HeroesLine];
                heroesDraw.heroes[star5HeroesLine] = h;
            }
            else
                i++;
        
        for(Hero* hero : heroes->heroes)
            PlayerData::getInstance()->addHero(hero);
        if(heroesDraw.soul.heroId != 0)
            PlayerData::getInstance()->addHeroSoul(heroesDraw.soul);
        
        drawAnim();
    };
    
    if(type == InformType::Hire10Draw)
        PuzzleRPC::getInstance()->draw10Hero(PlayerData::getInstance()->getToken(),
                                         [=](std::auto_ptr<Hero10Draw> heroes, std::auto_ptr<Error> error)
                                         {
                                             drawCall(heroes, error, 2000);
                                             PlayerData::getInstance()->hasTenDraw = true;
                                         });
    else if(type == InformType::ToolDraw)
        PuzzleRPC::getInstance()->draw10TransItems(PlayerData::getInstance()->getToken(),
                                             [=](std::auto_ptr<Hero10Draw> heroes, std::auto_ptr<Error> error)
                                             {
                                                 if(!error.get())
                                                     PlayerData::getInstance()->drawTransLeft--;
                                                 drawCall(heroes, error, 250);
                                             });
    else if(type == InformType::DanYaoDraw)
        PuzzleRPC::getInstance()->draw10ExpItems(PlayerData::getInstance()->getToken(),
                                             [=](std::auto_ptr<Hero10Draw> heroes, std::auto_ptr<Error> error)
                                             {
                                                 if(!error.get())
                                                     PlayerData::getInstance()->drawExpLeft--;
                                                 drawCall(heroes, error, 250);
                                             });
    
    return true;
}
    
    
void Hire10Layer::drawAnim()
{
    if(heroesDraw.heroes.size() == 0)
        return;
    
    phassert(heroesDraw.heroes.size() == 10);
    
    //open the box
    {
        CCArray* actions = CCArray::create();
        for (int i = 0; i<4; i++)
        {
            actions->addObject(CCTargetedAction::create(boxes[i],
                                                        CCSequence::create(
                                                                           CCDelayTime::create(i*0.1),
                                                                           CCShow::create(),
                                                                           CCDelayTime::create(0.1),
                                                                           CCCallLambda::create([=](){
                                                            if(i < 3)
                                                                boxes[i]->setVisible(false);
                                                        }),
                                                                           NULL)));
        }
        
        this->runAction(CCSpawn::create(actions));
        
        //the light scale and rotate
        CCSprite* light = GetSprite("gameui/10lian_light.png");
        light->setPosition(ccp(0, -600));
        light->setScale(0.3);
        light->setVisible(false);
        light->setOpacity(0);
        addChild(light);
        
        light->runAction(CCSequence::create(CCDelayTime::create(0.1),
                                            CCShow::create(),
                                            CCSpawn::create(CCRotateBy::create(1.2, 30),
                                                            CCFadeIn::create(0.6),
                                                            CCScaleTo::create(0.6, 1),
                                                            NULL),
                                            CCScaleTo::create(0.5, 0.1),
                                            CCCallLambda::create([=](){light->removeFromParentAndCleanup(true);}),
                                            NULL
                                            ));
        

        
        vector<CCSprite*> swards;
        for(int i = 0; i < heroesDraw.heroes.size(); i++)
        {
            string path;
            if(isSuperHero(heroesDraw.heroes[i]->heroId))
                path = "gameui/s_jue.png";
            else if(isGodHero(heroesDraw.heroes[i]->heroId))
                path = "gameui/s_shen.png";
            else if(isSkyHero(heroesDraw.heroes[i]->heroId))
            {
                int star = heroesDraw.heroes[i]->profile->star;
                if(star >= 5)
                    path = "gameui/s_tian.png";
                else
                    path = "gameui/s"+toStr(min(3, max(1, 6-star)))+"_tian.png";
            }
            else
            {
                int star = heroesDraw.heroes[i]->profile->star;
                path = "gameui/s"+toStr(min(3, max(1, 6-star)))+".png";
            }
            CCSprite* sward = GetSprite(path);
            sward->setVisible(false);
            swards.push_back(sward);
            addChild(sward, 10);
        }
        
        HeroIcon* soul = NULL;
        if(heroesDraw.soul.heroId != 0)
        {
            soul = HeroIcon::createWithProfile(&HPT(heroesDraw.soul.heroId), true);
            soul->setPosition(ccp(0, -700));
            soul->showPlusNum(heroesDraw.soul.count);
            soul->setVisible(false);
            addChild(soul);
        }
        
        runAction(CCSequence::create(createSwardJumpoutAction(swards),
                                     CCCallLambda::create([=]()
                                        {
                                            for(CCSprite* box : boxes)
                                                box->removeFromParentAndCleanup(true);
                                        }),
                                     createSwardDropAction(swards),
                                     createIconsAction(swards),
                                     CCCallLambda::create([=](){
                                            showMenu();
                                            this->setTouchEnabled(true);
                                            if(soul)soul->setVisible(true);
                                        }),
                                     NULL));
        
        
    
    }
    
}
    
    
CCActionInterval* Hire10Layer::createSwardJumpoutAction(std::vector<CCSprite *> swards)
{
    //sward jump out of the box
    CCArray* swardJumpOutActions = CCArray::create();
    
    for(int k = 0; k < swards.size(); k++)
    {
        //make a random rotation between -30 ~ 30
        float rot = rand()%60-30;
        swardJumpOutActions->addObject(CCTargetedAction::create(swards[k],
            CCSequence::create(CCCallLambda::create([=]()
                                                    {
                                                    swards[k]->setPosition(ccp(0, -600));
                                                    swards[k]->setScale(1 + (rand()%10)/10*2);
                                                    swards[k]->setRotation(rot+180);
                                                    }),
                               CCHide::create(),
                               CCDelayTime::create(0.1*(k+1)),
                               CCShow::create(),
                               CCCallLambda::create([](){ring("sound/v/sward_whip.mp3");}),
                               CCMoveBy::create(0.5, ccp(900*sin(rot*3.14/180),
                                                         900*cos(rot*3.14/180))),
                               NULL)));

    }
    return CCSpawn::create(swardJumpOutActions);
}
    
CCActionInterval* Hire10Layer::createSwardDropAction(std::vector<CCSprite*> swards)
{
    //sward jump out of the box
    CCArray* swardsDropActions = CCArray::create();
    MatrixTrans trans(ccp(0, -400), 1000);
    int radius = 380;
    float angle = 0.0f;
    float px = 0.0f;
    float py = 0.0f;
    float pz = 0.0f;
    float delta;
    for(int k = 0; k < swards.size(); k++)
    {
        //make sward drop actions
        if(k < star5HeroesLine)
        {
            angle = (2*3.1416)/star5HeroesLine*k;
            px = radius*cos(angle);
            pz = radius*sin(angle)+radius;
            py = -400;
            delta = 0.3*k;
        }
        else
        {
            int numOf5Stars = swards.size() - star5HeroesLine;
            int interval = 180;
            px = -(numOf5Stars-1)*interval/2+interval*(k-star5HeroesLine);
            pz = radius;
            py = -400;
            delta = 0.3*(star5HeroesLine-1)+1+0.6*(k-star5HeroesLine);
        }
        CCPoint targetPos = trans.get2DPoint(px, py, pz);
        CCPoint prePos = ccp(targetPos.x, targetPos.y+800);
        float scale = trans.getScale(pz);
        
        CCArray* shakeAction = CCArray::create();
        shakeAction->addObject(createShakeAction(bg));
        for(int i = 0; i <= k; i++)
            shakeAction->addObject(createShakeAction(swards[i]));
        
        swardsDropActions->addObject(CCTargetedAction::create(swards[k],
            CCSequence::create(CCCallLambda::create([=]()
                                    {
                                        swards[k]->setPosition(prePos);
                                        swards[k]->setRotation(0);
                                        swards[k]->setZOrder(1000-pz);
                                        swards[k]->setAnchorPoint(ccp(0.5, 0));
                                        swards[k]->setScale(scale);
                                    }),
                               CCDelayTime::create(delta),
                               CCCallLambda::create([](){ring("sound/v/sward_in.mp3");}),
                               CCMoveTo::create(0.3, targetPos),
                               CCCallLambda::create([=]()
                                    {
                                        string path = px > 0 ?
                                                        "gameui/s5.png" :  "gameui/s4.png";
                                        CCSprite* gap = GetSprite(path.c_str());
                                        gap->setPosition(ccp(swards[k]->getContentSize().width*0.5f, 20));
                                        swards[k]->addChild(gap);
                                    }),
                               CCSpawn::create(shakeAction),
                               NULL)));
    }
    return CCSpawn::create(swardsDropActions);
}
    
CCActionInterval* Hire10Layer::createShakeAction(CCSprite* target)
{
    return CCTargetedAction::create(target,
                                    CCSequence::create(CCMoveBy::create(0.1, ccp(0, -2)),
                                                       CCMoveBy::create(0.1, ccp(0, 2)),
                                                       NULL));
}
    
CCActionInterval* Hire10Layer::createIconsAction(std::vector<CCSprite*> swards)
{
    CCArray* actions = CCArray::create();
    actions->addObject(CCDelayTime::create(0.5));
    for(int i = 0; i < swards.size(); i++)
    {
        actions->addObject(
            CCSequence::create(
                   CCDelayTime::create(0.3*i),
                   CCCallLambda::create([=]()
                        {
                            CCPoint pos;
                            int numPerRow = 5;
                            float scale = 1.0f;
                            if(i < star5HeroesLine)
                            {
                                int num = std::min(star5HeroesLine-numPerRow*(i/numPerRow), 5);
                                float start = -(num-1)*110*0.5;
                                pos = ccp(start+110*(i%numPerRow),-190-i/numPerRow*120);
                            }
                            else
                            {
                                int numOf5Stars = swards.size() - star5HeroesLine;
                                int interval = 140;
                                pos = ccp(-(numOf5Stars-1)*interval/2+interval*(i-star5HeroesLine), -430);
                                scale = 1.2;
                            }
                                
                            CCParticleSystemQuad* light = CCParticleSystemQuad::create("particle/bluelight.plist");
                            light->setStartSize(100);
                            light->setStartColor(getHeroColor(heroesDraw.heroes[i]));
                            light->setStartColorVar(ccc4f(0.5, 0.5, 0.5, 0));
                            light->setPosition(ccp(swards[i]->getPositionX(), swards[i]->getPositionY()+50));
                            addChild(light);
                            
                            HeroIcon* icon = HeroIcon::create(heroesDraw.heroes[i]);
                            icon->setPosition(pos);
                            icon->setVisible(false);
                            icon->setScale(scale);
                            icons.push_back(icon);
                            addChild(icon);
                            
                            light->runAction(CCSequence::create(
                                                CCDelayTime::create(0.5),
                                                CCMoveTo::create(0.5, pos),
                                                CCTargetedAction::create(icon, CCShow::create()),
                                                CCTargetedAction::create(icon, CCFadeIn::create(0.5)),
                                                CCCallLambda::create([=]()
                                                    {
                                                        light->removeFromParentAndCleanup(true);
                                                    }),
                                                NULL));
                        }),
                   CCTargetedAction::create(swards[i],
                                            CCSequence::create(
                                                   CCFadeOut::create(0.5),
                                                   CCCallLambda::create([=]()
                                                        {
                                                            swards[i]->removeFromParentAndCleanup(true);
                                                        }),
                                                   NULL)),
                   NULL
                   ));
    }
    return CCSpawn::create(actions);
}
    
ccColor4F Hire10Layer::getHeroColor(const Hero* hero)
{
    ccColor3B color;
    if(hero->profile->color == GemUtils::Fire)
        color = ccRED;
    else if(hero->profile->color == GemUtils::Water)
        color = ccBLUE;
    else if(hero->profile->color == GemUtils::Wood)
        color = ccGREEN;
    else if(hero->profile->color == GemUtils::Light)
        color = ccYELLOW;
    else if(hero->profile->color == GemUtils::Dark)
        color = ccc3(0x94, 0x00, 0xd3);
    else
        assert("hero color should be in range");
    return ccc4FFromccc3B(color);
}
    
void Hire10Layer::onLongClick(CCTouch* touch)
{
    CCPoint localPoint;
    
    //check if touch on the heroIcon
    for (HeroIcon *icon : icons)
    {
        if(!icon)continue;
        localPoint = icon->getParent()->convertTouchToNodeSpace(touch);
        if(icon->boundingBox().containsPoint(localPoint))
        {
            auto* layer = HeroProfileLayer::create(icon->getHero());
            GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
        }
    }
    
}

void Hire10Layer::addBox(int i)
{
    CCSprite* box = GetSprite(("gameui/10lian_2_"+toStr(i+1)+".png").c_str());
    box->setAnchorPoint(ccp(0, 0));
    box->setPosition(ccp(-140, -770));
    box->setVisible(false);
    boxes.push_back(box);
    addChild(box);
}
    
void Hire10Layer::showMenu()
{
    topMenu->setVisible(true);
    goOnMenu->setVisible(true);
}
    
void Hire10Layer::onBack()
{
    GameScene::getInstance()->popLayer();
}

void Hire10Layer::onConfirm()
{
    GameScene::getInstance()->popLayer();
    GameScene::getInstance()->pushLayer(Hire10Layer::create(type));
}
    
void Hire10Layer::onShare()
{
    SystemInform::alertSharePanel(runningScene(),
                                  toStr("@群侠转 首次十一连招募必得天命侠客，还有五神五绝等着我，我的侠客阵容又强大了！")+
                                  "https://itunes.apple.com/cn/app/qun-xia-zhuan/id669428248");
}
    

////////////////////////////////////////////////
//ZhaoMuLayer
////////////////////////////////////////////////

    
bool ZhaoMuLayer::init()
{
	if(!ContentLayer::init())
        return false;
    
    gaoshouLabel = NULL;
    juedingLabel = NULL;
    toolBtn = NULL;
    superHireBtn = NULL;
    
    CCSprite* zoumadeng = GetSprite("gameui/zoumadeng.png");
    zoumadeng->setAnchorPoint(ccp(.5, 1));
    zoumadeng->setPositionY(20);
    addChild(zoumadeng);
    
    {
        normalHireBtn = ButtonSprite::create("gameui/gaoshou.png");
        normalHireBtn->setPosition(ccp(-190, -505));
        normalHireBtn->setOnClickListener([=]()
                                          {
                                              if(PlayerData::getInstance()->drawFreeLeft > 0 &&
                                                 PlayerData::getInstance()->isDrawFree(0))
                                              {
                                                  GameScene::getInstance()->pushLayerWithDefaultAnimation(PickNorHeroLayer::create());
                                              }
                                              else if(PlayerData::getInstance()->drawFreeLeft <= 0)
                                                  SystemInform::alertMessage(GameScene::getInstance(),
                                                                             "你的每日次数已用完",
                                                                             "提示",
                                                                             [](){});
                                              else
                                                  SystemInform::alertMessage(GameScene::getInstance(),
                                                                             "你现在还不能招募",
                                                                             "提示",
                                                                             [](){});
                                          });
        this->addChild(normalHireBtn);
        
        addLabelBMFont(normalHireBtn, gaoshouLabel,
                       "",
                       DRAW_FREE_FONT, ccp(92, 272), ccp(0.5, 0.5));
        gaoshouLabel->setScale(contentScale());
    }
    
    {
        /*addLabelBMFont(normalHireBtn, gaoshouFree2,
         "",
         "bmfont/PW_24_Red.fnt", ccp(92,272), ccp(0.5, 0.5));
         gaoshouFree2->setScale(contentScale());
         gaoshouFree2->setVisible(false);*/
        
        superHireBtn = ButtonSprite::create("gameui/juedingz.png");
        superHireBtn->setPosition(ccp(0, -505));
        superHireBtn->setOnClickListener([=](){
            onZhaoMu(InformType::NiceDraw);
        });
        this->addChild(superHireBtn);
        
        
        addLabelBMFont(superHireBtn, juedingLabel,
                       "",
                       DRAW_FREE_FONT, ccp(92, 272), ccp(0.5, 0.5));
        juedingLabel->setScale(contentScale());
    }
    
    
    {
        string path = PlayerData::getInstance()->player->vipLevel >= 2 ? "gameui/cailiaodaoju.png" :
                                                                        "gameui/cailiaodaoju_b.png";
        toolBtn = ButtonSprite::create(path.c_str());
        toolBtn->setPosition(ccp(190, -505));
        toolBtn->setOnClickListener([=](){
            if(PlayerData::getInstance()->player->vipLevel >= 2)
                onZhaoMu(InformType::DanYaoDraw);
            else
                SystemInform::alertMessage(runningScene(), "你的VIP等级不足！", "提示", [=](){
                    ContentLayer *layer = ChongZhiLayer::create();
                    layer->isShowBottom = false;
                    GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
                });
        });
        addChild(toolBtn);
        
        if(PlayerData::getInstance()->player->vipLevel < 2)
            addLabelBMFont(toolBtn, "VIP2可开启", DRAW_RED_FONT, ccp(92, 272), ccp(0.5, 0.5))->setScale(contentScale());
    }
    
    
    std::string imagePath = PlayerData::getInstance()->recruitHeroImgURL;
    imagePath = CCNative::getDocPath() + imagePath.substr(imagePath.find_last_of('/')+1);
    
    if (!PlayerData::getInstance()->recruitHeroImgURL.empty() &&
        resourcePresentForLoading(imagePath))
    {
        CCSprite* sp = GetSprite(imagePath);
        sp->setScaleX(518 / sp->getContentSize().width);
        sp->setScaleY(342 / sp->getContentSize().height);
        sp->setPosition(ccp(zoumadeng->getContentSize().width*0.5f,
                            zoumadeng->getContentSize().height*0.5f - 25));
        zoumadeng->addChild(sp);
    }
    else
    {
        CCSprite *model = GetSprite("gameui/moren.png");
        model->setPosition(ccp(zoumadeng->getContentSize().width*0.5f,
                               zoumadeng->getContentSize().height*0.5f - 25));
        zoumadeng->addChild(model);
    }

	schedule(schedule_selector(ZhaoMuLayer::updateTimer) , 1);

    if(GameConfig::shared()->getTutorialProgress() == TutorialSpendGoldOnHero)
    {
        // do not display countdown timer
        //unschedule(schedule_selector(ZhaoMuLayer::updateTimer));
        ArrowMask::shared()->setMask(CCRect(0, 0, 640, 960));
    }
    
	return true;
}
    
void ZhaoMuLayer::onEnter()
{
    ContentLayer::onEnter();
    updateTimer(0.0f);
}

void ZhaoMuLayer::onEnterAnimationDone()
{
    ContentLayer::onEnterAnimationDone();
    
    if(GameConfig::shared()->getTutorialProgress() == TutorialSpendGoldOnHero)
    {
        ArrowMask::shared()->setMask(CCRect(225, 150, 190, 290));
        ArrowMask::shared()->setArrow(ccp(200, 366));
    }
}
    
void ZhaoMuLayer::setTouchEnabled(bool value)
{
	ContentLayer::setTouchEnabled(value);
	//topMenu->setTouchEnabled(value);
}
    
void ZhaoMuLayer::updateTimer(float dt)
{
    using namespace boost::date_time;
    using namespace boost::posix_time;
    ptime gaoshouPTime = from_time_t(PlayerData::getInstance()->drawFreeTime[0]);
    ptime juedingPTime = from_time_t(PlayerData::getInstance()->drawFreeTime[1]);
    ptime now = boost::posix_time::second_clock::universal_time() +
               PlayerData::getInstance()->delta_local_server;
    
    auto gaoshouPInterval = gaoshouPTime - now;
    auto juedingPInterval = juedingPTime - now;
    
    if(PlayerData::getInstance()->drawFreeLeft <= 0)
    {
        gaoshouLabel->setString("今日已无次数");
        gaoshouLabel->setFntFile(DRAW_RED_FONT);
    }
    else if(gaoshouPInterval.is_negative())
    {
        std::string text = "今日" + toStr(PlayerData::getInstance()->drawFreeLeft) + "/5次免费";
        gaoshouLabel->setString(text.c_str());
        gaoshouLabel->setFntFile(DRAW_FREE_FONT);
    }
    else if(!gaoshouPInterval.is_negative())
    {
        int h = gaoshouPInterval.hours();
        int m = gaoshouPInterval.minutes();
        int s = gaoshouPInterval.seconds();
        
        char ss[128];
        if(h)
            sprintf(ss, "%02d:%02d:%02d后免费", h, m, s);
        else
            sprintf(ss, "%02d:%02d后免费", m, s);
        gaoshouLabel->setString(ss);
        gaoshouLabel->setFntFile(DRAW_NORMAL_FONT);
    }

	if(juedingPInterval.is_negative())
    {
        juedingLabel->setString("1次免费抽");
        juedingLabel->setFntFile(DRAW_FREE_FONT);
	}
    else
        juedingLabel->setString(" ");
    /*else if(!juedingPInterval.is_negative())
    {
		int h = juedingPInterval.hours();
		int m = juedingPInterval.minutes();
		int s = juedingPInterval.seconds();
        char ss[128];
        if(h)
            sprintf(ss, "%02d:%02d:%02d后免费", h, m, s);
        else
            sprintf(ss, "%02d:%02d后免费", m, s);
        std::string msg = toStr(ss)+"\nVIP可减少CD";
        juedingLabel->setString(msg.c_str());
        juedingLabel->setFntFile(DRAW_NORMAL_FONT);
        juedingLabel->setAlignment(kCCTextAlignmentCenter);
	}*/
}

void ZhaoMuLayer::onZhaoMu(const InformType& type)
{
    auto layer = InformLayer::create(type);
	GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);

    ring("sound/v/button_click.mp3");
}

}


