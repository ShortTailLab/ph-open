//
//  PVPBoard.h
//  HelloCpp
//
//  Created by Zeyang Li on 4/16/13.
//
//

#pragma once

#include "Board.h"
#include "BoardControls.h"
#include "PvPMessageHandler.h"
#include "PlayerData.h"
#include "PvPHourGlass.h"
#include "PvPUtils.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

namespace PH
{
    class PVPBoardLayer : public CCLayer, public IPvPInGameReceiver
    {
    public:
        BoardControlPtr   mBoardControl;
        SystemInform*   mLoading;
        LoadingDialog*   mWaitReconnect;
        
    private:
        GemPtr            mDragGem;
        CCSprite*       mShadowGem;
        CCSprite*       mBG;
        
    public:
        TaskQueue         mTaskQueue;
        PlayerControlPtr  mPlayer1;
        PlayerControlPtr  mPlayer2;
        
        PvPHourGlass*     mHourGlass1;
        PvPHourGlass*     mHourGlass2;
        
        CCSprite*         mRoundLabelUs;
        CCSprite*         mRoundLabelThem;
        
        Resistance        mResist1;
        Resistance        mResist2;
        
        CCSprite*        mBoardMask;
        CCSprite*        mBoardWhiteMask;

        CCLayer*        mTopLayer;
        CCLayer*        mBottomLayer;
        CCSpriteBatchNode*       mBoardBG;
        CCSpriteBatchNode*       mBoardBG2;
        
        CCLabelBMFont* mSmallTurnLabel;
        CCLabelBMFont* mBigTurnLabelOurs;
        CCLabelBMFont* mBigTurnLabelTheirs;
        
        GridArrow* mGridArrow;
        CCLabelBMFont*  mWaitAckText;
        
        PvPConfig mConfig;
        
        int mTurn;

        bool mIsMyTurn;
        bool mWaitingForACK;
        int mReconnectTimeout;

        BoardResultPtr mResult;
        
    public:
        PVPBoardLayer(int w, int h) :
        mBG(NULL),
        mShadowGem(NULL),
        mHourGlass1(NULL),
        mHourGlass2(NULL),
        mTurn(0),
        mLoading(NULL),
        mWaitReconnect(NULL),
        mWaitingForACK(false)
        {
        }
        
        virtual ~PVPBoardLayer()
        {
            printf("PvPBoardLayer destructor invoked. free battle resources\n");
            
            // delete team members manually
            for(HeroControl* hc : mPlayer1->team)
            {
                hc->icon->removeFromParentAndCleanup(true);
                delete hc;
            }
            
            for(HeroControl* hc : mPlayer2->team)
            {
                hc->icon->removeFromParentAndCleanup(true);
                delete hc;
            }
            
            mBoardMask->removeFromParentAndCleanup(true);
        }
        
        static PVPBoardLayer* create(int width,
                                     int height,
                                     const Player& p1,
                                     const Player& p2,
                                     const PvPConfig& config)
        {
            PVPBoardLayer* o = new PVPBoardLayer(width, height);
            
            if(o != NULL && o->init(p1, p2, config))
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }
        
        static PVPBoardLayer* create(int width,
                                     int height,
                                     int seed,
                                     int round,
                                     const Player& p1,
                                     const Player& p2,
                                     const rapidjson::Value& battleState,
                                     float skillTimeout,
                                     float moveTimeout,
                                     float skillTimeLeft,
                                     float moveTimeLeft,
                                     const rapidjson::Value& events)
        {
            PVPBoardLayer* o = new PVPBoardLayer(width, height);
            if(o != NULL && o->initWithRecover(seed,
                                               round,
                                               p1,
                                               p2,
                                               battleState,
                                               skillTimeout,
                                               moveTimeout,
                                               skillTimeLeft,
                                               moveTimeLeft,
                                               events))
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }
        
        virtual bool init(const Player& p1,
                          const Player& p2,
                          const PvPConfig& config);
        
        virtual bool initWithRecover(int seed,
                                     int round,
                                     const Player& p1,
                                     const Player& p2,
                                     const rapidjson::Value& battleState,
                                     float skillTimeout,
                                     float moveTimeout,
                                     float skillTimeLeft,
                                     float moveTimeLeft,
                                     const rapidjson::Value& events);
        
