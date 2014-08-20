//
//  Victory.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 12/14/12.
//
//

#include "Victory.h"
#include "GameScene.h"
#include "PlayerData.h"
#include "JiangHu.h"
#include "CCCallLambda.h"
#include "UIPresets.h"
#include "HeroProfileLayer.h"
#include "network/PuzzleRPC.h"
#include "LayoutUtils.h"
#include "PlayerControl.h"
#include <extensions/utils.h>

namespace PH
{
    USING_NS_CC;
    
#define VICTORY_SAFE_RELEASE(target) \
    if(NULL != target){ \
        if(NULL != target->getParent()) \
            target->removeFromParentAndCleanup(true); \
        target = NULL;\
    }

    bool AddFriendLayer::init()
    {
        if(!ContentLayer::init())
            return false;
        
        // name label and bg
        CCSprite* nameBG = GetSprite("gameui/yuanjunjiesuan.png");
        nameBG->setPosition(ccp(134, 292));
        nameBG->setAnchorPoint(ccp(0.0, 0.0));
        
        CCLabelTTF* nameLabel = CCLabelTTF::create(PlayerData::getInstance()->lastAllyPlayer->name.c_str(),
                                                   FONT_CN,
                                                   28);
        nameLabel->setAnchorPoint(ccp(0, 0));
        nameLabel->setPosition(ccp(8, 2));
        nameBG->addChild(nameLabel);
        
        // level label and bg
        CCSprite* levelBG = GetSprite("gameui/yuanjunjiesuan.png");
        levelBG->setPosition(ccp(134, 246));
        levelBG->setAnchorPoint(ccp(0.0, 0.0));
        
        int levelNum = PlayerData::getInstance()->lastAllyPlayer->level;
        CCLabelTTF* levelLabel = CCLabelTTF::create((std::string("等级：") + toStr(levelNum)).c_str(),
                                                    FONT_CN,
                                                    28);
        levelLabel->setAnchorPoint(ccp(0.0, 0.0));
        levelLabel->setPosition(ccp(8, 2));
        levelBG->addChild(levelLabel);
        
        
        // layer bg
        CCScale9Sprite* layerBG = Get9SpriteFromFrame("dialog_bg.png",
                                                      CCRectMake(50, 50, 208, 112));
        /*
        CCScale9Sprite::create("ui/dialog_bg.png",
                                                         CCRectMake(0, 0, 308, 212),
                                                         CCRectMake(50, 50, 208, 112));
         */
        layerBG->setAnchorPoint(ccp(0.5f, 1.0));
        layerBG->setPosition(ccp(0, -56));

        layerBG->setContentSize(CCSize(500, 400));
        
        
        layerBG->addChild(nameBG);
        layerBG->addChild(levelBG);
        
        this->addChild(layerBG);
        
        // buttons
        CCMenu* menu = CCMenu::create();
        CCMenuItem* confirm = CCMenuItemSprite::create(createBtn("确定", ButtonType::NORMAL_RED),
                                                       createBtn("确定", ButtonType::NORMAL_RED),
                                                       this,
                                                       menu_selector(AddFriendLayer::confirm));
        
        CCMenuItem* cancel = CCMenuItemSprite::create(createBtn("取消", ButtonType::NORMAL_BLUE),
                                                      createBtn("取消", ButtonType::NORMAL_BLUE),
                                                      this,
                                                      menu_selector(AddFriendLayer::cancel));
        
        menu->addChild(confirm);
        menu->addChild(cancel);
        menu->alignItemsHorizontallyWithPadding(25);
        menu->setPosition(ccp(237, 60));
        layerBG->addChild(menu);
        
        CCLabelTTF* title = CCLabelTTF::create("添加好友", FONT_CN, 30);
        title->setPosition(ccp(237, 350));
        title->setColor(TITLE_YELLOW);
        title->setAnchorPoint(ccp(0.5, 0));
        layerBG->addChild(title);
        
        CCLabelTTF* desc = CCLabelTTF::create("要添加为好友吗?\n添加好友后，\n可以随时使用他为援军.\n",
                                              FONT_CN,
                                              28);
        desc->setPosition(ccp(241, 130));
        desc->setAnchorPoint(ccp(0.5, 0));
        layerBG->addChild(desc);
        
        HeroIcon* icon = HeroIcon::create(PlayerData::getInstance()->lastAllyPlayer->team[0]);
        icon->setPosition(ccp(75, 280));
        
        layerBG->addChild(icon);
        
        isShowTop = true;
        
        return true;
    }
    
    void AddFriendLayer::onEnterAnimationDone()
    {
        ContentLayer::onEnterAnimationDone();
        if(GameConfig::shared()->getTutorialProgress() == TutorialSpendGoldOnHero)
        {
            ArrowMask::shared()->setMask(CCRect(0, 140, 640, 685));
            ArrowMask::shared()->setArrow(ccp(220, 340), ArrowDirection::Up);
        }
    }
    
    void AddFriendLayer::confirm(CCObject* o)
    {
        PlayerPtr ally = PlayerData::getInstance()->lastAllyPlayer;
        PlayerData::getInstance()->lastAllyPlayer.reset();
        
        std::string uid = ally->uid;
        
        PuzzleRPC::getInstance()->addFriend(PlayerData::getInstance()->getToken(),
                                            uid,
                                            [=](std::auto_ptr<Error> e)
        {
            if(e.get())
            {
                std::cout << "!!! addFriend error: " << e->toStr() << std::endl;
            }
        });
        
        //this->setTouchEnabled(false);
        GameScene::getInstance()->battleSceneEnded();
        CCTextureCache::sharedTextureCache()->removeUnusedTextures();
    }
    
