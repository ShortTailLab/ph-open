//create by ZQL

#include "ZqlScrollView.h"
#include "Animation/CCCallLambda.h"
#include "Board/Utils.h"
#include <extensions/utils.h>
#include <cmath>
#include <functional> 

USING_NS_CC;

using namespace PH;

bool ZqlScrollView::init()
{
    if ( !ContentLayer::init() )
        return false;

	contentHeight = 0.0;
	isTouching = false;
	touchNode = NULL;
	barScale = 0.0;
	actionPort = CCRect();
    ContentLayer::setTouchEnabled(true);
	scrollBar = NULL;
    content = CCNode::create();
    isScrollEnabled = true;
    
	this->addChild(content);
	this->scheduleUpdate();
    
	return true;
}


void ZqlScrollView::addLayer(CCNode* layer)
{
    
	content->addChild(layer);
	reArrange();
}
void ZqlScrollView::removeAllLayers()
{
    content->removeAllChildren();
}

//remove the specified subview
void ZqlScrollView::removeLayer(CCNode* view)
{
	content->removeChild(view,true);
    reArrange();
}

CCArray* ZqlScrollView::getChildren()
{
    return content->getChildren();
}

//set the size of the visible region 
void ZqlScrollView::setViewSize(CCSize size)
{
    setViewSize(size, CCRect(0, 0, size.width, size.height));
}

void ZqlScrollView::setViewSize(CCSize size, CCRect actionport)
{
	viewPort = size;
	actionPort = actionport;
	gap = contentHeight - actionPort.size.height;
	if(gap < 0)
        gap=0;

	content->setPositionY(actionPort.origin.y);
}

float ZqlScrollView::getContentY()
{
    return content->getPositionY();
}

void ZqlScrollView::setContentY(float y)
{
    content->setPositionY(y);
    reArrange();
}

void ZqlScrollView::reset()
{
    content->setPositionY(actionPort.origin.y);
}

CCRect ZqlScrollView::boundingBox()
{
	return CCRectMake(-actionPort.size.width*0.5f,
                      actionPort.origin.y - actionPort.size.height,
                      actionPort.size.width,
                      actionPort.size.height);
}


bool ZqlScrollView::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint localPoint = convertTouchToNodeSpace(pTouch);

    if(boundingBox().containsPoint(localPoint))
    {
        v0 = v = 0;
		isTouching=true;
		makeScrollBar();
	}
    return true;
}

void ZqlScrollView::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint currPoint = (CCPoint)pTouch->getLocation();
	CCPoint prevPoint = (CCPoint)pTouch->getPreviousLocation();
    
	if(isTouching)
    {
		v0 = currPoint.y - prevPoint.y;
		content->setPositionY(content->getPositionY() + v0);
	}
}

void ZqlScrollView::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	if(isTouching)
    {
        fadeScrollBar();
        if(v0 != 0)
        {
            v = std::abs(v0);
            direction = v0 / v;
            v0 = 0;
        }
        
        isTouching = false;
    }
}

// default implements are used to call script callback if exist
void ZqlScrollView::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
    ccTouchBegan((CCTouch*)pTouches->anyObject(), pEvent);
}
void ZqlScrollView::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
    ccTouchMoved((CCTouch*)pTouches->anyObject(), pEvent);
}
void ZqlScrollView::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
    ccTouchEnded((CCTouch*)pTouches->anyObject(), pEvent);
}


//adjust the position of subviews.Default vertically display
void ZqlScrollView::reArrange()
{
	CCArray *children = content->getChildren();

	float h = 0.0;

	for(int i = 0; i < content->getChildrenCount(); i++)
    {
        CCNode *temp = (CCNode*)children->objectAtIndex(i);
		temp->setAnchorPoint(ccp(0.5,1));
		temp->setPositionY(h);
        float nodeY = content->getPositionY()+h;
        CCSize nodeSize = temp->getContentSize();
        temp->setVisible(nodeY<nodeSize.height && nodeY>-viewPort.height);
        if(dynamic_cast<CCLayer*>(temp))
            dynamic_cast<CCLayer*>(temp)->setTouchEnabled(nodeY<nodeSize.height && nodeY>-viewPort.height);

		h -= temp->getContentSize().height;
	}
	
	contentHeight = abs(h);

	gap = contentHeight - actionPort.size.height + actionPort.origin.y;
	if(gap < actionPort.origin.y)
        gap=actionPort.origin.y;
	
}

