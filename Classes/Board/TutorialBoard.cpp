//
//  TutorialBoard.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 1/4/13.
//
//

#include "TutorialBoard.h"
#include "CCScale9Sprite.h"
#include "Fonts.h"
#include "UIPresets.h"

namespace PH
{
    
    const GemUtils::GemColor WA = GemUtils::Water;
    const GemUtils::GemColor FR = GemUtils::Fire;
    const GemUtils::GemColor WD = GemUtils::Wood;
    const GemUtils::GemColor DK = GemUtils::Dark;
    const GemUtils::GemColor LT = GemUtils::Light;
    const GemUtils::GemColor HH = GemUtils::Health;

    /*
     g(0,4) = ; g(1,4) = ; g(2,4) = ; g(3,4) = ; g(4,4) = ; g(5,4) = ;
     g(0,3) = ; g(1,3) = ; g(2,3) = ; g(3,3) = ; g(4,3) = ; g(5,3) = ;
     g(0,2) = ; g(1,2) = ; g(2,2) = ; g(3,2) = ; g(4,2) = ; g(5,2) = ;
     g(0,1) = ; g(1,1) = ; g(2,1) = ; g(3,1) = ; g(4,1) = ; g(5,1) = ;
     g(0,0) = ; g(1,0) = ; g(2,0) = ; g(3,0) = ; g(4,0) = ; g(5,0) = ;
     */

    GemColorGrid SimpleAttackInitialGrid()
    {
        GemColorGrid g(BOARD_WIDTH, BOARD_HEIGHT);
        
        g(0,4) = WA; g(1,4) = FR; g(2,4) = LT; g(3,4) = FR; g(4,4) = WD; g(5,4) = WA;
        g(0,3) = WD; g(1,3) = WA; g(2,3) = DK; g(3,3) = HH; g(4,3) = WA; g(5,3) = FR;
        g(0,2) = FR; g(1,2) = WD; g(2,2) = FR; g(3,2) = LT; g(4,2) = WA; g(5,2) = HH;
        g(0,1) = LT; g(1,1) = FR; g(2,1) = WA; g(3,1) = FR; g(4,1) = WD; g(5,1) = FR;
        g(0,0) = DK; g(1,0) = LT; g(2,0) = FR; g(3,0) = LT; g(4,0) = DK; g(5,0) = LT;
        
        return g;
    }
    
    GemColorGrid SimpleAttackDoneGrid()
    {
        GemColorGrid g(BOARD_WIDTH, BOARD_HEIGHT);
        
        g(0,4) = WA; g(1,4) = FR; g(2,4) = LT; g(3,4) = FR; g(4,4) = WA; g(5,4) = WD;
        g(0,3) = WD; g(1,3) = WA; g(2,3) = DK; g(3,3) = HH; g(4,3) = WA; g(5,3) = FR;
        g(0,2) = FR; g(1,2) = WD; g(2,2) = FR; g(3,2) = LT; g(4,2) = WA; g(5,2) = HH;
        g(0,1) = LT; g(1,1) = FR; g(2,1) = WA; g(3,1) = FR; g(4,1) = WD; g(5,1) = FR;
        g(0,0) = DK; g(1,0) = LT; g(2,0) = FR; g(3,0) = LT; g(4,0) = DK; g(5,0) = LT;
        
        return g;
    }
    
    
    GemColorGrid CriticalAttackInitialGrid()
    {
        GemColorGrid g(BOARD_WIDTH, BOARD_HEIGHT);
        
        g(0,4) = WA; g(1,4) = FR; g(2,4) = LT; g(3,4) = FR; g(4,4) = FR; g(5,4) = WD;
        g(0,3) = WD; g(1,3) = WA; g(2,3) = DK; g(3,3) = HH; g(4,3) = LT; g(5,3) = FR;
        g(0,2) = FR; g(1,2) = WD; g(2,2) = FR; g(3,2) = LT; g(4,2) = WA; g(5,2) = FR;
        g(0,1) = LT; g(1,1) = FR; g(2,1) = WA; g(3,1) = FR; g(4,1) = WD; g(5,1) = DK;
        g(0,0) = DK; g(1,0) = LT; g(2,0) = FR; g(3,0) = LT; g(4,0) = DK; g(5,0) = LT;
        
        return g;
    }
    