    void AddFriendLayer::cancel(CCObject* o)
    {
        GameScene::getInstance()->battleSceneEnded();
    }

    const int VictoryLayer::offset = 150;
    //----------------------
    // Victory Layer
    bool VictoryLayer::init(const LevelStats & stats,
                            const LevelInfo& levelInfo,
                            PlayerControlPtr control,
                            PlayerPtr newplayer)
    {
        if(!ContentLayer::init())
            return false;
        
        mLevelInfo = levelInfo;
        mStats = stats;
        
        //
        if(mStats.turnCount <= 0) mStats.turnCount = 1;
        
        dropLayer = NULL;
        labelLayer = NULL;
    
        this->newPlayer = newplayer;
        this->newPlayer->availableRewardsCount = std::min((int)this->newPlayer->availableRewardsCount,
                                                          (int)this->newPlayer->rewards.size());
        //---------------------
        // Fix tutorial state
        //---------------------
        if(GameConfig::shared()->getTutorialProgress() == TutorialFirstBattle)
        {
            GameConfig::shared()->setTutorialProgress(TutorialSpendGoldOnHero);
            ArrowMask::shared()->clear();
        }
        
        mLevelInfo = levelInfo;
        
        isShowBottom = false;
        setTouchEnabled(true);
                
        //------------
        // UI
        //------------
        bg = GetSprite("gameui/shengli_bg.png");
        bg->setAnchorPoint(ccp(0.0f, 1.0f));
        bg->setPosition(ccp(-320 + 60, 0));
        
        this->addChild(bg);
        
        // create confirm menu
        {
            confirmButton = CCMenuItemSprite::create(createBtn("确定", ButtonType::NORMAL_RED),
                                                     createBtn("确定", ButtonType::NORMAL_RED),
                                                     this,
                                                     menu_selector(VictoryLayer::confirm));
            
            skipChestButton = CCMenuItemSprite::create(createBtn("跳过", ButtonType::NORMAL_RED),
                                                       createBtn("跳过", ButtonType::NORMAL_RED),
                                                       this,
                                                       menu_selector(VictoryLayer::skipChestPick));
            
            skipGoldsButton = CCMenuItemSprite::create(createBtn("跳过", ButtonType::NORMAL_RED),
                                                       createBtn("跳过", ButtonType::NORMAL_RED),
                                                       this,
                                                       menu_selector(VictoryLayer::skipGolds));
            
            skipLevelUpButton = CCMenuItemSprite::create(createBtn("跳过", ButtonType::NORMAL_RED),
                                                          createBtn("跳过", ButtonType::NORMAL_RED),
                                                          this,
                                                          menu_selector(VictoryLayer::skipLevelUp));
            
            LayoutApply<CCMenuItem*>(VA(confirmButton,
                                        skipChestButton,
                                        skipGoldsButton,
                                        skipLevelUpButton, NULL),
            [=]( CCMenuItem* x)
            {
                x->setAnchorPoint(ccp(0.5, 0));
                x->setPosition(bg->boundingBox().size.width*0.5f-offset, 50);
                x->setVisible(false);
            });
            
            CCMenu* menu = CCMenu::create(confirmButton,
                                          skipChestButton, skipGoldsButton, skipLevelUpButton,
                                          NULL);
            menu->setAnchorPoint(ccp(0, 0));
            menu->setPosition(0, 0);
            
            bg->addChild(menu);
        }
        
        // clear delta
        this->deltaExp = this->deltaCoins = this->deltaGolds = 0;
        
        // PassiveSkillBonusCoins && PassiveSkillBonusExp

        if(control->passiveBonusCoins.isValid())
        {
            this->deltaCoins = true;
        }
        if(control->passiveBonusExp.isValid())
        {
            this->deltaExp = true;
        }
        // ->>>
        transitionToAccount(true);
        // ->>>
        
        //mLevelInfo.golds = 10;
        return true;
    }
    
