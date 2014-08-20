//
//  UIPresets.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 4/23/13.
//
//

#include "UIPresets.h"
#include "HeroTable.h"
#include "ControlExtensions.h"
#include "Fonts.h"

namespace PH
{
    USING_NS_CC;
    
    void addLabelTTF(CCNode *par,
                     CCLabelTTF *&target,
                     const char* value,
                     const char* font,
                     int size,
                     CCPoint pos,
                     CCPoint anchor,
                     ccColor3B color,
                     int zOrder)
    {
        target = CCLabelTTF::create(value, font, size);
        target->setAnchorPoint(anchor);
        target->setPosition(pos);
        target->setColor(color);
        par->addChild(target, zOrder);
    }
    
    CCLabelTTF* addLabelTTF(CCNode *par,
                     const char* value,
                     const char* font,
                     int size,
                     CCPoint pos,
                     CCPoint anchor,
                     ccColor3B color,
                     int zOrder)
    {
        CCLabelTTF* target = CCLabelTTF::create(value , font , size );
        target->setAnchorPoint(anchor);
        target->setPosition(pos);
        target->setColor(color);
        par->addChild(target, zOrder);
        return target;
    }
    
    void addLabelBMFont(CCNode *par,
                        CCLabelBMFont *&target,
                        const char* value,
                        const char* font,
                        CCPoint point,
                        CCPoint anchor)
    {
        target = CCLabelBMFont::create(value , font );
        target->setAnchorPoint( anchor);
        target->setPosition( point);
        par->addChild(target);
    }
    
    void addSprite(CCNode* par,
              CCSprite* target,
              CCPoint pos,
              CCPoint anchor)
    {
        target->setPosition(pos);
        target->setAnchorPoint(anchor);
        par->addChild(target);
    }
    
    CCSprite *makeHeroImg(Hero *hero)
    {
        CCSprite *img = GetSprite( hero->profile->bodyPath().c_str());
        if(hero->profile->star < 5)
            img->setScale(1.1);
        
        img->setUserData(hero);
        return img;
    }
    
    CCNode* createTitleLabel(const std::string& title, const CCPoint& p)
    {
        LabelX* text = LabelX::create(title.c_str(), FONT_CN, 42);
        text->setColor(ccWHITE);
        text->setAnchorPoint(ccp(0.5, 0.5));
        text->setPosition(p);
        return text;
    }
    
    CCSprite* createBtn(const std::string& title, ButtonType type, ccColor3B color)
    {
        std::string path;
        int fontSize = 0;
        //small , red
        if(type == ButtonType::SMALL_RED)
        {
            path = "gameui/chengdi_t_tongyong_x.png";
            fontSize = 26;
        }
        else if(type == ButtonType::SMALL_BLUE)
        {
            path = "gameui/chengdi_t_tongyong_x_l.png";
            fontSize = 26;
        }
        //normal red
        else if(type == ButtonType::NORMAL_RED)
        {
            path = "gameui/chengdi_t_tongyong_h.png";
            fontSize = 34;
        }
        //normal blue
        else if(type == ButtonType::NORMAL_BLUE)
        {
            path = "gameui/chengdi_t_tongyong_l.png";
            fontSize = 34;
        }
        //3words red
        else if(type == ButtonType::THREE_WORD_RED)
        {
            path = "gameui/3zidi.png";
            fontSize = 34;
        }
        //3words blue
        else if(type == ButtonType::THREE_WORD_BLUE)
        {
            path = "gameui/3zidi_l.png";
            fontSize = 34;
        }
        //4words red
        else if(type == ButtonType::FOUR_WORD_RED)
        {
            path = "gameui/4zidi.png";
            fontSize = 34;
        }
        
        CCSprite* item = GetSprite(path);
        LabelX* itemlabel =  LabelX::create(title.c_str(), FONT_CN, fontSize);
        itemlabel->setPosition(ccp(item->getContentSize().width*0.5f,
                                   item->getContentSize().height*0.5f+2));
        itemlabel->setColor(color);
        item->addChild(itemlabel);
        return item;
    }
    
    /*
    CCSprite* createTutorialScaleArrow(CCPoint p, float scale, int direction)
    {
        CCSprite* s = GetSprite("ui/jiantou.png");
        s->setAnchorPoint(ccp(0.0f, 0.5f));
        
        switch(direction)
        {
            case 0: break;
            case 1: s->setRotation(90); break;
            case 2: s->setRotation(180); break;
            case 3: s->setRotation(270); break;
            default:
                break;
        }
        
        s->setPosition(p);
        s->setScale(scale);
        
        CCFiniteTimeAction* action = CCSequence::create(CCScaleTo::create(0.01, 1.0f, 0.0f),
                                                        CCScaleTo::create(1, 1.0f, 1.0f),
                                                        NULL);
        
        s->runAction(CCRepeatForever::create((CCActionInterval*)action));
        
        return s;
    }*/
    
    bool easing(CCNode *target, float tX)
    {
        if(target->getPositionX() == tX)
            return true;
        
        float tempX = target->getPositionX();
        target->setPositionX( tempX + (tX - tempX) * 0.25);
        
        if(std::abs(tX - target->getPositionX())<0.1 ){
            target->setPositionX(tX);
            return true;
        }
        return false;
    }
}