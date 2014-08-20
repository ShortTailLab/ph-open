#include "Board/DebugMisc.h"
#include "Board/Board.h"
#include "CCLuaEngine.h"
#include "HeroTable.h"
#include "CCCallLambda.h"
#include "SkillContext.h"
#include "EnemySkill.h"
#include "Utils.h"
#include "GameScene.h"
#include "Victory.h"
#include "JiangHu.h"
#include "SimpleAudioEngine.h"
#include "CCLuaEngine.h"
#include "network/PuzzleRPC.h"
#include "GenericCallLua.h"
#include "AnimPreset.h"
#include "CCNative.h"

#include <algorithm>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace PH
{
    
void LuaGlobal::init(BoardLayer* layer, GemGrid* g, PlayerControl* p)
{
    using namespace boost;
    // Initialize globa lua variables
    auto args = make_tuple(make_tuple<void*, const char*>(g, "PH::Grid2D<PH::Gem*>"),
                           make_tuple<void*, const char*>(p, "PH::PlayerControl")
                           );
    std::string error;
    luacall("InitGlobals", args, error);
    
    updateHeroList(p->team);
}
void LuaGlobal::printGlobalInfo()
{
    using namespace boost;
    std::string error;
    luacall("PrintGlobalInfo", make_tuple(), error);
}
    
void LuaGlobal::updateHeroList(HeroControlList& l)
{
    using namespace boost;
    
    std::string error;
    luacall("ClearHeroList", make_tuple(), error);
    
    for(int i=0; i<l.size(); i++)
    {
        auto args = make_tuple(make_tuple<void*, const char*>(l[i],
                                                              "PH::HeroControl"));
        std::string error;
        luacall("AddToHeroList", args, error);
    }
}
    
void LuaGlobal::updateEnemyList(EnemyControlList& l)
{
    using namespace boost;

    std::string error;
    luacall("ClearEnemyList", make_tuple(), error);
    
    for(int i=0; i<l.size(); i++)
    {
        auto args = make_tuple(make_tuple<void*, const char*>(l[i].get(),
                                                              "PH::EnemyControl"));
        std::string error;
        luacall("AddToEnemyList", args, error);
    }
}

void LuaGlobal::free()
{
    using namespace boost;
    std::string error;
    luacall("FreeGlobals", make_tuple(), error);
}

//--------------------
// BoardLayer
//--------------------
bool BoardLayer::init(const HeroList& team, const LevelInfo& level)
{
    if(!CCLayer::init())
        return false;

    // --------------------------------------------
    // Only allow touches after a short moment to avoid
    // accidental touches before layer is displayed.
//    this->scheduleOnce(schedule_selector(BoardLayer::delayedTouchEnable), 0.4);
//

    this->scheduleUpdate();
    
    {
        std::string bgName = level.bg;
        boost::to_lower(bgName);
        std::string bgPath = std::string("bg/") + bgName + ".jpg";
        if(!resourcePresentForLoading(bgPath))
            bgPath = "bg/misc.jpg";
        
        CCSprite* bg = GetSprite(bgPath.c_str());
        bg->setPosition(ccp(0, 560));
        bg->setAnchorPoint(ccp(0, 0));
        this->addChild(bg, ORDER_BACKGROUND);
        
        {
            CCSpriteBatchNode* bgBatch = CCSpriteBatchNode::create("battleMisc.png");
            bgBatch->setAnchorPoint(ccp(0, 0));
            bgBatch->setPosition(ccp(0, 0));
            this->addChild(bgBatch, ORDER_BACKGROUND);
            
            // create board background
            // assuming battleMisc.png has both qipan tile textures
            for(int x = 0; x < BOARD_WIDTH; x++)
            for(int y = 0; y < BOARD_HEIGHT; y++)
            {
                int index = x * BOARD_HEIGHT + y;
                int color = index % 2;  
                
                CCSprite* tile = color == 0 ? GetSprite("ui/qipan_qian.png") : GetSprite("ui/qipan_shen.png");
                tile->setAnchorPoint(ccp(0, 0));
                tile->setPosition(g2w(Vec2i(x, y)));
                bgBatch->addChild(tile);
            }
        }

        mBoardMask = GetSprite("ui/1x1_b.png");
        mBoardMask->setAnchorPoint(ccp(0, 0));
        mBoardMask->setScaleX(640 * contentScale());
        mBoardMask->setScaleY(535 * contentScale());
        mBoardMask->setPosition(ccp(0,0));
        mBoardMask->setOpacity(150);
        mBoardMask->setVisible(false);
        this->addChild(mBoardMask, ORDER_MASK);
        
        mRoundBg = GetSprite("ui/round_bg.png");
        mRoundBg->setAnchorPoint(ccp(0.5, 0.0));
        mRoundBg->setPosition(ccp(1000, 730));
        this->addChild(mRoundBg, ORDER_FLOAT);
        
        mElementGraph = GetSprite("ui/element_graph.png");
        mElementGraph->setAnchorPoint(ccp(1.0, 1.0));
        mElementGraph->setPosition(ccp(638, 956));
        this->addChild(mElementGraph);
        
        if(!GameConfig::shared()->showAttrHint) this->mElementGraph->setVisible(false);
        
        mRoundText = CCLabelBMFont::create(toStr("3/5").c_str(), "bmfont/KTL_100_yellow.fnt");
        mRoundText->setScale(contentScale());
        mRoundText->setAnchorPoint(ccp(0, 0));
        mRoundText->setPosition(ccp(210, 40));
        mRoundBg->addChild(mRoundText);
        
        mBossRound = GetSprite("ui/shaqixilai_3.png");
        mBossRound->setAnchorPoint(ccp(0.5, 0.5));
        mBossRound->setPosition(ccp(1000, 730));
        mBossRound->setVisible(false);
        this->addChild(mBossRound, ORDER_FLOAT);
        
        CCSprite* dropChest = GetSprite("ui/diaoluoxiaoke_xiangzi.png");
        dropChest->setAnchorPoint(ccp(0, 1));
        dropChest->setPosition(ccp(8, 886));
        this->addChild(dropChest);
        
        mDropCountLabel = CCLabelBMFont::create("0", "bmfont/Junegull_28_yellow.fnt");
        mDropCountLabel->setAnchorPoint(ccp(0, 0));
        mDropCountLabel->setPosition(32, -4);
        mDropCountLabel->setScale(contentScale());
        dropChest->addChild(mDropCountLabel);
    }
    
    {
        CCMenu* menu = CCMenu::create();
        CCMenuItem* item = CCMenuItemSprite::create(GetSprite("ui/zanting.png"),
                                                    GetSprite("ui/zanting.png"));
        item->setTarget(this, menu_selector(BoardLayer::promptForMenu));
        item->setAnchorPoint(ccp(0, 1));
        item->setPosition(ccp(5, 955));
        
        menu->setPosition(ccp(0, 0));
        menu->addChild(item);

#if PH_DEBUG_BUILD == 1
        CCLabelTTF * debug_tips = CCLabelTTF::create("无敌模式", FONT_CN, 30);
        this->mDebugItem = CCMenuItemLabel::create(debug_tips);
        this->mDebugItem->setTarget(this, menu_selector(BoardLayer::promptForDebugMode));
        this->mDebugItem->setAnchorPoint(ccp(0.5,0.5));
        this->mDebugItem->setPosition(ccp(130,940));
        menu->addChild(this->mDebugItem);
#endif

        this->addChild(menu);
        
        mExitMenu = menu;
    }

    {
        mPlayer = createPlayerFromTeam(team);
        mResult = BoardResultPtr(new BoardResult(mPlayer->team));
    }

    {
        // initialize level
        mLevel = level;
        mRoundIndex = 0;
        
        this->mStats.maxComboPerLevel.resize(this->mLevel.roundList.size());
        this->mStats.roundPerLevel.resize(this->mLevel.roundList.size());
        //this->mStats.refCombo = this->mLevel.refCombo;
        
        this->mStats.refAvgCombo = this->mLevel.avgCombo;
        this->mStats.refMaxCombo = this->mLevel.maxCombo;
        this->mStats.refRound = this->mLevel.refRound;
    }
    
    {
        mTargetCross = GetSprite("ui/baxin.png");
        mTargetCross->setOpacity(0);
        mTargetCross->runAction(CCRepeatForever::create(CCRotateBy::create(2, 360)));
        this->addChild(mTargetCross, ORDER_FLOAT);
    }
    
    // selected ban certain types of gem
    {
        // allow only three types of banned gems
        
        // Disable certain gems for this level
        for(int si = 0; si < level.avoidGems.size() && si < 3; si++)
        {
            std::string lower = level.avoidGems[si];
            boost::algorithm::to_lower(lower);
            if(lower == "random")
            {
                // random color, including health
                std::vector<int> allowed;
                for(int i=0; i<GemUtils::AllColor; i++)
                    if(mGemAllowed[i])
                        allowed.push_back(i);
                
                if(allowed.size())
                {
                    int i = rand() % allowed.size();
                    mGemAllowed[allowed[i]] = false;
                }
            }
            else if(lower == "randomx")
            {
                // random color, not including health
                std::vector<int> allowed;
                for(int i=0; i<GemUtils::Health; i++)
                    if(mGemAllowed[i])
                        allowed.push_back(i);
                
                if(allowed.size())
                {
                    int i = rand() % allowed.size();
                    mGemAllowed[allowed[i]] = false;
                }
            }
            else
            {
                mGemAllowed[ GemUtils::strToColor(lower) ] = false;
            }
        }
    }

    // be the last thing
    {
        mBoardControl = BoardControl::make([=](){ return randGemWeighted(); });
        if(!mBoardControl)
            return false;
        
        this->addChild(mBoardControl->root, ORDER_ABOVE_ICON);
    }
    
    {
        LuaGlobal::init(this, &mBoardControl->getGrid(), mPlayer.get());
        LuaGlobal::printGlobalInfo();
    }

    mShowDragHintInTurn = true;
    showDragHintWithDelay(0.f);
    //mTaskQueue.enqueue(this->showDragHint());
    {
        // load first round
        mTaskQueue.enqueue(TaskLambda::make([=](){ this->disableUI(); }));
        loadNextRoundIfDone();
        mTaskQueue.enqueue(TaskLambda::make([=](){ this->enableUI(); }));
    }
    
    this->setKeypadEnabled(true);
    
	return true;
}

#if PH_DEBUG_BUILD == 1
void BoardLayer::promptForDebugMode() {
    this->mIsDebugMode = !this->mIsDebugMode;

    CCLabelTTF * debug_tips;
    if( this->mIsDebugMode )
    {
        debug_tips = CCLabelTTF::create("恢复正常", FONT_CN, 30);
    }
    else
    {
        debug_tips = CCLabelTTF::create("无敌模式", FONT_CN, 30);
    }
    this->mDebugItem->setLabel(debug_tips);
}
#endif
    
void BoardLayer::promptForMenu(CCObject* o)
{
    std::string levelId = mLevel.id;
    
    PlayerData* playerData = PlayerData::getInstance();
    std::vector<boost::shared_ptr<Stage>> stageList = playerData->normalStageList;
    // merge stage lists
    stageList.insert(stageList.end(),
                     playerData->instanceStageList.begin(),
                     playerData->instanceStageList.end());
    
    std::string stageName = "";
    std::string levelName = "";
    
    for(auto& stage : stageList)
    {
        const Level* level = stage->findLevelById(levelId);
        if(level != NULL)
        {
            stageName = stage->name;
            levelName = level->name;
            break;
        }
    }
    
    BattleExitMenu::create(this,
                           stageName,
                           levelName,
                           mRoundIndex,
                           mLevel.roundList.size(),
                           mTotalHeroDropCount,
                           mTotalSoulDropCount,
                           [=]{
                               SystemInform::alertConfirmPanel(this,
                                                               "确定逃跑吗？会丢失已经获得全部奖励噢！",
                                                               "逃跑",
                                                               [=](){ returnToMainMenu(); },
                                                               [](){});
                           },
                           [=]
    {
        this->mElementGraph->setVisible(GameConfig::shared()->showAttrHint);
        if(!GameConfig::shared()->showDragHint){
            this->clearArrowPath();
        }else
        {
            if( this->mDragHint.size() == 0 )
                this->showDragHint();
        }
    });
}
    
void BoardLayer::promptForRevival()
{
    mStartPromptForRevival = false;
    mStats.deathCount++;
    int golds = PlayerData::getInstance()->player->golds;
    
    /*
    if(golds < 60)
    {
        SystemInform::alertConfirmPanel(this,
                                        ("元宝不足！\n使用60个元宝可以复活,\n您有" + toStr(golds) +"个元宝！\n"+"是否补充？\n(战败会丢失已经获得的物品噢)\n").c_str(),
                                        "复活",
                                        // confirm revive
                                        [=]()
                                        {
                                            disableUI();
                                            ChongZhiLayer* chongzhi = ChongZhiLayer::create(true);
                                            chongzhi->setPosition(ccp(320, 850));
                                            chongzhi->inBattleReturnCall = [this]()
                                            {
                                                this->enableUI();
                                                this->promptForRevival();
                                            };
                                            this->addChild(chongzhi, ORDER_VICTORY);
                                        },
                                        // cancel button
                                        [=]()
                                        {
                                            returnToMainMenu();
                                        });
    }
    else*/
    if(golds < 60)
    {
        SystemInform::alertMessage(this,
                                   "复活需要60元宝，您的元宝不足，无法复活，点击确定离开战场.",
                                   "复活",
                                   [=]() { returnToMainMenu(); });
    }
    else
    {
        auto confirm = [=]()
        {
            SystemInform* alert = SystemInform::alertLoading(this);
            
            PuzzleRPC::getInstance()->revive(PlayerData::getInstance()->getToken(),
                                             [=](std::auto_ptr<Player> player,
                                                 std::auto_ptr<Error> e)
            {
                alert->cancel();
                
                if(e.get())
                {
                    SystemInform::alertConfirmPanel(this,
                                                    "无法连接到服务器，请检查网络后重试。取消会丢失本关进度噢。",
                                                    "连接失败",
                                                    [=](){ promptForRevival(); },
                                                    [=](){ returnToMainMenu(); });
                    return;
                }

                mBoardMask->setVisible(true);
                this->disableUI();
                
                this->getChildByTag(DEFEAT_ANIM_TAG)->removeFromParentAndCleanup(true);
                
                TaskPtr anim = healWithAnim(this, mPlayer.get(), mPlayer->getMaxHealth(), true);
                mTaskQueue.enqueue(anim);
                mTaskQueue.enqueue(TaskLambda::make([=]()
                                                    {
                                                        mBoardMask->setVisible(false);
                                                        this->enableUI();
                                                    }));
                
                PlayerData::getInstance()->updatePlayer(player.get());
                
                mStats.reviveCount++;
                
                if(mRoundIndex >= mLevel.roundList.size() &&
                   isRoundDone() )
                {
                    finishWithVictory();
                    return;
                }
            });
        };
        
        SystemInform::alertConfirmPanel(this,
                                        ("使用60个元宝可以复活,\n您现在有" + toStr(golds) +"个元宝！\n是否使用？\n战败会丢失已经获得的物品和侠客噢.\n").c_str(),
                                        "复活",
                                        confirm,
                                        // cancel button
                                        [=]() {  returnToMainMenu(); });
    }
}

void BoardLayer::disableUI()
{
    mExitMenu->setTouchEnabled(false);
    this->setTouchEnabled(false);
    for(HeroControl* hc : mPlayer->team)
        hc->icon->setEnabled(false);
    
}
    
void BoardLayer::enableUI()
{
    mExitMenu->setTouchEnabled(true);
    this->setTouchEnabled(true);
    for(HeroControl* hc : mPlayer->team)
        hc->icon->setEnabled(true);
}
    
PlayerControlPtr BoardLayer::createPlayerFromTeam(const HeroList& team)
{
    CCPoint p = ccp(1, 560);
    // Create and layout player related data and UI
    PlayerControlPtr player = boost::make_shared<PlayerControl>(p, PlayerControl::PVE_PLAYER);
    
    int health = 0;

    for(int i=0; i<team.size(); i++)
    {
        if(team[i] == NULL)
            continue;
        
        // make a copy of hero
        Hero h = boostHeroStatsByRelations(*team[i], team);
        
        float xbase = 1 + 106*i;
        float ybase = 560;
        CCPoint p = ccp(xbase+4, ybase+4);
        
        HeroControl* hc = new HeroControl(h, p, false);
        hc->setOnClick([=](HeroIconItem* item)
                       {
                           const SkillInfo& skill = AST(hc->info.profile->activeSkillId);
                           if(skill.sid != -1 && (hc->isSkillReady() || SKILL_NEED_NO_ENERGY))
                           {
                               // tutorial callback
                               onAlertSkillCast();
                               
                               SystemInform::alertSkill(this,
                                                        skill.name,
                                                        skill.desc,
                                                        toStr(hc->maxEnergy),
                                                        CCPoint(320, 300),
                                                        [=](){ castHeroSkill(item); },
                                                        [=](){ onAlertSkillCastCancelled(); } );
                           }
                       });
        player->menu->addChild(hc->icon);
        
        health += hc->health;
        player->team.push_back(hc);
    }
    
    // Initialize max health. This might be changed by passive skills
    player->setMaxHealthDirectly(health, true);
    this->addChild(player->healthBox->get());
    
    // Load player passive skills
    if(player->team.size() > 0)
    {
        SkillContext::invokePassive(this,
                                    player,
                                    player->team.front());
    }
    
    if(player->team.size() > 1)
    {
        SkillContext::invokePassive(this,
                                    player,
                                    player->team.back());
    }

    // initialize skill chains
    for(HeroControl* hc : player->team)
    {
        player->skillChain[hc->info.profile->activeSkillId].add(hc);
    }
    
    this->addChild(player->menu, ORDER_HERO_ICON);
    
    // add timers to layer
    {
        this->addChild(player->moveTimer.box, ORDER_FLOAT);
    }
    
    return player;
}
    
TaskPtr BoardLayer::loadRound(RoundInfo& round)
{
    assert(mEnemyList.size() == 0 && "mEnenyList size not 0, round is not yet cleared");

    printf("Loading next round...\n");
    TaskBatchPtr batch = TaskBatch::make();
    const int size = round.size();
    
    float moveLeft = 65;
    float screenWidth = 640;
    float step = (screenWidth + moveLeft*2)/ (size+1);
     
    for(int i=0; i<size; i++)
    {
        EnemyInfo ei = round[i];
        if(mPlayer->passiveExtraWaitForEnemy.isValid())
        {
            for(auto iter : mPlayer->passiveExtraWaitForEnemy.map())
            {
                ei.maxTurn += iter.second;
            }
        }
        
        bool isBoss = ( size <=2 && ei.star == 5 );
        EnemyControlPtr e(new EnemyControl(ei, isBoss));
        
        // root node
        float leftx = step * (i+1) - moveLeft;
        e->root->setPosition(ccp(leftx, 660));
        e->root->setAnchorPoint(ccp(0.5f, 0.0f));

        this->addChild(e->root);
        
        if(mPlayer->passiveBonusEnemyStartTurn.isValid()){
            int turn = e->getReadyInTurn() + sumPP(mPlayer->passiveBonusEnemyStartTurn);
            batch << e->setReadyInTurnWithAnim(turn);
        }
        
        //e->setReadyInTurnWithAnim(e->readyInTur)
        mEnemyList.push_back(e);
    }
    
    LuaGlobal::updateEnemyList(mEnemyList);
    return batch;
}

    
void BoardLayer::castHeroSkill(CCObject* sender)
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
        // WARNING: Only change this function if you know what you are doing.
        // this->loadNextRoundIfDone is full of side effects. Be warned.
        TaskSequencePtr seq = TaskSequence::make();
        
        // use up all energy
        
        TaskBatchPtr energyBatch = TaskBatch::make();
        float energy = hc->energy;
        
        //if(this->mPlayer->reduceEnergyCost.has() &&
        //   this->mPlayer->reduceEnergyCost.val().first > 0)
        if( mPlayer->buffGroup.invokeBuff("reduceEnergyCost", energyBatch) )
        {
            auto buff = mPlayer->buffGroup.getBuffContent<FloatBuff>("reduceEnergyCost");
            energy *= (std::min(buff->d1, 1.f));
            mPlayer->buffGroup.updateBuff("reduceEnergyCost");
        }else
            energy = 0;
        energyBatch << hc->setEnergy(energy);
        
        seq << energyBatch;
        
        // callback for tutorial mode
        seq << TaskLambda::make( [=](){this->onSkillCast();} );
        
        // no ui interaction duration skill cast animations
        this->disableUI();
        
        CCFiniteTimeAction* jump = createIconJumpSequence(hc->iconPositionFixed, 0.0f);
        seq << TaskAnim::make(hc->icon, jump, false);
        
        mPlayer->skillChain[hc->info.profile->activeSkillId].startCooldown();
        
        seq << SkillContext::invokePvE(this, hc, mTurn);
        
        // aggregate skill usage info
        mStats.skillUseCount++;
        
        seq << removeDeadEnemies();
        seq << removeDeadPlayer();
        mTaskQueue.enqueue(seq);
        
        if(this->isRoundDone())
        {
            this->startTurn();
        }
        else
        {
            mTaskQueue.enqueue(TaskLambda::make( [=](){this->enableUI();} ));
        }
        
        //mTaskQueue.enqueue(removeDeadEnemies());
        
    }
    else
    {
        printf("skill not ready or hero is not allowed to use skill, %.2f/%f\n",
               hc->energy,
               hc->maxEnergy);
    }
}
    