    void VictoryLayer::transitionToAccount(bool withRewards)
    {
        if(withRewards && newPlayer->rewards.size() && newPlayer->availableRewardsCount)
        {
            
            if(!PlayerData::getInstance()->isDailyInstance(mLevelInfo.id))
                showRewards();
            else
                transitionToAccount(false);
            
            //TutorialControl::shared()->ScoreTutorialStep1(this);
            return;
        }
        
        PlayerData* data = PlayerData::getInstance();
        VICTORY_SAFE_RELEASE(labelLayer);
        labelLayer = CCLayer::create();
        {
            labelLayer->setPosition(ccp(-260, -430));
            labelLayer->setAnchorPoint(ccp(0.f, 0.f));
            
            std::string levelId = mLevelInfo.id;
            
            PlayerData* playerData = PlayerData::getInstance();
            std::vector<StagePtr> stageList = playerData->normalStageList;
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
            
            CCLabelTTF* stageLabel = CCLabelTTF::create(stageName.c_str(), FONT_CN, 35);
            stageLabel->setPosition(ccp(-50, 332));
            stageLabel->setAnchorPoint(ccp(0.0, 0.0));
            stageLabel->setColor(ccBLACK);
            
            CCLabelTTF* levelLabel = CCLabelTTF::create(levelName.c_str(), FONT_CN, 35);
            levelLabel->setPosition(ccp(250, 292));
            levelLabel->setAnchorPoint(ccp(1.0, 0.0));
            levelLabel->setColor(ccBLACK);

            // coin label
            CCLabelTTF* coinTextLabel = CCLabelTTF::create("奖励金钱", FONT_CN, 35);
            CCLabelTTF* coinNum = CCLabelTTF::create(toStr(0).c_str(), FONT_NUM, 32);
            if(this->deltaCoins) coinNum->setColor(ccc3(0,255,0));
            labelLayer->addChild(coinNum);
            
            CCFiniteTimeAction* count = CCCountBMTextNumber::create(0, mLevelInfo.coins, 1.5f);
            CCFiniteTimeAction * action = CCSpawn::create(
                                                          CCEaseBounceOut::create(CCFadeIn::create(0.6f)),
                                                          CCMoveBy::create(0.4f, ccp(offset,0)),
                                                          NULL);
            coinNum->setOpacity(0);
            coinNum->runAction(CCSequence::create(CCDelayTime::create(0.5f),
                                            action,
                                            count,
                                            NULL));
            
            // exp label
            CCLabelTTF* expLabel = CCLabelTTF::create("奖励经验", FONT_CN, 35);
            CCLabelTTF* expNum = CCLabelTTF::create(toStr(0).c_str(), FONT_NUM, 32);
            if(this->deltaExp) expNum->setColor(ccc3(0,255,0));
            labelLayer->addChild(expNum);
            
            CCFiniteTimeAction* exp_count = CCCountBMTextNumber::create(0, mLevelInfo.exp, 1.5f);
            CCFiniteTimeAction* exp_action = CCSpawn::create(CCEaseBounceOut::create(CCFadeIn::create(0.6f)),
                                     CCMoveBy::create(0.4f, ccp(offset,0)), NULL);
            expNum->setOpacity(0);
            expNum->runAction(CCSequence::create(CCDelayTime::create(0.5f),
                                                 exp_action,
                                                 exp_count,
                                                 NULL));
            
            // next level label
            CCLabelTTF* nextLevelLabel = CCLabelTTF::create("下次升级", FONT_CN, 35);
            int nextLevel = newPlayer->nextLevelExp - newPlayer->exp;
            CCLabelTTF* nextLevelNum = CCLabelTTF::create(toStr(0).c_str(), FONT_NUM, 32);
        
            labelLayer->addChild(nextLevelNum);
            
            CCFiniteTimeAction* level_count = CCCountBMTextNumber::create(0, nextLevel, 1.5f);
            CCFiniteTimeAction* level_action = CCSpawn::create(CCEaseBounceOut::create(CCFadeIn::create(0.6f)),
                                     CCMoveBy::create(0.4f, ccp(offset,0)), NULL);
            nextLevelNum->setOpacity(0);
            nextLevelNum->runAction(CCSequence::create(CCDelayTime::create(0.5f),
                                                       level_action,
                                                       level_count,
                                                       NULL));
            // layouts

            LayoutColumn(ccp(240-offset, 268),
                         ccp(1.f, 1.f),
                         50.f,
                         true,
                         VA(coinTextLabel, expLabel, nextLevelLabel, NULL));
            
            LayoutColumn(ccp(415-offset, 268),
                         ccp(1.f, 1.f),
                         50.f,
                         true,
                         VA(coinNum, expNum, nextLevelNum, NULL));
            
            LayoutApply<CCLabelTTF*>(VA(coinTextLabel,
                                        expLabel,
                                        nextLevelLabel,
                                        stageLabel, levelLabel, NULL),
                                    [=]( CCLabelTTF * x)
                                    {
                                        labelLayer->addChild(x);
                                        CCFiniteTimeAction * action = CCSpawn::create(
                                                                            CCEaseBounceOut::create(CCFadeIn::create(0.6f)),
                                                                            CCMoveBy::create(0.4f, ccp(offset,0)), NULL);
                                        x->setOpacity(0);
                                        x->runAction(CCSequence::create(CCDelayTime::create(0.5f),
                                                                        action,
                                                                        NULL));
                                    });
        }
        this->addChild(labelLayer);
        
        if(newPlayer->level > PlayerData::getInstance()->player->level)
        {
            showLevelUp(*PlayerData::getInstance()->player, *newPlayer);
        }
        else if(mLevelInfo.golds > 0)
        {
            showGold();
        }
        else
        {
            showDrops();
        }
        
        int opLevel = PlayerData::getInstance()->isDailyInstance(mLevelInfo.id)
                        ? 1 : this->mStats.getOpLevel();
        
        std::string levelId = mLevelInfo.id;
        
        // update player data last
        data->updatePlayer(newPlayer.get());
        
        if(!data->findLevel(levelId)->cleared)
            data->findLevel(levelId)->cleared = opLevel;
        else
            data->findLevel(levelId)->cleared = std::min(data->findLevel(levelId)->cleared, opLevel);
        
        // clear saved jianghu state if a new stage is unlocked
        auto stage = PlayerData::getInstance()->findStage(GameConfig::shared()->stageId);
        if(stage)
        {
            bool stageCleared = true;
            for(const Level& l : stage->levelList)
            {
                if(!l.cleared)
                    stageCleared = false;
            }
            if(stageCleared)
                GameConfig::shared()->stageId = -1;
        }
    }
    
    void VictoryLayer::exitConfirm(cocos2d::CCObject *o)
    {
        CCTextureCache::sharedTextureCache()->removeUnusedTextures();
        
        if(!PlayerData::getInstance()->lastAllyPlayer->isFriend)
            GameScene::getInstance()->pushLayerWithDefaultAnimation( AddFriendLayer::create() );
        else
        {
            GameScene::getInstance()->battleSceneEnded();
        }
    }
    
	void VictoryLayer::confirm(CCObject *target)
    {
        if(PlayerData::getInstance()->isDailyInstance(mLevelInfo.id) &&
           newPlayer->rewards.size() && newPlayer->availableRewardsCount)
        {
            showDailyRewards(target);
        }else
            exitConfirm(target);
    }
    
