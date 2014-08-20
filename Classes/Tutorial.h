//
//  Tutorial.h
//  HelloCpp
//
//  Created by zql on 13-7-13.
//
//

#pragma once

#include "cocos2d.h"

USING_NS_CC;

namespace PH {

class JiangHu;
class PickSuperHeroLayer;
class ZhaoMuLayer;
class TutorialControl;
    class VictoryLayer;

class TutorialCaller
{
public:
    virtual void tutorialCall(TutorialControl* control){};
};

class TutorialControl
{
public:
    static TutorialControl* shared();
    
    void onLevelUpTutorial(int level);
    void openInstanceStep1(JiangHu* layer);
    void openInstanceStep2(JiangHu* layer);
    
    void ScoreTutorialStep1(VictoryLayer* layer);
    void ScoreTutorialStep2(VictoryLayer* layer);

};

}

