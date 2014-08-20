//
//  ScreenConsole.h
//  HelloCpp
//
//  Created by Zeyang Li on 5/10/13.
//
//

#pragma once

#include <cocos2d.h>

namespace PH
{
    USING_NS_CC;
    
    class ScreenConsole : public CCLayer
    {
    private:
        std::string buffer;
        CCLabelTTF* view;
        CCSprite* background;
        
    public:
        static ScreenConsole* getInstance()
        {
            static ScreenConsole* s = NULL;
            if(s == NULL)
            {
                s = new ScreenConsole();
                s->init();
            }
            return s;
        }
        
        void attach(CCScene* scene);
        void print(const char* format, ...);
        
    private:
        virtual bool init();
    };
    
#define SC(...) (ScreenConsole::getInstance()->print(__VA_ARGS__))
}
