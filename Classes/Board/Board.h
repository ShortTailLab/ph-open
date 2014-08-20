#pragma once

#include "Board/Common.h"
#include "Board/Grid2D.h"
#include "Board/Gem.h"
#include "Board/Task.h"
#include "Board/EnemyControl.h"
#include "Board/HeroControl.h"
#include "Board/PlayerControl.h"
#include "Board/Property.h"
#include "Board/Utils.h"
#include "Board/AnimPreset.h"
#include "Board/BoardControls.h"

#include "Types.h"

#include "Animation/CCCallLambda.h"

#include "CCScale9Sprite.h"

namespace PH { // tolua_export

    USING_NS_CC;
    USING_NS_CC_EXT;
    
    class BoardLayer;
    
    class LuaGlobal
    {
    public:
        static void init(BoardLayer* layer, GemGrid* g, PlayerControl* player);
        static void updateHeroList(HeroControlList& l);
        static void updateEnemyList(EnemyControlList& l);
        static void printGlobalInfo();
        static void free();
    };
    
    struct BoardFinalStateConstraint
    {
        boost::function<bool(GemGrid& grid)> call;
    };
    
    void flurryLogLevelStats(int leveId, const LevelStats& stats);

#define IS_FLAG_SET(val, flag) (val & flag)
    enum SkillFlags
    {
        SKILL_FLAG_NONE = 0,
        SKILL_FLAG_LEECH = 1 << 0,
        SKILL_FLAG_BONUS_ATTACK = 1 << 1,
    };
    
    class BoardLayer : public CCLayer
    {
    public:
        BoardControlPtr         mBoardControl;
        
        std::vector<CCSprite*>  mDragHint;
        bool                    mShowDragHintInTurn;
        
    private:
        GemPtr          mDragGem;
        Vec2i           mDragGemPos;
        CCSprite*       mShadowGem;

        bool                mIsDebugMode;
        CCMenuItemLabel *   mDebugItem;
        
    protected:
        // constraint is only used in tutorial
        // should be set to always return false otherwise
        BoardFinalStateConstraint finalStateConstraint;
        bool            mIsTutorialMode;
        bool            mForceGemNoStacking;
        
        bool            mStartPromptForRevival;
        
    public:
        TaskQueue        mTaskQueue;
        PlayerControlPtr mPlayer;
        
        EnemyControlList mEnemyList;
        EnemyControlList mDivisionEnemyList;
        LevelInfo        mLevel;
        CCSprite*        mBoardMask;

        CCSprite*        mRoundBg;
        CCLabelBMFont*   mRoundText;
        CCSprite*        mBossRound;
        
        CCSprite*        mElementGraph;
        
        int mRoundIndex;
        int mTurn;
        
        LevelStats mStats;
        
        int mTotalGemCount[GemUtils::GemColorMax];
        bool mGemAllowed[GemUtils::AllColor];
        
        int mTotalDropCount;
        int mTotalHeroDropCount;
        int mTotalSoulDropCount;
        CCLabelBMFont* mDropCountLabel;
        std::vector<CCSprite*> mDropList;
        
        EnemyControlPtr mManualTarget;
        CCSprite* mTargetCross;
        
        CCMenu* mExitMenu;
        
        BoardResultPtr mResult;
        
        void* mCurrTouch;
        
        // --------- [HACK] -----------
        int     mInfectedGemsOnBoard;
 
    public:
        BoardLayer(int w, int h) :
            mTurn(0),
            mTotalDropCount(0),
            mTotalHeroDropCount(0),
            mTotalSoulDropCount(0),
            mCurrTouch(NULL),
            mIsTutorialMode(false),
            mForceGemNoStacking(false),
            mStartPromptForRevival(false),
            mInfectedGemsOnBoard(0)
        {
            this->mIsDebugMode = false;
            
            finalStateConstraint.call = [](GemGrid&) {return false;};
            
            for(int i=0; i<GemUtils::GemColorMax; i++)
                mTotalGemCount[i] = 0;
            for(int i=0; i<GemUtils::AllColor; i++)
                mGemAllowed[i] = true;
        }
        
        virtual ~BoardLayer()
        {
            printf("BoardLayer destructor invoked. free battle resources\n");
            // delete team members manually
            for(HeroControl* hc : mPlayer->team)
            {
                hc->icon->removeFromParentAndCleanup(true);
                delete hc;
            }
            
            mBoardMask->removeFromParentAndCleanup(true);
        }

