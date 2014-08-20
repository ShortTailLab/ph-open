//
//  PvPEndGameWin.h
//  HelloCpp
//
//  Created by Zeyang Li on 6/7/13.
//
//

#pragma once

#include <cocos2d.h>
#include "ContentLayer.h"

namespace PH
{
    USING_NS_CC;
    
    class PvPEndGameWin : public ContentLayer
    {
    public:
        virtual bool init(int reputationGain, int scoreGain, int turnUsed);
        virtual void onEnterAnimationDone();
        
        static PvPEndGameWin* create(int reputationGain, int scoreGain, int turnUsed)
        {
            PvPEndGameWin* o = new PvPEndGameWin;
            if(o && o->init(reputationGain, scoreGain, turnUsed))
            {
                o->autorelease();
                return o;
            }
            
            CC_SAFE_DELETE(o);
            return o;
        }
        
    private:
        void onConfirm();
    };
    
    class PvPEndGameLoss : public ContentLayer
    {
    public:
        virtual bool init(int reputationGain, int scoreGain, int turnUsed);
        virtual void onEnterAnimationDone();
        
        static PvPEndGameLoss* create(int reputationGain, int scoreGain, int turnUsed)
        {
            PvPEndGameLoss* o = new PvPEndGameLoss;
            if(o && o->init(reputationGain, scoreGain, turnUsed))
            {
                o->autorelease();
                return o;
            }
            
            CC_SAFE_DELETE(o);
            return o;
        }
    private:
        void onConfirm();
    };
    
    class PvPEndGameTie : public ContentLayer
    {
    public:
        virtual bool init(int reputationGain, int scoreGain, int turnUsed);
        virtual void onEnterAnimationDone();
        
        static PvPEndGameTie* create(int reputationGain, int scoreGain, int turnUsed)
        {
            PvPEndGameTie* o = new PvPEndGameTie;
            if(o && o->init(reputationGain, scoreGain, turnUsed))
            {
                o->autorelease();
                return o;
            }
            
            CC_SAFE_DELETE(o);
            return o;
        }
    private:
        void onConfirm();
    };
}