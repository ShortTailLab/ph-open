#include "PageScrollView.h"
#include "Board/Utils.h"
#include "UIPresets.h"
#include "ButtonSprite.h"
#include "extensions/utils.h"
#include <cmath>
#include <algorithm>


#define PI 3.1415926

USING_NS_CC;

PageScrollView::~PageScrollView()
{
    
    this->unscheduleUpdate();
}

bool PageScrollView::init(CCSize page , bool isShowOne)
{
	if(!ContentLayer::init())
        return false;

	this->page = page;
	touchPort = CCRect( -page.width , 0 , page.width*2 , page.height );
	setTouchEnabled(true);
	v = 0;
	v0 = 0;
	a = 6;
    moveTargetId = 0;
    viewPort = touchPort;
    this->isShowOne = isShowOne;
    isHomePage = false;

	targetPos = currentPos = 0;
	isTouchMoving = false;
	currentNodeId = 0;
    currIdCashe = -1;
    
    callBack = [](CCSprite *node){};

	content = ContentLayer::create();
	addChild(content);

    
	return true;  
}

bool PageScrollView::initWithSprite(const std::vector<cocos2d::CCSprite *> &childNodes, cocos2d::CCSize page, bool isShowOne)
{
    if(!init(page , isShowOne))return false;
    
    //nodes = childNodes;
	size = childNodes.size();
	for(int i = 0 ; i < size ; i++){
		childNodes[i]->setAnchorPoint(ccp(0.5 , 0));
		childNodes[i]->setPosition(ccp(i*page.width , 0));
		if(isShowOne)childNodes[i]->setOpacity(0);
		content->addChild(childNodes[i]);
        nodes[i] = childNodes[i];
	}
    
    
    
	this->scheduleUpdate();
    
    return true;
}



void PageScrollView::setViewPort(cocos2d::CCRect value)
{
    this->viewPort = value;
}

CCRect PageScrollView::getViewPort()
{
    return viewPort;
}

void PageScrollView::setCurrentNode(int nodeId)
{
	content->setPositionX(-nodeId*page.width);
	nodes[nodeId]->setOpacity(255);
    moveTargetId = nodeId;
}

void PageScrollView::setMoveTargetId(int id)
{
    assert(id >= 0 && id <= size-1 && "id should be in the valid range");
    //this flag is to control if excute callBack
    isViewStop = false;
    moveTargetId = id;
}

void PageScrollView::onSettle()
{
    
}

int PageScrollView::getCurrentNodeId()
{
	float pos = content->getPositionX();
    float t = -pos/page.width;
    int id = round(t);
    endNodeIndex = isHomePage ? size-1 : size - int(viewPort.size.width/page.width);
    id = id > 0 ? (id < endNodeIndex ? id : endNodeIndex) : 0;
    
	return id;
}

int PageScrollView::getTargetPos()
{
	return -page.width*moveTargetId;
}

void PageScrollView::setCallBack(boost::function<void (cocos2d::CCSprite*)> callBack)
{
	this->callBack = callBack;
}

void PageScrollView::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch *touch = (CCTouch*)pTouches->anyObject();
	CCPoint point = convertTouchToNodeSpace(touch);

	if(viewPort.containsPoint(point)){
		isTouchMoving = true;
		isViewStop = false;
	}
	
}
void PageScrollView::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch *touch = (CCTouch *)pTouches->anyObject();
	CCPoint currentPoint = (CCPoint)touch->getLocation();
	CCPoint prePoint = (CCPoint)touch->getPreviousLocation();

	if(isTouchMoving){
		v = currentPoint.x - prePoint.x;
		content->setPositionX(content->getPositionX() + v);
	}
	
}
void PageScrollView::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	
	if(isTouchMoving){
		//std::cout<<"targetPos"<<targetPos<<std::endl;
		isTouchMoving = false;

        v0 = abs(v);
        if(v0 > 6)
        {
            direction = v/v0;
            moveTargetId = std::max(0, std::min(getCurrentNodeId()-direction , size-1));
        }
        else
            moveTargetId = getCurrentNodeId();
        v = 0;
	}
		
}

