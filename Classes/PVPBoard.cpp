//
//  PVPBoard.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 4/16/13.
//
//

#include "DebugMisc.h"
#include "PVPBoard.h"
#include "PuzzleRPC.h"
#include "GameScene.h"
#include "SkillContext.h"
#include "URLHelpers.h"
#include "memstream.h"
#include "sha1.h"
#include "ScreenConsole.h"
#include "ButtonSprite.h"
#include "PvPEndGame.h"
#include "PvPUtils.h"

#include <algorithm>
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>

namespace PH
{
//    static CCSpriteBatchNode* createBoardBackground(CCPoint p)
    static CCSpriteBatchNode* createBoardBackground(bool useSuite2)
    {
        std::string lightTile = useSuite2 ? "ui/qipan_qian_pvp.png" : "ui/qipan_qian.png";
        std::string darkTile = useSuite2 ? "ui/qipan_shen_pvp.png" : "ui/qipan_shen.png";
        
        CCSpriteBatchNode* bgBatch = CCSpriteBatchNode::create("battleMisc.png");
        
        // create board background
        // assuming battleMisc.png has both qipan tile textures
        for(int x = 0; x < BOARD_WIDTH; x++)
        {
            for(int y = 0; y < BOARD_HEIGHT; y++)
            {
                int index = x * BOARD_HEIGHT + y;
                int color = index % 2;
                
                CCSprite* tile = color == 0 ? GetSprite(lightTile) : GetSprite(darkTile);
                tile->setAnchorPoint(ccp(0, 0));
                tile->setPosition(g2w(Vec2i(x, y)));
                bgBatch->addChild(tile);
            }
        }
        
        return bgBatch;
    }
    
    //----------------------------------------------------
    // PVPBoardLayer
    bool PVPBoardLayer::init(const Player& p1, const Player& p2, const PvPConfig& config)
    {
        if(!CCLayer::init())
            return false;
        
        mConfig = config;
        
        this->scheduleUpdate();
        
        if(p1.uid == PlayerData::getInstance()->player->uid)
            initStaticUI(p1, p2);
        else
            initStaticUI(p2, p1);
        
        bool firstMove = p1.uid == PlayerData::getInstance()->player->uid;
        //std::cout<< firstMove << std::endl;
        if(firstMove)
        {
            mPlayer1 = createPlayerFromTeam(mBottomLayer, ccp(1, 80), p1, PlayerControl::PVP_SELF);
            mPlayer2 = createPlayerFromTeam(mTopLayer, ccp(108, 780), p2, PlayerControl::PVP_RIVAL);
            initPassiveSkill(mPlayer1, p1, mPlayer2);
            initPassiveSkill(mPlayer2, p2, mPlayer1);
        }
        else
        {
            mPlayer1 = createPlayerFromTeam(mBottomLayer, ccp(1, 80), p2, PlayerControl::PVP_SELF);
            mPlayer2 = createPlayerFromTeam(mTopLayer, ccp(108, 780), p1, PlayerControl::PVP_RIVAL);
            initPassiveSkill(mPlayer1, p2, mPlayer2);
            initPassiveSkill(mPlayer2, p1, mPlayer1);
        }
        
        mPlayer1->resist = mResist1;
        mPlayer2->resist = mResist2;
        
        LOGD("p1 health %f, p2 health %f\n", mPlayer1->getHealth(), mPlayer2->getHealth());
        

        
        LOGD("after passive skill, p1 health %f, p2 health %f\n", mPlayer1->getHealth(), mPlayer2->getHealth());

        // disable skills for player2
        for(HeroControl* hc : mPlayer2->team)
            hc->icon->setEnabled(false);
        
        mIsMyTurn = firstMove;
        // fill board for the first time. must be called last in init
        mBoardControl->fillBoardFirstTime([=](){ return randGemWeighted(); });
        
        this->disableUI();
        
        if(mIsMyTurn)
        {
            mResult = BoardResultPtr(new BoardResult(mPlayer1->team));

            mTaskQueue.enqueue( createStartAnimation() );
            mTaskQueue.enqueue(TaskLambda::make([=]()
            {
                mHourGlass1->start(mConfig.moveTimeout, [=](){} );
                this->enableUI();
            }));
        }
        else
        {
            mResult = BoardResultPtr(new BoardResult(mPlayer2->team));
            
            mTaskQueue.enqueue( createStartAnimation() );
            mTaskQueue.enqueue(TaskLambda::make([=]()
            {
                mHourGlass2->start(mConfig.moveTimeout, [=](){this->onOpponentTimeout();} );
            }));
        }
        
        PuzzleRPC::getInstance()->getPvP()->setInGameReceiver(this);
        
        {
            // must be delayed to after createStartAnimation
            mBoardWhiteMask = GetSprite("ui/1x1_w.png");
            mBoardWhiteMask->setAnchorPoint(ccp(0, 0));
            mBoardWhiteMask->setPosition(ccp(0, 211));
            mBoardWhiteMask->setScaleX(640 * contentScale());
            mBoardWhiteMask->setScaleY(535 * contentScale());
            mBoardWhiteMask->setOpacity(0);
            this->addChild(mBoardWhiteMask, ORDER_BACKGROUND+1);
            
            if(mIsMyTurn)
            {
                mTaskQueue.enqueue(TaskLambda::make([=]()
                {
                    startBoardBreathAnim();
                }));
            }
        }

        return true;
    }
    
    bool PVPBoardLayer::initWithRecover(int seed,
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
        if(!CCLayer::init())
            return false;
        
        /*
        this->scheduleUpdate();
//        this->schedule(schedule_selector(PVPBoardLayer::updateRival), 0.05);
        
        if(p1.uid == PlayerData::getInstance()->player->uid)
            initStaticUI(p1, p2);
        else
            initStaticUI(p2, p1);
        
        std::string jsonStr = URLHelper::write(battleState);
        LOGD("received battle state %s\n", jsonStr.c_str());
        
        // recreate players
        if(PlayerData::getInstance()->player->uid == p1.uid)
        {
            mPlayer1 = createPlayerFromTeam(mBottomLayer, ccp(1, 80), p1, PlayerControl::PVP_SELF);
            mPlayer2 = createPlayerFromTeam(mTopLayer, ccp(1, 780), p2, PlayerControl::PVP_RIVAL);
        }
        else
        {
            mPlayer1 = createPlayerFromTeam(mBottomLayer, ccp(1, 80), p2, PlayerControl::PVP_SELF);
            mPlayer2 = createPlayerFromTeam(mTopLayer, ccp(1, 780), p1, PlayerControl::PVP_RIVAL);
        }
        
        mPlayer1->resist = mResist1;
        mPlayer2->resist = mResist2;
        
        if(mPlayer1->uid < mPlayer2->uid)
        {
            //mPlayer1->recoverStates(battleState["player1"]);
            //mPlayer2->recoverStates(battleState["player2"]);
        }
        else
        {
            //mPlayer1->recoverStates(battleState["player2"]);
            //mPlayer2->recoverStates(battleState["player1"]);
        }
        
        std::string whoseTurn = battleState["whoseTurn"].GetString();
        mIsMyTurn = whoseTurn == mPlayer1->uid;
        
        if(mIsMyTurn)
        {
            mResult = BoardResultPtr(new BoardResult(mPlayer1->team));
            mHourGlass1->start(ROUND_TIMEOUT, [=](){});
            mTaskQueue.enqueue(TaskLambda::make([=](){ this->enableUI(); }));
        }
        else
        {
            mResult = BoardResultPtr(new BoardResult(mPlayer2->team));
            mHourGlass2->start(ROUND_TIMEOUT, [=](){this->onOpponentTimeout();} );
            mTaskQueue.enqueue(TaskLambda::make([=](){ this->disableUI(); }));
        }
        
        // reset timeout timers
        
        // Restore grid
        for(int i=0; i<mGemGrid.size(); i++)
        {
            using namespace rapidjson;
            
            const Value& v = battleState["grid"][i];
            GemUtils::GemColor color = (GemUtils::GemColor)v["color"].GetInt();
            float damage = v["damage"].GetInt();
            
            // BUG: not parsing bomb state
           // mGemGrid(i) = Gem::create(Vec2i(i % BOARD_WIDTH, i / BOARD_WIDTH), color, mGemLayer);
        }
         */
        
        return true;
    }
    