    void VictoryLayer::onLongClick(CCTouch* touch)
    {
        if(dropLayer && dropIcons.size() > 0)
        {
            CCPoint localPoint = dropLayer->convertTouchToNodeSpace(touch);
            for(HeroIcon* icon : dropIcons)
                if(icon->boundingBox().containsPoint(localPoint))
                {
                    auto* layer = HeroProfileLayer::create(icon->getHero(), false);
                    GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
                    return;
                }
        }
    }
    
    void VictoryLayer::showDailyRewards(CCObject *target)
    {
        SystemInform::alertRewardTips(runningScene(), "通关奖励", newPlayer->rewards, [=]()
        {
            exitConfirm(target);
        });
    }
    
    
    void VictoryLayer::showRewards()
    {
        this->setTouchEnabled(false);
        
        VICTORY_SAFE_RELEASE(labelLayer);
        labelLayer= CCLayer::create();
        
        {
            labelLayer->setPosition(ccp(-225, -430));
            labelLayer->setAnchorPoint(ccp(0.f, 0.f));
        }

        {
            const int kLeftSize = 30;
            const int kRightSize = 27;
            
            
            // coin label
            CCLabelTTF* coinTextLabel = CCLabelTTF::create("最高连击数", FONT_CN, kLeftSize);
            CCLabelTTF* coinNum = CCLabelTTF::create(toStr(0).c_str(), FONT_NUM, kRightSize);
            
            CCFiniteTimeAction* count = CCCountBMTextNumber::create(0, mStats.maxCombo, 1.0f);
            labelLayer->addChild(coinNum);
            CCFiniteTimeAction * action = CCSpawn::create(
                                                          CCEaseBounceOut::create(CCFadeIn::create(0.6f)),
                                                          CCMoveBy::create(0.4f, ccp(offset,0)), NULL);
            coinNum->setOpacity(0);
            coinNum->runAction(CCSequence::create(CCDelayTime::create(0.4f),
                                                  action,
                                                  count,
                                                  NULL));
            // exp label
            CCLabelTTF* expLabel = CCLabelTTF::create("平均连击数", FONT_CN, kLeftSize);
            CCLabelTTF* expNum = CCLabelTTF::create(toStr(0).c_str(), FONT_NUM, kRightSize);
            
            count = CCCountBMTextNumber::create(0, (int)((float)mStats.totalCombo/(float)mStats.turnCount), 1.0f);
            labelLayer->addChild(expNum);
            action = CCSpawn::create(CCEaseBounceOut::create(CCFadeIn::create(0.6f)),
                                     CCMoveBy::create(0.4f, ccp(offset,0)), NULL);
            expNum->setOpacity(0);
            expNum->runAction(CCSequence::create(CCDelayTime::create(0.4f),
                                                  action,
                                                  count,
                                                  NULL));
            
            CCLabelTTF* roundTextLabel = CCLabelTTF::create("通关回合数", FONT_CN, kLeftSize);
            CCLabelTTF* roundNum = CCLabelTTF::create(toStr(0).c_str(), FONT_NUM, kRightSize);
            
            count = CCCountBMTextNumber::create(0, mStats.turnCount, 1.0f);
            labelLayer->addChild(roundNum);
            action = CCSpawn::create(CCEaseBounceOut::create(CCFadeIn::create(0.6f)),
                                     CCMoveBy::create(0.4f, ccp(offset,0)), NULL);
            roundNum->setOpacity(0);
            roundNum->runAction(CCSequence::create(CCDelayTime::create(0.4f),
                                                   action,
                                                   count,
                                                   NULL));
            
            // next level label
            CCLabelTTF* nextLevelLabel = CCLabelTTF::create("复活次数", FONT_CN, kLeftSize);
            CCLabelTTF* nextLevelNum = CCLabelTTF::create(toStr(0).c_str(), FONT_NUM, kRightSize);
            
            count = CCCountBMTextNumber::create(0, mStats.reviveCount, 1.0f);
            labelLayer->addChild(nextLevelNum);
            action = CCSpawn::create(CCEaseBounceOut::create(CCFadeIn::create(0.6f)),
                                     CCMoveBy::create(0.4f, ccp(offset,0)), NULL);
            nextLevelNum->setOpacity(0);
            nextLevelNum->runAction(CCSequence::create(CCDelayTime::create(0.4f),
                                                 action,
                                                 count,
                                                 NULL));
            
            CCSprite * title = GetSprite("guanqiapingfen.png");
            
            // layouts
            title->setAnchorPoint(ccp(0, 0));
            title->setPosition(ccp(65-offset, 300));
            
            LayoutColumn(ccp(220-offset, 250),
                         ccp(1.f, 1.f),
                         40.f,
                         true,
                         VA(coinTextLabel, expLabel, roundTextLabel, nextLevelLabel, NULL));
            
            LayoutColumn(ccp(360-offset, 250),
                         ccp(1.f, 1.f),
                         40.f,
                         true,
                         VA(coinNum, expNum, roundNum, nextLevelNum, NULL));
            
            LayoutApply<CCLabelTTF*>(VA(coinTextLabel, 
                                        expLabel, 
                                        roundTextLabel, 
                                        nextLevelLabel, 
                                        title, NULL),
                                     [=]( CCLabelTTF * x)
            {
                labelLayer->addChild(x);
                CCFiniteTimeAction * action = CCSpawn::create(
                    CCEaseBounceOut::create(CCFadeIn::create(0.5f)),
                    CCMoveBy::create(0.4f, ccp(offset,0)),NULL);
                x->setOpacity(0);
                x->runAction(CCSequence::create(CCDelayTime::create(0.3f),
                                                action,
                                                NULL));
            });
            
            char name[128];
            sprintf(name, "pingfen%d.png", this->mStats.getOpLevel());
            CCSprite * score = GetSprite(name);
            score->setOpacity(0);
            score->setAnchorPoint(ccp(0.5,0.5));
            score->setPosition(ccp(350,350));
            score->setScale(2.5f);
            
            action = CCSpawn::create(
                            CCEaseBounceOut::create(CCFadeIn::create(0.5f)),
                            CCEaseBounceOut::create(CCScaleTo::create(0.5f, 1.f)), NULL);
            
            score->runAction(CCSequence::create(CCDelayTime::create(0.6f),
                                                action,
                                                NULL));
            labelLayer->addChild(score);
        }
        
        VICTORY_SAFE_RELEASE(dropLayer);
        dropLayer = CCLayer::create();
        
        {
            dropLayer->setPosition(ccp(-600, -480));
            dropLayer->runAction(CCSequence::create(CCDelayTime::create(1.0),
                                                    CCEaseBounceOut::create(CCMoveBy::create(0.6, ccp(600, 0))),
                                                    NULL));
        }
        
        {
            CCSprite * subTitle = GetSprite("kaibaoxiang.png");
            dropLayer->addChild(subTitle);
            
            for(int i=0; i<newPlayer->rewards.size(); i++)
            {
                /*
                printf("%d %d %d", newPlayer->rewards[i].first,
                       newPlayer->rewards[i].second.first,
                       newPlayer->rewards[i].second.second);
                */
                CCSprite * chest = GetSprite("gameui/baoxiangzhengmian.png");
                this->chests.push_back(chest);
            }
            
            //printf("with size %d \n\n\n", rewards->val.size());
            
            LayoutRow(ccp(220, -130),
                      ccp(0.5f, 0.5f),
                      150.f,
                      true,
                      this->chests);
            
            LayoutApply<CCSprite*>(this->chests,
                                   [=]( CCSprite * x)
            {
                dropLayer->addChild(x);
                x->setOpacity(0);
                x->runAction(CCSequence::create(CCDelayTime::create(1.0f),
                                                CCFadeIn::create(0.3f),
                                                CCCallLambda::create([=]()
                                                                     {
                                                                         this->setTouchEnabled(true);
                                                                     }),
                                                NULL));
            });
        }
        
        this->addChild(dropLayer);
        this->addChild(labelLayer);
    }