EnemyControlList BoardLayer::getEnemiesByColor(const GemUtils::GemColor color)
{
    return filter<EnemyControlPtr>(this->mEnemyList,
                                   [=](EnemyControlPtr ec) -> bool
                                   {
                                       return ec->getColor() == color || GemUtils::AllColor == color;
                                   });
}
    
EnemyControlList BoardLayer::getEnemiesInAttackOrder(const GemUtils::GemColor color,
                                                     const bool isArea)
{
    // filter out all enemies that are already killed.
    EnemyControlList validTargets = filter<EnemyControlPtr>(mEnemyList,
                                                            [](EnemyControlPtr ec)->bool
                                                            {
                                                                return !ec->isDead();
                                                            });
    if(isArea)
        return validTargets;
    
    if(mManualTarget.get() != NULL)
    {
        auto pp = std::find(validTargets.begin(), validTargets.end(), mManualTarget);
        if(pp != validTargets.end())
        {
            EnemyControlList targetList;
            targetList.push_back(mManualTarget);
            return targetList;
        }
    }
    
    EnemyControlList strongTargets = filter<EnemyControlPtr>(validTargets,
                                                             [color](EnemyControlPtr ec)
    {
         return GemUtils::compare(color, ec->info.color) > 0;
    });
    
    EnemyControlList neutralTargets = filter<EnemyControlPtr>(validTargets,
                                                              [color](EnemyControlPtr ec)
    {
          return GemUtils::compare(color, ec->info.color) == 0;
    });
    
    EnemyControlList weakTargets = filter<EnemyControlPtr>(validTargets,
                                                           [color](EnemyControlPtr ec)
    {
           return GemUtils::compare(color, ec->info.color) < 0;
    });
    
    EnemyControlList targetList;
    targetList.insert(targetList.end(), strongTargets.begin(), strongTargets.end());
    targetList.insert(targetList.end(), neutralTargets.begin(), neutralTargets.end());
    targetList.insert(targetList.end(), weakTargets.begin(), weakTargets.end());
    
    if(targetList.size() > 1)
        targetList.resize(1);
    
    return targetList;
}
    
