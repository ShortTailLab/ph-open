//
//  ScreenConsole.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 5/10/13.
//
//

#include "ScreenConsole.h"
#include "extensions/utils.h"

namespace PH
{
    bool ScreenConsole::init()
    {
        if(!CCLayer::init())
            return false;
        
        background = GetSprite("gameui/1x1_b.png");
        background->setAnchorPoint(ccp(0, 0));
        background->setPosition(ccp(0, 0));
        background->setOpacity(200);
        this->addChild(background);
        
        view = CCLabelTTF::create("", "Arial", 32, CCSize(0, 0), CCTextAlignment::kCCTextAlignmentLeft);
        view->setAnchorPoint(ccp(0.0f, 0.0f));
        view->setPosition(ccp(0,0));
        view->setColor(ccWHITE);
        
        this->addChild(view);
        
        return true;
    }
    
    void ScreenConsole::print(const char* format, ...)
    {
        char str[2048];
        
        va_list argptr;
        va_start(argptr, format);
        vsnprintf(str, 2048, format, argptr);
        va_end(argptr);
        
        buffer += str;
        buffer += "\n";
        view->setString(buffer.c_str());
        
        CCSize size = view->getContentSize();
        background->setScaleX(size.width);
        background->setScaleY(size.height);
    }
    
    void ScreenConsole::attach(CCScene* scene)
    {
        if(this->getParent() != NULL)
            this->removeFromParentAndCleanup(true);
        scene->addChild(this, 1000000);
    }
    
}