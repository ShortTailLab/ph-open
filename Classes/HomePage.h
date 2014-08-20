//
//  HomePage.h
//  HelloCpp
//
//  Created by zql on 13-10-7.
//
//

#pragma once

#include "ContentLayer.h"
#include "HeroTable.h"
#include "ButtonSprite.h"
#include "PageScrollView.h"

namespace PH
{

    class HomePage : public ContentLayer
    {
    public:
        bool init();
        virtual void onEnter();
        virtual void onEnterAnimationDone();
        void makeTeamGallery();
        CCSprite* createHeroCard(const HeroProfile* profile);
        virtual void onSingleClick(CCTouch* touch);
        virtual void onLongClick(CCTouch* touch);
        virtual bool tryExit(int msg);
        
        CREATE_FUNC(HomePage);
        
    private:
        ButtonSprite* task;
        ButtonSprite* huodong;
        PageScrollView* view;
        ButtonSprite* leftBtn;
        ButtonSprite* rightBtn;
        std::vector<CCSprite*> cards;
        std::vector<CCSprite*> dots;
        CCSprite* teamBg;
        int teamIdOnEnter;
        CCLabelBMFont* leitaiLabel;
        
        void updateBtnState();
        void schedule1Sec();
    };
    
    
}