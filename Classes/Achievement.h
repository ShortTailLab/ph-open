//
//  Achievement.h
//  HelloCpp
//
//  Created by zql on 13-9-9.
//
//

#pragma once

#include "UIComponent.h"
#include "Types.h"
#include "ZqlScrollView.h"

namespace PH
{
    
    class AchievementItem : public ContentLayer
    {
    public:
        virtual bool init(const PlayerTask& task);
        
        static AchievementItem* create(const PlayerTask& task)
        {
            AchievementItem* o = new AchievementItem();
            if(o != NULL && o->init(task))
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }
    private:
        PlayerTask task;
        cocos2d::CCSprite* logo;
    };
    
    class AchievementLayer : public ContentLayer
    {
    public:
        
        bool init();
        void updateTasks();
        void onBack();
        void initTips();
        void makeItems();
        
        CREATE_FUNC(AchievementLayer);
        
    private:
        ZqlScrollView* scrollView;
        AchievementSwitcher* switcher;
        std::auto_ptr<PlayerTaskVec> curTasks;
        std::vector<int> doneTasks;
        CCLayer* tipsLayer;
        bool isFirstEnter;
    };
    
    
}