TaskSequencePtr BoardLayer::createAttackSequence(const HeroControl* hc,
                                                 const EnemyControlPtr ec,
                                                 const float attack,
                                                 const GemUtils::GemColor heroColor,
                                                 const float delay)
{
    TaskSequencePtr seq = TaskSequence::make();
    
    //------------------------------------------------------
    // spawn an orb from hero icon to enemy position
    seq << createDelay(this, delay);
    if(hc != NULL)
        seq << createOrb(this, hc->center(), ec->center(), true);
    
    //-------------------------------------------------------------------
    // Spawn currently the attack animation and the floating damage text.
    TaskBatchPtr group = TaskBatch::make();
    
    // attack animation
    if(attack > 0.f)
        group << createAttackAnim(this, ec->getAttackPoint(), heroColor);

    seq << group;
    
    return seq;
}
    
void BoardLayer::attack(const BoardResult& result)
{
    // Hanle normal attack stuff
    {
        AttackTable attackTable(mPlayer->team);
        mTaskQueue.enqueue( applyComboModWithAnim(result, attackTable) );
        
        AttackTable finalTable(mPlayer->team);
        mTaskQueue.enqueue( applySkillModWithAnim(attackTable, finalTable) );
        
        mTaskQueue.enqueue(attackWithAnim(finalTable, SKILL_FLAG_LEECH));
        
        mTaskQueue.enqueue(healWithAnim(this,
                                        mPlayer.get(),
                                        finalTable.heal,
                                        false,
                                        finalTable.healLabel,
                                        true));
    }
    
    // Handle follow-up attacks
    if(mPlayer->passiveFollowUpAttacks.isValid() && mEnemyList.size())
    {
        TaskBatchPtr numberBatch = TaskBatch::make();
        
        AttackTable attackTable(mPlayer->team);
        for(auto atk : mPlayer->passiveFollowUpAttacks.map())
        {
            HeroControl* hc = atk.first;
            float factor = atk.second.second;

            if( hc->buffGroup.invokeBuff("active.blind", numberBatch) )
                continue;
            
            float attackFactor;
            numberBatch << mPlayer->getAttackFactor(hc, attackFactor, [](){return rand();});
            
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

        mTaskQueue.enqueue( attackWithAnim(attackTable) );
    }
    
    if(mPlayer->passiveFollowUpAttackWithoutResistence.isValid() && mEnemyList.size())
    {
        TaskBatchPtr numberBatch = TaskBatch::make();
        
        AttackTable attackTable(mPlayer->team);
        for(auto atk : mPlayer->passiveFollowUpAttackWithoutResistence.map())
        {
            HeroControl* hc = atk.first;
            
            if( hc->buffGroup.invokeBuff("active.blind", numberBatch) )
                continue;
            
            attackTable(hc).attack = atk.second.second;
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
        mTaskQueue.enqueue( numberBatch );
        mTaskQueue.enqueue( attackWithAnim(attackTable, SKILL_FLAG_NONE, true) );
    }
}

TaskPtr BoardLayer::applyComboModWithAnim(const BoardResult& result,
                                          AttackTable& attackTable)
{
    if(result.comboCount() == 0)
        return TaskEmpty::make();
    
    attackTable = result.baseAttack;
    
    auto batch = TaskBatch::make();
    
    // calculate combo bonus and play animations
    // accordingly do not count combo 1
    float delay = 0.f, acc = 0.f, comboFactor=0.25f;
    batch << this->mPlayer->getComboFactor(comboFactor);
    for(int i=1; i<mPlayer->minComboCount + result.comboCount(); i++)
    {
        auto seq = TaskSequence::make();
        seq << createDelay(this, delay);
        delay += 0.5f+acc;
        //acc += 0.25f;
        //acc = std::min(0.25f, acc);
        
        TaskBatchPtr numberBatch = TaskBatch::make();
        for(HeroControl* hc : mPlayer->team)
        {
            // skip hero with base attack 0.
            if(attackTable(hc).attack == 0.0f)
                continue;
        
            float prevAttack = attackTable(hc).attack;
            attackTable(hc).attack += result.baseAttack(hc).attack * comboFactor;
            CCFiniteTimeAction* count = createNumberAnimSeq(prevAttack,
                                                            attackTable(hc).attack,
                                                            NUMBER_DURATION);
            // recover states of label
            attackTable(hc).attackLabel->setScale(1.0);
            seq << TaskAnim::make(attackTable(hc).attackLabel, count, false);
            
        }
        
        if(result.baseAttack.heal > 0.0f && mPlayer->isHurt())
        {
            float prevHeal = attackTable.heal;
            attackTable.heal += result.baseAttack.heal * comboFactor;
            CCFiniteTimeAction* count = createNumberAnimSeq(prevHeal,
                                                            attackTable.heal,
                                                            NUMBER_DURATION);
            // recover states of label
            attackTable.healLabel->setScale(1.0f);
            seq << TaskAnim::make(attackTable.healLabel, count, false);
        }
        
        if(i >= mPlayer->minComboCount)
        {
            CCSprite* comboText = result.comboTextList[i - mPlayer->minComboCount];
            numberBatch << createComboTextFadeRemove(comboText, i);
        }
        
        // only play sound when we have an animation to play
        if(numberBatch->size())
        {
            numberBatch << TaskSound::make("sound/v/number_charge.mp3");
            seq << numberBatch;
        }
        
        batch << seq;
    }
    
    float forceArea = false;
    if( mPlayer->buffGroup.invokeBuff("active.force.area.attack", batch) )
        forceArea = true;
    
    for(HeroControl* hc : mPlayer->team)
    {
        attackTable(hc).isArea = attackTable(hc).isArea || isAreaAttack(result, hc->getColor())|| forceArea;
    }
    
    return batch;
}
    
TaskPtr BoardLayer::applySkillModWithAnim(const AttackTable& baseTable,
                                          AttackTable& finalTable)
{
    TaskBatchPtr batch = TaskBatch::make();

    bool doNumberCharge = false;

    finalTable = baseTable;
    
    for(HeroControl* hc : mPlayer->team)
    {
        // skip heroes who don't cause damage
        if(baseTable(hc).attack == 0.0f)
            continue;
        
        finalTable(hc).isArea = baseTable(hc).isArea;

        // apply the consolidated attack factor
        float skillAttackFactor;
        batch << mPlayer->getAttackFactor(hc, skillAttackFactor, [](){return rand();});
        
        // only used for normal attacks
        if(mPlayer->passiveForceAreaAttackAndFactor.isValid())
        {
            finalTable(hc).isArea = true;
            skillAttackFactor *= productPP(mPlayer->passiveForceAreaAttackAndFactor);
        }
        
        if(mPlayer->buffGroup.invokeBuff("skillAttackFactor", batch))
            finalTable(hc).isArea = true;
        
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

TaskPtr BoardLayer::attackWithAnim(AttackTable& attackTable, SkillFlags sflags, bool ignoreArmor)
{
    TaskBatchPtr batch = TaskBatch::make();
    DamageTable damageTable(mEnemyList);
    
    float delay = 0.0f;
    for(HeroControl* hc : mPlayer->team)
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
        
        // skip if player is dead
        if(mPlayer->isDead()){
            attackTable.heal = 0.f;
            continue;
        }
        
        // skip if no attack
        if(attackTable(hc).attack == 0.0f)
        {
            phassert(attackTable(hc).attackLabel == NULL &&
                     "attackLabel exists but attack is 0, attackLabel is never cleaned up");
            continue;
        }
        phassert(attackTable(hc).attackLabel != NULL && "attackLabel is NULL");
    

        bool  isArea = attackTable(hc).isArea; // area attack
        //DamageTable emptyTable(mEnemyList);
        EnemyControlList targetList = getEnemiesInAttackOrder(hc->getColor(),
                                                              isArea);
        // skip if no enemy
        if(targetList.size() == 0)
            continue;
        
        // skip if blinded
        if( hc->buffGroup.invokeBuff("active.blind", batch) )
        {
            delay += HERO_ATTACK_DELAY;
            continue;
        }
        
        {
            // hero icon jump
            CCFiniteTimeAction* jump = createIconJumpSequence(hc->iconPositionFixed, delay);
            batch << TaskAnim::make(hc->icon, jump);
        }

        for(EnemyControlPtr ec : targetList)
        {
            float finalAttack = attackTable(hc).attack;
            float finalDamage;
            
            batch << calcDamageForTarget(finalAttack, hc->getColor(), ec, finalDamage, ignoreArmor, delay+0.5f);
            batch << createAttackSequence(hc, ec, finalDamage, hc->getColor(), delay);
            //float acDelay = -0.35f+delay+calOrbDelay(hc->center(), ec->center());
            batch << dealDamageToOneWithAnim(finalDamage, ec, delay+0.5f);
            damageTable(ec) += finalDamage;
            delay += HERO_ATTACK_DELAY/2;
        }

        delay += HERO_ATTACK_DELAY;
    }
    
    // apply actual damages here as one animation
    /*
    TaskPtr enemyHitAnim = dealDamageToAllWithAnim(damageTable, delay-HERO_ATTACK_DELAY);
    batch << enemyHitAnim;
    */
    return TaskSequence::make()
    << batch
    << removeDeadEnemies()
    << removeDeadPlayer();
}
    
TaskPtr BoardLayer::calcDamageForTarget(const float attack,
                                        const GemUtils::GemColor color,
                                        const EnemyControlPtr ec,
                                        float& damage,
                                        bool ignoreArmor,
                                        float delay)
{
    float prevDamage = attack;
    
    // armor and armor modifier
    TaskBatchPtr batch = TaskBatch::make();
    
    auto seq = TaskSequence::make();
    float reflectFactor = 0.f; float damageFactor = 1.f;
    seq
    << createDelay(this, delay)
    << ec->onHitCalculation(this, color, attack, reflectFactor, damageFactor, ignoreArmor)
    << createTwoPhaseFloatText(this,
                               prevDamage,
                               prevDamage*damageFactor,
                               ec->center(),
                               color,
                               0.f);
    //printf("%f->%f\n", prevDamage, prevDamage*damageFactor);
    
    batch << seq;
    
    if(reflectFactor > 0.f)
    {
        auto seq = TaskSequence::make();
        float reflect = reflectFactor * prevDamage;
        seq << createDelay(this, delay+0.4f);
        
        seq << createOrb(this, ec->center(), getHealthCenter(), false);
        seq << createFloatText(this,
                               -reflect,
                               ccp(320,540),
                               GemUtils::Health,
                               0.f)
            << TaskSound::make("sound/v/player_hit.mp3")
            << mPlayer->hit(reflect, NULL);
        
        batch << seq;
    }
    reflectFactor = std::min(1.0f, reflectFactor);
    
    damage = attack*damageFactor;
    return batch;
}
    
TaskPtr BoardLayer::dealDamageToAllWithAnim(DamageTable& damageTable,
                                            const float delay)
{
    auto seq = TaskSequence::make();
    auto batch = TaskBatch::make();
    
    for(auto ec : mEnemyList)
    {
        if(damageTable.contains(ec) && damageTable(ec) > 0.0f)
            batch << dealDamageToOneWithAnim(damageTable(ec),
                                             ec,
                                             delay);
        if(mPlayer->isDead())
            break;
    }
    
    seq << batch;
    seq << removeDeadEnemies();
    return seq;
}
    
TaskPtr BoardLayer::dealDamageToOneWithAnim(float damage,
                                            EnemyControlPtr ec,
                                            float delay)
{
    TaskSequencePtr seq = TaskSequence::make();
 
    seq << createDelay(this, delay);

    if(damage > .0f)
    {
        seq << ec->onHit(damage);
        float leechFactor = 0.f;
        seq << mPlayer->onAttack(this, damage, leechFactor);
        if(this->mPlayer->isHurt() && !this->mPlayer->isDead() && leechFactor > 0.f)
            seq << healWithAnim(this, this->mPlayer.get(), leechFactor * damage);
    }
    
    return seq;
}

TaskPtr BoardLayer::removeDeadEnemies()
{
    if(mManualTarget && mManualTarget->isDead())
    {
        mManualTarget.reset();
        mTargetCross->setOpacity(0);
    }
    
    auto seq = TaskSequence::make();
    auto batch = TaskBatch::make();
    
    for(auto iter=this->mEnemyList.begin(); iter!=this->mEnemyList.end();)
    {
        auto ec = *iter;
        
        if(ec->isDead())
        {
            batch << ec->reborn();
        }
        if(ec->isDead())
        {
            if(ec->info.drop)
            {
                int star = ec->info.drop->profile->star;
                std::string iconPath = std::string("heroes/diaoluo") + toStr(star) + "_body.png";
                CCSprite* drop = GetSprite(iconPath.c_str());
                drop->setAnchorPoint(ccp(0.5, 0));
                drop->setPosition(ec->getPosition());
                drop->setOpacity(0);
                
                // offset the enemy anchor position
                drop->setPositionY(drop->getPositionY() + 20);
                
                mDropList.push_back(drop);
                this->addChild(drop);
                
                seq << TaskAnim::make(drop, CCFadeIn::create(0.5));
                
                seq << TaskLambda::make([=]()
                {
                    mTotalDropCount++;
                    mTotalHeroDropCount ++;
                    mDropCountLabel->setString(toStr(mTotalDropCount).c_str());
                });
            }
            else if(ec->info.soulDrop)
            {
                CCSprite* drop = GetSprite("ui/hunpo.png");
                drop->setAnchorPoint(ccp(0.5, 0));
                drop->setPosition(ec->getPosition());
                drop->setOpacity(0);
                
                // offset the enemy anchor position
                drop->setPositionY(drop->getPositionY() + 20);
                
                mDropList.push_back(drop);
                this->addChild(drop);
                
                seq << TaskAnim::make(drop, CCFadeIn::create(0.5));
                seq << TaskLambda::make([=]()
                {
                    mTotalDropCount++;
                    mTotalSoulDropCount += ec->info.soulDrop->count;
                    mDropCountLabel->setString(toStr(mTotalDropCount).c_str());
                });
            }

            iter = this->mEnemyList.erase(iter);
        }else
            iter ++;
    }
    
    return TaskBatch::make()
    << seq
    << batch;
}

void BoardLayer::finishWithVictory()
{
    if(this->mStartPromptForRevival)
        return;
    
    // disable battle UI
    disableUI();
    
    mTaskQueue.enqueue( createVictoryAnim(this, ccp(320, 800)) );
    
    mTaskQueue.enqueue(TaskLambda::make([=]()
    {
        SystemInform::alertHint(this,
                                randHint().c_str(),
                                [=]()
        {
            mTaskQueue.enqueue(TaskLambda::make([=]{ this->exitScene(true); }));
        });
    }));
}

void BoardLayer::returnToMainMenu()
{
    mTaskQueue.enqueue(TaskLambda::make([=]{ this->exitScene(false); }));
}
    
GemUtils::GemColor BoardLayer::randGemWeighted()
{
    auto batch = TaskBatch::make();
    
    float gemWeights[GemUtils::AllColor];
    for(int i=0; i<GemUtils::AllColor; i++)
    {
        gemWeights[i] = mPlayer->mGemWeights[i];
    }
    
    if( mPlayer->buffGroup.invokeBuff("active.bonus.gem.weight", batch) )
    {
        auto buff = mPlayer->buffGroup.getBuffContent<FloatBuff>("active.bonus.gem.weight");
        
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
        if(mGemAllowed[i])
        {
            sum += gemWeights[i];
        }
    }
    
    float number = randf(sum);
    for(int i=0; i<GemUtils::AllColor; i++)
    {
        if(mGemAllowed[i])
        {
            if(number < gemWeights[i])
                return (GemUtils::GemColor)i;
            number -= gemWeights[i];
        }
    }
    
    phassert(false && "should never get here");
    return GemUtils::Fire;
}
    
bool BoardLayer::isRoundDone()
{
    if(mEnemyList.size() != 0 || mDivisionEnemyList.size() != 0)
        return false;
    return true;
}

void BoardLayer::loadNextRoundIfDone()
{
    if(!this->isRoundDone()) return;
    
    onLoadNextRound();
    
    if(mRoundIndex >= mLevel.roundList.size())
    {
        finishWithVictory();
        return;
    }
 
    // handle drop items
    TaskSequencePtr seq = TaskSequence::make();
    
    {
        TaskBatchPtr hideDrops = TaskBatch::make();
        for(CCSprite* s : mDropList)
            hideDrops << TaskAnim::make(s, CCFadeOut::create(0.5f));
        seq << hideDrops;
        
        seq << TaskLambda::make([=]()
        {
            for(CCSprite* s : mDropList)
                s->removeAllChildrenWithCleanup(true);
            mDropList.clear();
        });
    }
    
    // load next round
    seq << this->loadRound(mLevel.roundList[mRoundIndex]);

    mRoundIndex++;
    
    CCFiniteTimeAction* splashAction =
    CCSequence::create(CCEaseElasticOut::create(CCMoveTo::create(0.6, ccp(320, 730)), 0.8f),
                       CCEaseElasticIn::create(CCMoveTo::create(0.6, ccp(-400, 730)), 0.8f),
                       NULL);
    
    if(mRoundIndex < mLevel.roundList.size())
    {
        mRoundBg->setPosition(ccp(1000, 730));
        std::string roundNum = toStr(mRoundIndex) + "/" + toStr(mLevel.roundList.size());
        mRoundText->setString(roundNum.c_str());
        
        seq << TaskAnim::make(mRoundBg, splashAction);
    }
    else
    {
        mBossRound->setVisible(true);
        mBossRound->setPosition(ccp(1000, 730));
        seq << TaskLambda::make([](){ playBG("sound/bg/boss.mp3"); });
        seq << TaskAnim::make(mBossRound, splashAction);
    }
    
    TaskBatchPtr debutBatch = TaskBatch::make();
    for(EnemyControlPtr enemy : mEnemyList)
    {
        enemy->setOpacity(0);
        debutBatch << enemy->fadeTo(0.3f, 255);
        debutBatch << enemy->invokePassiveSkill(this);
    }
    seq << debutBatch;
    
    mTaskQueue.enqueue(seq);
}
    
static void promptScriptError(EnemyControlPtr ec, const std::string& errorMessage)
{
    std::string error = ec->info.nameCN + "(" + toStr(ec->info.hid) + ")\n" + ec->info.script.c_str() + ":\n" + errorMessage;
    SystemInform::alertMessage(runningScene(),
                               error.c_str(),
                               "Script Error",
                               [](){});
}
    
void BoardLayer::updateEnemyTurn(BoardResult& result)
{
    {
        auto batch = TaskBatch::make();
        for(EnemyControlPtr ec : mEnemyList)
        {
            // update enemy turn and its label
            batch << ec->updateReadyTurn(this, mTurn);
        }
        
        for(EnemyControlPtr ec : mEnemyList)
        {
            if( ec->buffGroup.invokeBuff("poison", batch) )
            {
                auto buff = ec->buffGroup.getBuffContent<FloatBuff>("poison");
                batch << createFloatText(this, buff->d1, ec->center(), GemUtils::Dark, 0.3f);
                batch << dealDamageToOneWithAnim(buff->d1, ec, 0.0f);
            }
            batch << ec->updateStatusPreTurn(this);
        }
        
        mTaskQueue.enqueue(batch);
        mTaskQueue.enqueue(removeDeadEnemies());
    }

    // enemy attack
    TaskBatchPtr batch = TaskBatch::make();
    
    float totalDamage = 0.0f;
    float delay = 0.0f;
    
    CCPoint playerCenter = getHealthCenter();
    
    int enemyIndex = 0;
    for(EnemyControlPtr ec : mEnemyList)
    {
        if(!ec->ready)
            continue;
        ec->ready = false;
        
        //float prevHealth = mPlayer->getHealth();
        auto seq = TaskSequence::make();
        seq << createDelay(this, delay);
        delay += 0.25;
        enemyIndex++;
        
        // check blind effect
        if(ec->buffGroup.invokeBuff("blind", seq))
            continue;
        
        int doNormalAttack = 1;
        
        auto damageBatch = TaskBatch::make();
        
        seq << ec->invokeActiveSkill(this, enemyIndex-1, doNormalAttack);
        seq << createOrb(this, ec->center(), playerCenter);
        
        float damage = 0.f, damageFactor = 1.f, reflectFactor = 0.f;
        GemUtils::GemColor refColor = GemUtils::AllColor;
        if(doNormalAttack)
        {
            damageBatch << ec->getAttack(damage);
            if (damage <= 0.f)  continue;
            
            damageBatch << mPlayer->onHitCalculation(this,
                                                     ec->getColor(),
                                                     damage,
                                                     damageFactor,
                                                     reflectFactor,
                                                     refColor,
                                                     [](){return rand();});
            
            // handle normal attack
            float postDamage = damage * damageFactor;
            damageBatch << ec->onAttack(this, postDamage);
            
            //-----------------------
#if PH_DEBUG_BUILD == 1
            if(this->mIsDebugMode)
                postDamage = 1.f;
#endif
            
            totalDamage += postDamage;
            
            if( postDamage > 0.1f )
            {
                damageBatch << createFloatText(this,
                                               -postDamage,
                                               ccp(320, 540),
                                               GemUtils::Health,
                                               0.15f);
            }
            
            damageBatch << TaskSound::make("sound/v/player_hit.mp3");
            damageBatch << mPlayer->hit(postDamage, this);
        }

        seq << damageBatch;
        
        // # handle reflect stuff
        {
            if(reflectFactor > 0.f)
            {
                float reflectDamage = damage * reflectFactor, finalDamage;
                seq << createDelay(this, 0.1f);
                seq << createOrb(this, playerCenter, ec->center(), false);
                seq << createDelay(this, 0.1f);
                
                auto localBatch = TaskBatch::make();
                localBatch << calcDamageForTarget(reflectDamage, refColor, ec, finalDamage, false, .5f);
                
                if(finalDamage > 0.f)
                {
                    localBatch << createAttackSequence(NULL,
                                                       ec,
                                                       finalDamage,
                                                       refColor,
                                                       0.f);
                    localBatch << dealDamageToOneWithAnim(finalDamage, ec, 0.7f);
                }
                
                seq << localBatch;
                delay += 0.5f;
            }
        }
        
        batch << seq;
        
        if(mPlayer->isDead())
            break;
    }
    
    for(EnemyControlPtr ec : mEnemyList)
        batch << ec->updateStatusPostTurn(this);

    // clearup
    for(auto entry : this->mEnemyList)
    {
        if(entry->ready)
            entry->ready = false;
    }
    
    if(!mPlayer->isDead())
    {
        mTaskQueue.enqueue(batch);
        mTaskQueue.enqueue(removeDeadEnemies());
        this->loadNextRoundIfDone();
    }
    else if(mPlayer->isDead())
    {
        mTaskQueue.enqueue(batch);
        mTaskQueue.enqueue(removeDeadEnemies());
        mTaskQueue.enqueue(removeDeadPlayer());
    }
    
    if(totalDamage > 0.0f)
    {
        mTaskQueue.enqueue(TaskLambda::make([=](){
            this->afterEnemyAttack();
        }));
    }
}
    
TaskPtr BoardLayer::removeDeadPlayer()
{
    TaskSequencePtr seq = TaskSequence::make();
    if(mPlayer->isDead())
    {
        // special handling of player death?
        if(onPlayerDeath())
        {
        }
        else if(!mStartPromptForRevival)
        {
            mStartPromptForRevival = true;
            // Player killed
            seq << createDefeatAnim(this, ccp(320, 800));
            seq << TaskLambda::make([=]()
            {
                promptForRevival();
            });
        }
    }
    return seq;
}

TaskPtr BoardLayer::updatePlayerPoison()
{
    auto batch = TaskBatch::make();
    
    //if(mPlayer->poison.has() && mPlayer->isAlive())
    if ( mPlayer->buffGroup.invokeBuffWhen("poison", batch,
        [=](BuffPtr){ return mPlayer->isAlive(); }))
    {
        float damage = mPlayer->buffGroup.getBuffContent<FloatBuff>("poison")->d1;
        damage = std::min(damage, mPlayer->getHealth() - 1); // leave player with 1 health
        batch << mPlayer->hit(damage, NULL);
    }
    
    return batch;
}

TaskPtr BoardLayer::updatePlayerSuckBlood()
{
    TaskSequencePtr seq = TaskSequence::make();

    //if(this->mPlayer->isAlive())
    if( mPlayer->buffGroup.invokeBuffWhen("bloodSucker", seq,
        [=](BuffPtr){ return mPlayer->isAlive(); }))
    {
        auto buff = mPlayer->buffGroup.getBuffContent<IntFloatBoolBuff>("bloodSucker");
        auto hc = mPlayer->team[buff->d1];
        auto batch = TaskBatch::make();
        EnemyControlList targets = this->getEnemiesInAttackOrder(hc->getColor(),
                                                                 buff->d3);
        float delay = 0.0f, max = 0.f;
        for(EnemyControlPtr ec : targets)
        {
            float finalDamage;
            batch << this->calcDamageForTarget(buff->d2,
                                               hc->getColor(),
                                               ec,
                                               finalDamage,
                                               false,
                                               delay + 0.3f);
            
            batch << this->createAttackSequence(hc,
                                                ec,
                                                finalDamage,
                                                hc->getColor(),
                                                delay);
            
            float acDelay = -0.15f+delay+calOrbDelay(hc->center(), ec->center());
            TaskPtr hit = this->dealDamageToOneWithAnim(finalDamage, ec, acDelay);
            batch << TaskSequence::make(createDelay(this, 0.5),
                                        hit);
            delay += 0.2f;
            
            max += finalDamage;
            //max = std::max(max, finalDamage);
        }
        seq << batch
            << healWithAnim(this, mPlayer.get(), max)
            << removeDeadEnemies();
    }

    return seq;
}
    

    
TaskPtr BoardLayer::updatePlayerTurn(BoardResult& result)
{
    TaskSequencePtr seq = TaskSequence::make();
    
    if(mPlayer->passiveBleeding.isValid() && mPlayer->isAlive())
    {
        float damage = sumPP(mPlayer->passiveBleeding);
        TaskBatchPtr batch = TaskBatch::make();
        batch << mPlayer->hit(damage, NULL);

        seq << batch;
    }
    
    if(mPlayer->isAlive())
    {
        TaskBatchPtr batch = TaskBatch::make();
        for(HeroControl* hc : mPlayer->team)
            batch << hc->updateCooldown();
        
        float bonusEnergy = 0.f;
        auto  bonusColor = GemUtils::AllColor;
        if( mPlayer->buffGroup.invokeBuff("bonusEnergy", batch) )
        {
            auto buff = mPlayer->buffGroup.getBuffContent<FloatBuff>("bonusEnergy");
            bonusEnergy += buff->d1;
            bonusColor = buff->getColor();
        }

        for(HeroControl* hc : mPlayer->team)
        {
            float minEnergy = std::max(mPlayer->mMinEnergy[hc->getColor()],
                                       mPlayer->mMinEnergy[GemUtils::AllColor]);
            if(hc->info.profile->color ==  bonusColor || bonusColor == GemUtils::AllColor)
            {
                minEnergy += bonusEnergy;
            }
            
            if(minEnergy != 0.0f)
                batch << hc->addEnergy(minEnergy);
        }
        
        seq << batch;
    }
    
    return seq;
}

TaskPtr BoardLayer::updateGemTurn()
{
    auto ret = TaskSequence::make();
    
    ret << TaskSound::make("sound/v/player_hit.mp3");
    ret << mBoardControl->updateGemGrid();
    
    if(mBoardControl->damageFromGems > 0.f)
    {
        ret << TaskSound::make("sound/v/player_hit.mp3");
        ret << createFloatText(this,
                               -mBoardControl->damageFromGems,
                               mPlayer->center(),
                               GemUtils::Health,
                               0.0f);
        ret << mPlayer->hit(mBoardControl->damageFromGems, this);
        mBoardControl->damageFromGems = 0.f;
    }
    
    ret << mBoardControl->fillBoard([=](){ return randGemWeighted(); }, false);
    
    return ret;
}
    
void BoardLayer::update(float dt)
{
    mStats.gameTimeSinceLevelStart += dt;
    mTaskQueue.update(dt);
}
    
static std::set<CCNode*> nodeMap;

static void _initRefMap(CCNode* root)
{
    nodeMap.insert(root);
    
    CCArray* children = root->getChildren();
    for(int i=0; children && i<children->count(); i++)
    {
        CCNode* node = (CCNode*)children->objectAtIndex(i);
        _initRefMap(node);
    }
}
    
static void initRefMap(CCNode* root)
{
#if PH_DEBUG_BUILD
    nodeMap.clear();
    _initRefMap(root);
#endif
}
    
static void compareRefMap(CCNode* root)
{
#if PH_DEBUG_BUILD
    if(!nodeMap.count(root))
    {
        printf("found new node %p\n", root);
    }
    
    CCArray* children = root->getChildren();
    for(int i=0; children && i<children->count(); i++)
    {
        CCNode* node = (CCNode*)children->objectAtIndex(i);
        compareRefMap(node);
    }
#endif
}
    
void BoardLayer::onTurn()
{
    // For tutorial purpose only
    mTaskQueue.enqueue(TaskLambda::make([=](){ this->onStartTurn(); }));
    
    this->disableUI();
    
    fillSweepUntilDone(mResult);
    
    // if still got moves, simply reanble UI
    auto batch = TaskBatch::make();
    if( mPlayer->buffGroup.invokeBuff("moveAgain", batch) )
    {
        batch << mPlayer->buffGroup.updateBuff("moveAgain");
        mTaskQueue.enqueue(batch);
        mTaskQueue.enqueue(TaskLambda::make([=]
        {
            this->enableUI();
            this->mBoardMask->setVisible(false);
        }));
    }
    else
    {
        this->startTurn();
    }
}

void BoardLayer::startTurn()
{
    mTaskQueue.enqueue( removeDeadPlayer() );
    
    {
        if( mPlayer->passiveShiled.isValid() )
        {
            auto i = mPlayer->passiveShiled.map().begin();
            if( mResult->comboCount() >= i->second.comboCount &&
               !mPlayer->buffGroup.isBuffActive("passive.shield.sentinel"))
            {
                auto batch = TaskBatch::make();
                //printf("%d\n", i->second.lastRound);
                auto buff = FloatIntBuff::make(i->first->getColor(),
                                               i->second.lastRound,
                                               "护盾",
                                               i->second.reflectFactor,
                                               i->second.shieldCount);
                
                batch << mPlayer->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn, "passive.shield.invoke", this, buff);
                auto sentinel = IBuff::make(GemUtils::AllColor, i->second.gapRound, "");
                batch << mPlayer->buffGroup.attachBuff(PlayerControl::kGroup_PostTurn,
                                              "passive.shield.sentinel", this, sentinel);
                mTaskQueue.enqueue(batch);
                
                batch = TaskBatch::make();
                mPlayer->buffGroup.invokeBuff("passive.shield.invoke", batch);
                mTaskQueue.enqueue(batch);
            }
        }
    }
    
    attack(*mResult);
    // clear result
    mResult = BoardResultPtr(new BoardResult(mPlayer->team));
        
    mTaskQueue.enqueue( updatePlayerPoison() );
    mTaskQueue.enqueue( updatePlayerSuckBlood() );
    mTaskQueue.enqueue( this->updateGemTurn() );

    //----------------------------
    // update player status turns
    {
        TaskBatchPtr batch = TaskBatch::make();
        for(HeroControl* hc : mPlayer->team)
            batch << hc->updateStatusTurn();
        batch << mPlayer->updateStatusTurn();
        
        mTaskQueue.enqueue(batch);
    }
    mTaskQueue.enqueue(TaskLambda::make([=]()
                                        {
                                            //                initRefMap(this);
                                            mBoardMask->setVisible(true);
                                        }));
    if(mEnemyList.size() == 0)
    {
        loadNextRoundIfDone();
        mTaskQueue.enqueue( removeDeadPlayer() );
        mTaskQueue.enqueue( mPlayer->updateStatusTurnPostEnemy() );
    }
    else
    {
        updateEnemyTurn(*mResult);
        mTaskQueue.enqueue( mPlayer->updateStatusTurnPostEnemy() );
    }
    
    mTurn++;
    
    TaskPtr playerTask = updatePlayerTurn(*mResult);
    
    TaskSequencePtr endSeq = TaskSequence::make();
    endSeq << playerTask;
    endSeq << TaskLambda::make([=]
                               {
                                   this->enableUI();
                                   this->mBoardMask->setVisible(false);
                                   //                compareRefMap(this);
                               });
    
    mTaskQueue.enqueue(endSeq);
    
    mTaskQueue.enqueue(TaskLambda::make([=](){
        //mTaskQueue.enqueue(this->showDragHint());
        mShowDragHintInTurn = true;
        showDragHintWithDelay(0.f);
        this->onEndOfTurn();
    }));
}
    
void BoardLayer::exitScene(bool isVictory)
{
    // this is the end of all tasks, clear task queue.
    mTaskQueue.clear();
    
    mStats.turnCount = mTurn;
    mStats.endRound = this->mRoundIndex;
    
    if(isVictory)
    {
        sendVictoryRequest();
    }
    else
    {
        transitionToGameScene(false, boost::make_shared<Player>());
    }
}
    
void BoardLayer::sendVictoryRequest()
{
    std::string token = PlayerData::getInstance()->getToken();
    std::string levelId = mLevel.id;
    
    SystemInform* alert = SystemInform::alertLoading(runningScene());
    
    disableUI();
    
    auto endLevel = [=](std::auto_ptr<Player> p,
                        std::auto_ptr<Error> e)
    {
        alert->cancel();
        
        if(e.get())
        {
            char errorTitle[128];
            sprintf(errorTitle, "连接失败: %s", e->toStr().c_str());
            
            LOGD("endGame error: %s", e->toStr().c_str());
            SystemInform::alertConfirmPanel(runningScene(),
                                            "无法连接到服务器，请检查网络连接后点击确定重试。取消的话会丢关卡奖励噢。",
                                            errorTitle,
                                            [=]()
                                            {
                                                // infinite retries
                                                sendVictoryRequest();
                                            },
                                            [=]()
                                            {
                                                transitionToGameScene(false,
                                                                      boost::make_shared<Player>());
                                            });
            return;
        }
        
        PlayerPtr player(p.release());
        
        transitionToGameScene(true, player);
    };
    
    int opLevel = PlayerData::getInstance()->isDailyInstance(mLevel.id) ? 1 : this->mStats.getOpLevel();

    PuzzleRPC::getInstance()->endGame(token,
                                      levelId,
                                      opLevel,
                                      endLevel);
}
    
void BoardLayer::transitionToGameScene(bool isVictory, PlayerPtr newPlayer)
{   
    for(auto& s : PlayerData::getInstance()->instanceStageList)
        if(s->findLevelById(mLevel.id) != NULL && s->dailyQuota > 0)
            s->dailyQuota--;
    for(auto& s : PlayerData::getInstance()->normalStageList)
        if(s->findLevelById(mLevel.id) != NULL && s->dailyQuota > 0)
            s->dailyQuota--;
    
    for(auto& s : PlayerData::getInstance()->instanceStageList)
    {
        if(s->findLevelById(mLevel.id) != NULL && s->dailyInstanceProgress != -1)
        {
            bool flag = false;
            for(auto l : s->levelList)
            {
                if(!l.cleared) flag = true;
            }
            if(!flag)
                PlayerData::getInstance()->removeDailyStageByLevelId(mLevel.id);
        }
    }
    
    LevelStats statsCopy = this->mStats;
    LevelInfo levelCopy = this->mLevel;
    PlayerControlPtr playerCopy = this->mPlayer;
    
    this->removeFromParentAndCleanup(true);
    
    GameScene* gameScene = GameScene::getInstance();
    CCDirector::sharedDirector()->replaceScene(gameScene);
    stopBgMusic();
    gameScene->initBG();
    
    SystemInform* alert = SystemInform::alertLoading(gameScene);
    runOnMainThread([=]()
    {
        runOnMainThread([=]()
        {
            alert->cancel();
            unloadBattleResources();
                        
            GameScene *gameScene = GameScene::getInstance();
            // load UI resources and recreate interface.
            gameScene->recoverUI();
            
            if(isVictory)
            {
                auto layer = VictoryLayer::create(statsCopy, levelCopy, playerCopy, newPlayer);
                gameScene->pushLayerWithDefaultAnimation(layer);
            }
            else
            {
                gameScene->battleSceneEnded();
            }
        });
    });
}
    
void BoardLayer::fillSweepUntilDone(BoardResultPtr result)
{
    for(;;)
    { 
        mTaskQueue.enqueue( mBoardControl->fillBoard([=](){ return randGemWeighted(); },
                                                     mForceGemNoStacking)
                           );
        mTaskQueue.enqueue( mBoardControl->sweepBoard(mPlayer,
                                                      result) );
        
        if(!mBoardControl->hasEmptyCell())
            break;
    }
 
    mStats.maxCombo = std::max(mStats.maxCombo, result->comboCount());
    mStats.totalCombo += result->comboCount();

    int index = this->mRoundIndex - 1;
    mStats.maxComboPerLevel[index] = std::max(mStats.maxComboPerLevel[index], result->comboCount());
    mStats.roundPerLevel[index] ++;

    mStats.maxGemCount = std::max(mStats.maxGemCount, result->totalGemCount());
    
    auto batch = TaskBatch::make();
    for(auto enemy : this->mEnemyList)
    {
        batch << enemy->onAfterSweep(this, result->comboCount());
    }
    mTaskQueue.enqueue(batch);
    mTaskQueue.enqueue( mPlayer->onAfterSweep() );
    
    // this awkward callback exists only for tutorial mode
    mTaskQueue.enqueue(TaskLambda::make([=]() { this->onEndOfFillSweepCycle(); }));
}
    
void BoardLayer::showDragHintWithDelay(float delay)
{
    this->scheduleOnce(schedule_selector(BoardLayer::showDragHint), delay);
}
    
void BoardLayer::showArrowPath(const std::vector<int> & path,
                               int opacity)
{
    TaskBatchPtr batch = TaskBatch::make();

    this->clearArrowPath();
    for(int i=0; i<path.size(); i++)
    {
        CCSprite * arrow = NULL;
        
        if(i != path.size() -1 && i!=0 &&
           path[i]-path[i-1] != path[i+1]-path[i])
        {
            arrow = GetSprite("jiantou_2.png");
            int d1 = path[i+1]-path[i];
            int d2 = path[i]-path[i-1];
            
            if((d1 == -1 && d2 == 6) || (d1 == -6 && d2 == 1))
            {
                arrow->setRotation(0);
            }else if((d1 == -1 && d2 == -6) || (d1 == 6 && d2 == 1))
            {
                arrow->setRotation(90);
            }else if((d1 == 6 && d2 == -1) || (d1 == 1 && d2 == -6))
            {
                arrow->setRotation(180);
            }else if((d1 == 1 && d2 == 6) || (d1 == -6 && d2 == -1))
            {
                arrow->setRotation(-90);
            }
        }else
        {
            if(i==0)
                arrow = GetSprite("jiantou_3.png");
            else if(i == path.size() - 1)
                arrow = GetSprite("jiantou_1.png");
            else
                arrow = GetSprite("jiantou_4.png");
            int delta;
            if(i != path.size() -1)
                delta = path[i+1] - path[i];
            else
                delta = path[i] - path[i-1];
            
            if(delta == 1) arrow->setRotation(180);
            if(delta == skBoardWidth) arrow->setRotation(90);
            if(delta == -skBoardWidth) arrow->setRotation(-90);
        }
    
        arrow->setAnchorPoint(ccp(0.5f, 0.5f));
        Vec2i pos(path[i]% skBoardWidth , path[i] / skBoardWidth);
        arrow->setPosition(g2w_center(pos));
        
        mBoardControl->root->addChild(arrow, 9999999);
        arrow->setOpacity(0);
        batch << TaskAnim::make(arrow, CCFadeTo::create(0.3f, opacity), false);
        
        this->mDragHint.push_back(arrow);
        
        //printf("%d ", path[i]);
    }
    //printf("\n");
    this->mTaskQueue.enqueue(batch);
}
    
void BoardLayer::showDragHint()
{
    if(!GameConfig::shared()->showDragHint || this->mIsTutorialMode || !this->mShowDragHintInTurn) return;
    std::vector<int> path = genReasonableFingerPath(mBoardControl->getGrid());
    this->showArrowPath(path);
}
    
void BoardLayer::clearArrowPath()
{
    TaskBatchPtr batch = TaskBatch::make();
    for(auto & item : this->mDragHint)
    {
        TaskSequencePtr localSeq = TaskSequence::make();
        localSeq << TaskAnim::make(item, CCFadeOut::create(0.3f), false);
        localSeq << TaskLambda::make([=]()
        {
            item->removeFromParent();
        });
        
        batch << localSeq;
    }
    this->mDragHint.clear();
    
    this->mTaskQueue.enqueue(batch);
}

static bool moveStarted = false;
    
void BoardLayer::endDrag()
{
    this->clearArrowPath();
    
    mCurrTouch = NULL;
    moveStarted = false;
    mPlayer->stopTicking();
    
    if(mDragGem != NULL)
    {
        mDragGem->root->setAnchorPoint(ccp(0.5f, 0.5f));
        mDragGem->root->setPosition(g2w_center(mDragGem->position));
        mDragGem.reset();
    
        mShadowGem->removeFromParentAndCleanup(true);
    }
    
    // satisfies final board state constraint
    if(finalStateConstraint.call(mBoardControl->getGrid()))
    {
        return;
    }
    else
    {
        // final state constraint is only valid before it's met first time.
        // reset final state constraint
        finalStateConstraint.call = [](GemGrid& g)->bool{return false;};
    }
    
    // must immediately disable UI on end of a drag events,
    // to prevent from receiving incomplete touch began/end pair.
    disableUI();
    
    mTaskQueue.enqueue(TaskLambda::make([=](){ this->onTurn();} ));
}

// default implements are used to call script callback if exist
bool BoardLayer::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    if(mCurrTouch)
        return false;
    else
        mCurrTouch = touch;
    
    CCPoint p = this->convertTouchToNodeSpace(touch);
    
    Vec2i gp = w2g(p);
    mDragGem.reset();
    
    if(mBoardControl->getGrid().isInside(gp))
    {
        mShowDragHintInTurn = false;
        GemPtr pGem = mBoardControl->get(gp);
        if(pGem && pGem->isMoveable())
        {
            mDragGem = pGem;
            
            CCSprite* sprite = mDragGem->root;
            sprite->setAnchorPoint(ccp(0.5, 0.25));
            
            // force this sprite on top of all others
            sprite->removeFromParentAndCleanup(false);
            mBoardControl->root->addChild(sprite);
            
            mShadowGem = createShadowGem(mDragGem->color());
            mShadowGem->setPosition(sprite->getPosition());
            mBoardControl->root->addChild(mShadowGem, -1);
            
            return true;
        }
    }
    return true;
}

void BoardLayer::ccTouchMoved(CCTouch *touch, CCEvent *event)
{
    CCPoint p = this->convertTouchToNodeSpace(touch);
    
    mPlayer->moveTimer.setPosition(p);
    
    // if dragging a gem
    if(mDragGem != NULL)
    {
        p.x = clampx(p.x, 0.0f, 620.0f);
        p.y = clampx(p.y, 0.0f, 515.0f);
        
        mDragGem->root->setAnchorPoint(ccp(0.5, 0.25));
        mDragGem->root->setPosition(p);
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
                    auto task = mPlayer->startTicking(mTurn, [=]()
                    {
                        this->endDrag();
                        this->mTaskQueue.enqueue( mPlayer->buffGroup.updateBuff("disableSweep") );
                    });
                    mTaskQueue.enqueue(task);
                    moveStarted = true;
                }
                
                TaskBatchPtr batch = TaskBatch::make();
                
                TaskPtr moveDstGem = moveGemAnim(dstGem->root, mDragGem->position);
                TaskPtr moveShadowGem = moveGemAnim(mShadowGem, dstGem->position);
                
                // swap grid item
                std::swap(mBoardControl->getGrid()(mDragGem->position),
                          mBoardControl->getGrid()(dstGem->position));
                // swap position of the two gems
                std::swap(dstGem->position, mDragGem->position);
                
                mTaskQueue.enqueue(TaskBatch::make(moveDstGem, moveShadowGem));

                ring("sound/v/drag_gem.mp3");
            }
        }
    }
}
    
