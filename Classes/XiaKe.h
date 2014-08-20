#pragma once

#include "ContentLayer.h"
#include "PlayerData.h"
#include "UIComponent.h"
#include "ButtonSprite.h"
#include <CCRichLabelTTF.h>

class ZqlScrollView;
class PageScrollView;

namespace PH
{
    class HeroIcon;
    class LabelX;
    
    class BianDuiJuanZhou : public JuanZhou
    {
    public:
        std::vector<PH::Hero *> team;
        CCLabelBMFont* hpLabel;
        CCLabelBMFont* attackLabel;
        CCLabelBMFont* regenLabel;
        CCLabelBMFont* lpLabel;
        CCLabelBMFont* lpMaxLabel;
        std::vector<CCLabelBMFont*> resistLabels;
        bool isUpdate;
        BianDuiSwitcher* switcher;
        
        virtual bool init(const std::vector<PH::Hero *>& heroVec2);
        virtual void clear();
        void updateLabel();
        
        
        static BianDuiJuanZhou* create(const std::vector<PH::Hero *>& heroVec2)
        {
            BianDuiJuanZhou* o = new BianDuiJuanZhou();
            if(o != NULL && o->init(heroVec2))
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
    
    class XiangQingJuanZhou : public JuanZhou
    {
        
    public:
        XiangQingJuanZhou();
        
        virtual bool init(std::vector<Hero*>* team);
        virtual bool addHero(PH::Hero *hero, int id, bool isUpdate = true);
        virtual void openDone();
        virtual void initHeroRelations();
        void updateRelationBoard(const HeroRelation* relation);
        void refresh();
        
        virtual void onSingleClick(CCTouch* touch);
        virtual void onLongClick(CCTouch* touch);
        
        static XiangQingJuanZhou* create(std::vector<Hero*>* team)
        {
            XiangQingJuanZhou* o = new XiangQingJuanZhou();
            if(o != NULL && o->init(team))
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }
        
    protected:
        std::vector<Hero*> targetTeam;
        TeamData teamData;
        PageScrollView *relations;
        cocos2d::CCSprite *relationBoard;
        cocos2d::CCLabelTTF *rName;
        cocos2d::CCLabelTTF *rHeroes;
        cocos2d::CCLabelTTF *rEffect;
        std::vector<HeroRelation> relats;
        std::vector<cocos2d::CCSprite*> relationItems;
        cocos2d::CCSprite * tempRItem;
        
        CCLabelBMFont* lpLabel;
        CCLabelBMFont* hpLabel;
        CCLabelBMFont* fightLabel;
        CCLabelBMFont* regenLabel;
        CCLabelBMFont* redAttackLabel;
        CCLabelBMFont* blueAttackLabel;
        CCLabelBMFont* greenAttackLabel;
        CCLabelBMFont* purpleAttackLabel;
        CCLabelBMFont* yellowAttackLabel;
        
        std::vector<CCLabelBMFont*> resistLabels;
        
        bool isShowNormalSkill;
        CCLabelTTF* skillLabel;
        LabelX* descLabel;
        
        HeroIcon *tempIcon;
        
        void onPressedTimer(float dt);
        void switchSkillType();
        void updateSkillType();
        
    private:
        ScrollRelations* relationsView;
        
    };
    
    class XiangQingLayer :public ContentLayer
    {
    public:
        virtual bool init(std::vector<Hero*>* team);
        virtual void onEnterAnimationDone();
        
        XiangQingJuanZhou *juanzhou;
        cocos2d::CCSprite *zhou1;
        cocos2d::CCSprite *zhou2;
        JuanZhouLayer *jz;
        
        static XiangQingLayer* create(std::vector<Hero*>* team = NULL)
        {
            XiangQingLayer* o = new XiangQingLayer();
            if(o != NULL && o->init(team))
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }
        void onBackClick(cocos2d::CCObject *target);
        void onTitleClick(cocos2d::CCObject *target);
        void onShare(cocos2d::CCObject *target);
    };
    
    class BianDuiBoard : public HeroBoard
    {
    public:
        virtual bool init(const std::vector<PH::Hero *>& heroVec);
        virtual void refresh(const std::vector<PH::Hero *>& heroVec);
        
        static BianDuiBoard* create(const std::vector<PH::Hero *>& heroVec)
        {
            BianDuiBoard* o = new BianDuiBoard();
            if(o != NULL && o->init(heroVec))
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }
        
    };
    
    class HeroPoolBoard : public HeroBoard
    {
    public:
        HeroPoolBoard();
        virtual void refresh(const std::vector<PH::Hero *>& heroVec);
        bool isEnlargeClick(CCTouch* touch);
        
        static HeroPoolBoard* create(const std::vector<PH::Hero *>& heroVec)
        {
            HeroPoolBoard* o = new HeroPoolBoard();
            if(o != NULL && o->init(heroVec, true))
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }
    private:
        CCSprite* enlargeBtn;
    };
    
    class BianDuiLayer : public ContentLayer
    {
    protected:
        CCMenu *topMenu;
        HeroBoard *board;
        JuanZhou *juanzhou;
        
        JuanZhouLayer *jz;
        ZqlScrollView *scrollView;
        CCLabelTTF *title;
        
        HeroIcon *curTouchIcon;
        bool isClick;
        
    private:
        CCMenuItem* backButton;
        CCMenuItem* sortButton;
        CCMenuItem* detailButton;
        
        std::vector<std::string> idsOnEnter;
        int activeTeamOnEnter;
        
        boost::function<void()> tutInit;
        boost::function<void()> tutGroupFirstHero;
        boost::function<void()> tutShowLongClick;
        boost::function<void()> tutShowBackButton;
        
    public:
        virtual bool init();
        
        virtual void onEnter();
        virtual void onEnterAnimationDone();
        virtual void onExit();
        virtual void cleanup();
        
        virtual void putHeroToJz(HeroIcon *icon);
        void putBackHeroFJzToBoard(HeroIcon *icon);
        virtual void putBackHeroToBoard(Hero *hero);
        void pullHeroIconFrBoard(HeroIcon *icon);
        void disableOverLPHeroes();
        
        virtual void setTouchEnabled(bool value);
        
        virtual void onSingleClick(CCTouch* touch);
        virtual void onLongClick(CCTouch* touch);
        
        void switchActiveTeam(int i);
        
        static BianDuiLayer* create()
        {
            BianDuiLayer* o = new BianDuiLayer();
            if(o != NULL && o->init())
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        };
        
        virtual bool tryExit(int msg = -1);
        
    protected:
        virtual void onBackClick(cocos2d::CCObject *target);
        void onDetailClick(cocos2d::CCObject *target);
        void onBianduiClick(cocos2d::CCObject *target);
    };
    
    
    
    class HeroPoolLayer : public ContentLayer
    {
    public:
        std::vector<PH::Hero*> storage;
        HeroPoolBoard *board;
        
        virtual bool init();
        virtual void onEnter();
        virtual void onExit();
        
        void initHeroNumTips();
        void onBackClick(cocos2d::CCObject *target);
        void onTitleClick(cocos2d::CCObject *target);
        virtual void onSingleClick(CCTouch* touch);
        virtual void onLongClick(CCTouch* touch);
        virtual void cleanup();
        
        static HeroPoolLayer* create()  
        {
            HeroPoolLayer* o = new HeroPoolLayer();
            if(o != NULL && o->init())
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }
        
    private:
        int currSwitchType;
        std::vector<Hero*> heroesOfCurrType;
        HeroBoardSwitcher* switcher;
        Hero *tempHero;
        CCLabelTTF *title;
        CCSprite *xian;
        ZqlScrollView *scrollView;
        CCSprite* addSeatBtn;
        CCSprite *heroNumBg;
        CCLabelTTF *numLabel;
    };
    
    class SoulLayer;
    class SoulItem : public ContentLayer
    {
    public:
        HeroProfile hProfile;
        SoulLayer* soulLayer;
        
        bool init(HeroProfile hProfile, int num, bool isNew);
        void onChange(CCObject* target);
        void update();
        virtual void onSingleClick(CCTouch* touch);
        virtual void onLongClick(CCTouch* touch);
        
        static SoulItem *create(HeroProfile hProfile, int num, bool isNew)
        {
            SoulItem *o = new SoulItem();
            if(o && o->init(hProfile, num, isNew)){
                o->autorelease();
                return o;
            }
            return NULL;
        };
        
    private:
        HeroIcon* heroImg;
        int ghostsNum;
        CCScale9Sprite* numBar;
        CCLabelBMFont* numLabel;
        CCSprite* btnSpr;
    };
    
    class SoulLayer : public ContentLayer
    {
    public:
        virtual bool init();
        virtual void onEnter();
        virtual void onExit();
        void makeItems(std::vector<std::pair<int , int>> souls);
        void initSouls();
        void onBack();
        void onChangeSortType(cocos2d::CCObject* target);
        void reOrder();
        std::string getSortStr();
        std::vector<std::pair<int , int>> getSoulsBySortType();
        CREATE_FUNC(SoulLayer);
        
    private:
        ZqlScrollView* scrollView;
        std::vector<HeroPtr> heroes;
        CCLabelTTF* title;
        std::vector<std::pair<int, int>> soulsVec;
        std::vector<int> newSouls;
    };
    
    
    ccColor3B getColorOfBonus(int bonus);
    std::string getColorStrOfBonus(int bonus);
    
    class JiQiJuanZhou : public JuanZhou
    {
    public:
        virtual bool init();
        virtual void onSingleClick(CCTouch* touch);
        virtual void onLongClick(CCTouch* touch);
        virtual void setTouchEnabled(bool val);
        
        void updateMsg();
        void playSuccessAnim();
        void playFailAnim();
        
        CREATE_FUNC(JiQiJuanZhou);
        
    private:
        CCLabelTTF* skillName;
        CCLabelTTF* skillDes;
        CCRichLabelTTF* orgGasNum;
        CCRichLabelTTF* desGasNum;
        CCLabelTTF* gasTotal;
        CCLabelTTF* goldTotal;
        CCRichLabelTTF* sucProb;
        CCLabelBMFont* gasRange;
        CCLabelBMFont* gasCost;
        CCLabelBMFont* goldCost;
        ButtonSprite* gasBtn;
        ButtonSprite* goldBtn;
        ButtonSprite* btn;
        bool isUseGas;
        
        void selectPayChanel(bool isGas);
        void swtichSortType();
    };
    
    class JiQiBoard : public HeroBoard
    {
    public:
        virtual bool init();
        virtual void refresh(const std::vector<PH::Hero *>& heroVec);
        virtual void reOrder(const std::vector<PH::Hero *>& heroVec);
        
        CREATE_FUNC(JiQiBoard);
    };
    
    class JiQiLayer : public ContentLayer
    {
    public:
        HeroBoard* board;
        
        virtual bool init();
        
        virtual void onEnterAnimationDone();
        virtual void onSingleClick(CCTouch* touch);
        virtual void onLongClick(CCTouch* touch);
        
        void choose(Hero* h);
        
        CREATE_FUNC(JiQiLayer);
        
    private:
        JiQiJuanZhou* juanzhou;
        CCLabelTTF* title;
        JuanZhouLayer* jz;
        ZqlScrollView* scrollView;
        
        Hero* chosenHero;
        
        void onTitleClick();
        void onBack();
    };
    
    
    class XiaKe : public ContentLayer
    {
    public:
        virtual bool init();
        virtual void setTouchEnabled(bool value);
        virtual void onEnter();
        virtual void onEnterAnimationDone();
        
        virtual void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
        virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
        virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
        
        static XiaKe* create()
        {
            XiaKe* o = new XiaKe();
            if(o != NULL && o->init())
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }
    protected:
        void disableAllButtons()
        {
            for(int i=0; i<items.size(); i++)
                items[i]->setTag(0);
        }
        
        void enableAllButtons()
        {
            for(int i=0; i<items.size(); i++)
                items[i]->setTag(1);
        }
        
    private:
        std::vector<CCSprite*> items;
        bool isClick;
        
        CCLabelBMFont* heroesNum;
        
        ZqlScrollView* scrollView;
        
        CCSprite* biandui;
        CCSprite* qianghua;
        CCSprite* zhuansheng;
        CCSprite* chushou;
        CCSprite* xiake;
        CCSprite* relChange;
        CCSprite* hunpo;
        CCSprite* jiqi;
    };

}
