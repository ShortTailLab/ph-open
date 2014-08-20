#pragma once

#include <map>
#include <cocos2d.h>

//namespace PH
//{
    USING_NS_CC;
    class CCSpriteTouchDispatcher : public CCLayer
    {
        typedef boost::function<void (CCTouch * touch, CCEvent * event)> OnClickedCallback;
        typedef boost::function<void (CCTouch * touch, CCEvent * event)> OnMovedCallback;
        typedef std::map<CCSprite*, 
                         std::pair<OnClickedCallback, OnMovedCallback> > CallbackMap;

    protected:
        CallbackMap    mRegister;

        virtual bool init()
        {
            this->setTouchEnabled(true);
            
            return true;
        }

    public:
        CCSpriteTouchDispatcher();
        static CCSpriteTouchDispatcher * create()
        {
            CCSpriteTouchDispatcher * ret = new CCSpriteTouchDispatcher();
            if(ret != NULL)
            {
                if(ret->init()){
                    ret->autorelease();
                }else
                {
                    delete ret;
                    ret = NULL;
                }
            }
            return ret;
            
        }

        virtual void attachSpriteTouchDelegate(
                                            CCSprite * tar,
                                            OnClickedCallback clicked = [](CCTouch*, CCEvent*){},
                                            OnMovedCallback moved = [](CCTouch*, CCEvent*){});
        
        virtual void detachSpriteTouchDelegate(CCSprite * tar);
        
        // default implements are used to call script callback if exist
        virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
        virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);
        virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
        virtual void ccTouchCancelled(CCTouch *touch, CCEvent *event);
        virtual void registerWithTouchDispatcher(void);
    };
//}