//called every 0.01 second by the schedule function to do all the movements.
void ZqlScrollView::update(float dt)
{
    if(!isScrollEnabled)
        return;
    
	if(!isTouching )
    {
		if(content->getPositionY() < actionPort.origin.y)
        {
			easing(actionPort.origin.y);
			v = 0;
		}
        
		if(content->getPositionY() > gap)
        {
			easing(gap);
			v = 0;
		}
        
		if(v != 0)
        {
			v-=1;
			if(v < 0 )v = 0;
			content->setPositionY(content->getPositionY() + direction * v);
		}
	}

	if(scrollBar && scrollBar->getOpacity())
	{
		float p = content->getPositionY();

		if(p < actionPort.origin.y)
            p = actionPort.origin.y;
		else if(p > gap)
            p = gap;

		float d = p - actionPort.origin.y;
		scrollBar->setPositionY(actionPort.origin.y-d*barScale);

	}
	
	reArrange();
}

void ZqlScrollView::makeScrollBar()
{
	float actionHeight = actionPort.size.height;
	float tempBarScale = actionHeight / contentHeight;
    if(tempBarScale >= 1)
    {
        if(scrollBar)scrollBar->setVisible(false);
        return;
    }
    barScale = tempBarScale;
	int barHeight = std::max(int(actionHeight*barScale) , 30);
	
	if(!scrollBar)
    {
        scrollBar = Get9SpriteFromFrame("guntiao.png", 5, 5, 1, 1);
		scrollBar->setAnchorPoint(ccp(0.5 , 1));
        
        CCSize winSize = CCEGLView::sharedOpenGLView()->getVisibleSize();
		scrollBar->setPosition(ccp(winSize.width * 0.5f - 20, actionPort.origin.y));
		addChild(scrollBar , 2);
	}
    
	if(scrollBar->isRunning())
    {
		scrollBar->stopAllActions();
		scrollBar->setOpacity(255);
	}
	scrollBar->setContentSize(CCSize(scrollBar->getContentSize().width, barHeight));
	scrollBar->setVisible(true);
}

void ZqlScrollView::fadeScrollBar()
{
	CCFiniteTimeAction *fade = CCFadeOut::create(1);
	CCAction *seq = CCSequence::create(fade,
									   NULL);
	if(scrollBar)
        scrollBar->runAction(seq);
}

//make the visible region
void ZqlScrollView::visit()
{
    CCDirector *director = CCDirector::sharedDirector();

	CCPoint pos = getPosition();
	CCPoint pos2 = ccp(pos.x - viewPort.width*0.5f , pos.y - viewPort.height);
    
    // apply world transform
	CCPoint wp = getParent()->convertToWorldSpace(pos2);
    
    glEnable(GL_SCISSOR_TEST);
	director->getOpenGLView()->setScissorInPoints(wp.x, wp.y, viewPort.width, viewPort.height);
    CCLayer::visit();    
    glDisable(GL_SCISSOR_TEST);
}

//to make the easing movement of scollview
bool ZqlScrollView::easing(float tY)
{
	float tempY = content->getPositionY();
	content->setPositionY( tempY + (tY - tempY) * 0.3);

	if(std::abs(tY - content->getPositionY())<0.1 ){
		content->setPositionY(tY);
		return true;
	}
	return false;
}

void ZqlScrollView::setScrollEnabled(bool val)
{
    isScrollEnabled = val;
    this->setTouchEnabled(isScrollEnabled);
    reArrange();
}

