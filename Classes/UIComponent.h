//
//  UIComponent.h
//  HelloCpp
//
//  Created by zql on 13-6-23.
//
//

#pragma once

#include <iostream>

#include "HeroTable.h"
#include "ContentLayer.h"
#include "HeroIcon.h"
#include "ControlExtensions.h"

class PageScrollView;

namespace PH
{

LabelX* createTitleMenu(CCLayer* layer,
                        std::string titleMsg,
                        std::string sortMsg,
                        SEL_MenuHandler onBack,
                        SEL_MenuHandler onTitle = NULL,
                        bool isShowTop = true);
std::vector<Hero*> electHeroes(const std::vector<Hero*>& heroes);
std::vector<Hero*> electDanYaoFromHeroes(const std::vector<Hero*>& heroes);
std::vector<Hero*> electToolsFromHeroes(const std::vector<Hero*>& heroes);
std::vector<std::string> getMatSource(const Hero* hero);
std::vector<Hero*> getHeroesBySortAndSwitchType(int currIndex);
void runCircleAction(CCNode* target, int radius);
inline HeroIcon::LabelType getIconTypeOnSortType(int sortType);
cocos2d::CCSprite* makeTips(int num);
bool isSharedAvailable();
    
template<class T> inline T easing(T curr, T target, float rate){return curr;};
template<> int easing<>(int curr, int target, float rate);
    
class MatrixTrans
{
public:
    MatrixTrans(const CCPoint& _viewPoint, float _distance):viewPoint(_viewPoint),
                                                          distance(_distance){};
    //the 3D point below should all base on the hint coordinate system
    //that use the 2d viewPoint as origin
    CCPoint get2DPoint(float px, float py, float pz);
    float getScale(float pz);
    
private:
    CCPoint viewPoint;
    //distance means the distance from eyes to the screen.
    float distance;
};
    
class JuanZhou : public ContentLayer
{
public:
    virtual bool init();
    virtual void visit();
    
    std::vector<PH::Hero *> heroVec;
    std::vector<HeroIcon *> heroImgVec;
    std::vector<cocos2d::CCRect> locations;
    
    virtual bool addHero(PH::Hero *hero , int id ,bool isUpdate = true);
    
    void addHeroImg(HeroIcon *heroImg , int id , bool isUpdate = true);
    
    void removeHeroById(int id , bool isUpdate = true);
    void removeHero(const Hero *hero , bool isUpdate = true);
    
    virtual void clear();
    
    int getIdByTouch(cocos2d::CCTouch *touch);
    int getIdByPoint(cocos2d::CCPoint worldPoint);
    int getHeroId(Hero *hero);
    PH::Hero *getHeroById(int id);
    
    virtual Hero *getHeroByTouch(cocos2d::CCTouch *touch);
    HeroIcon* getImgById(int id);
    HeroIcon* getImgByPoint(cocos2d::CCPoint worldPoint);
    CCSize viewPort;
    
    virtual void openDone();
    
    CCSprite* background;
    
    int heroNum;
    
    static JuanZhou* create()
    {
        JuanZhou* o = new JuanZhou();
        if(o != NULL && o->init())
        {
            o->autorelease();
            return o;
        }
        CC_SAFE_DELETE(o);
        return NULL;
    }
    
protected:
    virtual void updateData();
    
};

class JuanZhouLayer : public ContentLayer
{
public:
    bool init();
    void addSkin(JuanZhou *juanzhou , cocos2d::CCSprite *zhouLeft , cocos2d::CCSprite *zhouRight);
    
    JuanZhou *juanzhou;
    cocos2d::CCSprite *zhou1;
    cocos2d::CCSprite *zhou2;
    bool isOpen;
    virtual void update(float dt);
    void startAnim();
    
    static JuanZhouLayer* create()
    {
        JuanZhouLayer* o = new JuanZhouLayer();
        if(o != NULL && o->init())
        {
            o->autorelease();
            return o;
        }
        CC_SAFE_DELETE(o);
        return NULL;
    }
};

class HeroBoard : public ContentLayer
{
protected:
    static const int skHeroIconWidth = 106;
    static const int skHeroIconHeight= 120;
public:
    bool displayPlaceholder;
    CCRect viewPort;
    
    HeroBoard();
    virtual ~HeroBoard();
    
    void setDisplayStoragePlaceholder(bool);
    
