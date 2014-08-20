#include "Sale.h"
#include "GameScene.h"
#include "PlayerData.h"
#include "HeroProfileLayer.h"
#include "UIPresets.h"
#include "ZqlScrollView.h"
#include "HeroIcon.h"
#include "network/PuzzleRPC.h"
#include "PageScrollView.h"
#include "Common.h"

namespace PH
{

USING_NS_CC;
    
bool SaleBoard::init(const std::vector<PH::Hero *>& heroVec, bool displayHolder)
{
	//Player *data = PlayerData::getInstance()->getPlayer();
	if(!HeroBoard::init(heroVec, displayHolder))
       return false;

	updateHero();

	return true;
}

void SaleBoard::updateHero()
{
    
	for(int i = 0 ; i < heroVec.size() ; i++)
    {
		if(PlayerData::getInstance()->isHeroInTeam(heroVec[i])){
			heroImgVec[i]->setEnabled(false);
			heroImgVec[i]->setBottomMsg(HeroIcon::LabelType::InTeam);
			continue;
		}
		heroImgVec[i]->setEnabled(!heroVec[i]->isLock);
	}
    
}

void SaleBoard::refresh(const std::vector<PH::Hero *>& heroVec)
{
	HeroBoard::refresh(heroVec);
	
	updateHero();
    
    for(Hero* hero :  PlayerData::getInstance()->getNewHeroes("sale"))
        if(getHeroImg(hero))
            getHeroImg(hero)->showNew();
}

bool SaleJuanZhou::init()
{
	if(!JuanZhou::init())
        return false;

	viewPort = CCSize(20 , 82);

	CCSprite *bg = GetSprite("gameui/xiake_1_3.png");
	bg->setAnchorPoint(ccp(0.5 , 1));
	this->addChild(bg);

	addLabelBMFont(this, priceLabel, "", "bmfont/Sumo_22_yellow.fnt", ccp(-66 , -29));
    priceLabel->setScale(contentScale());
    
    addLabelBMFont(this, numLabel, "", "bmfont/Sumo_22_yellow.fnt",ccp(263 , -29),ccp(1,1));
    numLabel->setScale(contentScale());

	return true;
}

void SaleJuanZhou::setPrice(int price)
{
	priceLabel->setString(boost::lexical_cast<std::string>(price).c_str());
}

void SaleJuanZhou::setNum(int num)
{
	numLabel->setString( (toStr(num) + "/" +toStr(PlayerData::getInstance()->getStorage().size())).c_str());
}


bool SaleLayer::init()
{
	if(!ContentLayer::init())
        return false;

	totalprice = 0;
	num = 0;
    heroesOfCurrType = PlayerData::getInstance()->getStorage();
    
	heroBoard = SaleBoard::create(heroesOfCurrType, true);
    
    scrollView = ZqlScrollView::create();
	scrollView->addLayer(heroBoard );
	scrollView->setPositionY(-120);
	scrollView->setViewSize(CCSize(winSize.width , 600) , CCRect(0 , -96 , winSize.width , 430));
	this->addChild(scrollView);
    
	topMenu = CCMenu::create();
	CCMenuItem *itemLeft = CCMenuItemSprite::create(GetSprite("gameui/back_t.png"), GetSprite("gameui/back_t.png"), this, menu_selector(SaleLayer::onBackClick));
	itemLeft->setAnchorPoint(ccp(0.5 , 1));
	topMenu->addChild(itemLeft );
    
    {
        CCMenuItem* sortButton = CCMenuItemSprite::create(GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                          GetSprite("gameui/hongchengdi_t_tongyong.png"),
                                                          this,
                                                          menu_selector(SaleLayer::onTitleClick));
        sortButton->setAnchorPoint(ccp(0.5 , 1));
        
        LabelX* name = LabelX::create("出售", FONT_CN, 36);
        name->setColor(ccWHITE);
        name->setAnchorPoint(ccp(0.5, 0.5));
        name->setPosition(ccp(90 , sortButton->getContentSize().height*0.5f+2));
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

	CCMenuItem *itemRight = CCMenuItemSprite::create(createBtn("确定",ButtonType::NORMAL_BLUE, TITLE_YELLOW), createBtn("确定",ButtonType::NORMAL_BLUE, TITLE_YELLOW), this ,  menu_selector(SaleLayer::onConfirm));
	itemRight->setAnchorPoint(ccp(0.5 , 1));
	topMenu->addChild(itemRight);

	topMenu->alignItemsHorizontallyWithPadding(8);
	topMenu->setPosition(ccp(0 , 18));
	addChild(topMenu);

	board = SaleJuanZhou::create();
	CCSprite *zhou1 = GetSprite("gameui/xiake_1_1.png");
	CCSprite *zhou2 = GetSprite("gameui/xiake_1_1.png");
    zhou2->setFlipX(true);
	
	jz = JuanZhouLayer::create();
	jz->addSkin(board , zhou1 , zhou2);
	jz->setPositionY(-30);
	this->addChild(jz);
    
    switcher = HeroBoardSwitcher::create();
    switcher->setPosition(ccp(0, -180));
    switcher->setCallBack([=](int i){
        scrollView->reset();
        clear();
        updateMsg();
        heroesOfCurrType = getHeroesBySortAndSwitchType(switcher->getCurNode());
        heroBoard->removeFromParentAndCleanup(true);
        heroBoard = SaleBoard::create(heroesOfCurrType, (switcher->getCurNode()==0));
        scrollView->addLayer(heroBoard);
    });
    addChild(switcher, 1);
    
    initHeroNumTips();

	updateMsg();

	ContentLayer::setTouchEnabled(true);

	return true;
}

void SaleLayer::onEnter()
{
    ContentLayer::onEnter();
    
    refreshBoard();
    updateMsg();
}
    
void SaleLayer::cleanup()
{
    CCNode::cleanup();
    PlayerData::getInstance()->clearNewHeroes("sale");
}
    
void SaleLayer::refreshBoard()
{
    heroBoard->refresh(heroesOfCurrType);
    std::vector<Hero*> sale = onSaleHeros;
    clear();
    for(Hero *h : sale)
        if(!PlayerData::getInstance()->isHeroInTeam(h) && !h->isLock)
            addToSale(h);
}
    
void SaleLayer::initHeroNumTips()
{
    CCSprite *heroNumBg = GetSprite("gameui/duiwurenshu.png");
    heroNumBg->setAnchorPoint(ccp(1 , 0.5));
    heroNumBg->setPosition(ccp(winSize.width*0.5f , -634));
    //heroNumBg->setOpacity(200);
    addChild(heroNumBg , 1);
    addLabelTTF(heroNumBg, numLabel, "", FONT_NUM, 24, ccp(100 , heroNumBg->getContentSize().height*0.5f) , ccp(0.5 , 0.5));
    
}

void SaleLayer::updateMsg()
{
	board->setPrice(totalprice);
	board->setNum(num);
    
    int num = PlayerData::getInstance()->getStorage().size();
    int maxNum = PlayerData::getInstance()->player->storageLimit;
    numLabel->setString((toStr(num) + "/" + toStr(maxNum)).c_str());
    
    if(num > maxNum){
        numLabel->setColor(ccRED);
        
        CCFiniteTimeAction *fade1 = CCFadeTo::create(0.3 , 100);
        CCFiniteTimeAction *fade2 = CCFadeTo::create(0.3 , 255);
        CCFiniteTimeAction *sequence = CCSequence::create(fade1 , fade2 , NULL);
        CCAction *repeatBlink = CCRepeatForever::create((CCActionInterval*)sequence);
        
        numLabel->runAction(repeatBlink);
    }else{
        numLabel->setColor(ccWHITE);
        numLabel->setOpacity(255);
        numLabel->stopAllActions();
    }

}

void SaleLayer::addToSale(Hero *hero)
{
	HeroIcon *icon = heroBoard->getHeroImg(hero);
	onSaleHeros.push_back(hero);
	totalprice += hero->profile->price.eval(hero->level);
	num++;
	updateMsg();
	icon->setEnabled(false);
	icon->mark(true);
}

void SaleLayer::removeFromSale(Hero *hero)
{
	HeroIcon *img = heroBoard->getHeroImg(hero);
	std::vector<Hero*>::iterator it = std::find(onSaleHeros.begin() , onSaleHeros.end() , hero);
	if(it != onSaleHeros.end()){
		onSaleHeros.erase(it);
	}
	totalprice -=hero->profile->price.eval(hero->level);
	num--;
	updateMsg();
	img->setEnabled(true);
	img->mark(false);
}
    
void SaleLayer::clear()
{
    onSaleHeros.clear();
    totalprice = 0;
    num = 0;
}

bool SaleLayer::isOnSale(Hero *hero)
{
	std::vector<Hero*>::iterator it = std::find(onSaleHeros.begin() , onSaleHeros.end() , hero);
	if(it != onSaleHeros.end()){
		return true;
	}else return false;
}

void SaleLayer::setTouchEnabled(bool value)
{
	ContentLayer::setTouchEnabled(value);
	topMenu->setTouchEnabled(value);
	heroBoard->setTouchEnabled(value);
	scrollView->setTouchEnabled(value);
}
    
void SaleLayer::onSingleClick(CCTouch* touch)
{
    CCPoint localPoint = scrollView->convertTouchToNodeSpace(touch);
    Hero* hero = heroBoard->getHero(touch);
    
    if( scrollView->boundingBox().containsPoint(localPoint) && hero && !hero->isLock && !PlayerData::getInstance()->isHeroInTeam(hero)){
        if(!isOnSale(hero))addToSale(hero);
        else removeFromSale(hero);
    }
}
    
void SaleLayer::onLongClick(cocos2d::CCTouch *touch)
{
    Hero* hero = heroBoard->getHero(touch);
    if(hero)
        GameScene::getInstance()->pushLayerWithDefaultAnimation(HeroProfileLayer::create(hero));
}

void SaleLayer::onConfirm(CCObject *target)
{
	if(!num)
		SystemInform::alertMessage(GameScene::getInstance(),
                                   "还没有选择要出售的侠客.",
                                   "警告" ,
                                   [=](){});
    else
    {
		std::vector<std::string> hs;
		for(int i=0;i<onSaleHeros.size();i++)
			hs.push_back(onSaleHeros[i]->id);

		boost::function<void ()> toSell = [=]()
        {
            SystemInform *inform = SystemInform::alertLoading(GameScene::getInstance());
            PuzzleRPC::getInstance()->sellHero(PlayerData::getInstance()->getToken(),
                                               hs,
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
                heroesOfCurrType = getHeroesBySortAndSwitchType(switcher->getCurNode());
                clear();
                refreshBoard();
                updateMsg();
            });
		};

		SystemInform::alertConfirmPanel(GameScene::getInstance(),
									   "确定出售？",
                                        "警告",
									   toSell,
									   [=](){});
	}
    
    ring("sound/v/button_click.mp3");
}

void SaleLayer::onTitleClick(cocos2d::CCObject *target)
{
	PlayerData::getInstance()->switchSortType();
	heroesOfCurrType = getHeroesBySortAndSwitchType(switcher->getCurNode());
    heroBoard->reOrder(heroesOfCurrType);
    
	title->setString(PlayerData::getInstance()->getSortTypeMsg());
    
    ring("sound/v/button_click.mp3");
}

void SaleLayer::onBackClick(CCObject *target)
{
	GameScene::getInstance()->popLayer();
    ring("sound/v/button_click.mp3");
}

void SaleLayer::onEnterAnimationDone()
{
	jz->startAnim();
}

}