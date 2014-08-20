
#include "HelloWorldScene.h"
#include "ZqlScrollView.h"

#include <iomanip>

USING_NS_CC;

namespace PH
{

void CompaignListLayer::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{

}

void CompaignListLayer::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
    

}

void CompaignListLayer::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
	
}

void CompaignListLayer::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{

}

// on "init" you need to initialize your instance
bool CompaignListLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

    this->setTouchEnabled(true);
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(CompaignListLayer::menuCloseCallback));
    
    if (CCApplication::sharedApplication()->getTargetPlatform() == kTargetIphone)
    {
        pCloseItem->setPosition(ccp(visibleSize.width - 20 + origin.x, 20 + origin.y));
    }
    else 
    {
        pCloseItem->setPosition(ccp(visibleSize.width - 40 + origin.x, 40 + origin.y));
    }

	CCSize size = CCDirector::sharedDirector()->getWinSize();

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);


    CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Arial", 24);
    pLabel->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height - 50 + origin.y));
    this->addChild(pLabel, 1);



	return true;
}

void CompaignListLayer::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

}