    virtual bool init(const std::vector<PH::Hero *>& heroVecbool, bool isPlaceHolder = true);
    virtual void refresh(const std::vector<PH::Hero *>& heroVec);
    virtual void reOrder(const std::vector<PH::Hero *>& heroVec);
    void refreshBottomMsgOnIcons();
    void initTeam();
    
    virtual Hero *getHero(cocos2d::CCTouch *touch);
    virtual void putBackHero(Hero *hero);
    HeroIcon *getHeroImg(const Hero *hero);
    HeroIcon *getHeroImgByPoint(CCPoint worldPoint);
    
    virtual void update(float dt);
    
        void setClip(float lower, float upper)
        {
            this->mClipLower = lower;
            this->mClipUpper = upper;
        }
        
        static HeroBoard* create(const std::vector<PH::Hero *>& heroVec, bool isPlaceHolder = false)
        {
            HeroBoard* o = new HeroBoard();
            if(o != NULL && o->init(heroVec, isPlaceHolder))
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }
        
    protected:
        std::vector<Hero*> heroVec;
        std::vector<HeroIcon*> heroImgVec;
        std::vector<CCSprite*> placeHolder;
        
        size_t capacity;
        
        float mClipLower, mClipUpper;
        float mLastPos;
        int   mKidsCount;
    
        CCPoint getPos(int num);
        void addHero(CCSprite *heroImg , int num, bool isUpdateSize = true);
        CCPoint getPosByNum(int num);
        virtual void refresh();
        virtual void clear();
};
    
class ScrollRelations : public ContentLayer
{
public:
    PageScrollView *scrollView;
    std::vector<CCSprite*> relationItems;
    std::vector<HeroRelation> relations;
    
    virtual bool init(const std::vector<HeroRelation>& relations, const CCRect& viewPort);
    virtual void setTouchEnabled(bool value);
    virtual void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    
    static ScrollRelations* create(const std::vector<HeroRelation>& relations, const CCRect& viewPort)
    {
        ScrollRelations* o = new ScrollRelations();
        if(o != NULL && o->init(relations, viewPort))
        {
            o->autorelease();
            return o;
        }
        CC_SAFE_DELETE(o);
        return NULL;
    }
private:
    CCSprite* relationBoard;
    CCLabelTTF* rName;
    CCLabelTTF* rHeroes;
    CCLabelTTF* rEffect;
    CCSprite* tempRItem;
    
    void updateRelationBoard(const HeroRelation* r);
};

class Switcher : public ContentLayer
{
public:
    virtual bool init(const std::vector<std::string>& paths);
    virtual void onSingleClick(CCTouch* touch);
    virtual void setAnchorPoint(CCPoint anchor);
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    
    void setCallBack(boost::function<void (int)> _callBack);
    void setCurNode(int i);
    int getCurNode();
    
    static Switcher* create(const std::vector<std::string>& paths)
    {
        Switcher* o = new Switcher();
        if(o != NULL && o->init(paths))
        {
            o->autorelease();
            return o;
        }
        CC_SAFE_DELETE(o);
        return NULL;
    }
protected:
    ContentLayer* container;
    CCSize contentSize;
    std::vector<CCSprite*> btns;
    boost::function<void (int)> callBack;
    int curNode;
};
    
class ScissorLayer : public ContentLayer
{
public:
    CCSize viewPort;
    
    virtual bool init();
    virtual void visit();
    
    CREATE_FUNC(ScissorLayer);
};
    
class BianDuiSwitcher : public Switcher
{
public:
    virtual bool init(bool isBlack);
    
    static BianDuiSwitcher* create(bool isBlack = false)
    {
        BianDuiSwitcher* o = new BianDuiSwitcher();
        if(o != NULL && o->init(isBlack))
        {
            o->autorelease();
            return o;
        }
        CC_SAFE_DELETE(o);
        return NULL;
    }
    
};
    
class HeroBoardSwitcher : public Switcher
{
public:
    int currSwitchType;
    
    virtual bool init();
    
    CREATE_FUNC(HeroBoardSwitcher);
};
    
        
class MailSwitcher : public Switcher
{
public:
    virtual bool init();
    virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);

    CREATE_FUNC(MailSwitcher);
    
};

class AchievementSwitcher : public Switcher
{
public:
    virtual bool init();
    
    CREATE_FUNC(AchievementSwitcher);
};
    
class RankSwitcher : public Switcher
{
public:
    virtual bool init();
    
    CREATE_FUNC(RankSwitcher);
};
    
}