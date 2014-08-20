//
//  WashRelation.h
//  HelloCpp
//
//  Created by zql on 13-5-2.
//
//

#ifndef __HelloCpp__WashRelation__
#define __HelloCpp__WashRelation__

#include "ContentLayer.h"
#include "HeroTable.h"
#include "UIComponent.h"
#include "ButtonSprite.h"
#include "ControlExtensions.h"

class RoundPageScrollView;
class ZqlScrollView;

namespace PH{
    
class WashPoolLayer;
    
    class RelationItem : public CCSprite
    {
    public:
        HeroRelation relation;
        
        virtual bool init(HeroRelation& relation);
        virtual void setOpacity(GLubyte value);
        void lock(bool value);
        
        static RelationItem *create(HeroRelation& relation)
        {
            RelationItem *o = new RelationItem();
            if(o && o->init(relation)){
                o->autorelease();
                return o;
            }
            return NULL;
        };
    private:
        CCSprite *mask;
        CCLabelTTF* label;
    };

class RelationJuanZhou : public JuanZhou
{
public:
    std::vector<CCPoint> itemPos;
    std::vector<RelationItem*> relItems;
    WashPoolLayer* attachLayer;
    
    virtual bool init(Hero* hero);
    void listRelations();
    void lock(int i, bool value);
    
    virtual void onSingleClick(CCTouch* touch);
    virtual void onLongClick(CCTouch* touch);
    
    static RelationJuanZhou *create(Hero* hero)
    {
        RelationJuanZhou *o = new RelationJuanZhou();
        if(o && o->init(hero)){
            o->autorelease();
            return o;
        }
        return NULL;
    };

private:
    std::vector<CCSprite*> btns;
    CCLayer* itemContainer;
    
    
    CCSprite* createRelItem(HeroRelation& r);
    void lockRelItem(CCSprite* target, bool value);
};

class RelationBoard : public CCSprite
{
public:
    HeroRelation relation;
    virtual bool init(HeroRelation& relation);
    virtual void setOpacity(GLubyte value);
    virtual void setColor(const ccColor3B& value);
    void lock(bool value);
    void occupied(bool value);
    
    static RelationBoard *create(HeroRelation& relation)
    {
        RelationBoard *o = new RelationBoard();
        if(o && o->init(relation)){
            o->autorelease();
            return o;
        }
        return NULL;
    };
    
private:
    CCSize size;
    CCSprite* bg;
    CCSprite* mask;
    CCLabelTTF *heroLabel;
    CCLabelTTF* name;
    CCLabelTTF* effectLabel;
    CCSprite* lockSigh;
    CCSprite* sigh;
};
    
    
class WashPoolLayer : public ContentLayer
{
public:
    RoundPageScrollView* scrollView;
    
    virtual bool init(Hero* hero);
    virtual void onEnterAnimationDone();
    void lock(int i);
    void locateBoard(HeroRelation& r);
    void makeRelations(std::vector<HeroRelation>& relations);
    
    virtual void onSingleClick(CCTouch* touch);
    
    static WashPoolLayer *create(Hero* hero)
    {
        WashPoolLayer *o = new WashPoolLayer();
        if(o && o->init(hero)){
            o->autorelease();
            return o;
        }
        return NULL;
    };
    
private:
    RelationJuanZhou* juanzhou;
    JuanZhouLayer* jz;
    std::vector<CCNode*> boards;
    std::vector<bool> lockMap;
    Hero* target;
    CCSprite* price;
    CCLabelBMFont* pillsLabel;
    CCLabelBMFont* priceLabel;
    CCSprite* confirmBtn;
    CCParticleSystemQuad *boil;
    CCSprite* light_front;
    ButtonSprite* relPillBtn;
    ButtonSprite* goldBtn;
    bool isPayByGold;
    
    void lockRelationBoard(HeroRelation& r,bool value);
    void showOwnedRelations();
    void showPrice();
    void choosePayWay(bool isGold);
    void updatePrice();
    int calPrice();
    int lockNum();
    CCAction* createLightAction();
    CCAction* createRItemOutAction();
    JuanZhou* createJuanZhou(Hero* hero);
    CCPoint getItemPos(int i);
    void washAnima();
    void onConfirm();
    void onBack();
};

class WashRelHeroBoard : public HeroBoard
{
public:
    virtual bool init();
    virtual void refresh(const std::vector<PH::Hero *>& heroVec);
    virtual void reOrder(const std::vector<PH::Hero *>& heroVec);
    
    CREATE_FUNC(WashRelHeroBoard);
};

class WashRelationLayer : public ContentLayer
{
public:
    virtual bool init();
    virtual void onEnter();
    virtual void onSingleClick(CCTouch* touch);
    virtual void onLongClick(CCTouch* touch);
    
    void onBack();
    
    CREATE_FUNC(WashRelationLayer);
    
private:
    ZqlScrollView* scrollView;
    HeroBoard* board;
    LabelX* title;
    
    
    void onTitleClick();
};

}

#endif /* defined(__HelloCpp__WashRelation__) */