    GemColorGrid CriticalAttackDoneGrid()
    {
        GemColorGrid g(BOARD_WIDTH, BOARD_HEIGHT);
        
        g(0,4) = WA; g(1,4) = FR; g(2,4) = LT; g(3,4) = FR; g(4,4) = WD; g(5,4) = FR;
        g(0,3) = WD; g(1,3) = WA; g(2,3) = DK; g(3,3) = HH; g(4,3) = LT; g(5,3) = FR;
        g(0,2) = FR; g(1,2) = WD; g(2,2) = FR; g(3,2) = LT; g(4,2) = WA; g(5,2) = FR;
        g(0,1) = LT; g(1,1) = FR; g(2,1) = WA; g(3,1) = FR; g(4,1) = WD; g(5,1) = DK;
        g(0,0) = DK; g(1,0) = LT; g(2,0) = FR; g(3,0) = LT; g(4,0) = DK; g(5,0) = LT;
        
        return g;
    }
    
    
    GemColorGrid AreaAttackInitalGrid()
    {
        GemColorGrid g(BOARD_WIDTH, BOARD_HEIGHT);
        
        g(0,4) = WA; g(1,4) = FR; g(2,4) = LT; g(3,4) = FR; g(4,4) = WD; g(5,4) = WA;
        g(0,3) = WD; g(1,3) = WA; g(2,3) = DK; g(3,3) = HH; g(4,3) = LT; g(5,3) = WD;
        g(0,2) = FR; g(1,2) = WD; g(2,2) = FR; g(3,2) = LT; g(4,2) = DK; g(5,2) = WD;
        g(0,1) = LT; g(1,1) = FR; g(2,1) = WA; g(3,1) = FR; g(4,1) = WD; g(5,1) = WD;
        g(0,0) = DK; g(1,0) = LT; g(2,0) = FR; g(3,0) = LT; g(4,0) = DK; g(5,0) = WD;
        
        return g;
    }
    
    GemColorGrid AreaAttackDoneGrid()
    {
        GemColorGrid g(BOARD_WIDTH, BOARD_HEIGHT);
        
        g(0,4) = WA; g(1,4) = FR; g(2,4) = LT; g(3,4) = FR; g(4,4) = WA; g(5,4) = WD;
        g(0,3) = WD; g(1,3) = WA; g(2,3) = DK; g(3,3) = HH; g(4,3) = LT; g(5,3) = WD;
        g(0,2) = FR; g(1,2) = WD; g(2,2) = FR; g(3,2) = LT; g(4,2) = DK; g(5,2) = WD;
        g(0,1) = LT; g(1,1) = FR; g(2,1) = WA; g(3,1) = FR; g(4,1) = WD; g(5,1) = WD;
        g(0,0) = DK; g(1,0) = LT; g(2,0) = FR; g(3,0) = LT; g(4,0) = DK; g(5,0) = WD;
        
        return g;
    }
    
    GemColorGrid ComboAttackInitialGrid()
    {
        GemColorGrid g(BOARD_WIDTH, BOARD_HEIGHT);
        
        g(0,4) = WA; g(1,4) = FR; g(2,4) = LT; g(3,4) = FR; g(4,4) = WA; g(5,4) = WD;
        g(0,3) = WD; g(1,3) = WA; g(2,3) = DK; g(3,3) = WD; g(4,3) = FR; g(5,3) = WA;
        g(0,2) = FR; g(1,2) = WD; g(2,2) = FR; g(3,2) = WD; g(4,2) = FR; g(5,2) = WA;
        g(0,1) = LT; g(1,1) = FR; g(2,1) = WA; g(3,1) = FR; g(4,1) = WA; g(5,1) = LT;
        g(0,0) = DK; g(1,0) = LT; g(2,0) = FR; g(3,0) = LT; g(4,0) = DK; g(5,0) = WA;
        
        return g;
    }
    
    GemColorGrid ComboAttackDoneGrid()
    {
        GemColorGrid g(BOARD_WIDTH, BOARD_HEIGHT);
        
        g(0,4) = WA; g(1,4) = FR; g(2,4) = LT; g(3,4) = WD; g(4,4) = FR; g(5,4) = WA;
        g(0,3) = WD; g(1,3) = WA; g(2,3) = DK; g(3,3) = WD; g(4,3) = FR; g(5,3) = WA;
        g(0,2) = FR; g(1,2) = WD; g(2,2) = FR; g(3,2) = WD; g(4,2) = FR; g(5,2) = WA;
        g(0,1) = LT; g(1,1) = FR; g(2,1) = WA; g(3,1) = FR; g(4,1) = WA; g(5,1) = LT;
        g(0,0) = DK; g(1,0) = LT; g(2,0) = FR; g(3,0) = LT; g(4,0) = DK; g(5,0) = WA;
        
        return g;
    }
    
