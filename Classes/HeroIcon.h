#pragma once

#include <cocos2d.h>
#include <boost/move/move.hpp>
#include <boost/function.hpp>

namespace PH
{
    struct Hero;
    struct HeroProfile;
    
    USING_NS_CC;
    
    class HeroIcon : public CCSprite, public CCTargetedTouchDelegate
    {
    public:
        enum class LabelType
        {
            Level = 0,
            HP,
            Attack,
            Regen,
            Type,
            Ultimate,
            InTeam,
            LpLack,
            Fight
        };
        
    public:
        HeroIcon();
        ~HeroIcon();
        
        virtual bool init(Hero *hero, bool isShowLock, bool isShowLevel);
        virtual bool init(const HeroProfile *hero, bool isGhost);
        
        virtual void setColor(const ccColor3B& color3);
        virtual void setOpacity(GLubyte opacity);
        
        void setBottomMsg(const LabelType& type);
        //level hp regen attack
        void defaultBottmMsg();
        
        void showNum(int num);
        void showElite(bool isShow);
        void setEnabled(bool val);
        void mark(bool isMark);
        void levelBlink();
        void showPlusNum(int value);
        void showNew();
        void setVisible(bool);
        
        bool isClickEnabled() const { return mIsClickEnabled; };
        void setClickEnabled(bool val);
        
        
        bool isLongClickEnabled() const { return mIsLongClickEnabled; }
        void setLongClickEnabled(bool val) { mIsLongClickEnabled = val; }
        
        void setDark(bool value);
        
        Hero* getHero() { return cacheHero; }
        LabelType getLabelType() const { return mLabelType; }

        void setClickListener(boost::function<void(HeroIcon* self)> c)
        {
            mClick = boost::move(c);
        }
        
        void setLongClickListener(boost::function<void(HeroIcon* self)> c)
        {
            mLongClick = boost::move(c);
        }
        
        virtual void onEnter();
        virtual void onExit();
        
        static HeroIcon* create(Hero *hero,
                                bool isShowLock=false,
                                bool isShowLevel=false)
        {
            HeroIcon* o = new HeroIcon();
            if(o != NULL && o->init(hero, isShowLock, isShowLevel))
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }

        static HeroIcon* createWithProfile(const HeroProfile *heroProfile,
                                           bool isGhost = false)
        {
            HeroIcon* o = new HeroIcon();
            if(o != NULL && o->init(heroProfile, isGhost))
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }
    protected:
        virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
        virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
        virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
        virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
        
    private:
        virtual void  setUserData(void*) { assert(false && "method disabled"); };
        virtual void* getUserData() { assert(false && "method disabled"); };
        
        void onPressTimer(float dt);
    private:
        CCSprite *frame, *bg, *elite;
        CCNode *bottom, *lock;
        bool levelVisible;
        CCLabelBMFont* eliteNum;
        CCLabelBMFont* number;
        CCLabelBMFont* atlasNum;
        
        CCSprite* icon;
        CCSprite* check;
        
        bool mIsClickEnabled;
        bool mIsLongClickEnabled;
        LabelType mLabelType;
        
        Hero* cacheHero;
        bool ownHero;
        
        bool mIsTouchable;
        bool mIsSwallow;
        int  mTouchPriority;
        bool mIsDown;
        
        boost::function<void(HeroIcon* self)> mClick;
        boost::function<void(HeroIcon* self)> mLongClick;
    };
}
