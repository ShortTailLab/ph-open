//
//  TouchMaskLayer.h
//  HelloCpp
//
//  Created by Zeyang Li on 1/3/13.
//
//

#pragma once

#include "cocos2d.h"

#include <vector>

namespace PH
{
    USING_NS_CC;
    
    enum class ArrowSize
    {
        Small,
        Medium,
        Big,
    };
    
    enum class ArrowDirection
    {
        Left,
        Right,
        Up,
        Down,
    };
    
    CCSprite* createTutorialArrow(const CCPoint& p,
                                  ArrowSize type,
                                  ArrowDirection direction,
                                  bool useMoveAnim = false);

    
    class ArrowMask : public CCLayer
    {
    public:
        virtual bool init();
        void setMask(const CCRect& rect, bool useDarkBG = true);
        
        void setArrow(const CCPoint& p, ArrowSize type = ArrowSize::Medium, ArrowDirection dir = ArrowDirection::Right);
        void setArrow(const CCPoint& p, ArrowDirection dir);
        void clearArrow();

        void setArrow2(const CCPoint& p, ArrowSize type = ArrowSize::Medium, ArrowDirection dir = ArrowDirection::Right);
        void setArrow2(const CCPoint& p, ArrowDirection dir);
        void addArrow(const CCPoint& p, ArrowDirection dir);
        void clearArrow2();
         
        
        void clear();
        
        void fadeInBackground(float time);
        
        static ArrowMask* shared()
        {
            static ArrowMask* mask = NULL;
            
            if(!mask)
            {
                mask = new ArrowMask;
                mask->init();
            }
            
            return mask;
        }
        
        const CCRect& getCutout() { return mCutout; }

    protected:
        virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
        virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent){}
        virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent){}
        virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent){}
        
        virtual void registerWithTouchDispatcher();
        
    private:
        CCRect mCutout;
        CCSprite* mMask;
        CCSprite* mArrow;
        CCSprite* mArrow2;
        CCLayer* mArrows;
        CCSprite* light;
    };
}