        void initStaticUI(const Player& p1, const Player& p2);
        void initPassiveSkill(PlayerControlPtr player, const Player& pp, PlayerControlPtr other);
        TaskPtr createStartAnimation();
        
        PlayerControlPtr createPlayerFromTeam(CCLayer* parent,
                                              const CCPoint& p,
                                              const Player& pp,
                                              PlayerControl::Style style);
        void loadPlayerPassiveSkills();
        
        // a selector callback
        void menuCloseCallback(CCObject* pSender);
        
        virtual void registerWithTouchDispatcher(void)
        {
            CCLayer::registerWithTouchDispatcher();
            CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
        }
        
        // default implements are used to call script callback if exist
        virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
        virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);
        virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
        virtual void ccTouchCancelled(CCTouch *touch, CCEvent *event);
        
        bool hasEmptyCell();
        
        float isAreaAttack(const BoardResult& result,
                           PlayerControlPtr player,
                           PH::GemUtils::GemColor c) const
        {
            NGemComboList combos = result.combosByColor(c);
            
            for(int i=0; i<combos.size(); ++i)
                if(combos[i].first >= (5 - player->minGemCount))
                    return true;
            return false;
        }
        
        void onTurn();
        void startTurn();
        void fillSweepUntilDone(PlayerControlPtr player, BoardResultPtr result);
        
        void attack(PlayerControlPtr attacker,
                    PlayerControlPtr defender,
                    const BoardResult& result);
        
        TaskPtr applyComboModWithAnim(PlayerControlPtr player,
                                      const BoardResult& result,
                                      AttackTable& outTable);
        
        TaskPtr applySkillModWithAnim(PlayerControlPtr player,
                                      const AttackTable& attackTable,
                                      AttackTable& outTable);
        
        TaskPtr attackWithAnim(PlayerControlPtr attacker,
                               PlayerControlPtr defender,
                               const AttackTable& attackTable);
        
        TaskPtr calcDamageWithReflect(const float atk,
                                      const GemUtils::GemColor atkCol,
                                      const PlayerControlPtr attacker,
                                      const PlayerControlPtr defender,
                                      float delay,
                                      float & damage,
                                      bool ignore = false);
        TaskPtr dealDamageToPlayer(const float damage,
                                   const PlayerControlPtr attacker,
                                   const PlayerControlPtr defender,
                                   float delay);
        
        void onSelfTimeout();
        void onOpponentTimeout();
        
        void endDrag();
        
        TaskPtr updatePlayerTurn(PlayerControlPtr player, BoardResult& result);
        
        void castHeroSkill(CCObject* sender);
        
        PlayerControlPtr getAttacker()
        {
            return mIsMyTurn ? mPlayer1 : mPlayer2;
        }
        
        PlayerControlPtr getDefender()
        {
            return mIsMyTurn ? mPlayer2 : mPlayer1;
        }
        
        void quitMatch();
        void transitionToGameScene(PvPEndGame endGame,
                                   int reputationGain,
                                   int scoreGain,
                                   int turnUsed);
        
        GemUtils::GemColor randGemWeighted();
        
        void disableUI();
        void enableUI();
        
        virtual void update(float dt);
        void updateRival(float dt);
        
        void toJson(rapidjson::Value& doc,
                    rapidjson::Document::AllocatorType& alloc);
        
        void updateReconnectTimeout(float dt);
        
        void startBoardBreathAnim();
        void stopBoardBreathAnim();
        
        std::string turnStr() const;
        
        // receivers
        virtual bool onRivalBoardMove(int srcx, int srcy, int dstx, int dsty);
        virtual bool onRivalCastSkill(int heroSlotId);
        virtual bool onRivalEndRound(std::string stateHash, PvPEndGame victory);
        virtual bool onAckEndRound();
        
        virtual bool onTimeOut();
        virtual bool onRivalQuit();
        virtual bool onGameEnd(PvPEndGame victory, int reputationGain, int scoreGain);
        
        virtual bool onRivalDisconnect(int timeout);
        virtual bool onRivalReconnect();
        virtual bool onError(const std::string& error);
        
        virtual bool onCleanStart();
        virtual bool onConnect();
        virtual bool onDisconnect();
        
    protected:
        friend class ActiveSkill;
        friend class PassiveSkill;
        friend class EnemySkill;
    };
}