//check if any subviw is clicked.
CCNode *ZqlScrollView::touchCheck(CCPoint touchPoint)
{
	CCArray *children = content->getChildren();

    if(children == NULL)
        return NULL;

    for(int i=0; i<children->count(); i++)
    {
        CCNode *node = (CCNode*)children->objectAtIndex(i);
        CCPoint np = node->getParent()->convertToNodeSpace(touchPoint);
        if(node->boundingBox().containsPoint(np))
            return node;
    }

	return NULL;
}

//---------------------------------------------------------
//MenuScrollView
//---------------------------------------------------------

bool MenuScrollView::init()
{
	if(!ZqlScrollView::init())return false;
    
	targetPos = 0;
	targetId = -1;
    //adjust for jianghu items. it will be fix soon.
    scrollBarAdjust = 30;
    
	return true;
}

void MenuScrollView::moveTo(int id)
{
	//设置要移动的目标Id
	targetId = id;
}

void MenuScrollView::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
    CCTouch* touch = (CCTouch*)pTouches->anyObject();
    CCPoint localPoint = convertTouchToNodeSpace(touch);
    
    if(boundingBox().containsPoint(localPoint))
    {
		isTouching=true;
		makeScrollBar();
        targetId = -1;
	}
}

//adjust the position of subviews.Default vertically display
void MenuScrollView::reArrange()
{
	CCArray *children = content->getChildren();
    
	float menuHeight = 0.0;
	float menuHeightLess = 0.0;
	CCNode *temp;
	int count = content->getChildrenCount();
	for(int i = 0 ; i <  count  ; i++){
		//当设置了targetId后记录相应的子项的位置，用于后面移动到该位置
		if(i == targetId)targetPos = abs(menuHeight) + actionPort.origin.y;
        
		temp = (CCNode*)(children->objectAtIndex(i));
		temp->setAnchorPoint(ccp(0.5,1));
        
		temp->setPositionY(menuHeight);
        
		
		menuHeight -= temp->getContentSize().height;
		if(i == count-2)menuHeightLess = menuHeight ;
	}
	
	contentHeight =  abs(menuHeight) ;
    float hLess = abs(menuHeightLess) ;
    
	gap = contentHeight - actionPort.size.height + actionPort.origin.y;
	float gap2 = hLess + actionPort.origin.y;
	menuGap = std::max(gap , gap2);
    
    
	if(menuGap < actionPort.origin.y )menuGap=actionPort.origin.y;
    
    updateScrollBar();
	
}

void MenuScrollView::update(float dt)
{
	if(!isTouching )
    {
		if(content->getPositionY() < actionPort.origin.y){
			easing(actionPort.origin.y);
			v = 0;
		}
		if(content->getPositionY() > menuGap){
			easing(menuGap);
			v = 0;
		}
		if(v != 0){
			v-=1;
			if(v < 0 )v = 0;
			content->setPositionY(content->getPositionY() + direction * v);
            
			//scrollPercentage = content->getPositionY()/menuGap;
		}
        
        //当设置了移动的Id后，往之前记录好的位置移动
		if(targetId != -1  && easing(targetPos)){
			targetPos = 0;
			targetId = -1;
		}
	}
    
    //makeScrollBar();
    
	if(scrollBar && scrollBar->getOpacity())
	{
		float p = content->getPositionY();
        
		if(p < actionPort.origin.y)p = actionPort.origin.y;
		else if(p > gap)p = gap;
        
		float d = p - actionPort.origin.y;
		scrollBar->setPositionY(actionPort.origin.y-d*barScale + scrollBarAdjust);
        
	}
    
	reArrange();
    
}

void MenuScrollView::updateScrollBar()
{
    if (!scrollBar)return;
    float actionHeight = actionPort.size.height;
    float tempBarScale = actionHeight/contentHeight;
    if(tempBarScale >= 1)
    {
        scrollBar->setVisible(false);
        return;
    }
    if(tempBarScale == barScale)
        return;
    barScale = tempBarScale;
    float barHeight = actionHeight*barScale;
    scrollBar->setContentSize(CCSize(scrollBar->getContentSize().width, barHeight));
}

void MenuScrollView::setTouchEnabled(bool value)
{
    ZqlScrollView::setTouchEnabled(value);
}



