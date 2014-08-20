//
//  TutorialBoard.h
//  HelloCpp
//
//  Created by Zeyang Li on 1/4/13.
//
//

#pragma once

#include "cocos2d.h"
#include "Board.h"

namespace PH
{
    class TutorialBoardLayer : public BoardLayer
    {
        enum Phase
        {
            PHASE_WELCOME = 0,
            
            PHASE_HINT_MENU = 1,
            PHASE_HINT_COLOR_GRAPH = 2,
            PHASE_HINT_DROP = 3,
            PHASE_HINT_ENEMY = 4,
            PHASE_HINT_PLAYER_HEROES = 5,
            PHASE_HINT_FRIENDLY = 6,
            PHASE_HINT_HP = 7,
            
            PHASE_MOVE_GEM_INTRO = 8,
            PHASE_GEM_COLOR_INTRO = 9,
            PHASE_AREA_ATTACK_INTRO = 10,
            PHASE_COMBO_INTRO = 11,
            PHASE_ENEMY_ATTACK_INTRO = 12,
            PHASE_ENEMY_ATTACK_DONE = 13,
            PHASE_HEAL_INTRO = 14,
            PHASE_USE_SKILL_INTRO = 15,
            
            PHASE_END = 16,
        };
        
    private:
        int mPhase;
        bool skipTouch;
        CCSprite* touchContinue;
        CCSprite* gemHalo;
        CCSprite* gemHalo2;
        
    public:
        TutorialBoardLayer(int w, int h) : BoardLayer(w, h){
            this->mIsTutorialMode = true;
        }
        
        bool init(const HeroList& team, const LevelInfo& level);
        
        void setGrid(GemColorGrid begin, GemColorGrid end);
        
        virtual void onStartTurn();
        virtual void onEndOfTurn();
        virtual void afterEnemyAttack();
        virtual void onSkillCast();
        virtual void onAlertSkillCast();
        virtual void onAlertSkillCastCancelled();
        virtual void onLoadNextRound();
        virtual bool onPlayerDeath();

        virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
        virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);
        virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
        virtual void ccTouchCancelled(CCTouch *touch, CCEvent *event);
        
        void showGemHalo(Vec2i p);
        void showGemHalo2(Vec2i p);
        void hideGemHalo();
        void hideGemHalo2();
        
        virtual void registerWithTouchDispatcher(void)
        {
            CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
        }
        
    public:
        static TutorialBoardLayer* create(const int width,
                                          const int height,
                                          const HeroList& team,
                                          const LevelInfo& level)
        {
            TutorialBoardLayer* o = new TutorialBoardLayer(width, height);
            if(o->init(team, level))
            {
                o->autorelease();
                return o;
            }
        
            CC_SAFE_DELETE(o);
            return o;
        }
    };
}