void PageScrollView::update(float dt)
{
    if(!isTouchMoving){
        int currId = getCurrentNodeId();
        if(PH::easing(content , getTargetPos()) && currIdCashe != currId)
        {
            callBack(nodes[currId]);
            currIdCashe = currId;
        }
	}

	if(isShowOne && !isViewStop){
        for(boost::container::flat_map<int, cocos2d::CCSprite*>::iterator it = nodes.begin() ; it != nodes.end() ; it++)
            it->second->setOpacity(0);
        
		float tp = -page.width*getCurrentNodeId();
		float cp = content->getPositionX();
		float temp = abs(tp - cp)/page.width;
        int id = getCurrentNodeId();
		nodes[id]->setOpacity(255*(1-temp));
		if(id < size-1 && cp < tp)nodes[id+1]->setOpacity(255*temp);
		if(id > 0 && cp > tp)nodes[id-1]->setOpacity(255*temp);
	}
}

void PageScrollView::visit()
{
    CCDirector *director = CCDirector::sharedDirector();
    
	float n_width = viewPort.size.width ;
    float n_height = viewPort.size.height ;
    
	CCPoint pos = getPosition();
	CCPoint pos2 = ccp(pos.x + viewPort.origin.x , pos.y+viewPort.origin.y);
    
    // apply world transform
	CCPoint temp = getParent()->convertToWorldSpace(pos2);
    
    float x = temp.x;
    float y = temp.y;
    
    glEnable(GL_SCISSOR_TEST);
	director->getOpenGLView()->setScissorInPoints(x, y, n_width, n_height);
    CCLayer::visit();
    glDisable(GL_SCISSOR_TEST);
}



bool HeroPageScrollView::init(const std::vector<PH::Hero*> &heroes , cocos2d::CCSize page , bool isShowOne)
{
    if(!PageScrollView::init(page , isShowOne))return false;
    
    this->heroes = heroes;
    cashe = 3;
    
    size = heroes.size();
    
    this->scheduleUpdate();

    return true;
}

int HeroPageScrollView::getCurrentNodeId()
{
    float pos = content->getPositionX();
    float t = -pos/page.width;
    int id = round(t);
    int end = size - 1;
    id = id > 0 ? (id < end ? id : end) : 0;
    
	return id;
}


void HeroPageScrollView::setCurrentNode(int nodeId)
{
    content->setPositionX(-nodeId*page.width);
    for(int i = 0 ; i < cashe*2+1 ; i++)
        if(nodeId-cashe+i >=0 && nodeId-cashe+i < size)addHero(nodeId-cashe+i);
    nodes[nodeId]->setOpacity(255);
    moveTargetId = nodeId;
}

void HeroPageScrollView::update(float dt)
{
    updateCheck();
    PageScrollView::update(dt);
    
}

void HeroPageScrollView::updateCheck()
{
    int id = getCurrentNodeId();
    if(id < cashe || id > size-cashe-1 )return;
    for(int i = 1 ; i <= cashe ; i++)
        if(!nodes.count(id - i*direction))addHero(id - i*direction);
}

void HeroPageScrollView::onSettle()
{
    
}

void HeroPageScrollView::addHero(int id)
{
    CCSprite* img = makeHeroImg(heroes[id]);
    img->setAnchorPoint(ccp(0.5 , 0));
    img->setPositionX(id * page.width);
    img->setOpacity(0);
    content->addChild(img);
    nodes[id] = img;
}


bool RoundPageScrollView::init(const std::vector<CCNode*> &_nodes, int _radius)
{
    if(!ContentLayer::init())
        return false;
    posY = -180;
    w = 0;
    v = 0;
    currId = 0;
    radius = Radius = _radius;
    nodeScaleControll = 1.0;
    distance = 300;
    isOpen = false;
    viewPort = CCRectMake(-winSize.width*0.5, -400, winSize.width, 700);
    nodes = _nodes;
    nodeNum = nodes.size();
    defaultAction = true;
    callBack = [](int id){};
    
    updateNodes(nodes);
    
    setTouchEnabled(true);
    
    //star = PH::GetSprite("gameui/cha.png");
    //star->setPosition(ccp(0,100));
    //addChild(star);
    
    update(0.0);
    this->scheduleUpdate();
    
    return true;
}



