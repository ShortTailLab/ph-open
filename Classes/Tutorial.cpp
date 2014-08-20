//
//  Tutorial.cpp
//  HelloCpp
//
//  Created by zql on 13-7-13.
//
//

#include "Tutorial.h"
#include "Common.h"
#include "JiangHu.h"
#include "SystemInform.h"
#include "Victory.h"

namespace PH {
    
TutorialControl* TutorialControl::shared()
{
    static TutorialControl* o = NULL;
    if(o == NULL)
        o = new TutorialControl;
    
    return o;
}

void TutorialControl::onLevelUpTutorial(int level)
{
    if(level == 10)
    {
        GameConfig::shared()->openInstanceStep = 1;
        GameConfig::shared()->save();
    }
}
    

    
void TutorialControl::openInstanceStep1(PH::JiangHu *layer)
{
    //first time instance tutorual
    if(GameConfig::shared()->openInstanceStep == 1)
    {
        AlertMessage::create("",
                             "活动开启，恭喜！掌门你已提升至10级，可以进入活动啦！");
        ArrowMask::shared()->setMask(CCRect(320, 700, 310, 105));
        ArrowMask::shared()->setArrow(ccp(300, 750), ArrowDirection::Right);
        
    }
}
    
void TutorialControl::openInstanceStep2(PH::JiangHu *layer)
{
    //first time instance tutorual
    if(GameConfig::shared()->openInstanceStep == 1)
    {
        ArrowMask::shared()->clear();
        AlertMessage::create("",
                             "活动类型，活动分为日常活动和节日活动！日常活动每天可进入的次\t数有限，而且等级越高可以参加的活动越多哦！掌门请加油升级吧！");
        GameConfig::shared()->openInstanceStep = 2;
        GameConfig::shared()->save();
    }
}
    
void TutorialControl::ScoreTutorialStep1(VictoryLayer* layer)
{
    
    if(GameConfig::shared()->isFirstScoreAfterBattle)
    {
        AlertMessage::create("",
                             "关卡评分，每次完成关卡后将得到系统评分！评分跟通关回合、连击数、复活数有关！",
                             true,
         [=]()
         {
             AlertMessage::create("",
                                  "由于你的副本表现优异！请从下面几个宝箱中选择奖励！",
                                  true,
              [=]()
              {
                  ArrowMask::shared()->setMask(CCRect(15, 110, 600, 160));
                  for(int i = 0; i < 4; i++)
                      ArrowMask::shared()->addArrow(ccp(90+150*i, 300), ArrowDirection::Down);
              }); 
         });
    }
}
    
void TutorialControl::ScoreTutorialStep2(VictoryLayer* layer)
{
    if(GameConfig::shared()->isFirstScoreAfterBattle)
    {
        ArrowMask::shared()->clear();
        GameConfig::shared()->isFirstScoreAfterBattle = false;
    }
}

    
}

