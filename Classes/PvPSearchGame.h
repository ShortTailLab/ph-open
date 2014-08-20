//
//  PvPSearchGame.h
//  HelloCpp
//
//  Created by Zeyang Li on 6/5/13.
//
//

#pragma once

#include <cocos2d.h>
#include "ContentLayer.h"
#include "SystemInform.h"
#include "PvPMessageHandler.h"

namespace PH
{
    USING_NS_CC;
    
    class PvPSearchGame : public ContentLayer, public IPvPFindMatchReceiver
    {
    public:
        virtual bool init();
        virtual void onEnterAnimationDone();
        virtual void onExit();
        
        void stopMatching();
        
        virtual bool onSearchCancelled();
        virtual bool onRivalTimeoutOrRejected();
        
        static PvPSearchGame* create()
        {
            PvPSearchGame* o = new PvPSearchGame;
            if(o && o->init())
            {
                o->autorelease();
                return o;
            }
            
            CC_SAFE_DELETE(o);
            return o;
        }
        
    private:
        CCLabelTTF* mLoading;
        CCLabelTTF* mServerStatus;
        CCNode* mLoadingAnim;
        SystemInform* mLoadingCancel;
    };
}