void RoundPageScrollView::updateNodes(std::vector<CCNode*> _nodes)
{
    clear();
    nodes = _nodes;
    
    for(CCNode* node : nodes)
    {
        addChild(node);
    }
}

void RoundPageScrollView::clear()
{
    for(CCNode* node : nodes)
        node->removeFromParentAndCleanup(true);
    
    nodes.clear();
}

void RoundPageScrollView::removeNode(CCNode* node)
{
    std::vector<CCNode*>::iterator it = find(nodes.begin(), nodes.end(), node);
    if(it != nodes.end())
    {
        (*it)->removeFromParentAndCleanup(true);
        nodes.erase(it);
    }
}

void RoundPageScrollView::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch *touch = (CCTouch*)pTouches->anyObject();
	CCPoint point = convertTouchToNodeSpace(touch);
    
	if(viewPort.containsPoint(point))
    {
		isTouchMoving = true;
	}
}
void RoundPageScrollView::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	CCTouch *touch = (CCTouch *)pTouches->anyObject();
    
    CCPoint curLocalPoint = convertToNodeSpace((CCPoint)touch->getLocation());
    CCPoint preLocalPoint = convertToNodeSpace((CCPoint)touch->getPreviousLocation());
    
	if(isTouchMoving){
        v = atan(curLocalPoint.x/radius) - atan(preLocalPoint.x/radius);
        w += v;
	}
	
}
void RoundPageScrollView::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	if(isTouchMoving){
		//std::cout<<"targetPos"<<targetPos<<std::endl;
		isTouchMoving = false;
        
        if(v!=0){
			v0 = v>0?v:-v;
		    direction = v/v0;
			v0 = std::min(v0 , 1.0f);
            v = 0;
		}
        calDesW();
	}
    
}

void RoundPageScrollView::calDesW()
{
    float wPerPiece = 2*PI/nodeNum;
    int id = 0;
    if(v0 < 0.02)
    {
        id = round(w/wPerPiece);
    }
    else
    {
        float temp = direction == 1? ceil(w/wPerPiece) : floor(w/wPerPiece);
        id = temp + direction*int((v0-0.02)/0.15);
    }
    desW = id*wPerPiece;
    //consider when id is negative.
    desId = (id%nodeNum+nodeNum)%nodeNum;
}

int RoundPageScrollView::calCurrId()    
{
    float wPerPiece = 2*PI/nodeNum;
    int id = round(w/wPerPiece);
    return (id%nodeNum+nodeNum)%nodeNum;
}
void RoundPageScrollView::release()
{
    nodes.clear();
}

void RoundPageScrollView::close()
{
    setTouchEnabled(false);
    defaultAction = false;
    isOpen = false;
}
void RoundPageScrollView::open()
{
    isOpen = true;
}

void RoundPageScrollView::moveTo(int nodeId)
{
    if(nodeId >=0 && nodeId < nodes.size())
    {
        float wPerPiece = 2*PI/nodeNum;
        desW = round((w - wPerPiece*(nodeNum-nodeId))/(2*PI))*2*PI + wPerPiece*(nodeNum-nodeId);
        //desW = -wPerPiece*nodeId;
    }
}
void RoundPageScrollView::setCallBack(boost::function<void (int)> callBack)
{
    this->callBack = callBack;
}

void RoundPageScrollView::update(float dt)
{
    int calId = calCurrId();
    if(currId != calId)
    {
        callBack(calId);
        currId = calId;
    }
    
    if(!isTouchMoving)
    {
        if(defaultAction)
        {
            easing(desW);
        }
        else
        {
            if(!isOpen)
            {
                radius = radius>80 ? radius-5 : 80;
                nodeScaleControll = nodeScaleControll>0.21? nodeScaleControll-0.04 : 0.2;
            }
            else
            {
                radius = radius<Radius ? radius+5 : Radius;
                nodeScaleControll = nodeScaleControll<1? nodeScaleControll+0.05 : 1;
                if(radius == Radius && nodeScaleControll == 1)
                {
                    setTouchEnabled(true);
                    defaultAction = true;
                    v0 = 0;
                    calDesW();
                }
            }
            //CCLog(PH::toStr(nodeScaleControll).c_str());
            w += 0.25*(dt*60);
        }
    }
    
    updatePos();
    render();
}