    GemColorGrid IdleInitialGrid()
    {
        GemColorGrid g(BOARD_WIDTH, BOARD_HEIGHT);
        
        g(0,4) = WA; g(1,4) = FR; g(2,4) = LT; g(3,4) = FR; g(4,4) = WD; g(5,4) = WA;
        g(0,3) = WD; g(1,3) = WA; g(2,3) = DK; g(3,3) = HH; g(4,3) = LT; g(5,3) = FR;
        g(0,2) = FR; g(1,2) = WD; g(2,2) = FR; g(3,2) = LT; g(4,2) = WA; g(5,2) = HH;
        g(0,1) = LT; g(1,1) = FR; g(2,1) = WA; g(3,1) = FR; g(4,1) = WD; g(5,1) = FR;
        g(0,0) = DK; g(1,0) = LT; g(2,0) = FR; g(3,0) = LT; g(4,0) = DK; g(5,0) = LT;
        
        return g;
    }
    
    GemColorGrid HealInitialGrid()
    {
        GemColorGrid g(BOARD_WIDTH, BOARD_HEIGHT);
        
        g(0,4) = WA; g(1,4) = FR; g(2,4) = HH; g(3,4) = HH; g(4,4) = WD; g(5,4) = HH;
        g(0,3) = WD; g(1,3) = WA; g(2,3) = DK; g(3,3) = HH; g(4,3) = LT; g(5,3) = FR;
        g(0,2) = FR; g(1,2) = WD; g(2,2) = FR; g(3,2) = LT; g(4,2) = WA; g(5,2) = HH;
        g(0,1) = LT; g(1,1) = FR; g(2,1) = WA; g(3,1) = FR; g(4,1) = WD; g(5,1) = FR;
        g(0,0) = DK; g(1,0) = LT; g(2,0) = FR; g(3,0) = LT; g(4,0) = DK; g(5,0) = LT;
        
        return g;
    }
    
    GemColorGrid HealDoneGrid()
    {
        GemColorGrid g(BOARD_WIDTH, BOARD_HEIGHT);
        
        g(0,4) = WA; g(1,4) = FR; g(2,4) = HH; g(3,4) = HH; g(4,4) = HH; g(5,4) = WD;
        g(0,3) = WD; g(1,3) = WA; g(2,3) = DK; g(3,3) = HH; g(4,3) = LT; g(5,3) = FR;
        g(0,2) = FR; g(1,2) = WD; g(2,2) = FR; g(3,2) = LT; g(4,2) = WA; g(5,2) = HH;
        g(0,1) = LT; g(1,1) = FR; g(2,1) = WA; g(3,1) = FR; g(4,1) = WD; g(5,1) = FR;
        g(0,0) = DK; g(1,0) = LT; g(2,0) = FR; g(3,0) = LT; g(4,0) = DK; g(5,0) = LT;
        
        return g;
    }
    
    CCScale9Sprite* createBracket(float width, float height)
    {
        CCScale9Sprite* s = Get9SpriteFromFrame("bracket_full.png",
                                                CCRect(30, 30, 40, 40));
                                                   /*
        CCScale9Sprite::create("ui/bracket_full.png",
                                                   CCRect(0, 0, 100, 100),
                                                   CCRect(30, 30, 40, 40));
                                                    */
        s->setAnchorPoint(ccp(0, 0));
        s->setContentSize(CCSize(width, height));
        s->runAction(CCBlink::create(2, 6));
        
        return s;
    }
    
    static bool isGridColorEqual(GemColorGrid ref, GemGrid& grid)
    {
        for(int i=0; i<ref.size(); i++)
        {
            if(grid(i)->color() != ref(i))
                return false;
        }
        return true;
    }
    
