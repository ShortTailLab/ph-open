//
//  LoadingScene.h
//  HelloCpp
//
//  Created by Zeyang Li on 8/15/13.
//
//

#pragma once

#include <cocos2d.h>
#include <boost/shared_ptr.hpp>

namespace PH
{
    USING_NS_CC;
    
    class SplashScene : public CCScene
    {
    public:
        CREATE_FUNC(SplashScene);
        virtual bool init();
    };
    
    class JSCSplashScene : public CCScene
    {
    public:
        CREATE_FUNC(JSCSplashScene);
        virtual bool init();
    };
    
    //---------------------
    // LoadingScreen
    class ResourceSyncMan;
    
    class LoadingScreen : public CCScene
    {
    private:
        CCSprite* mProgressBar;
        CCLabelTTF* mText;
        
    public:
        bool init(bool highResResource);
        
        static LoadingScreen* create(bool highResResource)
        {
            LoadingScreen* o = new LoadingScreen();
            if(o && o->init(highResResource))
            {
                o->autorelease();
                return o;
            }
            assert(false && "loading screen cannot be created");
            CC_SAFE_DELETE(o);
            return NULL;
        }
        
        static std::string bytesToReadableString(int bytes);
        
        void setProgress(float progress);
        void setString(const std::string& str);
        void launchSyncMan(bool useHD);
        
    private:
        bool isGemAction;
        CCSprite* logo;
        boost::shared_ptr<ResourceSyncMan> syncMan;
        
        void performGemsAction();
        CCLayer* createGem(const char* gemPath, const char* lightPath, float scale);
        CCAction* createGemOutAction(CCPoint org, CCPoint target);
    };
}