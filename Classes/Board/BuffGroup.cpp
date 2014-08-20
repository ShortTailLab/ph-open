//
//  BuffGroup.cpp
//  HelloCpp
//
//  Created by 毛 靖凯 on 13-8-29.
//
//

#include "BuffGroup.h"
#include "CCCallLambda.h"
#include <extensions/utils.h>

using namespace PH;

// --- register basic buff ---

auto a = BuffFactoryReg<IBuff>(IBuff::getId());
auto b = BuffFactoryReg<FloatBuff>(FloatBuff::getId());
auto c = BuffFactoryReg<IntBuff>(IntBuff::getId());
auto d = BuffFactoryReg<Float2Buff>(Float2Buff::getId());
auto e = BuffFactoryReg<FloatIntBuff>(Float2Buff::getId());
auto f = BuffFactoryReg<Int2Buff>(Int2Buff::getId());

// --- end ---

TaskPtr IBuff::update()
{
    if(mTurnLeft <= 0) return TaskIgnore::make();
    mTurnLeft --;
    //printf("[updated]%s\n", mText.c_str());
    return TaskIgnore::make();
}

TaskPtr IBuff::attach()
{
    return TaskIgnore::make();
}

BuffGroup::~BuffGroup()
{
    //for(auto iter)
}

void BuffGroup::vSerialize(std::ostringstream& out) const
{
    out << mBuffMap.size();
    for(auto iter : mBuffMap)
    {
        out << iter.second.first;
        out << iter.second.second->getId();
        out << iter.first;
        iter.second.second->vSerialize(out);
    }
}

void BuffGroup::vDeserialize(std::istringstream &in)
{
    // rock it
    auto factory = IntObjectFactory::getInstance();
    int size,ident,group;
    std::string name;
    
    in >> size;
    for(int i=0; i<size; i++)
    {
        in >> group; in >> ident; in >> name;
        BuffPtr buff(dynamic_cast<IBuff*>(factory->create(ident)));
        //BuffPtr buff((IBuff*)factory.create(ident));
        mBuffMap[name] = std::pair<int, BuffPtr>(group, buff);
        
        buff->vDeserialize(in);
    }
}

TaskPtr BuffGroup::attachBuff(const size_t groupId,
                              const std::string& key,
                              CCNode* parent,
                              BuffPtr buff)
{
    //printf("[attach buff] %s\n", key.c_str());
    if(!buff || !parent) return TaskIgnore::make();
    
    auto batch = TaskBatch::make();
    
    auto findIt = mBuffMap.find(key);
    if( findIt != mBuffMap.end() )
        batch << this->clearLabelWithAnim(findIt->second.second->getLabel());
    
    mBuffMap[key] = std::make_pair(groupId, buff);
    if(mEvalPosition && buff->getText().size() )
        batch << this->setLabelWithAnim(parent, buff);
    batch << buff->attach();
    
    return batch;
}

TaskPtr BuffGroup::detachBuff(const std::string &key)
{
    //printf("[detach buff] %s\n", key.c_str());
    auto findIt = mBuffMap.find(key);
    if( findIt == mBuffMap.end() ) return TaskIgnore::make();
    
    auto batch = TaskBatch::make();
    
    batch << this->clearLabelWithAnim(findIt->second.second->getLabel());
    mBuffMap.erase(findIt);
    
    return batch;
}

TaskPtr BuffGroup::updateBuff(const std::string &key)
{
    auto findIt = mBuffMap.find(key);
    if(findIt == mBuffMap.end()) return TaskIgnore::make();
    
    auto seq = TaskSequence::make();
    seq << findIt->second.second->update();
    if(!findIt->second.second->isActive())
    {
        seq << this->clearLabelWithAnim(findIt->second.second->getLabel());
        mBuffMap.erase(findIt);
    }
    
    return seq;
}