    bool TutorialBoardLayer::init(const HeroList& team, const LevelInfo& level)
    {
        if(!BoardLayer::init(team, level))
            return false;
     
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("tutorialMisc.plist");
        
        // create touch to continue banner
        {
            touchContinue = GetSprite("ui/touch_continue.png");
            touchContinue->setAnchorPoint(ccp(0, 0));
            touchContinue->setPosition(ccp(0, 0));
            touchContinue->setVisible(false);
            this->addChild(touchContinue, ORDER_FLOAT+1);
            
            CCFiniteTimeAction* fadeAnim = CCSequence::create(CCFadeTo::create(0.613, 100),
                                                              CCFadeTo::create(0.613, 255),
                                                              NULL);
            touchContinue->runAction(CCRepeatForever::create((CCActionInterval*)fadeAnim));
        }
        
        mExitMenu->setEnabled(false);
        
        mPhase = PHASE_HINT_MENU;
        
        gemHalo = NULL;
        gemHalo2 = NULL;
        
        skipTouch = false;
        
        for(HeroControl* hc : mPlayer->team)
        {
            hc->icon->enabledInTutorial = false;
        }
        
        AlertMessage::create(this,
                             "",
                             "这是我们的第一场战斗，首先介绍一下界面。",
                             false,
                             [=]()
        {
            touchContinue->setVisible(true);
            
            ccTouchBegan(NULL, NULL);
        });
        
        mForceGemNoStacking = true;

        
        return true;
    }
    
    void TutorialBoardLayer::showGemHalo(Vec2i gemXY)
    {
        gemHalo = GetSprite("ui/light_zhuzi.png");
        gemHalo->setAnchorPoint(ccp(0, 0));
        gemHalo->setPosition(ccp(-10, -10));
        gemHalo->setScale(1.05);
        gemHalo->setColor(ccc3(0xff, 0xd0, 0x40));
        CCFiniteTimeAction* fadeAnim = CCSequence::create(CCFadeTo::create(0.5, 0),
                                                          CCFadeTo::create(0.5, 255),
                                                          NULL);
        gemHalo->runAction(CCRepeatForever::create((CCActionInterval*)fadeAnim));
        
        mBoardControl->get(gemXY)->root->addChild(gemHalo, -1);
        
        gemHalo->retain();
    }
    
    void TutorialBoardLayer::showGemHalo2(Vec2i gemXY)
    {
        gemHalo2 = GetSprite("ui/light_zhuzi.png");
        gemHalo2->setAnchorPoint(ccp(0, 0));
        gemHalo2->setPosition(ccp(-10, -10));
        gemHalo2->setScale(1.05);
        CCFiniteTimeAction* fadeAnim = CCSequence::create(CCFadeTo::create(0.5, 0),
                                                          CCFadeTo::create(0.5, 255),
                                                          NULL);
        gemHalo2->runAction(CCRepeatForever::create((CCActionInterval*)fadeAnim));
        
        mBoardControl->get(gemXY)->root->addChild(gemHalo2, -1);
        
        gemHalo2->retain();
    }
    
    void TutorialBoardLayer::hideGemHalo()
    {
        gemHalo->removeFromParentAndCleanup(true);
        gemHalo->release();
        gemHalo = NULL;
    }
    
    void TutorialBoardLayer::hideGemHalo2()
    {
        if(gemHalo2)
        {
            gemHalo2->removeFromParentAndCleanup(true);
            gemHalo2->release();
            gemHalo2 = NULL;
        }
    }
    
    void TutorialBoardLayer::setGrid(GemColorGrid begin, GemColorGrid end)
    {
        mTaskQueue.enqueue(mBoardControl->patchGemGrid(begin));
        finalStateConstraint.call = [=](GemGrid& grid) -> bool
        {
            // make a copy to avoid making lambda mutable
            GemColorGrid endGrid = end;
            GemColorGrid beginGrid = begin;
            if(isGridColorEqual(endGrid, grid))
            {
                // constraint does not break
                return false;
            }
            else
            {
                // constraint breaks
                mTaskQueue.enqueue(mBoardControl->patchGemGrid(beginGrid));
                return true;
            }
        };
    }
    
