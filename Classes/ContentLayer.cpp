#include "ContentLayer.h"
#include "Animation/CCCallLambda.h"
#include "CCNative.h"

USING_NS_CC;

//create by ZQL

using namespace std;

bool ContentLayer::init()
{
	 if ( !CCLayer::init() )
    {
        return false;
    }

	winSize = CCEGLView::sharedOpenGLView()->getVisibleSize();
    isShowTop = true;
    isShowBottom = true;
    CCLayer::setTouchEnabled(true);
    isSingleClick = false;
    
	return true;
}

void ContentLayer::addChildWithAnim(ContentLayer *layer,
                                    bool isAnimate,
                                    CCFiniteTimeAction *action)
{
	this->addChild(layer);

	if(isAnimate)
    {
        //record the previous touch state of layer and return to that state after action.
        bool touchstate = layer->isTouchEnabled();
		layer->setTouchEnabled(false);
        
		CCFiniteTimeAction *showUpAction = action;
		if(showUpAction == NULL)
        {
            CCPoint p = layer->getPosition();
			layer->setPositionY(p.y-winSize.height);
            
            showUpAction = CCEaseSineOut::create(CCMoveTo::create(0.5 , p));
		}
		CCAction *seq = CCSequence::create(showUpAction,
										   CCCallLambda::create([=]() { layer->setTouchEnabled(touchstate); }) ,
										   NULL);
		layer->runAction(seq); 
	}
}

void ContentLayer::setTouchEnabled(bool value)
{
    CCLayer::setTouchEnabled(value);
}

// default implements are used to call script callback if exist
bool ContentLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    currentTouch = pTouch;
    isSingleClick = true;
    this->scheduleOnce(schedule_selector(ContentLayer::onPressTimer), 0.5);
    return true;
}
void ContentLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint currPoint = pTouch->getLocationInView();
	CCPoint prevPoint = pTouch->getPreviousLocationInView();
    
	if(ccpDistance(currPoint, prevPoint) > 5)
    {
        isSingleClick = false;
        this->unschedule(schedule_selector(ContentLayer::onPressTimer));
    }
}
void ContentLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    this->unschedule(schedule_selector(ContentLayer::onPressTimer));
    if(isSingleClick)
    {
        this->onSingleClick(pTouch);
        isSingleClick = false;
    }
}

// default implements are used to call script callback if exist
void ContentLayer::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    ccTouchBegan((CCTouch*)pTouches->anyObject(), pEvent);
}
void ContentLayer::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    ccTouchMoved((CCTouch*)pTouches->anyObject(), pEvent);
        
}
void ContentLayer::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    ccTouchEnded((CCTouch*)pTouches->anyObject(), pEvent);
}

void ContentLayer::onPressTimer(float dt)
{
    isSingleClick = false;
    onLongClick(currentTouch);
}
