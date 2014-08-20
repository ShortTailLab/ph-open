//
//  M1HeroEnemy.h
//  HelloCpp
//
//  Created by Zeyang Li on 10/30/12.
//
//

#pragma once

#include "cocos2d.h"
#include "TouchMaskLayer.h"
#include "../UIPresets.h"

#include <string>
#include <vector>
#include <climits>

USING_NS_CC;

namespace PH
{

#define PH_WHITE ((ccc3(0xff, 0xff, 0xff)))
#define PH_FIRE ((ccc3(0xff, 0x00, 0x00)))
#define PH_WATER  ((ccc3(0x00, 0xe1, 0xff)))
#define PH_WOOD ((ccc3(0x00, 0xff, 0x00)))
#define PH_DARK ((ccc3(0x96, 0x00, 0xff)))
#define PH_LIGHT ((ccc3(0xff, 0xff, 0x00)))
    
#define BOARD_WIDTH 6
#define BOARD_HEIGHT 5
    
#define HERO_ATTACK_DELAY 0.2f    // how long we pause inbetween two hero attacks
#define NUMBER_DURATION 0.5f

    extern const int InitialHeroIds[6];
    extern const int SuperHeroIds[5];
    extern const int GodHeroIds[5];
    bool isInitialHero(int hid);
    bool isSuperHero(int hid);
    bool isGodHero(int hid);
    bool isSkyHero(int hid);
    
    enum UITouchOrder
    {
        UI_TOUCH_ORDER_MODAL_MENU = -1000001,
        UI_TOUCH_ORDER_MODAL      = -1000000,
    };
    
    enum UIDisplayOrder
    {
        UI_DISPLAY_ORDER_MODAL = 1000000,
    };
    
    enum BattleUIOrder
    {
        ORDER_BACKGROUND = -10,    // background
        ORDER_BOARD = 0,           // items in board
        ORDER_ENEMY_STATUS_LABEL = 25, //
        ORDER_HERO_ICON = 50,
        ORDER_ABOVE_ICON = 75,     // right above icons
        ORDER_MASK = 100,          // board mask
        ORDER_FLOAT = 125,         // stuff above mask
        ORDER_STATUS_LABEL = 150,
        ORDER_ORB = 200,
        ORDER_VICTORY = 600,
    };
    
    enum TutorialProgress
    {
        TutorialNotStarted = -1,
        TutorialSkip = 0,
        TutorialDrawMainHero = 1,
        TutorialFirstBattle = 2,
        TutorialSpendGoldOnHero = 3,
        TutorialTeam = 4,
        TutorialQiangHua = 5,
        TutorialJustFinished = 6,
        
        TutorialTransformHero = 7,
        TutorialSoul = 8,
        TutorialRelation = 9,
        TutorialNeili = 10,
        TutorialShuffleRelation = 11,
        TutorialProfile = 12,
        TutorialProfileDone = 13,
    };
    
#define TUTORIAL_ARROW_TAG 2012
    
#define UNLOCK_NEILI_LEVEL    2
#define UNLOCK_HUNPO_LEVEL    1
#define UNLOCK_INSTANCE_STAGE_LEVEL 10
#define UNLOCK_YUANFEN_LEVEL  20
#define UNLOCK_EVERYTHING     40
    
    class GameConfig
    {
    public:
        static GameConfig* shared();
        void save();
        void load();
        
        bool isSFXEnabled() const { return mEnableSFX; }
        bool isBGMEnabled() const { return mEnableBGM; }
        bool isAndroidHDEnabled() const { return mEnableAndroidHD; }
        
        TutorialProgress getTutorialProgress() const { return tutorialMode; }
        
        void setSFXEnabled(bool val);
        void setBGMEnabled(bool val);
        void setAndroidHDEnabled(bool val);
        void setTutorialProgress(TutorialProgress progress);
        
    private:
        bool mEnableSFX;
        bool mEnableBGM;
        bool mEnableAndroidHD;
        TutorialProgress tutorialMode;
        
    public:
        bool showDragHint;
        bool showAttrHint;
        
        bool showInstanceStage;
        bool showSoulExchange;
        bool showShuffleRelation;

        bool firstTimeNeili;
        bool firstTimeShowInstance;
        bool firstTimeZhuansheng;
        
        int openInstanceStep;
        bool openJueDing10;
        bool isFirstScoreAfterBattle;

        std::string pushToken;

        int stageTab;
        int stageId;
        
        bool useTouchSmooth;
    };
    
    void ring(const char* file, bool loop = false);
    void playBG(const char* file);
    void stopBgMusic();
    
    std::string randHint();
    
    enum PurchaseItemList
    {
        ITEM_DRAW_ELITE_HERO = 0,
        ITEM_DRAW_10_ELITE_HERO = 1,
        ITEM_DRAW_10_EXP_ITEM = 2,
        ITEM_DRAW_10_TRANS_ITEM = 3,
        ITEM_BUY_SPIRIT_ITEM_FULL = 4,
        ITEM_BUY_SPIRIT_ITEM_HALF = 5,
        ITEM_BUY_SPIRIT_ITEM_SOME = 6,
        ITEM_RESHUFFLE_RELATION = 7,
        ITEM_BUY_REVIVE = 8,
        ITEM_BUY_STORAGE_SMALL = 9,
        ITEM_BUY_STORAGE_MID = 10,
        ITEM_BUY_STORAGE_BIG = 11,
        ITEM_BUY_VIP_PACK = 12,
        ITEM_BUY_VIP_OTHER_PACK = 13,
    };
}