    void VictoryLayer::showDrops()
    {
        // hero reward tag
        VICTORY_SAFE_RELEASE(dropLayer);
        dropLayer = CCLayer::create();
    
        {
            dropLayer->setPosition(ccp(-400 - 320, 0));
            dropLayer->runAction(CCSequence::create(CCDelayTime::create(0.5),
                                                    CCEaseBounceOut::create(CCMoveTo::create(0.7, ccp(0, 0))),
                                                    CCDelayTime::create(1.5),
                                                    NULL));
        }
        
        {
            CCSprite* heroTag = GetSprite("huodexiake.png");
            heroTag->setAnchorPoint(ccp(0.5, 0.0));
            heroTag->setPosition(ccp(0, -500));
            heroTag->setScale(0.8f);
            dropLayer->addChild(heroTag);
        }

        for(int i=0; i<mLevelInfo.roundList.size(); ++i)
        {
            const RoundInfo& r = mLevelInfo.roundList[i];
            for(const EnemyInfo& e : r)
            {
                if(e.drop)
                    deltaHero.push_back(e.drop->heroId);
                else if(e.soulDrop)
                    deltaSoul.push_back(std::make_pair(e.soulDrop->heroId, e.soulDrop->count));
            }
        }
        
        for(int i=0; i<deltaHero.size() + deltaSoul.size(); ++i)
        {

            int xi = i % 5;
            int yi = i / 5;
            CCPoint p = ccp(50 + xi * 110 - 320,  -640 - yi * 110);
            
            HeroIcon* icon = NULL;
            
            if(i < deltaHero.size())
            {
                icon = HeroIcon::createWithProfile(&HPT(deltaHero[i]));
            }
            else
            {
                auto val = this->deltaSoul[i-this->deltaHero.size()];
                icon = HeroIcon::createWithProfile(&HPT(val.first), true);
                icon->showPlusNum(val.second);
            }
            icon->setPosition(p);
            icon->setAnchorPoint(ccp(0, 0));
            this->dropIcons.push_back(icon);
            dropLayer->addChild(icon);
        }
        
        confirmButton->setVisible(true);
        
        CCAction * action = CCSpawn::create(CCEaseBounceOut::create(CCFadeIn::create(0.6f)),
                                               CCMoveBy::create(0.4f, ccp(offset, 0)), NULL);
        confirmButton->setOpacity(0);
        confirmButton->runAction(action);
        
        this->addChild(dropLayer);
    }
    
