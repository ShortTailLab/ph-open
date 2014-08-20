//
//  PvPEndGameWin.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 6/7/13.
//
//

#include "PvPEndGame.h"
#include "Fonts.h"
#include "ControlExtensions.h"
#include "ButtonSprite.h"
#include "UIPresets.h"
#include "extensions/utils.h"
#include "Board/Utils.h"
#include "LayoutUtils.h"
#include "GameScene.h"
#include "SystemInform.h"
#include "PuzzleRPC.h"
#include "PlayerData.h"

#include <cstdarg>

namespace PH
{
    //---------------------
    // EndGame Win
    //---------------------
    bool PvPEndGameWin::init(int reputationGain, int scoreGain, int turnUsed)
    {
        if(!ContentLayer::init())
            return false;
        
        this->isShowBottom = false;
        this->isShowTop = true;
        
        CCNode* root = CCNode::create();
        root->setPosition(ccp(-320, -800));
        this->addChild(root);
        
        CCSprite* bg = GetSprite("gameui/pvp_jiesuan_bg1.jpg");
        bg->setAnchorPoint(ccp(0, 0));
        root->addChild(bg);
        
        CCSprite* banner = GetSprite("gameui/pvp_shengli.png");
        banner->setPosition(ccp(320, 825));
        banner->setAnchorPoint(ccp(0.5f, 1.0f));
        root->addChild(banner);
        
        CCLabelTTF* reputation = CCLabelTTF::create("获得名望", FONT_CN, 30);
        CCLabelTTF* repValue = CCLabelTTF::create(toStr(reputationGain).c_str(), FONT_NUM, 25);
        
        CCLabelTTF* tongbao = CCLabelTTF::create("获得通宝", FONT_CN, 30);
        CCLabelTTF* tongbaoValue = CCLabelTTF::create(toStr(scoreGain).c_str(), FONT_NUM, 25);
        
        CCLabelTTF* turnUsedLabel = CCLabelTTF::create("所用回合", FONT_CN, 30);
        CCLabelTTF* turnValue = CCLabelTTF::create(toStr(turnUsed).c_str(), FONT_NUM, 25);
        
        LayoutColumn(ccp(162, 641),
                     ccp(0.0f, 1.0f),
                     54.0f,
                     true,
                     VA(reputation, tongbao, turnUsedLabel, NULL));
        
        LayoutColumn(ccp(481, 638),
                     ccp(1.0f, 1.0f),
                     54.0f,
                     true,
                     VA(repValue, tongbaoValue, turnValue, NULL));
        
        LayoutApply<CCLabelTTF*>(VA(reputation, repValue,
                                tongbao,    tongbaoValue,
                                turnUsedLabel, turnValue,
                                NULL),
                             [=](CCLabelTTF* x)
                             {
                                 x->setColor(ccc3(84, 0, 0));
                                 root->addChild(x);
                             });
        
        
        ButtonSprite* confirm = ButtonSprite::create(createBtn("确定"),
                                                     createBtn("确定"));
        confirm->setScale(1.3f);
        confirm->setPosition(ccp(320, 423));
        confirm->setOnClickListener([=](){ this->onConfirm(); });
        root->addChild(confirm);
        
        return true;
    }
    
    void PvPEndGameWin::onEnterAnimationDone()
    {
    }
    
    void PvPEndGameWin::onConfirm()
    {
        SystemInform* inform = SystemInform::alertLoading(runningScene());
        PuzzleRPC::getInstance()->pvpMyRanks(PlayerData::getInstance()->getToken(),
         [=](std::auto_ptr<Player> p, std::auto_ptr<Error> e)
         {
             inform->cancel();
             if(e.get())
             {
                 e->handleErrorInform();
                 return;
             }
             PlayerData::getInstance()->updatePlayer(p.get());
             CCTextureCache::sharedTextureCache()->removeUnusedTextures();
             GameScene::getInstance()->battleSceneEnded("pvp");
         });
        
    }
    
    
    //---------------------
    // EndGame Loss
    //---------------------
    bool PvPEndGameLoss::init(int reputationGain, int scoreGain, int turnUsed)
    {
        if(!ContentLayer::init())
            return false;
        
        this->isShowBottom = false;
        this->isShowTop = true;
        
        CCNode* root = CCNode::create();
        root->setPosition(ccp(-320, -800));
        this->addChild(root);
        
        CCSprite* bg = GetSprite("gameui/pvp_jiesuan_bg2.jpg");
        bg->setAnchorPoint(ccp(0, 0));
        root->addChild(bg);
        
        CCSprite* banner = GetSprite("gameui/pvp_baibei.png");
        banner->setPosition(ccp(320, 825));
        banner->setAnchorPoint(ccp(0.5f, 1.0f));
        root->addChild(banner);
        
        CCLabelTTF* reputation = CCLabelTTF::create("获得名望", FONT_CN, 30);
        CCLabelTTF* repValue = CCLabelTTF::create(toStr(reputationGain).c_str(), FONT_NUM, 25);
        
        CCLabelTTF* tongbao = CCLabelTTF::create("获得通宝", FONT_CN, 30);
        CCLabelTTF* tongbaoValue = CCLabelTTF::create(toStr(scoreGain).c_str(), FONT_NUM, 25);
        
        CCLabelTTF* turnUsedLabel = CCLabelTTF::create("所用回合", FONT_CN, 30);
        CCLabelTTF* turnValue = CCLabelTTF::create(toStr(turnUsed).c_str(), FONT_NUM, 25);
        
        LayoutColumn(ccp(162, 641),
                     ccp(0.0f, 1.0f),
                     54.0f,
                     true,
                     VA(reputation, tongbao, turnUsedLabel, NULL));
        
        LayoutColumn(ccp(481, 638),
                     ccp(1.0f, 1.0f),
                     54.0f,
                     true,
                     VA(repValue, tongbaoValue, turnValue, NULL));
        
        LayoutApply<CCLabelTTF*>(VA(reputation, repValue,
                                tongbao,    tongbaoValue,
                                turnUsedLabel, turnValue,
                                NULL),
                             [=](CCLabelTTF* x)
                             {
                                 x->setColor(ccc3(84, 0, 0));
                                 root->addChild(x);
                             });
        
        ButtonSprite* confirm = ButtonSprite::create(createBtn("确定"),
                                                     createBtn("确定"));
        confirm->setPosition(ccp(320, 423));
        confirm->setScale(1.3f);
        confirm->setOnClickListener([=](){ this->onConfirm(); });
        root->addChild(confirm);
        
        return true;
    }
    
