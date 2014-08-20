//
//  HeroDebugList.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 2/28/13.
//
//

#include "HeroDebugList.h"
#include "HeroTable.h"

namespace PH
{
    bool HeroDebugList::init()
    {
        if(!CCLayer::init())
            return false;
        
        scroll = ZqlScrollView::create();
        this->addChild(scroll);
        
        scroll->setViewSize(CCSize(640, 960));
        
        {
            CCLayerColor* bg = CCLayerColor::create(ccc4(0, 0, 0, 100));
            this->addChild(bg, -1);
        }
        
        
        {
            CCMenu* menu = CCMenu::create();
//            menu->setTouchEnabled(false);
        
            std::vector<HeroProfile*> profiles = heroProfiles();
            CCFileUtils::sharedFileUtils()->setPopupNotify(false);
            
            int y = 0;
            int x = 0;
            
            for(HeroProfile* p : profiles)
            {
                CCSprite* s = GetSprite(p->iconPath());
                if(s)
                {
                    CCMenuItem* item = CCMenuItemSprite::create(s,
                                                                s,
                                                                this,
                                                                menu_selector(HeroDebugList::onClick));
                    item->setAnchorPoint(ccp(0.0, 1.0));
                    item->setPosition(ccp(x*105.0f, y*-105.0f));
                    item->setUserData(p);
                    
                    menu->addChild(item);

                    if(x > 5)
                    {
                        y++;
                        x = 0;
                    }
                    else
                        x++;
                }
            }
            CCFileUtils::sharedFileUtils()->setPopupNotify(true);
            
            menu->setContentSize(CCSize(640.0f, y*105.0f));
            menu->setPosition(ccp(-320, 0));
            scroll->addLayer(menu);
        }
        
        func = [](HeroProfile* h) { };
        
        return true;
    }
    
    void HeroDebugList::onClick(CCObject* o)
    {
        HeroProfile* h = (HeroProfile*) ((CCMenuItem*)o)->getUserData();
        printf("hahaha: %s\n", h->nameCN.c_str() );
        func(h);
    }
}