    void VictoryLayer::showGold()
    {
        CCSprite* s = GetSprite("gameui/chucitongguan.png");
        
        {
            CCSprite * bg = GetSprite("gameui/tongguanyuanbao.png");
            bg->setAnchorPoint(ccp(0.5f, 1.f));
            bg->setPosition(ccp(180,-20));
            s->addChild(bg);
            
            std::string detailStr = toStr(mLevelInfo.golds);
            auto detail = CCLabelBMFont::create(detailStr.c_str(), "bmfont/KTL_100_yellow.fnt");
            detail->setScale(contentScale()*0.6f);
            detail->setAnchorPoint(ccp(0.5, 1.0));
            detail->setPosition(ccp(275, -80));
            s->addChild(detail);
            
        }
        
        s->setPosition(ccp(-400 - 320, -490));
        s->runAction(CCSequence::create(CCDelayTime::create(0.5),
                                        CCEaseBounceOut::create(CCMoveTo::create(0.7, ccp(0, -490))),
                                        CCDelayTime::create(3.f),
                                        CCCallLambda::create([=]()
                                                             {
                                                                 s->removeFromParentAndCleanup(true); showDrops();
                                                             }),
                                        NULL));
        this->addChild(s);
    }
    
    void VictoryLayer::showLevelUp(const Player & prev, const Player & now)
    {
        const unsigned long kNumSize    = 25;
        
        std::string lpContent="";
        if(now.maxLP > prev.maxLP) lpContent = "+"+toStr(now.maxLP-prev.maxLP);
        LabelX * lpValPrev      = LabelX::create(toStr(prev.maxLP).c_str(), FONT_NUM, kNumSize);
        LabelX * lpValNow       = LabelX::create(lpContent.c_str(), FONT_NUM, kNumSize);
        
        if(now.maxLP > prev.maxLP)
        lpValNow->setColor(ccc3(0,255,0));
        else
            lpValNow->setColor(ccc3(255,255,255));
        
        
        std::string spiritContent = "";
        if(now.maxSpirit > prev.maxSpirit) spiritContent = "+"+toStr(now.maxSpirit-prev.maxSpirit);
        LabelX * spiritValPrev  = LabelX::create((toStr(prev.maxSpirit)).c_str(), FONT_NUM, kNumSize);
        LabelX * spiritValNow   = LabelX::create(spiritContent.c_str(), FONT_NUM, kNumSize);

        if(now.maxSpirit > prev.maxSpirit)
            spiritValNow->setColor(ccc3(0,255,0));
        else
            spiritValNow->setColor(ccc3(255,255,255));

        std::string frContent = "";
        if(now.maxFriends > prev.maxFriends) frContent = "+"+toStr(now.maxFriends-prev.maxFriends);
        LabelX * frValPrev      = LabelX::create(toStr(prev.maxFriends).c_str(), FONT_NUM, kNumSize);
        LabelX * frValNow       = LabelX::create(frContent.c_str(), FONT_NUM, kNumSize);

        if(now.maxFriends > prev.maxFriends)
            frValNow->setColor(ccc3(0,255,0));
        else
            frValNow->setColor(ccc3(255,255,255));

        CCSprite * lp           = GetSprite("ui/vic_weiwang.png");
        CCSprite * spirit       = GetSprite("ui/vic_tilizhi.png");
        CCSprite * fr           = GetSprite("ui/vic_haoyoushangxian.png");
        
        CCSprite * lpBg         = GetSprite("gameui/shengli_shuzi_bg.png");
        CCSprite * spBg         = GetSprite("gameui/shengli_shuzi_bg.png");
        CCSprite * frBg         = GetSprite("gameui/shengli_shuzi_bg.png");
        LayoutApply<CCNode*>(VA<CCNode*>(lpBg, spBg, frBg, NULL),
                             [=](CCNode * x)
                             {
                                 x->setAnchorPoint(ccp(0.5,0.5));
                             });
        LayoutColumn(ccp(375, -23),
                     ccp(1.f, 1.f),
                     54.f,
                     true,
                     VA(lpBg, spBg, frBg, NULL));
        
        LayoutColumn(ccp(165, -25),
                     ccp(1.0f, 1.f),
                     54.0f,
                     true,
                     VA(spirit, lp, fr, NULL));
        LayoutColumn(ccp(215, -30),
                     ccp(1.f, 1.f),
                     54.0f,
                     true,
                     VA(spiritValPrev, lpValPrev, frValPrev, NULL));

        LayoutColumn(ccp(265, -30),
                     ccp(1.f, 1.f),
                     54.f,
                     true,
                     VA(spiritValNow, lpValNow, frValNow, NULL));

        CCSprite* s = GetSprite("gameui/dengjitisheng.png");
        
        LayoutApply<CCNode*>(VA<CCNode*>(lpBg, spBg, frBg,
                                         lp, spirit, fr, NULL),
                             [=](CCNode * x)
                             {
                                 s->addChild(x);
                             });
        
        LayoutApply<LabelX*>(VA(lpValPrev, lpValNow,
                                spiritValPrev, spiritValNow,
                                frValPrev, frValNow, NULL),
                             [=](LabelX * x)
                             {
                                s->addChild(x);
                             });
        
        s->setPosition(ccp(-400 - 320, -490));
        
        s->runAction(CCSequence::create(CCDelayTime::create(0.5),
                                        CCEaseBounceOut::create(CCMoveTo::create(0.7, ccp(0, -490))),
                                        CCDelayTime::create(2.5),
                                        CCCallLambda::create([=]()
                                                             {    
                                                                 s->removeFromParentAndCleanup(true);
                                                                 if(mLevelInfo.golds>0) { showGold(); }
                                                                 else showDrops();
                                                             }),
                                        NULL));
        this->addChild(s); 
    }
    