bool RoundPageScrollView::easing(float to)
{
    if(w == to)
        return true;
    
	w += (to-w)*0.25;
    
	if(std::abs(to - w) < 0.01){
		w = to;
        
		return true;
	}
	return false;
}

void RoundPageScrollView::updatePos()
{
    for(int i = 0 ; i < nodes.size(); i++)
    {
        float angle = w + 2*PI*i/nodes.size();
        nodes[i]->setPosition(ccp(radius*sin(angle), radius*cos(angle)-radius));
    }
}

void RoundPageScrollView::render()
{
    for(CCNode* node : nodes)
        render(node);
}

void RoundPageScrollView::render(CCNode* node)
{
    float scale = distance/(distance-node->getPositionY());
    node->setScale(scale*nodeScaleControll);
    node->setZOrder(int(node->getPositionY()));
    int color = 255 + 200*node->getZOrder()/(2*radius);
    if(dynamic_cast<CCSprite*>(node))
        dynamic_cast<CCSprite*>(node)->setColor(ccc3(color, color, color));
    node->setPosition(ccp(node->getPositionX(), posY*node->getScale()));
}


bool ScrollMenu::init(std::vector<CCSprite*> nodes)
{
    isTouchable = true;
    btns = nodes;
    
    arrowColor = PH::GetSprite("gameui/fangxiang_daohang.png");
    addChild(arrowColor);
    arrowGray = PH::GetSprite("gameui/fangxiang_daohang2.png");
    addChild(arrowGray);
    
    std::vector<CCSprite*> btnsToScroll;
    for(CCSprite* btn : btns)
    {
        cocos2d::CCSprite* btnBg = PH::GetSprite("gameui/yuandi.png");
        btn->setPosition(ccp(btnBg->getContentSize().width*0.5f, btnBg->getContentSize().height*0.5f));
        btnBg->addChild(btn);
        btnsToScroll.push_back(btnBg);
    }
    
    scrollView  = PageScrollView::create(btnsToScroll,CCSizeMake(110, 100),false);
    scrollView->setViewPort(CCRectMake(-60, 0, 550, 120));
    scrollView->setPositionX(45);
    addChild(scrollView);
    
    scrollView->setTouchEnabled(false);
    
    menuLeft();
    
    return true;
}

void ScrollMenu::menuLeft()
{
    arrowColor->setFlipX(false);
    arrowColor->setPosition(ccp(560,50));
    arrowGray->setFlipX(false);
    arrowGray->setPosition(ccp(-40, 50));
}

void ScrollMenu::menuRight()
{
    arrowColor->setFlipX(true);
    arrowColor->setPosition(ccp(-40,50));
    arrowGray->setFlipX(true);
    arrowGray->setPosition(ccp(560, 50));
}

void ScrollMenu::setScrollEnabled(bool value)
{
    if (isTouchable != value)
    {
        isTouchable = value;
        if (m_bRunning)
        {
            if (value)
            {
                CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-200, false);
            }
            else
            {
                CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
            }
        }
    }
}

void ScrollMenu::setTouchEnabled(bool value)
{
    for(CCSprite* btn : btns)
        ((ButtonSprite*)btn)->setTouchEnabled(value);
    
    //setScrollEnabled(value);
}

/* 触摸相关 */
void ScrollMenu::onEnter()
{
    CCNode::onEnter();
    if(isTouchable)
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-128, false);
    
}

void ScrollMenu::onExit()
{
    CCNode::onExit();
    if(isTouchable)
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

bool ScrollMenu::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    CCSet set;
    set.addObject(pTouch);
    scrollView->ccTouchesBegan(&set, pEvent);
    return true;
}
// optional

void ScrollMenu::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    CCSet set;
    set.addObject(pTouch);
    scrollView->ccTouchesMoved(&set, pEvent);
}

void ScrollMenu::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    CCSet set;
    set.addObject(pTouch);
    scrollView->ccTouchesEnded(&set, pEvent);
    if(scrollView->getCurrentNodeId() == 0)
        menuLeft();
    else if(scrollView->getCurrentNodeId() == scrollView->endNodeIndex)
        menuRight();
}

void ScrollMenu::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    CCSet set;
    set.addObject(pTouch);
    scrollView->ccTouchesCancelled(&set, pEvent);
}