    bool TutorialBoardLayer::ccTouchBegan(CCTouch* touch, CCEvent* event)
    {
        if(skipTouch || mPhase == PHASE_END)
        {
            return BoardLayer::ccTouchBegan(touch, event);
        }
        
        ArrowMask::shared()->clear();
        
        //-----------------------------------------------
        // waterfall of state actions.
        // create stuff for the corresponding states.
        // all board states should directly transition
        // to their wait states
        if(mPhase == PHASE_HINT_MENU)
        {
            CCSprite* ss = GetSprite("ui/yindao_caidan.png");
            ss->setAnchorPoint(ccp(0, 1));
            ss->setPosition(ccp(66, 948));
            ss->runAction(CCBlink::create(2, 6));
            this->addChild(ss, ORDER_FLOAT);
            mPhase++;
        }
        else if(mPhase == PHASE_HINT_COLOR_GRAPH)
        {
            CCSprite* ss = GetSprite("ui/shuxingxiangke.png");
            ss->setAnchorPoint(ccp(1.0, 0.5));
            ss->setPosition(ccp(560, 870));
            ss->runAction(CCBlink::create(2, 6));
            this->addChild(ss, ORDER_FLOAT);
            mPhase++;
        }
        else if(mPhase == PHASE_HINT_DROP)
        {
            CCSprite* ss = GetSprite("ui/yindao_diaoluoshu.png");
            ss->setAnchorPoint(ccp(0.5f, 1.0f));
            ss->setPosition(ccp(51, 840));
            ss->runAction(CCBlink::create(2, 6));
            this->addChild(ss, ORDER_FLOAT);
            mPhase++;
        }
        else if(mPhase == PHASE_HINT_ENEMY)
        {
            CCScale9Sprite* hintArea = createBracket(480, 220);
            hintArea->setPosition(ccp(80, 670));
            this->addChild(hintArea, ORDER_FLOAT);
            
            CCSprite* ss = GetSprite("ui/direnxiake.png");
            ss->setPosition(ccp(330, 850));
            ss->runAction(CCBlink::create(2, 6));
            this->addChild(ss, ORDER_FLOAT);
            mPhase++;
        }
        else if(mPhase == PHASE_HINT_PLAYER_HEROES)
        {
            CCScale9Sprite* hintArea = createBracket(425, 115);
            hintArea->setPosition(ccp(0, 550));
            this->addChild(hintArea, ORDER_FLOAT);
            
            CCSprite* ss = GetSprite("ui/wofangxiake.png");
            ss->setPosition(ccp(18, 668));
            ss->setAnchorPoint(ccp(0, 0));
            ss->runAction(CCBlink::create(2, 6));
            this->addChild(ss, ORDER_FLOAT);
            mPhase++;
        }
        else if(mPhase == PHASE_HINT_FRIENDLY)
        {
            CCScale9Sprite* hintArea = createBracket(105, 105);
            hintArea->setPosition(ccp(530, 560));
            this->addChild(hintArea, ORDER_FLOAT);
            
            CCSprite* ss = GetSprite("ui/zhiyuanxiake.png");
            ss->setPosition(ccp(533, 667));
            ss->setAnchorPoint(ccp(0, 0));
            ss->runAction(CCBlink::create(2, 6));
            this->addChild(ss, ORDER_FLOAT);
            mPhase++;
        }
        else if(mPhase == PHASE_HINT_HP)
        {
            CCSprite* ss = GetSprite("ui/wofanghp_xiangxia.png");
            ss->setAnchorPoint(ccp(0.5, 0));
            ss->setPosition(ccp(505, 567));
            ss->runAction(CCBlink::create(2, 6));
            this->addChild(ss, ORDER_FLOAT);
            mPhase++;
        }
        else if(mPhase == PHASE_MOVE_GEM_INTRO)
        {
            touchContinue->setVisible(false);
            
            setGrid(SimpleAttackInitialGrid(), SimpleAttackDoneGrid());
            AlertMessage::create("",
                                 "转珠可以任意拖拽，尝试把[color=ffff0000]同色[/color]的转珠连在一起即可发动攻击！",
                                 true,
                                 [=]()
            {
                showGemHalo(Vec2i(5, 4));
                
                std::vector<int> path;
                path.push_back(29);
                path.push_back(28);
                this->showArrowPath(path,255);
                ArrowMask::shared()->setMask(CCRect(425, 425, 210, 105));
                touchContinue->setVisible(false);
                
                skipTouch = true;
            });
        }
        else if(mPhase == PHASE_GEM_COLOR_INTRO)
        {
            setGrid(CriticalAttackInitialGrid(), CriticalAttackDoneGrid());
            AlertMessage::create("",
                                 "不同颜色的转珠有相互的克制关系！",
                                 true,
                                 [=]()
            {
                showGemHalo(Vec2i(5, 4));
                std::vector<int> path;
                path.push_back(29);
                path.push_back(28);
                //ArrowMask::shared()->setArrow(ccp(530, 475), ArrowDirection::Left);
                this->showArrowPath(path,255);
                ArrowMask::shared()->setMask(CCRect(425, 425, 210, 105));
                skipTouch = true;
            })->attachImg(GetSprite("element_graph.png"));
        }
        else if(mPhase == PHASE_AREA_ATTACK_INTRO)
        {
            setGrid(AreaAttackInitalGrid(), AreaAttackDoneGrid());
            AlertMessage::create("",
                                 "消除5个同色转珠可发动[color=ffff0000]群体攻击[/color]！",
                                 true,
                                 [=]()
            {
                showGemHalo(Vec2i(5, 4));
                std::vector<int> path;
                path.push_back(29);
                path.push_back(28);
                //ArrowMask::shared()->setArrow(ccp(530, 475), ArrowDirection::Left);
                this->showArrowPath(path,255);
                ArrowMask::shared()->setMask(CCRect(425, 425, 210, 105));
                skipTouch = true;
            });
        }
        else if(mPhase == PHASE_COMBO_INTRO)
        {
            setGrid(ComboAttackInitialGrid(), ComboAttackDoneGrid());
            AlertMessage::create("",
                                 "同时消除多组同色转珠会产生[color=ffff0000]连击[/color]！连击越多，伤害越高！",
                                 true,
                                 [=]()
            {
                showGemHalo(Vec2i(5, 4));
                showGemHalo2(Vec2i(3, 4));
                
                std::vector<int> path;
                path.push_back(29);
                path.push_back(28);
                path.push_back(27);
                //ArrowMask::shared()->setArrow(ccp(530, 475), ArrowDirection::Left);
                this->showArrowPath(path,255);
                ArrowMask::shared()->setMask(CCRect(325, 425, 315, 105));
                skipTouch = true;
            });
        }
        else if(mPhase == PHASE_ENEMY_ATTACK_INTRO)
        {
            GemColorGrid begin = IdleInitialGrid();
            mTaskQueue.enqueue(mBoardControl->patchGemGrid(begin));
            finalStateConstraint.call = [](GemGrid& grid)->bool { return false; };
            
            AlertMessage::create("",
                                 "注意！敌人头顶上的数字表示[color=ffff0000]攻击回合[/color]，移动转珠将会消耗回合数。",
                                 true,
                                 [=]()
            {
                std::vector<int> path;
                path.push_back(29);
                path.push_back(28);
                //ArrowMask::shared()->setArrow(ccp(530, 475), ArrowDirection::Left);
                this->showArrowPath(path,255);
                ArrowMask::shared()->setMask(CCRect(425, 425, 315, 105));
                skipTouch = true;
            });
        }
        else if(mPhase == PHASE_HEAL_INTRO)
        {
            setGrid(HealInitialGrid(), HealDoneGrid());
            AlertMessage::create("",
                                 "在战斗中受到伤害，消除[color=ffff0000]恢复转珠[/color]可以回复生命！",
                                 true,
                                 [=]()
            {
                showGemHalo(Vec2i(5, 4));
                std::vector<int> path;
                path.push_back(29);
                path.push_back(28);
                //ArrowMask::shared()->setArrow(ccp(530, 475), ArrowDirection::Left);
                this->showArrowPath(path,255);
                ArrowMask::shared()->setMask(CCRect(425, 425, 315, 105));
                skipTouch = true;
            });
        }
        else if(mPhase == PHASE_USE_SKILL_INTRO)
        {
            AlertMessage::create("",
                                 "侠客拥有自己的独门绝技！\n当侠客[color=ffff0000]集气[/color]全满时，可以发动技能！",
                                 true,
                                 [=]()
            {
                mTaskQueue.enqueue(mPlayer->team[0]->setEnergy(mPlayer->team[0]->maxEnergy));
                
                mPlayer->team[0]->icon->enabledInTutorial = true;

                ArrowMask::shared()->clear();
                ArrowMask::shared()->setArrow(ccp(50, 515), ArrowDirection::Up);
//                ArrowMask::shared()->setMask(CCRect(0, 545, 106, 120));
                skipTouch = false;
            });
        }
        
        LOGD("Tutorial touch consumed ineffective touch event\n");
        return true;
    }
    
