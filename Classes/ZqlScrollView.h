//create by ZQL

#pragma once

#include "cocos2d.h"
#include "CCScale9Sprite.h"
#include "ContentLayer.h"

USING_NS_CC;
USING_NS_CC_EXT;

enum class ScrollType
{
    Horizontal = 1,
    Vertical,
};

class ZqlScrollView : public ContentLayer
{
public:
	virtual bool init();
	virtual CCRect boundingBox();
    
	void addLayer(CCNode* layer);
    void removeLayer(CCNode* layer);
    void removeAllLayers();
    virtual CCArray* getChildren();
    
	void setViewSize(CCSize size);
	void setViewSize(CCSize size, CCRect actionport);
    
	virtual void reArrange();
	virtual void makeScrollBar();
	void fadeScrollBar();
    void reset();

    // default implements are used to call script callback if exist
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    // default implements are used to call script callback if exist
	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);

	virtual void visit();
    
    void setScrollEnabled(bool val);
    bool getScrollEnabled() const { return isScrollEnabled; }
    
    float getContentY();
    void setContentY(float y);

	// implement the "static node()" method manually
    CREATE_FUNC(ZqlScrollView);

protected:
    CCScale9Sprite* scrollBar;
//    CCSprite* scrollBar;
    float barScale;

    
    float v0;
	float v;
	int direction;
	CCSize viewPort;
	CCRect actionPort;
	//the gap between viewport height and the content height
	float gap;
	float contentHeight;

    // scroll view root node
    CCNode* content;
    
	CCNode *touchNode;
	//record if scroll is touched
	bool isTouching;
    bool isScrollEnabled;
	
	bool easing(float tY);
	CCNode *touchCheck(CCPoint touchPoint);
	virtual void update(float dt);
};

class MenuScrollView : public ZqlScrollView
{
public:
    int targetId;
    
    int scrollBarAdjust;
    
	virtual bool init();
	virtual void update(float dt);
	virtual void reArrange();
    virtual void setTouchEnabled(bool value);
    void updateScrollBar();
	void moveTo(int id);
    
	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
    
	static MenuScrollView* create()
	{
		MenuScrollView* o = new MenuScrollView();
		if(o != NULL && o->init())
		{
			o->autorelease();
			return o;
		}
		CC_SAFE_DELETE(o);
		return NULL;
	};
    
private:
	int targetPos;
    
	int menuGap;
    
};