    void PVPBoardLayer::initStaticUI(const Player& p1, const Player& p2)
    {
        {
            mBG = GetSprite("pvp/pvp_bg.jpg");
            mBG->setAnchorPoint(ccp(0, 0));
            this->addChild(mBG, ORDER_BACKGROUND-1);
        }
        
        {
            mBoardControl = BoardControl::make();
            mBoardControl->root->setPosition(ccp(0, 211));
            this->addChild(mBoardControl->root, ORDER_BOARD);
            
            mGridArrow = GridArrow::create();
            mBoardControl->root->addChild(mGridArrow, 99999);
        }
        
        {
            mBoardBG = createBoardBackground(false);
            mBoardBG->setPosition(ccp(0, 211));
            mBoardBG->setAnchorPoint(ccp(0, 0));
            this->addChild(mBoardBG, ORDER_BACKGROUND);
            
            mBoardMask = GetSprite("ui/1x1_b.png");
            mBoardMask->setAnchorPoint(ccp(0, 0));
            mBoardMask->setScaleX(640 * contentScale());
            mBoardMask->setScaleY(535 * contentScale());
            mBoardMask->setPosition(ccp(0, 211));
            mBoardMask->setOpacity(0);
            this->addChild(mBoardMask, ORDER_MASK);
            
            mWaitAckText = CCLabelBMFont::create("等待敌方行动...", "bmfont/wait_enemy_move.fnt");
            mWaitAckText->setPosition(ccp(320, 480));
            mWaitAckText->setOpacity(0);
            this->addChild(mWaitAckText, ORDER_FLOAT);
        }
        
        // P2 basic info
        {
            CCLayer* topLayer = CCLayer::create();
            this->addChild(topLayer, ORDER_HERO_ICON);
            
            CCSprite* theirsDashboard = GetSprite("pvp_dashboard_theirs.png");
            theirsDashboard->setAnchorPoint(ccp(0.0f, 1.0f));
            theirsDashboard->setPosition(ccp(0, 960));
            topLayer->addChild(theirsDashboard);
            
            LabelX* name = LabelX::create(p2.name.c_str(), FONT_CN, 24);
            name->setAnchorPoint(ccp(0, 0));
            name->setPosition(ccp(40, 926));
            topLayer->addChild(name);
            
            CCLabelBMFont* level = CCLabelBMFont::create(toStr(p2.level).c_str(),
                                                         "bmfont/Sumo_22_yellow.fnt");
            level->setAnchorPoint(ccp(0, 0));
            level->setPosition(ccp(253, 926));
            level->setScale(contentScale());
            topLayer->addChild(level);
            
            LabelX* title = LabelX::create(getTitleOfRepu(p2.pvpReputation).c_str(), FONT_CN, 24);
            title->setAnchorPoint(ccp(0, 0));
            title->setPosition(ccp(367, 926));
            topLayer->addChild(title);
            
            mHourGlass2 = PvPHourGlass::create();
            mHourGlass2->setPosition(ccp(54, 830));
            topLayer->addChild(mHourGlass2);
            
            std::vector<float> resistance = calcResistance(p2.team);
            for(int i=0; i<GemUtils::Health; i++)
            {
                float res = resistance[i];
                CCLabelBMFont* l = CCLabelBMFont::create(toStr((int)(resistance[i] * 100 + 100)).c_str(),
                                                         GemUtils::resistanceFont((GemUtils::GemColor)i));
                l->setScale(contentScale());
                l->setAnchorPoint(ccp(0, 0));
                
                int i2 = i;
                if(i == 1)
                    i2 = 2;
                else if(i == 2)
                    i2 = 1;
                else if(i == 3)
                    i2 = 4;
                else if(i == 4)
                    i2 = 3;
                
                l->setPosition(ccp(68.0f + i2*88.5f, 891.0f));
                topLayer->addChild(l);
                
                mResist2.val[i] = res;
                mResist2.label[i] = l;
            }
            
            mSmallTurnLabel = CCLabelBMFont::create("30/30", "bmfont/Sumo_22_yellow.fnt");
            mSmallTurnLabel->setPosition(ccp(535, 30));
            theirsDashboard->addChild(mSmallTurnLabel);
            
            mTopLayer = topLayer;
        }
        
        // P1 basic info
        {
            CCLayer* bottomLayer = CCLayer::create();
            this->addChild(bottomLayer, ORDER_HERO_ICON);
            
            CCSprite* oursDashboard = GetSprite("pvp_dashboard_ours.png");
            oursDashboard->setAnchorPoint(ccp(0, 0));
            oursDashboard->setPosition(ccp(0, 0));
            bottomLayer->addChild(oursDashboard);
            
            LabelX* name = LabelX::create(p1.name.c_str(), FONT_CN, 24);
            name->setAnchorPoint(ccp(0, 0));
            name->setPosition(ccp(40, 38));
            bottomLayer->addChild(name);
            
            CCLabelBMFont* level = CCLabelBMFont::create(toStr(p1.level).c_str(),
                                                         "bmfont/Sumo_22_yellow.fnt");
            level->setAnchorPoint(ccp(0, 0));
            level->setPosition(ccp(253, 38));
            level->setScale(contentScale());
            bottomLayer->addChild(level);
            
            LabelX* title = LabelX::create(getTitleOfRepu(p1.pvpReputation).c_str(), FONT_CN, 24);
            title->setAnchorPoint(ccp(0, 0));
            title->setPosition(ccp(367, 38));
            bottomLayer->addChild(title);
            
            mHourGlass1 = PvPHourGlass::create();
            mHourGlass1->setPosition(ccp(580, 130));
            bottomLayer->addChild(mHourGlass1);
            
            std::vector<float> resistance = calcResistance(p1.team);
            for(int i=0; i<GemUtils::Health; i++)
            {
                float res = resistance[i];
                CCLabelBMFont* l = CCLabelBMFont::create(toStr((int)(resistance[i] * 100 + 100)).c_str(),
                                                         GemUtils::resistanceFont((GemUtils::GemColor)i));
                l->setScale(contentScale());
                int i2 = i;
                if(i == 1)
                    i2 = 2;
                else if(i == 2)
                    i2 = 1;
                else if(i == 3)
                    i2 = 4;
                else if(i == 4)
                    i2 = 3;
                
                l->setAnchorPoint(ccp(0, 0));
                l->setPosition(ccp(68.0f + i2*88.5f, 4.0f));
                bottomLayer->addChild(l);
                
                mResist1.val[i] = res;
                mResist1.label[i] = l;
            }

            mBottomLayer = bottomLayer;
        }
        
        {
            ButtonSprite* escape = ButtonSprite::create(createBtn("逃跑", ButtonType::NORMAL_RED),
                                                        createBtn("逃跑", ButtonType::NORMAL_RED));
            escape->setOnClickListener([=](){ this->quitMatch(); });
            escape->setAnchorPoint(ccp(0.5f, 0.5f));
            escape->setPosition(ccp(542, 34));
            mBottomLayer->addChild(escape, 1);
        }
    }
    
    static CCSequence* showFadeRemove(CCSprite* s, float scale, float time)
    {
        return CCSequence::create(CCShow::create(),
                                  CCSpawn::create(CCScaleTo::create(time, scale),
                                                  CCFadeOut::create(time),
                                                  NULL),
                                  CCCallLambda::create([=]{s->removeFromParentAndCleanup(true);}),
                                  NULL);
    }
    
    // This might be slow.
    static TaskPtr fadeInNodes(CCNode* root, float time)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        CCSprite* s = dynamic_cast<CCSprite*>(root);
        if(s)
        {
            s->setOpacity(0);
            batch << TaskAnim::make(s, CCFadeIn::create(time));
        }
        
        for(int i=0; i<root->getChildrenCount(); i++)
        {
            CCNode* n = static_cast<CCNode*>(root->getChildren()->objectAtIndex(i));
            batch << fadeInNodes(n, time);
        }
        