    void TutorialBoardLayer::ccTouchMoved(CCTouch* touch, CCEvent* event)
    {
        if(mPhase == PHASE_GEM_COLOR_INTRO ||
           mPhase == PHASE_MOVE_GEM_INTRO ||
           mPhase == PHASE_AREA_ATTACK_INTRO ||
           mPhase == PHASE_COMBO_INTRO ||
           mPhase == PHASE_ENEMY_ATTACK_INTRO ||
           mPhase == PHASE_HEAL_INTRO)
        {
            CCPoint p = ArrowMask::shared()->convertTouchToNodeSpace(touch);
            
            const CCRect& r = ArrowMask::shared()->getCutout();
            if(r.containsPoint(p))
                BoardLayer::ccTouchMoved(touch, event);
        }
        else if(mPhase == PHASE_END)
            BoardLayer::ccTouchMoved(touch, event);
    }
    
    void TutorialBoardLayer::ccTouchEnded(CCTouch* touch, CCEvent* event)
    {
        if(mPhase == PHASE_GEM_COLOR_INTRO ||
           mPhase == PHASE_MOVE_GEM_INTRO ||
           mPhase == PHASE_AREA_ATTACK_INTRO ||
           mPhase == PHASE_COMBO_INTRO ||
           mPhase == PHASE_ENEMY_ATTACK_INTRO ||
           mPhase == PHASE_HEAL_INTRO)
        {
            BoardLayer::ccTouchEnded(touch, event);
        }
        else if(mPhase == PHASE_END)
            BoardLayer::ccTouchEnded(touch, event);
    }
    
