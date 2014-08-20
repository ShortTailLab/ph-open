//
//  PvPSearchGame.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 6/5/13.
//
//

#include "PvPSearchGame.h"
#include "extensions/utils.h"
#include "Fonts.h"
#include "CCCallLambda.h"
#include "PvPMessageHandler.h"
#include "PuzzleRPC.h"
#include "AnimPreset.h"
#include "UIPresets.h"
#include "ButtonSprite.h"
#include "GameScene.h"

namespace PH
{
    USING_NS_CC;
    
    static CCCallLambda* setTextAction(CCLabelTTF* label, const char* s)
    {
        return CCCallLambda::create([=](){ label->setString(s); });
    }
    
    bool PvPSearchGame::init()
    {
        this->isShowBottom = false;
        this->isShowTop = false;
        
        mLoadingCancel = NULL;
        
        CCNode* root = CCNode::create();
        root->setPosition(ccp(-320, -960));
        this->addChild(root);
        
        CCSprite* bg = GetSprite("pvp/pvp_bg.jpg");
        bg->setAnchorPoint(ccp(0, 0));
        root->addChild(bg);
        
        mLoading = CCLabelTTF::create("正在匹配对手中", FONT_CN, 32);
        mLoading->setPosition(ccp(320, 810));
        root->addChild(mLoading);
        
        std::string text = std::string("服务器连接状态: ") + (PuzzleRPC::getInstance()->getPvP()->isConnected() ? "正常" : "断开");
        mServerStatus = CCLabelTTF::create(text.c_str(), FONT_CN, 32);
        mServerStatus->setPosition(ccp(320, 750));
        root->addChild(mServerStatus);
        
        mLoadingAnim = GetSprite("loading_anim1.png");
        mLoadingAnim->setAnchorPoint(ccp(0.5, 0));
        mLoadingAnim->setPosition(ccp(330, 250));
        root->addChild(mLoadingAnim);
        
        ButtonSprite* stop = ButtonSprite::create(createBtn("退出", ButtonType::NORMAL_RED),
                                                  createBtn("退出", ButtonType::NORMAL_RED));
        stop->setOnClickListener([=](){ this->stopMatching(); });
        stop->setPosition(ccp(320, 45));
        root->addChild(stop);
        
        PuzzleRPC::getInstance()->getPvP()->setFindMatchReceiver(this);
        
        return true;
    }
    
    void PvPSearchGame::onEnterAnimationDone()
    {
        CCActionInterval* seq = CCSequence::create(CCDelayTime::create(0.67f),
                                                   setTextAction(mLoading, "正在匹配对手中."),
                                                   CCDelayTime::create(0.67f),
                                                   setTextAction(mLoading, "正在匹配对手中.."),
                                                   CCDelayTime::create(0.67f),
                                                   setTextAction(mLoading, "正在匹配对手中..."),
                                                   NULL);
        mLoading->runAction(CCRepeatForever::create(seq));

        mLoadingAnim->runAction(CCRepeatForever::create(CCAnimate::create(createLoadingCharacter())));
    }
    
    void PvPSearchGame::onExit()
    {
        ContentLayer::onExit();
        PuzzleRPC::getInstance()->getPvP()->setFindMatchReceiver(NULL);
        if(mLoadingCancel)
        {
            mLoadingCancel->cancel();
            mLoadingCancel = NULL;
        }
    }
    
    void PvPSearchGame::stopMatching()
    {
        if(!mLoadingCancel)
        {
            mLoadingCancel = SystemInform::alertLoading(GameScene::getInstance());
            PuzzleRPC::getInstance()->getPvP()->sendCancelGameRequest();
            this->scheduleOnce(schedule_selector(PvPSearchGame::onSearchCancelled), 8.0f);
        }
    }
    
    bool PvPSearchGame::onRivalTimeoutOrRejected()
    {
        if(mLoadingCancel)
        {
            mLoadingCancel->cancel();
            mLoadingCancel = NULL;
        }
        
        SystemInform::alertMessage(runningScene(), "搜索不到对手，请一会儿再试试吧。", "搜索超时", []()
        {
            GameScene::getInstance()->popLayer();
        });
        
        return true;
    }
    
    bool PvPSearchGame::onSearchCancelled()
    {
        if(mLoadingCancel)
        {
            this->unschedule(schedule_selector(PvPSearchGame::onSearchCancelled));
            mLoadingCancel->cancel();
            mLoadingCancel = NULL;
            GameScene::getInstance()->popLayer();
        }
        else
            GameScene::getInstance()->popLayer();
        
        return true;
    }
}