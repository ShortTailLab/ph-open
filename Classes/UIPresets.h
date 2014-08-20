//
//  UIPresets.h
//  HelloCpp
//
//  Created by Zeyang Li on 4/23/13.
//
//

#pragma once

#include <cocos2d.h>

namespace PH
{
    USING_NS_CC;
    
    CCNode* createTitleLabel(const std::string& title, const CCPoint& p);
//    CCSprite* createTutorialArrow(CCPoint p, float scale, int direction, bool useMoveAnim = false);
    void addSprite(CCNode* par, CCSprite* target, CCPoint pos, CCPoint anchor = ccp(0.5, 0.5));
    
    enum class ButtonType
    {
        
        SMALL_RED = 0,
        SMALL_BLUE,
        NORMAL_RED,
        NORMAL_BLUE ,
        THREE_WORD_RED,
        THREE_WORD_BLUE,
        FOUR_WORD_RED,
    };
    
    CCSprite* createBtn(const std::string& title,
                        ButtonType kind = ButtonType::SMALL_RED,
                        ccColor3B color = ccWHITE);
    
    void addLabelTTF(CCNode *par,
                     CCLabelTTF *&target,
                     const char* value,
                     const char* font,
                     int size,
                     CCPoint pos,
                     CCPoint anchor=ccp(0.5, 1),
                     ccColor3B color = ccWHITE,
                     int zOrder=0);
    CCLabelTTF* addLabelTTF(CCNode *par,
                     const char* value,
                     const char* font,
                     int size,
                     CCPoint pos,
                     CCPoint anchor=ccp(0.5, 1),
                     ccColor3B color = ccWHITE,
                     int zOrder=0);
    
    void addLabelBMFont(CCNode *par,
                        CCLabelBMFont *&target,
                        const char* value,
                        const char* font,
                        CCPoint point,
                        CCPoint anchor=ccp(0.5, 1));
    
    inline CCLabelBMFont* addLabelBMFont(CCNode *par,
                               const char* value,
                               const char* font,
                               CCPoint point,
                               CCPoint anchor=ccp(0.5, 1))
    {
        CCLabelBMFont* temp;
        addLabelBMFont(par, temp, value ,font ,point, anchor);
        return temp;
    }
    
    class Hero;
    CCSprite *makeHeroImg(Hero *hero);
    
    bool easing(CCNode *target , float tX);
}