void BoardLayer::ccTouchEnded(CCTouch *touch, CCEvent *event)
{
    mCurrTouch = NULL;
    
    // TODO: merge this ugliness to endDrag
    if(mDragGem)
    {
        // player started dragging but no swapping has happened yet
        // do not count this as a move.
        auto batch = TaskBatch::make();
        if(!moveStarted || mPlayer->buffGroup.invokeBuff("disableSweep", batch) )
        {
            mTaskQueue.enqueue(batch);
            
            mDragGem->root->setAnchorPoint(ccp(0.5f, 0.5f));
            mDragGem->root->setPosition(g2w_center(mDragGem->position));
            mDragGem.reset();
            
            mShadowGem->removeFromParentAndCleanup(true);
            return;
        }
        else
        {
            endDrag();
        }
    }
    else
    {
        //------------------------------------------------------
        // When no gem is dragged, assuming this is a touch event
        // in enemy area. Handle targeting on enemies.
        for(EnemyControlPtr ec : mEnemyList)
        {
            CCRect bound = ec->sprite->boundingBox();
            
            CCPoint touchPointLocal = ec->root->convertTouchToNodeSpace(touch);
            
            if(bound.containsPoint(touchPointLocal))
            {
                if(mManualTarget == ec)
                {
                    mManualTarget.reset();
                    mTargetCross->setOpacity(0);
                }
                else
                {
                    mManualTarget = ec;
                    mTargetCross->setPosition(ec->center());
                    mTargetCross->setOpacity(255);
                }
                break;
            }
        }
    }
}

void BoardLayer::ccTouchCancelled(CCTouch *touch, CCEvent *event)
{
    LOGD("touch cancelled\n");
    ccTouchEnded(touch, event);
}
    
}