    void VictoryLayer::pickAChest(std::vector<CCSprite*>::iterator iter,
                                  bool withLight)
    {
        auto pointer = this->newPlayer->rewards.begin();
        
        CCSprite * chest = (*iter);
        
        CCAction *seq_alert = CCSequence::create(CCScaleTo::create(0.5, 0, 1.f),
                                                 VictoryLayer::createModifiedContentAction(chest,
                                                                                           withLight,
                                                                                           pointer->type,
                                                                                           pointer->ident,
                                                                                           pointer->count,
                                                                                           pointer->level),
                                                 CCScaleTo::create(0.5, 1, 1),
                                                 NULL);
        chest->runAction(seq_alert);
        
        if(withLight){
            CCSprite* light = GetSprite("gameui/card_light.png");
            light->setScale(0);
            light->setPosition(ccp(chest->getContentSize().width/2,
                                   chest->getContentSize().height/2));
            
            chest->addChild(light, 1);
            
            CCAction *lSeq = CCSequence::create(CCDelayTime::create(0.3f),
                                                CCSpawn::create(CCScaleTo::create(0.8, 1, 1),
                                                                CCRotateBy::create(0.8, 60),
                                                                NULL),
                                                CCSpawn::create(CCFadeOut::create(0.8),
                                                                CCRotateBy::create(0.8, 60),
                                                                NULL),
                                                NULL);
            light->runAction(lSeq);
        }
        this->newPlayer->rewards.erase(pointer);
    }
    
    CCAction * VictoryLayer::createModifiedContentAction(CCSprite * tar,
                                                         bool withLight,
                                                         const int type,
                                                         const int ident,
                                                         const int count,
                                                         const int level)
    {
        CCAction * modifiedContent =
                             CCCallLambda::create([=]()
                             {
                                 CCTexture2D * tex
                                 = CCTextureCache::sharedTextureCache()->addImage("gameui/jinbaoxiang.png");
                                 tar->setTexture(tex);
                                 
                                 char name[128];
                                 sprintf(name, "gameui/jl_%d.png", type);
                                 CCNode * rewards=NULL;
                                 
                                 CCLabelTTF *countLabel = LabelX::create(toStr(count).c_str() , FONT_CN , 28, LabelX::FontEffect::SHADOW, ccp(2, -2));
                                 countLabel->setAnchorPoint(ccp(1.0f,0.f));
                                 
                                 if(!withLight) tar->setColor(ccc3(100, 100, 100));
                                
                                 switch(type)
                                 {
                                     case 0: // exp
                                     {
                                         rewards = GetSprite("gameui/jl_0.png");
                                         countLabel->setPositionX(rewards->getContentSize().width);
                                         rewards->addChild(countLabel);
                                         if(!withLight) ((CCSprite*)rewards)->setColor(ccc3(100, 100, 100));
                                         else{
                                             mLevelInfo.exp += count;
                                             this->deltaExp = true;
                                         }
                                         break;
                                     }
                                     case 1: // coins
                                     {
                                         rewards = GetSprite("gameui/jl_1.png");
                                         countLabel->setPositionX(rewards->getContentSize().width);
                                         rewards->addChild(countLabel);
                                         if(!withLight) ((CCSprite*)rewards)->setColor(ccc3(100, 100, 100));
                                         else{
                                             mLevelInfo.coins += count;
                                             this->deltaCoins = true;
                                         }
                                         break;
                                     }
                                     case 2: // golds
                                     {
                                         rewards = GetSprite("gameui/jl_2.png");
                                         countLabel->setPositionX(rewards->getContentSize().width);
                                         rewards->addChild(countLabel);
                                         if(!withLight) ((CCSprite*)rewards)->setColor(ccc3(100, 100, 100));
                                         else this->deltaGolds = true;
                                         break;
                                     }
                                     case 3: // hero
                                     {
                                         rewards = HeroIcon::createWithProfile(&HPT(ident));
                                         if(!withLight) ((HeroIcon*)rewards)->setEnabled(false);
                                         else this->deltaHero.push_back(ident);
                                         break;
                                     }
                                     case 4: // soul
                                     {
                                         rewards = HeroIcon::createWithProfile(&HPT(ident), true);
                                         ((HeroIcon*)rewards)->showPlusNum(count);
                                         if(!withLight) ((HeroIcon*)rewards)->setEnabled(false);
                                         else this->deltaSoul.push_back(std::make_pair(ident, count));
                                         break;
                                     }
                                     case 5: // exp per
                                     {
                                         rewards = GetSprite("gameui/jl_0.png");
                                         
                                         std::string r5 = "%";
                                         
                                         CCLabelTTF *count5 = LabelX::create((toStr(count)+r5).c_str() , FONT_CN , 28, LabelX::FontEffect::SHADOW, ccp(2, -2));
                                         count5->setAnchorPoint(ccp(1.0f,0.f));
                                         
                                         count5->setPositionX(rewards->getContentSize().width);
                                         rewards->addChild(count5);
                                         if(!withLight){
                                             ((CCSprite*)rewards)->setColor(ccc3(100, 100, 100));
                                             count5->setColor(ccc3(100, 100, 100));
                                         }
                                         else{
                                             mLevelInfo.exp += (float)(mLevelInfo.exp*count)/100.f;
                                             this->deltaExp = true;
                                         }
                                         break;
                                     }
                                     case 6: // coins per
                                     {
                                         rewards = GetSprite("gameui/jl_1.png");
                                         std::string r6 = "%";
                                         
                                         CCLabelTTF *count6 = LabelX::create((toStr(count)+r6).c_str() , FONT_CN , 28, LabelX::FontEffect::SHADOW, ccp(2, -2));
                                         count6->setAnchorPoint(ccp(1.0f,0.f));
                                         
                                         count6->setPositionX(rewards->getContentSize().width);
                                         rewards->addChild(count6);
                                         if(!withLight){
                                             ((CCSprite*)rewards)->setColor(ccc3(100, 100, 100));
                                             count6->setColor(ccc3(100, 100, 100));
                                         }
                                         else{
                                             mLevelInfo.coins += (float)(mLevelInfo.coins*count)/100.f;
                                             this->deltaCoins = true;
                                         }
                                         break;
                                     }
                                     case 7: // spirit drugs
                                     {
                                         rewards = GetSprite("gameui/jingliping_100.png");
                                         std::string r7 = "体力+";
                                         
                                         CCLabelTTF *count7 = LabelX::create((r7+toStr(count)).c_str(), FONT_CN, 28, LabelX::FontEffect::SHADOW, ccp(2, -2));
                                         count7->setAnchorPoint(ccp(1.0f,0.f));
                                         count7->setPositionX(rewards->getContentSize().width);
                                         rewards->addChild(count7);
                                         if(!withLight)
                                         {
                                             ((CCSprite*)rewards)->setColor(ccc3(100,100,100));
                                             count7->setColor(ccc3(100, 100, 100));
                                         }
                                         break;
                                     }
                                 }
                                 
                                 if(rewards)
                                 {
                                     rewards->setAnchorPoint(ccp(0.5f, 0.5f));
                                     rewards->setPosition(ccp(tar->getContentSize().width/2,
                                                              tar->getContentSize().height/2));
                                     tar->addChild(rewards);
                                 }
                             });

        return modifiedContent;
    }
    