    void TutorialBoardLayer::ccTouchCancelled(CCTouch *touch, CCEvent *event)
    {
        BoardLayer::ccTouchCancelled(touch, event);
    }
    
    void TutorialBoardLayer::onStartTurn()
    {
        if(mPhase == PHASE_MOVE_GEM_INTRO ||
           mPhase == PHASE_GEM_COLOR_INTRO ||
           mPhase == PHASE_AREA_ATTACK_INTRO ||
           mPhase == PHASE_COMBO_INTRO ||
           mPhase == PHASE_HEAL_INTRO)
        {
            skipTouch = false;
            
            this->clearArrowPath();
            ArrowMask::shared()->clear();
            hideGemHalo();
            hideGemHalo2();
        }
    }
    
    void TutorialBoardLayer::afterEnemyAttack()
    {
        if(mPhase == PHASE_ENEMY_ATTACK_INTRO)
        {
            skipTouch = false;
            this->clearArrowPath();
            ArrowMask::shared()->clear();
            
            mPhase++;
            // This will be follwed by onEndOfTurn
        }
    }
    
    void TutorialBoardLayer::onAlertSkillCast()
    {
        ArrowMask::shared()->setArrow(ccp(250, 130), ArrowDirection::Up);
    }
    
    void TutorialBoardLayer::onAlertSkillCastCancelled()
    {
        ArrowMask::shared()->clear();
    }
    
    void TutorialBoardLayer::onSkillCast()
    {
        if(mPhase == PHASE_USE_SKILL_INTRO)
        {
            skipTouch = false;
            this->clearArrowPath();
            ArrowMask::shared()->clear();
            mPhase++;
        }
    }
    
    void TutorialBoardLayer::onLoadNextRound()
    {
    }
    
    bool TutorialBoardLayer::onPlayerDeath()
    {
        mTaskQueue.enqueue(healWithAnim(this, mPlayer.get(), mPlayer->getMaxHealth()));
        return true;
    };
    
    void TutorialBoardLayer::onEndOfTurn()
    {
        if(mPhase == PHASE_MOVE_GEM_INTRO ||
           mPhase == PHASE_GEM_COLOR_INTRO ||
           mPhase == PHASE_AREA_ATTACK_INTRO ||
           mPhase == PHASE_COMBO_INTRO ||
           mPhase == PHASE_HEAL_INTRO)
        {
            skipTouch = false;
            
            mPhase++;
            ccTouchBegan(NULL, NULL);
        }
        else if(mPhase == PHASE_ENEMY_ATTACK_DONE)
        {
            mPhase++;
            ccTouchBegan(NULL, NULL);
        }
    }
}
