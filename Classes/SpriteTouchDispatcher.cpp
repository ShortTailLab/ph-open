#include "SpriteTouchDispatcher.h"

//using namespace PH;

CCSpriteTouchDispatcher::CCSpriteTouchDispatcher(){}

void CCSpriteTouchDispatcher::attachSpriteTouchDelegate(
                            CCSprite * tar,
                            OnClickedCallback clicked,
                            OnMovedCallback moved)
{
    if(NULL == tar) return;
    
    this->mRegister[tar].first  = clicked;
    this->mRegister[tar].second = moved; 
}

void CCSpriteTouchDispatcher::detachSpriteTouchDelegate(
                            CCSprite * tar)
{
    auto iter = this->mRegister.find(tar);
    if(this->mRegister.end() != iter)
    {
        this->mRegister.erase(iter);
    }
}

bool CCSpriteTouchDispatcher::ccTouchBegan(CCTouch * touch, CCEvent * event)
{

    CCPoint pos = this->convertTouchToNodeSpace(touch);
    printf(" [Sprite - Began - TouchDispatcher] <%f, %f> \n", pos.x, pos.y);

    return true;
}

void CCSpriteTouchDispatcher::ccTouchMoved(CCTouch* touch, CCEvent* event){
    CCPoint pos = this->convertTouchToNodeSpace(touch);
    printf(" [Sprite - Moved - TouchDispatcher] <%f, %f> \n", pos.x, pos.y);
    for(auto entry : this->mRegister)
    {
        auto sprite = entry.first;
        CCRect bound = sprite->boundingBox();
        if(NULL == sprite->getParent()) continue;
        CCPoint localPoint = 
            sprite->getParent()->convertTouchToNodeSpace(touch);
        if(bound.containsPoint(localPoint))
        {
            entry.second.first(touch, event);
        }
    }
}
void CCSpriteTouchDispatcher::ccTouchEnded(CCTouch* touch, CCEvent* event){
    for(auto entry : this->mRegister)
    {
        auto sprite = entry.first;
        CCRect bound = sprite->boundingBox();
        if(NULL == sprite->getParent()) continue;
        CCPoint localPoint = 
            sprite->getParent()->convertTouchToNodeSpace(touch);
        if(bound.containsPoint(localPoint))
        {
            entry.second.second(touch, event);
        }
    }
}
void CCSpriteTouchDispatcher::ccTouchCancelled(CCTouch *touch, CCEvent *event){}

void CCSpriteTouchDispatcher::registerWithTouchDispatcher(void)
{
    CCLayer::registerWithTouchDispatcher();
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -999, true);
}