    void PvPEndGameLoss::onEnterAnimationDone()
    {
    }
    
    void PvPEndGameLoss::onConfirm()
    {
        SystemInform* inform = SystemInform::alertLoading(runningScene());
        PuzzleRPC::getInstance()->pvpMyRanks(PlayerData::getInstance()->getToken(),
                                             [=](std::auto_ptr<Player> p, std::auto_ptr<Error> e)
        {
            inform->cancel();
            if(e.get())
            {
                e->handleErrorInform();
                return;
            }
            PlayerData::getInstance()->updatePlayer(p.get());
            CCTextureCache::sharedTextureCache()->removeUnusedTextures();
            GameScene::getInstance()->battleSceneEnded("pvp");
        });
        
    }
    
    //---------------------
    // EndGame Tie
    //---------------------
    bool PvPEndGameTie::init(int reputationGain, int scoreGain, int turnUsed)
    {
        if(!ContentLayer::init())
            return false;
        
        this->isShowBottom = false;
        this->isShowTop = true;
        
        CCNode* root = CCNode::create();
        root->setPosition(ccp(-320, -800));
        this->addChild(root);
        
        CCSprite* bg = GetSprite("gameui/pvp_jiesuan_bg2.jpg");
        bg->setAnchorPoint(ccp(0, 0));
        root->addChild(bg);
        
        CCSprite* banner = GetSprite("gameui/pvp_pingju.png");
        banner->setPosition(ccp(320, 825));
        banner->setAnchorPoint(ccp(0.5f, 1.0f));
        root->addChild(banner);
        
        CCLabelTTF* reputation = CCLabelTTF::create("获得名望", FONT_CN, 30);
        CCLabelTTF* repValue = CCLabelTTF::create(toStr(reputationGain).c_str(), FONT_NUM, 25);
        
        CCLabelTTF* tongbao = CCLabelTTF::create("获得通宝", FONT_CN, 30);
        CCLabelTTF* tongbaoValue = CCLabelTTF::create(toStr(scoreGain).c_str(), FONT_NUM, 25);
        
        CCLabelTTF* turnUsedLabel = CCLabelTTF::create("所用回合", FONT_CN, 30);
        CCLabelTTF* turnValue = CCLabelTTF::create(toStr(turnUsed).c_str(), FONT_NUM, 25);
        
        LayoutColumn(ccp(162, 641),
                     ccp(0.0f, 1.0f),
                     54.0f,
                     true,
                     VA(reputation, tongbao, turnUsedLabel, NULL));
        
        LayoutColumn(ccp(481, 638),
                     ccp(1.0f, 1.0f),
                     54.0f,
                     true,
                     VA(repValue, tongbaoValue, turnValue, NULL));
        
        LayoutApply<CCLabelTTF*>(VA(reputation, repValue,
                                    tongbao,    tongbaoValue,
                                    turnUsedLabel, turnValue,
                                    NULL),
                                 [=](CCLabelTTF* x)
                                 {
                                     x->setColor(ccc3(84, 0, 0));
                                     root->addChild(x);
                                 });
        
        ButtonSprite* confirm = ButtonSprite::create(createBtn("确定"),
                                                     createBtn("确定"));
        confirm->setPosition(ccp(320, 423));
        confirm->setScale(1.3f);
        confirm->setOnClickListener([=](){ this->onConfirm(); });
        root->addChild(confirm);
        
        return true;
    }
    
    void PvPEndGameTie::onEnterAnimationDone()
    {
    }
    
    void PvPEndGameTie::onConfirm()
    {
        SystemInform* inform = SystemInform::alertLoading(runningScene());
        PuzzleRPC::getInstance()->pvpMyRanks(PlayerData::getInstance()->getToken(),
                                             [=](std::auto_ptr<Player> p, std::auto_ptr<Error> e)
                                             {
                                                 inform->cancel();
                                                 if(e.get())
                                                 {
                                                     e->handleErrorInform();
                                                     return;
                                                 }
                                                 PlayerData::getInstance()->updatePlayer(p.get());
                                                 CCTextureCache::sharedTextureCache()->removeUnusedTextures();
                                                 GameScene::getInstance()->battleSceneEnded("pvp");
                                             });
    }

}