TaskPtr BuffGroup::updateBuff(const size_t groupId)
{
    auto updateBatch = TaskBatch::make();
    auto clearBatch = TaskBatch::make();
    
    for(auto iter=mBuffMap.begin(); iter!=mBuffMap.end();)
    {
        if(iter->second.first == groupId)
            updateBatch << iter->second.second->update();
        
        if(!iter->second.second->isActive())
        {
            clearBatch << this->clearLabelWithAnim(iter->second.second->getLabel());
            // printf("[BuffGroup] trying to delete %s\n",  iter->second.second->getText().c_str());
            mBuffMap.erase(iter++);
        }
        else
            iter++;
    }
    
    return TaskSequence::make()
    << updateBatch
    << clearBatch;
}

static inline bool isInSlots(std::vector<StatusLabelPtr>& slots, StatusLabelPtr label)
{
    return std::find(slots.begin(), slots.end(), label) != slots.end();
}

static int findSlotForLabel(std::vector<StatusLabelPtr>& slots, StatusLabelPtr label)
{
    for(int i=0; i<slots.size(); i++)
        if(slots[i] == NULL || slots[i] == label)
            return i;
    return slots.size();
}
TaskPtr BuffGroup::setLabelWithAnim(cocos2d::CCNode *layer, BuffPtr buff)
{
    //printf("[set]%d\n",int(buff.get()));
    TaskSequencePtr seq = TaskSequence::make();
    
    /*
     ISSUES:
     StatusLabel::show will fail if setTextAndColor is not called.
     adding status label must be a delayed operation since
     status labels could be removed in previous updates in the same frame.
     */
    
    
    // This must be delayed since label could be already scheduled
    // to be removed frame parent in the same frame.
    auto label = buff->getLabel();
    if(!label) return TaskIgnore::make();
    
    seq << TaskLambda::make([=]()
    {
        int i = findSlotForLabel(mStatusSlots, label);
        
        CCPoint p = mEvalPosition(i);
        label->setPosition(p);
        
        // not in slots
        if(mStatusSlots.size() == i)
        {
            mStatusSlots.push_back(label);
            if(layer)
                layer->addChild(label->getRoot(), ORDER_MASK+1);
        }
        // check if already in slots
        else if(!isInSlots(mStatusSlots, label))
        {
            mStatusSlots[i] = label;
            if(layer)
                layer->addChild(label->getRoot(), ORDER_MASK+1);
        }
        
        label->setTextAndColor(buff->getText(), buff->getColor());
    });
    
    seq << label->show();
    
    return seq;
}

TaskPtr BuffGroup::createLabelAnim(StatusLabelPtr propertyLabel)
{
    //printf("[src]%d\n",int(propertyLabel.get()));
    CCNode* l = propertyLabel->getRoot();
    // NOTE: property anim must because it's really easy to overwrite earlier anims
    // in different batches and we get a lost action blocking everything.
    l->stopAllActions();
    l->setScale(1.f);
    
    
    CCSprite * medium = GetSprite("fadong_01.png");
    
    l->addChild(medium, -100);
    medium->setOpacity(0);
    
    auto seq = CCSequence::create(CCScaleTo::create(0.35f, 1.3f),
                                  CCScaleTo::create(0.35f, 1.0f),
                                  NULL);
    return
    TaskSequence::make()
    //<< TaskAnim::make(medium, act, false)
    << TaskLambda::make([=]()
    {
        auto anim = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("skill_trigger"));
        medium->setOpacity(175);
        medium->runAction(CCSequence::create(anim,
                                             CCCallLambda::create([=]()
                                              {
                                                  medium->removeFromParent();
                                              }), NULL));
    })
    << TaskAnim::make(l, seq, false);
}

TaskPtr BuffGroup::clearLabelWithAnim(StatusLabelPtr label)
{
    //printf("[clear]%d\n",int(label.get()));
    // Do not ever erase from status slots, only reset its pointers.
    // Each slot's index is use to compute its position
    return TaskSequence::make()
    << TaskLambda::make([=]()
    {
        auto x = std::find(mStatusSlots.begin(),
                           mStatusSlots.end(),
                           label);
        if(x != mStatusSlots.end())
        {
            x->reset();
            label->hide();
        }
    });
}