    void VictoryLayer::skipChestPick(cocos2d::CCObject *o)
    {
        CCCallLambda * leaving =
        CCCallLambda::create([=]()
        {
            dropLayer->runAction(
                                 CCSequence::create(
                                                    CCEaseBounceOut::create(CCMoveBy::create(0.5, ccp(800, 0))),
                                                    CCCallLambda::create([=]()
                                                    {
                                                        this->chests.clear();
                                                        this->transitionToAccount(false);
                                                    }),NULL));
            labelLayer->runAction(CCEaseBounceOut::create(CCMoveBy::create(0.5, ccp(800, 0))));
            
        });
        
        bg->stopAllActions();
        bg->runAction(leaving);
        
        skipChestButton->runAction(CCSequence::create(CCFadeOut::create(0.3f),
                                                      CCCallLambda::create([=]()
                                                      {
                                                          skipChestButton->setVisible(false);
                                                      }), NULL));
        
    }
    void VictoryLayer::skipGolds(cocos2d::CCObject *o)
    {
        
    }
    void VictoryLayer::skipLevelUp(cocos2d::CCObject *o)
    {
        
    }
    
    bool VictoryLayer::ccTouchBegan(CCTouch * pTouch, CCEvent * pEvent)
    {
        return true;
    }
    
    void VictoryLayer::ccTouchEnded(CCTouch * pTouch, CCEvent * pEvent)
    {
        if(NULL == dropLayer) return;
        
        CCPoint p = dropLayer->convertTouchToNodeSpace(pTouch);
        
        bool flag = false;
        for( auto & chest : chests )
        {
            CCRect rect = chest->boundingBox();
            if(rect.containsPoint(p))
            {
                flag = true;
                break;
            }
        }
        if(!flag) return;
        
        this->setTouchEnabled(false);
        
        phassert(newPlayer->rewards.size() == chests.size());
        
        for(auto iter=chests.begin();
            iter!=chests.end();
            iter++)
        {
            CCRect rect = (*iter)->boundingBox();
            if(rect.containsPoint(p))
            {
                this->pickAChest(iter, true);
                this->chests.erase(iter);
                
                //TutorialControl::shared()->ScoreTutorialStep2(this);
                break;
            }
        }
        
        if(--this->newPlayer->availableRewardsCount <= 0)
        {
            
            CCCallLambda * leaving =
                                 CCCallLambda::create([=]()
                                 {
                                     dropLayer->runAction(
                                        CCSequence::create(
                                              CCEaseBounceOut::create(CCMoveBy::create(0.5, ccp(800, 0))),
                                               CCCallLambda::create([=]()
                                                                    {
                                                                        this->chests.clear();
                                                                        this->transitionToAccount(false);
                                                                    }),NULL));
                                     labelLayer->runAction(CCEaseBounceOut::create(CCMoveBy::create(0.5, ccp(800, 0))));
                                     skipChestButton->runAction(CCSequence::create(CCFadeOut::create(0.3f),
                                                                                   CCCallLambda::create([=]()
                                                                                                        {
                                                                                                            skipChestButton->setVisible(false);
                                                                                                        }), NULL));
                                     
                                 });
            CCCallLambda * pickRest =
                                 CCCallLambda::create([=]()
                                 {
                                     for(auto iter = this->chests.begin();
                                         iter != this->chests.end();
                                         iter++)
                                     {
                                         this->pickAChest(iter, false);
                                     }
                                 });
            
            bg->runAction(CCSequence::create(CCDelayTime::create(1.0f),
                                             pickRest,
                                             CCDelayTime::create(3.0f),
                                             leaving,
                                             NULL));
            
            skipChestButton->setVisible(true);
            skipChestButton->setPosition(bg->boundingBox().size.width*0.5f-offset, -340);
            CCAction * action = CCSpawn::create(CCEaseBounceOut::create(CCFadeIn::create(0.4f)),
                                                CCMoveBy::create(0.4f, ccp(offset, 0)), NULL);
            skipChestButton->setOpacity(0);
            skipChestButton->runAction(action);
    
        }
        else
        {
            runOnMainThread([=]()
                            {
                                runOnMainThread([=]()
                                                {
                                                    this->setTouchEnabled(true);
                                                });
                            });
        }
        
    }
    
    void VictoryLayer::registerWithTouchDispatcher()
    {
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    }
}
