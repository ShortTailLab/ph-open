//
//  Property.h
//  HelloCpp
//
//  Created by Zeyang Li on 12/4/12.
//
//

#pragma once

#include <vector>
#include "StatusLabel.h"

namespace PH
{
    class HeroControl;
    class EnemyControl;

    template<typename T>
    class Property
    {
    public:
        int turnLeft;
        T value;
        StatusLabelPtr label;
        GemUtils::GemColor color;
        
    public:
        Property() : label(new StatusLabel), turnLeft(0) {}
        
        void set(int d, T val, GemUtils::GemColor c = GemUtils::AllColor)
        {
            color = c;
            turnLeft = d;
            value = val;
        }
        
        bool has() const { return turnLeft > 0; }
        T    val() const { return value; }
        T&   val()       { return value; }
        bool isLastTurn() const { return turnLeft == 1; }
        int updateTurn() { turnLeft = std::max(turnLeft-1, 0);  return turnLeft; }
        
        void clear() { turnLeft = -1; }
        int turnCount() { return turnLeft; }
        
        StatusLabelPtr getLabel() { return label; }
    };
    
    typedef Property<int>                       IntProperty;
    typedef Property<float>                     FloatProperty;
    typedef Property<bool>                      BoolProperty;
    typedef Property<std::pair<bool, float>>    BoolFloatProperty;
    typedef Property<std::pair<float, float>>   FloatFloatProperty;
    
    template<typename Target, typename T>
    class PassiveProperty
    {
    protected:
        std::map<Target*, T> valueMap;
        
    public:
        bool isValid() { return valueMap.size() > 0; }
        void operator ()(Target* h, T v)
        {
            valueMap[h] = v;
        }
        
        std::vector<Target*> casters() const
        {
            std::vector<Target*> h;
            for(auto x : valueMap)
                h.push_back(x.first);
            return h;
        }
        
        const std::map<Target*, T>& map() const { return valueMap; }
    };

    typedef PassiveProperty<HeroControl,float>                      FloatPassiveProperty;
    typedef PassiveProperty<HeroControl,int>                        IntPassiveProperty;
    typedef PassiveProperty<HeroControl,std::pair<bool, float>>     BoolFloatPassiveProperty;
    typedef PassiveProperty<HeroControl,std::pair<float, float> >   FloatFloatPassiveProperty;
    typedef PassiveProperty<HeroControl, std::pair<std::pair<float, float>, float> >   Float3PassiveProperty;
    
    template<class Target, class Type>
    inline Type sumPP(const PassiveProperty<Target, Type>& p)
    {
        float y = 0.0f;
        for(auto x : p.map())
            y += x.second;
        return y;
    }

    inline float productPP(const FloatPassiveProperty& p)
    {
        float y = 1.0f;
        for(auto x : p.map())
            y *= x.second;
        return y;
    }
    
    inline float minPP(const FloatPassiveProperty& p)
    {
        float y = 100000000.0f;
        for(auto x : p.map())
            y = std::min(y, x.second);
        return y;
    }
    
    inline float maxPP(const FloatPassiveProperty& p)
    {
        float y = -100000000.0f;
        for(auto x : p.map())
            y = std::max(y, x.second);
        return y;
    }
}
