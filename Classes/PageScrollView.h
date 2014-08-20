//
//
//  Created by ZQL on 11.30
//
//
#pragma once 

#include "ContentLayer.h"
#include "cocos2d.h"
#include <boost/container/flat_map.hpp>
#include <boost/function.hpp>

namespace PH
{
    struct Hero;
}


class PageScrollView : public ContentLayer
{
public:
    int endNodeIndex;
    bool isHomePage;
    int size;
    
	~PageScrollView();

    ContentLayer *content;
    bool isShowOne;

	bool init(cocos2d::CCSize page , bool isShowOne);
    bool initWithSprite(const std::vector<cocos2d::CCSprite*>& childNodes , cocos2d::CCSize page , bool isShowOne);
	virtual void setCurrentNode(int nodeId);
    virtual void setMoveTargetId(int id);
    virtual void onSettle();
	int getTargetPos();
	void setCallBack(boost::function<void (cocos2d::CCSprite*)> callBack);
	virtual int getCurrentNodeId();
    void setViewPort(cocos2d::CCRect value);
    CCRect getViewPort();

	virtual void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);


	virtual void update(float dt);
    virtual void visit();

	static PageScrollView *create(const std::vector<cocos2d::CCSprite*> &nodes , cocos2d::CCSize page , bool isShowOne = true)
	{
		PageScrollView* o = new PageScrollView();
		if(o != NULL && o->initWithSprite(nodes , page , isShowOne))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	};

protected:
	//std::vector<cocos2d::CCSprite*> nodes;
    boost::container::flat_map<int, cocos2d::CCSprite*> nodes;
    std::vector<PH::Hero*> heroes;
	cocos2d::CCSize page;
    cocos2d::CCRect viewPort;
	int v;
	int a;
	int v0;
	int direction;
    
	cocos2d::CCRect touchPort;
	int currentPos;
	int targetPos;
	int currentNodeId;
    int moveTargetId;
    int currIdCashe;

	bool isTouchMoving;
	bool isViewStop;
	
	boost::function<void (cocos2d::CCSprite*)> callBack;
};

class HeroPageScrollView : public PageScrollView
{
public:
    bool init(const std::vector<PH::Hero*> &heroes , cocos2d::CCSize page , bool isShowOne = true);
    virtual int getCurrentNodeId();
    virtual void setCurrentNode(int nodeId);
    virtual void onSettle();
    
    virtual void update(float dt);
    void updateCheck();
    
    static HeroPageScrollView *create(const std::vector<PH::Hero*> &heroes , cocos2d::CCSize page , bool isShowOne = true)
	{
		HeroPageScrollView* o = new HeroPageScrollView();
		if(o != NULL && o->init(heroes , page , isShowOne))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	};
    
    
private:
    std::vector<PH::Hero*> heroes;
    int cashe;
    
    void addHero(int id);
};


class RoundPageScrollView : public ContentLayer
{
public:
    float posY;
    int radius;
    float w;
    
    virtual bool init(const std::vector<CCNode*> &nodes, int radius);
    virtual void update(float dt);
    void setCallBack(boost::function<void (int)> callBack);
    void moveTo(int nodeId);
    void updateNodes(std::vector<CCNode*> nodes);
    void removeNode(CCNode* node);
    void clear();
    void release();
    void close();
    void open();
    
    virtual void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    
    static RoundPageScrollView* create(const std::vector<CCNode*> &nodes, int radius)
    {
        RoundPageScrollView* o = new RoundPageScrollView();
        if(o != NULL && o->init(nodes, radius))
        {
            o->autorelease();
            return o;
        }
        CC_SAFE_DELETE(o);
        return NULL;
    };
    
private:
    int nodeNum;
    std::vector<CCNode*> nodes;
    float distance;
    float desW;
    int desId;
    int currId;
    float v;
    float v0;
    int direction;
    float nodeScaleControll;
    bool isTouchMoving;
    bool defaultAction;
    CCRect viewPort;
    bool isOpen;
    float Radius;
    boost::function<void (int)> callBack;
    CCSprite* star;
    
    void calDesW();
    int calCurrId();
    void updatePos();
    void render();
    void render(CCNode* node);
    bool easing(float to);
};

class ScrollMenu : public CCNode, public CCTouchDelegate
{
public:
    
    virtual bool init(std::vector<CCSprite*> nodes);
    
    void menuLeft();
    void menuRight();
    
    void setTouchEnabled(bool value);
    void setScrollEnabled(bool value);
    
    /* 触摸相关 */
    virtual void onEnter();
    virtual void onExit();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    // optional
    
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
    static ScrollMenu *create(std::vector<CCSprite*> nodes)
	{
		ScrollMenu* o = new ScrollMenu();
		if(o != NULL && o->init(nodes))
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	};
    
private:
    PageScrollView* scrollView;
    CCSprite* arrowColor;
    CCSprite* arrowGray;
    bool isTouchable;
    std::vector<CCSprite*> btns;
    CCRect touchRegion;
};