        return batch;
    }
    
    
    static void runOnSprites(CCNode* root, boost::function<void(CCSprite*)> func)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        CCSprite* s = dynamic_cast<CCSprite*>(root);
        if(s)
            func(s);
        
        for(int i=0; i<root->getChildrenCount(); i++)
        {
            CCNode* n = static_cast<CCNode*>(root->getChildren()->objectAtIndex(i));
            runOnSprites(n, func);
        }
    }
    
    void PVPBoardLayer::startBoardBreathAnim()
    {
        /*
        runOnSprites(mBoardBG, [=](CCSprite* s)
        {
            CCRepeatForever* repeat = CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.7, 80),
                                                                                 CCFadeTo::create(0.7, 255),
                                                                                 NULL));
            s->runAction(repeat);
        });
         */
        CCRepeatForever* repeat = CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.6, 40),
                                                                             CCFadeTo::create(0.7, 0),
                                                                             NULL));
        mBoardWhiteMask->runAction(repeat);
    }
    
    void PVPBoardLayer::stopBoardBreathAnim()
    {
        /*
        runOnSprites(mBoardBG, [=](CCSprite* s)
        {
            s->stopAllActions();
            s->runAction(CCFadeTo::create(0.3, 255));
        });
         */
        mBoardWhiteMask->stopAllActions();
        mBoardWhiteMask->setOpacity(0);
    }
    
    std::string PVPBoardLayer::turnStr() const
    {
        return boost::lexical_cast<std::string>(mConfig.maxRound - mTurn/2) + "/" + boost::lexical_cast<std::string>(mConfig.maxRound);
    }
    
    TaskPtr PVPBoardLayer::createStartAnimation()
    {
        TaskSequencePtr seq = TaskSequence::make();
        
        {
            std::string countdowns[] = {"pvp_cd1.png", "pvp_cd2.png", "pvp_cd3.png"};
            for(int i=0; i<3; i++)
            {
                CCSprite* s = GetSprite(countdowns[i]);
                s->setPosition(ccp(320, 480));
                s->setVisible(false);
                this->addChild(s);
                seq << TaskAnim::make(s, showFadeRemove(s, 1.6f, 0.4f));
            }
        }
        
        TaskBatchPtr bb = TaskBatch::make();
        {
            CCSprite* v = GetSprite("pvp_v.png");
            v->setAnchorPoint(ccp(0.9f, 0.4f));
            v->setPosition(ccp(0, 480));
            v->setVisible(false);
            this->addChild(v);
            
            CCSprite* s = GetSprite("pvp_s.png");
            s->setAnchorPoint(ccp(0.1f, 0.6f));
            s->setPosition(ccp(640, 480));
            s->setVisible(false);
            this->addChild(s);
            
            bb << TaskAnim::make(v, CCSequence::create(CCShow::create(),
                                                       CCMoveTo::create(0.25f, ccp(320, 480)),
                                                       CCFadeOut::create(0.4f),
                                                       NULL));
            bb << TaskAnim::make(s, CCSequence::create(CCShow::create(),
                                                       CCMoveTo::create(0.25f, ccp(320, 480)),
                                                       CCFadeOut::create(0.4f),
                                                       NULL));
            
            // top/bottom dashboard animations
            {
                TaskBatchPtr batch = TaskBatch::make();
                
                mTopLayer->setPosition(ccp(0, 220));
                batch << TaskAnim::make(mTopLayer, CCMoveTo::create(0.4f, ccp(0, 0)));
                
                mBottomLayer->setPosition(ccp(0, -220));
                batch << TaskAnim::make(mBottomLayer, CCMoveTo::create(0.4f, ccp(0, 0)));
                
                bb << TaskSequence::make(createDelay(this, 0.25f), batch);
            }
            
            // gem animations
            {
                TaskBatchPtr batch = TaskBatch::make();
                batch << fadeInNodes(mBoardControl->root, 0.5f);
                batch << fadeInNodes(mBoardBG,  0.5f);
                
                bb << TaskSequence::make(createDelay(this, 0.5f), batch);
            }
            
            {
                bb << TaskAnim::make(mBG, CCFadeTo::create(1.0f, 100));
            }
            
            seq << bb;
        }
        
        {
            mRoundLabelUs = GetSprite("pvp_firsthand_ours.png");
            mRoundLabelUs->setPosition(ccp(1000, 480));
            this->addChild(mRoundLabelUs, ORDER_FLOAT);
            
            mBigTurnLabelOurs = CCLabelBMFont::create(turnStr().c_str(), "bmfont/Sumo_36_yellow_turn.fnt");
            mBigTurnLabelOurs->setAnchorPoint(ccp(0, 0));
            mBigTurnLabelOurs->setPosition(ccp(180, 0));
            mRoundLabelUs->addChild(mBigTurnLabelOurs);
        }
        
        {
            mRoundLabelThem = GetSprite("pvp_firsthand_theirs.png");
            mRoundLabelThem->setPosition(ccp(1000, 480));
            this->addChild(mRoundLabelThem, ORDER_FLOAT);
            
            mBigTurnLabelTheirs = CCLabelBMFont::create(turnStr().c_str(), "bmfont/Sumo_36_yellow_turn.fnt");
            mBigTurnLabelTheirs->setAnchorPoint(ccp(0, 0));
            mBigTurnLabelTheirs->setPosition(ccp(180, 0));
            mRoundLabelThem->addChild(mBigTurnLabelTheirs);
        }
        
        {
            CCSprite* s = mIsMyTurn ? mRoundLabelUs : mRoundLabelThem;
            CCFiniteTimeAction* splashAction =
            CCSequence::create(CCEaseElasticOut::create(CCMoveTo::create(0.4, ccp(320, 480)), 0.8f),
                               CCEaseElasticIn::create(CCMoveTo::create(0.4, ccp(-160, 480)), 0.8f),
                               NULL);
            seq << TaskAnim::make(s, splashAction);
        }

        return seq;
    }
    
    void PVPBoardLayer::disableUI()
    {
        this->setTouchEnabled(false);
        for(HeroControl* hc : mPlayer1->team)
            hc->icon->setEnabled(false);
    }
    
    void PVPBoardLayer::enableUI()
    {
        this->setTouchEnabled(true);
        for(HeroControl* hc : mPlayer1->team)
            hc->icon->setEnabled(true);
    }
    
    void PVPBoardLayer::onSelfTimeout()
    {
        onTimeOut(); // same behavior as server notifying us timeout
    }
    
    void PVPBoardLayer::onOpponentTimeout()
    {
        PuzzleRPC::getInstance()->getPvP()->sendRivalShallTimeout();
    }
    
    void PVPBoardLayer::quitMatch()
    {
        SystemInform::alertConfirmPanel(this,
                                        "是否离开战斗？逃跑会被判定为",
                                        "逃跑",
                                        [=]()
                                        {
                                            PuzzleRPC::getInstance()->getPvP()->sendQuitMatch();
                                            // wait for onGameEnd
                                            mLoading = SystemInform::alertLoading(this);;
                                        },
                                        [](){});
    }
    
    PlayerControlPtr PVPBoardLayer::createPlayerFromTeam(CCLayer* parent,
                                                         const CCPoint& p,
                                                         const Player& pp,
                                                         PlayerControl::Style style)
    {
        // Create and layout player related data and UI
        PlayerControlPtr player = boost::make_shared<PlayerControl>(p, style);
        
        player->uid = pp.uid;
        
        int health = 0;
        
        for(int i=0; i<pp.team.size(); i++)
        {
            if(pp.team[i] == NULL)
                continue;
            
            Hero hhx = *pp.team[i];
            hhx.regen = int(hhx.regen * mConfig.healthFactor / 5.0f);
            
            // make a copy of hero
            Hero h = boostHeroStatsByRelations(hhx, pp.team);
            
            float xbase = p.x + 106*i;
            float ybase = p.y;
            CCPoint p = ccp(xbase+4, ybase+4);
            
            HeroControl* hc = new HeroControl(h, p, true);
            hc->setOnClick([=](HeroIconItem* item)
            {
                const SkillInfo& skill = AST(hc->info.profile->pvpActiveSkillId);
                if(hc->isSkillReady() || SKILL_NEED_NO_ENERGY)
                {
                    SystemInform::alertSkill(this,
                                             skill.name,
                                             skill.desc,
                                             toStr(hc->maxEnergyPvP),
                                             CCPoint(320, 300),
                                             [=]()
                                             {
                                                 // if no longer in player's turn
                                                 if(!mIsMyTurn)
                                                     return;
                                                 
                                                 int i =0;
                                                 for(; i<player->team.size(); i++)
                                                     if(player->team[i] == hc)
                                                         break;

                                                 PuzzleRPC::getInstance()->getPvP()->sendCastSkill(i);
                                                 
                                                 // no ui interaction duration skill cast animations
                                                 this->disableUI();
                                                 castHeroSkill(item);
                                                 
                                                 if(mPlayer2->isDead())
                                                 {
                                                     LOGD("skill cast. EndRound win\n");
//                                                     endDrag();
                                                     this->startTurn();
                                                 }
                                                 else if(mPlayer1->isDead()) // something happened that player killed himself.
                                                 {
                                                     LOGD("skill cast. EndRound loss\n");
//                                                     endDrag();
                                                     this->startTurn();
                                                 }
                                                 else
                                                     mTaskQueue.enqueue(TaskLambda::make( [=](){this->enableUI();} ));
                                             },
                                             [](){});
                }
            });
            player->menu->addChild(hc->icon);
            
            health += hc->health;
            player->team.push_back(hc);
        }
        // set normal health
        player->setMaxHealthDirectly(health, true);
        
        // Initialize max health. This might be changed by passive skills
        parent->addChild(player->healthBox->get());
        
        // connect same skills
        for(HeroControl* hc : player->team)
        {
            player->skillChain[hc->info.profile->pvpActiveSkillId].add(hc);
        }
        
        // connect menu
        parent->addChild(player->menu, ORDER_HERO_ICON);
        
        // connect timer box
        parent->addChild(player->moveTimer.box, ORDER_FLOAT);
        
        return player;
    }
    
    void PVPBoardLayer::initPassiveSkill(PlayerControlPtr player, const Player& pp, PlayerControlPtr other)
    {
        // Load player passive skills.
        // this may change any already initialized properties.
        if(player->team.size() > 0)
        {
            SkillContext::invokePvPPassive(this,
                                           player,
                                           other,
                                           player->team.front());
        }
        
        // Adjust player health by a factor.
        // healthSum * baseFactor * (1 + level / (level + baseLevel))
        float adjustedMaxHealth = player->getMaxHealth() * mConfig.healthFactor * (1.0f + (float)pp.level / (pp.level + mConfig.healthBase));
        player->setMaxHealthDirectly(adjustedMaxHealth, true);
    }
    
    void PVPBoardLayer::castHeroSkill(CCObject* sender)
    {
        HeroControl* hc = ((HeroIconItem*)sender)->getHeroControl();
        
        auto batch = TaskBatch::make();
        if( hc->buffGroup.invokeBuff("active.blind", batch) )
        {
            mTaskQueue.enqueue(batch);
            return;
        }
        
        if(hc->isSkillReady() || SKILL_NEED_NO_ENERGY)
        {
            TaskSequencePtr seq = TaskSequence::make();
            
            auto energyBatch = TaskBatch::make();
            float energy = hc->energy;
            // use up all energy
            if( this->getAttacker()->buffGroup.invokeBuff("reduceEnergyCost", energyBatch) )
            {
                auto buff = this->getAttacker()->buffGroup.getBuffContent<FloatBuff>("reduceEnergyCost");
                energy *= (std::min(buff->d1, 1.f));
                this->getAttacker()->buffGroup.updateBuff("reduceEnergyCost");
            }else
                energy = 0;
            
            energyBatch << hc->setEnergy(energy);
            seq << energyBatch;
            
            // no ui interaction duration skill cast animations
            this->disableUI();
        
            CCFiniteTimeAction* jump = createIconJumpSequence(hc->iconPositionFixed, 0.0f);
            seq << TaskAnim::make(hc->icon, jump, false);
            
            PlayerControlPtr attacker = getAttacker();
            attacker->skillChain[hc->info.profile->pvpActiveSkillId].startCooldown();
            seq << SkillContext::invokePvP(this, hc, mTurn);
            
            mTaskQueue.enqueue(seq);
        }
        else
        {
            LOGD("skill not ready or hero is not allowed to use skill, %.2f/%f\n",
                 hc->energy,
                 hc->maxEnergy);
        }
    }
    
    void PVPBoardLayer::toJson(rapidjson::Value& o,
                               rapidjson::Document::AllocatorType& alloc)
    {
        using namespace rapidjson;
        
        // mark version
        o.AddMember("version", PH_VERSION, alloc);
        
        // turn
        o.AddMember("turn", mTurn, alloc);
        
        // players
        Value p1(kObjectType);
        mPlayer1->vSerialize(p1, alloc);
        
        Value p2(kObjectType);
        mPlayer2->vSerialize(p2, alloc);
        
        std::string whoseTurn = mIsMyTurn ? mPlayer1->uid : mPlayer2->uid;
        o.AddMember("whoseTurn", whoseTurn, alloc);
        
        // make sure that first move player is always player1
        if(mPlayer1->uid < mPlayer2->uid)
        {
            o.AddMember("player1", p1, alloc);
            o.AddMember("player2", p2, alloc);
        }
        else
        {
            o.AddMember("player1", p2, alloc);
            o.AddMember("player2", p1, alloc);
        }
        
        // grid info
        Value gridJson(kArrayType);
        mBoardControl->vSerialize(gridJson, alloc);
        o.AddMember("grid", gridJson, alloc);
        
        std::string randStr = PvPRand::toStr();
        std::string randStr64 = base64_encode((unsigned char const*)randStr.c_str(),
                                              randStr.size());

        o.AddMember("randState", randStr64, alloc);
    }
    
    bool PVPBoardLayer::onRivalBoardMove(int srcx, int srcy, int dstx, int dsty)
    {
        Vec2i src(srcx, srcy);
        Vec2i dst(dstx, dsty);
        TaskBatchPtr batch = TaskBatch::make();
        TaskPtr p1 = moveGemAnimSlow(mBoardControl->get(src)->root, dst);
        TaskPtr p2 = moveGemAnimSlow(mBoardControl->get(dst)->root, src);
        
        batch << p1;
        batch << p2;
        batch << TaskSound::make("sound/v/drag_gem.mp3");

        mBoardControl->swapGem(src, dst);
        
        mTaskQueue.enqueue(batch);
       
        /*
        // clamp src with prev dst
        if(mGridArrow->size())
            mGridArrow->addPoint(dst);
        else
        {
            mGridArrow->addPoint(src);
            mGridArrow->addPoint(dst);
        }*/
        
        return true;
    }
    
    bool PVPBoardLayer::onRivalCastSkill(int heroSlotId)
    {
        LOGD("rival hero slot id %d\n", heroSlotId);
        castHeroSkill(getAttacker()->team[heroSlotId]->icon);

        return true;
    }
    
    bool PVPBoardLayer::onRivalEndRound(std::string stateHash, PH::PvPEndGame victory)
    {
        mGridArrow->clear();
        
        // begin calculating current turn.
        this->startTurn();
    
        // compute and verify state hashes
        std::string remoteHash = stateHash;
        
        rapidjson::Document doc;
        doc.SetObject();
        this->toJson(doc, doc.GetAllocator());
        
        std::string localHash = URLHelper::sha1FromJson(doc);
        
        if(remoteHash != localHash)
        {
            LOGD("!!! Out of sync error:\n");
            LOGD("\tlocal remote hash %s/%s\n", localHash.c_str(), remoteHash.c_str());
            
            std::string localState = URLHelper::write(doc).c_str();
            LOGD("\tlocal state: %s\n", localState.c_str());
            LOGD("\tstill send ack for server resolution\n");
            
            PuzzleRPC::getInstance()->getPvP()->sendAckEndRound(this, PvPEndGame::NONE);
        }
        else
        {
            LOGD("\tRemote/local hashes agree. Game is in sync\n");
            if(mPlayer2->isDead() && victory != PvPEndGame::NONE)
            {
                LOGD("AckEndRound win\n");
                PuzzleRPC::getInstance()->getPvP()->sendAckEndRound(this, PvPEndGame::WIN);
            }
            else if(mPlayer1->isDead() && victory != PvPEndGame::NONE)
            {
                LOGD("AckEndRound loss\n");
                PuzzleRPC::getInstance()->getPvP()->sendAckEndRound(this, PvPEndGame::LOSS);
            }
            else
            {
                PuzzleRPC::getInstance()->getPvP()->sendAckEndRound(this, PvPEndGame::NONE);
            }
        }
        return true;
    }
    
    bool PVPBoardLayer::onGameEnd(PH::PvPEndGame victory, int reputationGain, int scoreGain)
    {
        // in case we encounter a delay
        if(mLoading)
        {
            mLoading->cancel();
            mLoading = NULL;
        }
        
        if(mWaitReconnect)
        {
            this->unschedule(schedule_selector(PVPBoardLayer::updateReconnectTimeout));
            mWaitReconnect->cancel();
            mWaitReconnect = NULL;
        }
        
        LOGD("GameEnd now, victory status %d, reputation +%d, score +%d\n",
             victory, reputationGain, scoreGain);
        
        if(victory == PvPEndGame::WIN)
        {
            TaskPtr endAnim = createVictoryAnim(this, ccp(320, 480));
            mTaskQueue.enqueue(endAnim);
        }
        else if(victory == PvPEndGame::LOSS)
        {
            TaskPtr endAnim = createDefeatAnim(this, ccp(320, 480));
            mTaskQueue.enqueue(endAnim);
        }
        else
            ; // do nothing on tie
        
        mTaskQueue.enqueue(TaskLambda::make([=]()
        {
            transitionToGameScene(victory,
                                  reputationGain,
                                  scoreGain,
                                  mTurn);
        }));
        
        return true;
    }
    
    void PVPBoardLayer::updateReconnectTimeout(float dt)
    {
        mReconnectTimeout--;
        mReconnectTimeout = std::max(0, mReconnectTimeout);
        mWaitReconnect->setMessage("正在等待对手重连...\n" + boost::lexical_cast<std::string>(mReconnectTimeout) + "秒");
    }
    
    bool PVPBoardLayer::onRivalDisconnect(int timeout)
    {
        mReconnectTimeout = timeout;
        mWaitReconnect = LoadingDialog::create(runningScene(),
                                               "对手掉线",
                                               "正在等待对手重连...\n" + boost::lexical_cast<std::string>(mReconnectTimeout) + "秒");
        this->schedule(schedule_selector(PVPBoardLayer::updateReconnectTimeout), 1.0f);
        return true;
    }
    
    bool PVPBoardLayer::onRivalReconnect()
    {
        if(mWaitReconnect)
        {
            mWaitReconnect->cancel();
            mWaitReconnect = NULL;
            this->unschedule(schedule_selector(PVPBoardLayer::updateReconnectTimeout));
        }
        return true;
    }
    
    bool PVPBoardLayer::onAckEndRound()
    {
        return true;
    }
    
    bool PVPBoardLayer::onTimeOut()
    {
        LOGD("self timeout detected. need to end round now.\n");
        if(mWaitingForACK)
        {
            LOGD("EndRound already sent, ignore timeout request.\n");
        }
        else
        {
            endDrag();
        }
        return true;
    }
    
    bool PVPBoardLayer::onRivalQuit()
    {
        LOGD("onRivalQuit not handled\n");
        return true;
    }
    
    bool PVPBoardLayer::onCleanStart()
    {        
        mTaskQueue.enqueue(TaskLambda::make([=]()
        {
            transitionToGameScene(PvPEndGame::LOSS, 0, 0, 0);
        }));
        return true;
    }
    
    static SystemInform* whenDisconnect = NULL;
    
    bool PVPBoardLayer::onDisconnect()
    {
        LOGD("inGame: server disconnected\n");
        if(!whenDisconnect)
        {
            whenDisconnect = SystemInform::alertLoading(runningScene());
            mHourGlass1->pause();
            mHourGlass2->pause();
        }
        else
            LOGD("already displaying loading...");
        
        return true;
    }
    
    bool PVPBoardLayer::onConnect()
    {
        if(whenDisconnect)
        {
            whenDisconnect->cancel();
            whenDisconnect = NULL;
            
            mHourGlass1->resume();
            mHourGlass2->resume();
        }
        return true;
    }
    
    bool PVPBoardLayer::onError(const std::string& error)
    {
        mTaskQueue.enqueue(TaskLambda::make([=]()
        {
            SystemInform::alertMessage(runningScene(),
                                       ("客户端状态不同步，确定后退出对战: " + error).c_str(),
                                       "同步错误",
                                       [=](){ this->transitionToGameScene(PvPEndGame::TIE, 0, 0, 0); });
        }));
        return true;
    }
    
    void PVPBoardLayer::attack(PlayerControlPtr attacker,
                               PlayerControlPtr defender,
                               const BoardResult& result)
    {
        // Handle normal attack stuff
        {
            AttackTable attackTable(attacker->team);
            mTaskQueue.enqueue( applyComboModWithAnim(attacker,
                                                      result,
                                                      attackTable) );
            
            AttackTable finalTable(attacker->team);
            mTaskQueue.enqueue( applySkillModWithAnim(attacker,
                                                      attackTable,
                                                      finalTable) );

            mTaskQueue.enqueue( attackWithAnim(attacker,
                                               defender,
                                               finalTable) );
            
            mTaskQueue.enqueue( healWithAnim(this,
                                             attacker.get(),
                                             finalTable.heal,
                                             false,
                                             finalTable.healLabel,
                                             true) );
        }
        
        // Handle follow-up attacks
        if(attacker->passiveFollowUpAttacks.isValid() && defender->isAlive())
        {
            TaskBatchPtr numberBatch = TaskBatch::make();
            
            AttackTable attackTable(attacker->team);
            for(auto atk : attacker->passiveFollowUpAttacks.map())
            {
                HeroControl* hc = atk.first;
                float factor = atk.second.second;
                
                if( hc->buffGroup.invokeBuff("active.blind", numberBatch) )
                    continue;
                
                float attackFactor;
                numberBatch << attacker->getAttackFactor(hc, attackFactor, [](){ return PvPRand::rand(); });
                
                attackTable(hc).attack = hc->attack * (factor * attackFactor);
                attackTable(hc).isArea = atk.second.first;
                attackTable(hc).attackLabel = hc->createAttackLabel();
                
                CCFiniteTimeAction* count = createNumberAnimSeq(0,
                                                                attackTable(hc).attack,
                                                                NUMBER_DURATION);
                
                numberBatch << TaskAnim::make(attackTable(hc).attackLabel,
                                              CCSpawn::create(CCFadeIn::create(0.1f),
                                                              count,
                                                              NULL));
            }
            mTaskQueue.enqueue(numberBatch);
            
            mTaskQueue.enqueue( attackWithAnim(attacker, defender, attackTable) );
        }
    }
    
    TaskPtr PVPBoardLayer::applyComboModWithAnim(PlayerControlPtr player,
                                                 const BoardResult& result,
                                                 AttackTable& attackTable)
    {
        if(result.comboCount() == 0)
            return TaskEmpty::make();
        
        attackTable = result.baseAttack;
        
        TaskSequencePtr numberSeq = TaskSequence::make();
        
        // calculate combo bonus and play animations
        // accordingly do not count combo 1
        float comboFactor = 0.2f;
        numberSeq << player->getComboFactor(comboFactor, true);
        
        for(int i=1; i<player->minComboCount + result.comboCount(); i++)
        {
            TaskBatchPtr numberBatch = TaskBatch::make();
            for(HeroControl* hc : player->team)
            {
                // skip hero with base attack 0.
                if(attackTable(hc).attack == 0.0f)
                    continue;
                
                float from = attackTable(hc).attack;
                // combo adds damage, not mutiply
                float to = from + result.baseAttack(hc).attack * 0.20f;
                
                attackTable(hc).attack = to;
                attackTable(hc).isArea = attackTable(hc).isArea || isAreaAttack(result,
                                                                                player,
                                                                                hc->getColor());
                
                CCFiniteTimeAction* count = createNumberAnimSeq(from, to, NUMBER_DURATION);
                numberBatch << TaskAnim::make(attackTable(hc).attackLabel, count);
            }
            
            if(result.baseAttack.heal > 0.0f && player->isHurt())
            {
                float from = attackTable.heal;
                attackTable.heal = from + result.baseAttack.heal * 0.20;
                
                CCFiniteTimeAction* count = createNumberAnimSeq(from, attackTable.heal, NUMBER_DURATION);
                numberBatch << TaskAnim::make(attackTable.healLabel, count);
            }
            
            if(i >= player->minComboCount)
            {
                CCSprite* comboText = result.comboTextList[i-player->minComboCount];
                numberBatch << createComboTextFadeRemove(comboText, i);
            }
            
            // only play sound when we have an animation to play
            if(numberBatch->size())
            {
                numberBatch << TaskSound::make("sound/v/number_charge.mp3");
                numberSeq << numberBatch;
            }
        }
        
        float forceArea = false;
        if( player->buffGroup.invokeBuff("active.force.area.attack", numberSeq) )
            forceArea = true;
        
        for(HeroControl* hc : player->team)
        {
            attackTable(hc).isArea = attackTable(hc).isArea || isAreaAttack(result, player, hc->getColor()) || forceArea;
        }
        
        return numberSeq;
    }
    
    TaskPtr PVPBoardLayer::applySkillModWithAnim(PlayerControlPtr player,
                                                 const AttackTable& baseTable,
                                                 AttackTable& finalTable)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        bool doNumberCharge = false;
        
        finalTable = baseTable;
        
        for(HeroControl* hc : player->team)
        {
            // skip heroes who don't cause damage
            if(baseTable(hc).attack == 0.0f)
                continue;
            
            finalTable(hc).isArea = baseTable(hc).isArea;
            
            // apply the consolidated attack factor
            float skillAttackFactor;
            batch << player->getAttackFactor(hc, skillAttackFactor, [](){ return PvPRand::rand(); });
            
            if(player->passiveForceAreaAttackAndFactor.isValid())
            {
                finalTable(hc).isArea = true;
                skillAttackFactor *= maxPP(player->passiveForceAreaAttackAndFactor);
            }
            
            if(skillAttackFactor != 1.0f)
            {
                finalTable(hc).attack = baseTable(hc).attack * skillAttackFactor;
                
                // attack combo bonus
                TaskSequencePtr seq = TaskSequence::make();
                
                CCFiniteTimeAction* count = createNumberAnimSeq(baseTable(hc).attack,
                                                                finalTable(hc).attack,
                                                                NUMBER_DURATION);
                if(!finalTable(hc).attackLabel)
                {
                    batch << TaskAnim::make(finalTable(hc).attackLabel,
                                            CCSpawn::create(CCFadeTo::create(0.2, 255),
                                                            count,
                                                            NULL));
                }
                else
                {
                    batch << TaskAnim::make(finalTable(hc).attackLabel, count);
                }
                doNumberCharge = true;
            }
        }
        
        if(doNumberCharge)
            batch << TaskSound::make("sound/v/number_charge.mp3");
        
        return batch;
    }
    
    TaskPtr PVPBoardLayer::attackWithAnim(PlayerControlPtr attacker,
                                          PlayerControlPtr defender,
                                          const AttackTable& attackTable)
    {
        TaskBatchPtr batch = TaskBatch::make();
        
        float delay = 0.0f;
        
        float totalDamage = 0.0f;
        
        for(HeroControl* hc : attacker->team)
        {
            if(attackTable(hc).attackLabel != NULL)
            {
                // fade and remove attack counter
                CCFiniteTimeAction* seq = CCSequence::create(CCDelayTime::create(0.8f),
                                                             CCFadeOut::create(0.35f),
                                                             CCCallLambda::create(removeGen(attackTable(hc).attackLabel)),
                                                             NULL);
                batch << TaskAnim::make(attackTable(hc).attackLabel, seq);
            }
            
            // skip if no attack
            if(attackTable(hc).attack == 0.0f)
            {
                phassert(attackTable(hc).attackLabel == NULL &&
                         "attackLabel exists but attack is 0, attackLabel is never cleaned up");
                continue;
            }
            phassert(attackTable(hc).attackLabel != NULL && "attackLabel is NULL");
            
            // skip if blinded
            if( hc->buffGroup.invokeBuff("active.blind", batch) )
                continue;
            
            {
                // hero icon jump
                CCFiniteTimeAction* jump = createIconJumpSequence(hc->iconPositionFixed,
                                                                  delay);
                batch << TaskAnim::make(hc->icon, jump);
            }
            
            {
                float xoffset = randf(-80, 80);
                CCPoint cc(xoffset + defender->center().x, defender->center().y);
                batch << createOrb(this,
    //                               attacker->center(),
                                   hc->center(),
//                                   defender->center(),
                                   cc,
                                   delay);
            }
            
            float damage = 0.0f;
            
            batch << this->calcDamageWithReflect(attackTable(hc).attack,
                                                 hc->getColor(),
                                                 attacker, defender,
                                                 delay, damage);
            
            totalDamage += damage;
            
            {
                float xoffset = randf(-60, 60);
                CCPoint cc(xoffset + defender->center().x, defender->center().y);
                
                TaskPtr damageAnim = createFloatText(this,
                                                     -damage,
                                                     cc,
                                                     GemUtils::Health,
                                                     delay);
                batch << damageAnim;
            }
            
            delay += HERO_ATTACK_DELAY;
        }

        if(totalDamage != 0.0f)
        {
            TaskSequencePtr seq = TaskSequence::make();
            
            TaskPtr hitAnim = this->dealDamageToPlayer(totalDamage,
                                                       attacker,
                                                       defender,
                                                       0.3f);
            /*
            TaskPtr totalHitAnim = createFloatText(this,
                                                   -totalDamage,
                                                   defender->center(),
                                                   GemUtils::Health,
                                                   0.0f);
             */
        
            seq << createDelay(this, 0.5);
//            seq << TaskBatch::make(hitAnim, totalHitAnim);
            seq << hitAnim;
            
            batch << seq;
        }
        
        return batch;
    }
    
    TaskPtr PVPBoardLayer::calcDamageWithReflect(const float atk,
                                                 const GemUtils::GemColor atkCol,
                                                 const PlayerControlPtr attacker,
                                                 const PlayerControlPtr defender,
                                                 float delay,
                                                 float& damage,
                                                 bool ignore)
    {
        auto seq    = TaskSequence::make();
        auto batch  = TaskBatch::make();
        
        float resistant = 0.f;
        batch << defender->getResistanceByColor(atkCol, resistant);
        
        float realDamage = atk * (1.0f - resistant);
        float damageFactor = 1.f, reflectFactor = 0.f;
        GemUtils::GemColor reflectColor = GemUtils::Fire;
        
        batch << defender->onHitCalculation(this, atkCol, realDamage,
                                            damageFactor, reflectFactor, reflectColor,
                                            [](){return PvPRand::rand(); });
        
        if( !ignore ){
            damage = realDamage*damageFactor;
            seq << createDelay(this, delay);
            seq << batch;
            
            if( reflectFactor > 0.f)
            {
                float reflectDamage = realDamage * reflectFactor;
                seq << createDelay(this, 0.1f);
                seq << createOrb(this, defender->center(), attacker->center(), false);
                seq << createDelay(this, 0.1f);
                seq << attacker->hit(reflectDamage, this);
            }
        }
        else
            damage = realDamage;
        
        return seq;
    }
    
    TaskPtr PVPBoardLayer::dealDamageToPlayer(const float damage,
                                              const PlayerControlPtr attacker,
                                              const PlayerControlPtr defender,
                                              float delay)
    {
        TaskSequencePtr seq = TaskSequence::make();
        
        seq << createDelay(this, delay);
        
        if(damage > .0f)
        {
            seq << defender->hit(damage, this);
            float leechFactor = 0.f;
            seq << attacker->onAttack(this, damage, leechFactor);
            if( attacker->isHurt() && !attacker->isDead() && leechFactor > 0.f)
                seq << healWithAnim(this, attacker.get(), leechFactor * damage);
        }
        
        return seq;
    }
    
    GemUtils::GemColor PVPBoardLayer::randGemWeighted()
    {
        // Make this a paramter!
        
        //std::cout<< mIsMyTurn << std::endl;
        
        PlayerControlPtr player = getAttacker();
        PlayerControlPtr player2 = getDefender();
        
        auto batch = TaskBatch::make();
        
        float gemWeights[GemUtils::AllColor];
        for(int i=0; i<GemUtils::AllColor; i++)
        {
            gemWeights[i] = player->mGemWeights[i] * player2->mGemWeights[i];
            //std::cout << gemWeights[i] << " ";
        }
        //std::cout<<std::endl;
        
        if( player->buffGroup.invokeBuff("active.bonus.gem.weight", batch) )
        {
            auto buff = player->buffGroup.getBuffContent<FloatBuff>("active.bonus.gem.weight");
            
            int p = static_cast<int>(buff->getColor());
            float t = 0.f, o = gemWeights[p];
            for(int i=0; i<GemUtils::AllColor; i++) t += gemWeights[i];
            
            float bonus = (t*o*(1-buff->d1))/(buff->d1*o-t);
            if(bonus >= 0)
            {
                gemWeights[p] += bonus;
                mTaskQueue.enqueue(batch);
            }
        }
        
        float sum = 0.0f;
        for(int i=0; i<GemUtils::AllColor; i++)
        {
            sum += gemWeights[i];
        }
        
        float number = PvPRand::randf(sum);
        for(int i=0; i<GemUtils::AllColor; i++)
        {
            if(number < gemWeights[i])
                return (GemUtils::GemColor)i;
            number -= gemWeights[i];
        }
        
        return GemUtils::Fire;
    }
    
    TaskPtr PVPBoardLayer::updatePlayerTurn(PlayerControlPtr player,
                                            BoardResult& result)
    {
        TaskSequencePtr seq = TaskSequence::make();
        
        //if(player->poison.has())
        {
            auto batch = TaskBatch::make();
            if( player->buffGroup.invokeBuff("poison", batch) )
            {
                float damage = player->buffGroup.getBuffContent<FloatBuff>("poison")->d1;
                damage = std::min(damage, player->getHealth() - 1); // leave player with 1 health
                batch << createFloatText(this, damage, player->center(), GemUtils::Dark, 0.3f);
                batch << player->hit(damage, NULL);
                seq << batch;
            }
        }
        
        if(player->passiveBleeding.isValid())
        {
            float damage = sumPP(player->passiveBleeding);
            TaskBatchPtr batch = TaskBatch::make();
            batch << player->hit(damage, NULL);
            
            seq << batch;
        }
        
        {
            TaskBatchPtr batch = TaskBatch::make();
            for(HeroControl* hc : player->team)
                batch << hc->updateCooldown();
            
            float bonusEnergy = 0.f;
            auto  bonusColor = GemUtils::AllColor;
            if( player->buffGroup.invokeBuff("bonusEnergy", batch) )
            {
                auto buff = player->buffGroup.getBuffContent<FloatBuff>("bonusEnergy");
                bonusEnergy += buff->d1;
                bonusColor = buff->getColor();
            }
            
            for(HeroControl* hc : player->team)
            {
                float minEnergy = std::max(player->mMinEnergy[hc->getColor()],
                                           player->mMinEnergy[GemUtils::AllColor]);
                
                if(hc->info.profile->color ==  bonusColor || bonusColor == GemUtils::AllColor)
                {
                    minEnergy += bonusEnergy;
                }
                
                if(minEnergy != 0.0f)
                    batch << hc->addEnergy(minEnergy);
            }
            
            seq << batch;
        }
        
        //if(this->mPlayer->isAlive())
        if( player->buffGroup.invokeBuffWhen("bloodSucker", seq,
                                              [=](BuffPtr){ return player->isAlive(); }))
        {
            auto buff = player->buffGroup.getBuffContent<IntFloatBoolBuff>("bloodSucker");
            auto hc = player->team[buff->d1];
            auto batch = TaskBatch::make();
            
            float damage;
            this->calcDamageWithReflect(buff->d2, hc->getColor(), getAttacker(), getDefender(), 0.f, damage, true);
            seq << getDefender()->hit(damage, NULL);
            seq << healWithAnim(this, player.get(), damage, false, NULL, false);
        }
        
        //----------------------------
        // update player status turns
        {
            TaskBatchPtr batch = TaskBatch::make();
            for(HeroControl* hc : player->team)
                batch << hc->updateStatusTurn();
            seq << batch;
        }
        
        return seq;
    }
    
    void PVPBoardLayer::transitionToGameScene(PvPEndGame endGame,
                                              int reputationGain,
                                              int scoreGain,
                                              int turnUsed)
    {
        this->removeFromParentAndCleanup(true);
        
        turnUsed = turnUsed / 2;
        
        GameScene* gameScene = GameScene::getInstance();
        CCDirector::sharedDirector()->replaceScene(gameScene);
        
        stopBgMusic();
        gameScene->initBG();
        
        // clear inGame state receiver
        PuzzleRPC::getInstance()->getPvP()->setInGameReceiver(NULL);
        
        // clear any pending wait jobs
        PuzzleRPC::getInstance()->getPvP()->clearWaitJobs();
        
        SystemInform* alert = SystemInform::alertLoading(gameScene);
        runOnMainThread([=]()
        {
            runOnMainThread([=]()
            {
                alert->cancel();
                unloadBattleResources();
                
                // load UI resources and recreate interface.
                gameScene->recoverUI();
                
                if(endGame == PvPEndGame::WIN)
                {
                    PvPEndGameWin* win = PvPEndGameWin::create(reputationGain,
                                                               scoreGain,
                                                               turnUsed);
                    gameScene->pushLayerWithDefaultAnimation(win);
                }
                else if(endGame == PvPEndGame::LOSS)
                {
                    PvPEndGameLoss* loss = PvPEndGameLoss::create(reputationGain,
                                                                  scoreGain,
                                                                  turnUsed);
                    gameScene->pushLayerWithDefaultAnimation(loss);
                }
                else
                {
                    PvPEndGameTie* tie = PvPEndGameTie::create(reputationGain,
                                                                scoreGain,
                                                                turnUsed);
                    gameScene->pushLayerWithDefaultAnimation(tie);
                }
            });
        });
    }
    
    void PVPBoardLayer::update(float dt)
    {
        mTaskQueue.update(dt);
    }
    
    void PVPBoardLayer::startTurn()
    {
        this->disableUI();
        
        PlayerControlPtr attacker = getAttacker();
        PlayerControlPtr defender = getDefender();
        
        fillSweepUntilDone(attacker, mResult);
        
        // if still got moves, simply reanble UI
        auto batch = TaskBatch::make();
        if( attacker->buffGroup.invokeBuff("moveAgain", batch) )
        {   
            attacker->buffGroup.updateBuff("moveAgain");
            mTaskQueue.enqueue(batch);
            mTaskQueue.enqueue(TaskLambda::make([=]
                                                {
                                                    this->enableUI();
                                                    mBoardMask->runAction(CCFadeTo::create(0.2, 0));
                                                }));
        }
        else
        {
            mTaskQueue.enqueue(TaskLambda::make([=]()
                                                {
                                                    //                initRefMap(this);
                                                    mBoardMask->runAction(CCFadeTo::create(0.2, 150));
                                                }));
            
            attack(attacker, defender, *mResult);
            
            // clear result
            mResult = BoardResultPtr(new BoardResult(defender->team));
            
            // this will implicitly change attacker/defender role
            mTurn++;
            TaskSequencePtr postSeq = TaskSequence::make();
            
            postSeq << defender->updateStatusTurnPostEnemy();
            postSeq << updatePlayerTurn(attacker, *mResult);
            postSeq << attacker->updateStatusTurn();
            
            mTaskQueue.enqueue(postSeq);
            
            TaskSequencePtr endSeq = TaskSequence::make();
            
            this->mTaskQueue.enqueue( attacker->buffGroup.detachBuff("disableSweep") );
            this->mTaskQueue.enqueue( defender->buffGroup.detachBuff("disableSweep") );
            
            if(mPlayer1 == attacker)
            {
                mIsMyTurn = false;
                
                if(defender->isDead())
                {
                    PuzzleRPC::getInstance()->getPvP()->sendEndRound(this, PvPEndGame::WIN);
                    mHourGlass1->stop();
                }
                else if(attacker->isDead())
                {
                    PuzzleRPC::getInstance()->getPvP()->sendEndRound(this, PvPEndGame::LOSS);
                    mHourGlass1->stop();
                }
                else
                {
                    endSeq << TaskLambda::make([=]()
                    {
                        mSmallTurnLabel->setString(turnStr().c_str());
                        mBigTurnLabelOurs->setString(turnStr().c_str());
                        mBigTurnLabelTheirs->setString(turnStr().c_str());
                        stopBoardBreathAnim();
                    });
                    endSeq << createFlyin(mRoundLabelThem, 0.6);
                    endSeq << TaskLambda::make([=]()
                    {
                        mBoardMask->runAction(CCFadeTo::create(0.2, 0));
                        
                        // Wait for a ACK confirm
                        if(mWaitingForACK)
                        {
                            mWaitAckText->setOpacity(0);
                            mWaitAckText->runAction(CCSequence::create(CCDelayTime::create(1.0f),
                                                                       CCFadeIn::create(0.5f),
                                                                       NULL));
                        }
                    });

                    PuzzleRPC::getInstance()->getPvP()->sendEndRound(this);
                    mHourGlass1->stop();
                    
                    LOGD("    Waiting for ACK\n");
                    mWaitingForACK = true;
                    
                    PuzzleRPC::getInstance()->getPvP()->waitOnEvent(GameEvents::AckEndRound,
                                [=](GameEvents e, const rapidjson::Value& unused)
                    {
                        {
                            LOGD("    Confirm ACK\n");
                            mWaitingForACK = false;
                            mWaitAckText->stopAllActions();
                            mWaitAckText->runAction(CCFadeOut::create(0.3f));
                        }
                        
                        mTaskQueue.enqueue(TaskLambda::make([=]()
                        {
                            mHourGlass2->start(mConfig.moveTimeout, [=](){this->onOpponentTimeout();} );
                        }));
                    });
                }
            }
            else
            {
                mIsMyTurn = true;
                if(defender->isDead())
                {
                    // im dead, just wait for EndGame
                    mHourGlass2->stop();
                }
                else if(attacker->isDead())
                {
                    mHourGlass2->stop();
                }
                else
                {
                    mHourGlass2->stop();

                    endSeq << TaskLambda::make([=]()
                    {
                        mSmallTurnLabel->setString(turnStr().c_str());
                        mBigTurnLabelOurs->setString(turnStr().c_str());
                        mBigTurnLabelTheirs->setString(turnStr().c_str());
                    });
                    endSeq << createFlyin(mRoundLabelUs, 0.6);
                    
                    endSeq << TaskLambda::make([=]()
                    {
                        this->enableUI();
                        mHourGlass1->start(mConfig.moveTimeout, [=](){ this->onSelfTimeout(); });
                        mBoardMask->runAction(CCFadeTo::create(0.2, 0));
                        startBoardBreathAnim();
                    });
                }
            }
            mTaskQueue.enqueue(endSeq);
        }
    }
    
    void PVPBoardLayer::fillSweepUntilDone(PlayerControlPtr player,
                                           BoardResultPtr result)
    {
        for(;;)
        {
            mTaskQueue.enqueue( mBoardControl->fillBoard([=](){ return this->randGemWeighted();}) );
            mTaskQueue.enqueue( mBoardControl->sweepBoard(player, result) );
            
            if(!mBoardControl->hasEmptyCell())
                break;
        }
    }
    
    static bool moveStarted = false;
    
    void PVPBoardLayer::endDrag()
    {
        PlayerControlPtr player = getAttacker();
        
        player->stopTicking();
        moveStarted = false;
        if(mDragGem != NULL)
        {
            mDragGem->root->setAnchorPoint(ccp(0.5f, 0.5f));
            mDragGem->root->setPosition(g2w_center(mDragGem->position));
            mDragGem->root->removeFromParent();
            mBoardControl->root->addChild(mDragGem->root);
            mDragGem.reset();
            
            mShadowGem->removeFromParent();
        }
        
        // exec immediately
        this->startTurn();
    }

    // default implements are used to call script callback if exist
    bool PVPBoardLayer::ccTouchBegan(CCTouch* touch, CCEvent* event)
    {
        CCPoint p = mBoardControl->root->convertTouchToNodeSpace(touch);
        
        Vec2i gp = w2g(p);
        mDragGem.reset();
        
        if(mBoardControl->getGrid().isInside(gp))
        {
            GemPtr pGem = mBoardControl->get(gp);
            if(pGem && pGem->isMoveable())
            {
                mDragGem = pGem;
                CCPoint originP = mDragGem->root->getPosition();
                mDragGem->root->setAnchorPoint(ccp(0.5, 0.25));
                mDragGem->root->setPosition(mBoardControl->root->convertToWorldSpace(mDragGem->root->getPosition()));
                
                // remove gem sprite and add it to top level layer
                // force this sprite on top of player icons.
                mDragGem->root->removeFromParentAndCleanup(false);
                this->addChild(mDragGem->root, ORDER_ABOVE_ICON);
                
                // shadow gem
                mShadowGem = createShadowGem(mDragGem->color());
                mShadowGem->setPosition(originP);
                mBoardControl->root->addChild(mShadowGem, -1);
                
                return true;
            }
        }
        
        return true;
    }
    
    void PVPBoardLayer::ccTouchMoved(CCTouch *touch, CCEvent *event)
    {
        CCPoint p = mBoardControl->root->convertTouchToNodeSpace(touch);

        auto attacker = getAttacker();
        auto defender = getDefender();
        
        // in PvPBoard, timer is a children of top/bottom layer.
        // This might not be the best way to handle it.
        if(attacker == mPlayer1)
            attacker->moveTimer.setPosition(mBottomLayer->convertTouchToNodeSpace(touch));
        else
            attacker->moveTimer.setPosition(mTopLayer->convertTouchToNodeSpace(touch));
        
        // if dragging a gem
        if(mDragGem != NULL)
        {
            p.x = clampx(p.x, 0.0f, 620.0f);
            p.y = clampx(p.y, 0.0f, 515.0f);
            
            mDragGem->root->setAnchorPoint(ccp(0.5, 0.25));
            mDragGem->root->setPosition(mBoardControl->root->convertToWorldSpace(p));
            
            Vec2i gp = w2g(p);
            
            if(mBoardControl->getGrid().isInside(gp))
            {
                GemPtr dstGem = mBoardControl->get(gp);
                
                if(dstGem != mDragGem)
                {
                    if(moveStarted)
                        ;
                    else
                    {
                        TaskPtr task = attacker->startTicking(mTurn, [=]()
                        {
                            endDrag();
                        });
                        mTaskQueue.enqueue(task);
                        moveStarted = true;
                        stopBoardBreathAnim();
                    }
                    
                    TaskBatchPtr batch = TaskBatch::make();
                    
                    TaskPtr moveDstGem = moveGemAnim(dstGem->root, mDragGem->position);
                    TaskPtr moveShadowGem = moveGemAnim(mShadowGem, dstGem->position);

                    PuzzleRPC::getInstance()->getPvP()->sendMove(mDragGem->position,
                                                                 dstGem->position);
                    
                    mBoardControl->swapGem(mDragGem->position, dstGem->position);
                    
                    mTaskQueue.enqueue(TaskBatch::make(moveDstGem, moveShadowGem));
                    
                    ring("sound/v/drag_gem.mp3");
                }
            }
        }
    }
    
    void PVPBoardLayer::ccTouchEnded(CCTouch *touch, CCEvent *event)
    {
        PlayerControlPtr attacker = getAttacker();
        // TODO: merge this ugliness to endDrag
        if(mDragGem)
        {
            // player started dragging but no swapping has happened yet
            // do not count this as a move.
            auto batch = TaskBatch::make();
            if( !moveStarted || attacker->buffGroup.invokeBuff("disableSweep", batch) )
            {
                mTaskQueue.enqueue(batch);
                
                mDragGem->root->setAnchorPoint(ccp(0.5, 0.5));
                mDragGem->root->setPosition(g2w_center(mDragGem->position));
                mDragGem->root->removeFromParent();
                mBoardControl->root->addChild(mDragGem->root);
                mDragGem.reset();

                mShadowGem->removeFromParent();
                return;
            }
            else
            {
                endDrag();
            }
        }
    }
    
    void PVPBoardLayer::ccTouchCancelled(CCTouch *touch, CCEvent *event)
    {
        LOGD("touch cancelled\n");
    }

}