        void delayedTouchEnable()
        {
            this->setTouchEnabled(true);
        }
        
        static BoardLayer* create(int width, int height, const HeroList& team, const LevelInfo& level)
        {
            BoardLayer* o = new BoardLayer(width, height);
            
            if(o != NULL && o->init(team, level))
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }
        
        virtual bool init(const HeroList& team, const LevelInfo& level);
        
        PlayerControlPtr createPlayerFromTeam(const HeroList& team);
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
        
        float isAreaAttack(const BoardResult& result, PH::GemUtils::GemColor c) const
        {
            int maxCount = 0;
            if( mPlayer->passiveBonusGemInAreaAttackCheck.isValid() )
            {
                for(auto iter : mPlayer->passiveBonusGemInAreaAttackCheck.map() )
                    if( iter.second > maxCount ) maxCount = iter.second;
            }
            
            NGemComboList combos = result.combosByColor(c);
            bool flag = false;
            for(int i=0; i<combos.size(); ++i){
                if(combos[i].first >= (5 - maxCount)){
                    flag = true;
                }
            }
            return flag;
        }
        
        void onTurn();
        void startTurn();
        void fillSweepUntilDone(BoardResultPtr result);
        
        void attack(const BoardResult& result);
        TaskPtr applyComboModWithAnim(const BoardResult& result,
                                      AttackTable& outTable);
        TaskPtr applySkillModWithAnim(const AttackTable& attackTable,
                                      AttackTable& outTable);
        TaskPtr attackWithAnim(AttackTable& attackTable, SkillFlags sflags = SKILL_FLAG_NONE, bool ignoreArmor=false);
        
        TaskPtr calcDamageForTarget(const float attack,
                                    const GemUtils::GemColor color,
                                    const EnemyControlPtr ec,
                                    float& damage,
                                    bool ignoreArmor = false,
                                    float delay = 0.f);
        TaskPtr dealDamageToAllWithAnim(DamageTable& damageTable,
                                        const float delay);
        
        TaskPtr dealDamageToOneWithAnim(float damage,
                                        EnemyControlPtr ec,
                                        float delay);
        
        TaskSequencePtr createAttackSequence(const HeroControl* hc,
                                             const EnemyControlPtr ec,
                                             const float attack1,
                                             const GemUtils::GemColor heroColor,
                                             const float delay);
        
        TaskPtr removeDeadEnemies();
        TaskPtr removeDeadPlayer();
        void endDrag();
                
        EnemyControlList getEnemiesInAttackOrder(const GemUtils::GemColor color,
                                                 const bool isArea);
        EnemyControlList getEnemiesByColor(const GemUtils::GemColor color);
        
        TaskPtr updatePlayerTurn(BoardResult& result);
        TaskPtr updateGemTurn();
        void updateEnemyTurn(BoardResult& result);
        TaskPtr updatePlayerPoison();
        TaskPtr updatePlayerSuckBlood();
        
        void castHeroSkill(CCObject* sender);
        
        bool isRoundDone();
        TaskPtr loadRound(RoundInfo& round);
        void loadNextRoundIfDone();
                
        void finishWithVictory();
        
        void returnToMainMenu();
        void promptForDebugMode();
        void promptForRevival();
        void promptForMenu(CCObject* o);
        
        void exitScene(bool isVictory);
        void sendVictoryRequest();
        void transitionToGameScene(bool isVictory,
                                   PlayerPtr newPlayer);
        
        void showDragHintWithDelay(float delay = 1.5f);
        
        void showDragHint();
        void showArrowPath(const std::vector<int> & path, int opacity = 205);
        void clearArrowPath();
        
        CCPoint getHealthCenter() const
        {
            return mPlayer->healthBox->center();
        }
        
        GemUtils::GemColor randGemWeighted();
        
        void disableUI();
        void enableUI();
        
        virtual void update(float dt);
        
        virtual void onEndOfFillSweepCycle(){};
        
        virtual void onStartTurn(){};
        virtual void onEndOfTurn(){};
        
        virtual void afterEnemyAttack() {};
        virtual void onAlertSkillCast() {};
        virtual void onAlertSkillCastCancelled() {};
        virtual void onSkillCast() {};
        virtual void onLoadNextRound() {};
        virtual bool onPlayerDeath() { return false; };

        
    protected:
        friend class ActiveSkill;
        friend class PassiveSkill;
        friend class EnemySkill;
    };
} // tolua_export
