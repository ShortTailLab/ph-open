//
//  Victory.h
//  HelloCpp
//
//  Created by Zeyang Li on 12/14/12.
//
//

#pragma once

#include "ContentLayer.h"
#include "Types.h"
#include "PlayerControl.h"
//#include "HeroControl.h"
#include "HeroIcon.h"

namespace PH
{
    class AddFriendLayer : public ContentLayer
    {
    public:
        virtual bool init();
        virtual void onEnterAnimationDone();
        
        static AddFriendLayer* create()
        {
            AddFriendLayer* o = new AddFriendLayer();
            if(o && o->init())
            {
                o->autorelease();
                return o;
            }
            
            CC_SAFE_DELETE(o);
            return NULL;
        }
        
        void confirm(CCObject* o);
        void cancel(CCObject* o);
    };
    
    
    class VictoryLayer : public ContentLayer
    {
    private:
        CCMenuItem* confirmButton;
        CCMenuItem* skipChestButton,* skipGoldsButton,* skipLevelUpButton;
        LevelInfo mLevelInfo;
        LevelStats mStats;
        CCLayer* dropLayer, *labelLayer;
        
        std::vector<HeroIcon*> dropIcons;
        PlayerPtr newPlayer;
        CCSprite * bg;
        LoginRewards::iterator rewards_iter;
        
        static const int offset;
        
        bool deltaExp;
        bool deltaCoins;
        bool deltaGolds;
        std::vector<int> deltaHero;
        std::vector<std::pair<int,int>> deltaSoul;
        
    public:
        std::vector<CCSprite*> chests;
        
        static VictoryLayer* create(const LevelStats & stats, 
                                    const LevelInfo& levelInfo,
                                    PlayerControlPtr playerCtrl,
                                    PlayerPtr newPlayer)
        {
            VictoryLayer* o = new VictoryLayer();
            if(o && o->init(stats, levelInfo, playerCtrl, newPlayer))
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }

        virtual bool init(const LevelStats & stats,
                          const LevelInfo& levelInfo,
                          PlayerControlPtr playerCtrl,
                          PlayerPtr newPlayer);

        virtual void onLongClick(CCTouch* touch);
        
		void confirm(CCObject* o);
        void exitConfirm(CCObject* o);
        
        void skipChestPick(CCObject* o);
        void skipGolds(CCObject* o);
        void skipLevelUp(CCObject * o);
 
        void notifyEndGame();
        
        void showRewards();
        void showDailyRewards(CCObject *target);
        void showDrops();
        void showGold();
        void showLevelUp(const Player & prev, const Player & now );
        
        void transitionToAccount(bool withRewards);
        void pickAChest(std::vector<CCSprite*>::iterator iter, bool withLight = false);
        
        CCAction * createModifiedContentAction(CCSprite * tar,
                                               bool withLight,
                                               const int type,
                                               const int ident,
                                               const int count,
                                               const int level);
        
        virtual bool ccTouchBegan(CCTouch * pTouch, CCEvent * pEvent);
        virtual void ccTouchEnded(CCTouch * pTouch, CCEvent * pEvent);
        virtual void registerWithTouchDispatcher(void);
    };
}
