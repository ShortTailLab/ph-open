//
//  CalUtils.h
//  HelloCpp
//
//  Created by zql on 13-6-7.
//
//

#pragma once

#include <iostream>
#include "Gem.h"
#include "HeroTable.h"

namespace PH
{
    inline GemUtils::GemColor weakColor(GemUtils::GemColor c)
    {
        switch (c) {
            case GemUtils::Fire:
                return GemUtils::Wood;
            case GemUtils::Wood:
                return GemUtils::Water;
            case GemUtils::Water:
                return GemUtils::Fire;
            case GemUtils::Light:
                return GemUtils::Dark;
            case GemUtils::Dark:
                return GemUtils::Light;
            default:
                phassert(false && "should never reach this");
                break;
        }
        return GemUtils::GemColorMax;
    };
    
    inline GemUtils::GemColor strongColor(GemUtils::GemColor c)
    {
        switch (c) {
            case GemUtils::Fire:
                return GemUtils::Water;
            case GemUtils::Wood:
                return GemUtils::Fire;
            case GemUtils::Water:
                return GemUtils::Wood;
            case GemUtils::Light:
                return GemUtils::Dark;
            case GemUtils::Dark:
                return GemUtils::Light;
            default:
                phassert(false && "should never reach this");
                break;
        }
        return GemUtils::GemColorMax;
    }
    
    inline std::vector<float> calcResistance(const std::vector<Hero*>& team)
    {
        std::vector<float> resist(GemUtils::Health, 0);
        for(int i=0; i<team.size() && i<5; i++)
        {
            if(!team[i])
                continue;
            GemUtils::GemColor color = team[i]->profile->color;
            if(i == 0)
            {
                resist[weakColor(color)] += 0.5 * 0.4f;
                resist[strongColor(color)] += -1.0f * 0.4f;
            }  
            else
            {
                resist[weakColor(color)] += 0.5 * 0.15f;
                resist[strongColor(color)] += -1.0f * 0.15f;
            }
        }
        
        return resist;
    }
    
    inline std::vector<HeroRelation> calcRelationsInTeam(const std::vector<Hero*>& team)
    {
        std::vector<HeroRelation> relats;
        for(const Hero* hero : team)
        {
            if(!hero)
                continue;
            
            for(const HeroRelation& relation : hero->relations)
            {
                if(std::find(relats.begin(), relats.end(), relation) != relats.end())
                    continue;
                
                if(isRelationActive(relation, team))
                    relats.push_back(relation);
            }
        }
        
        return relats;
    }
    
    class GridArrow : public CCNode
    {
    public:
        static GridArrow* create()
        {
            GridArrow* o = new GridArrow();
            if(o && o->init())
            {
                o->autorelease();
                return o;
            }
            CC_SAFE_DELETE(o);
            return NULL;
        }

        virtual bool init();

        void clear();
        
        int size() const { return path.size(); }
        
        void addPoint(const Vec2i& p);
    private:
        std::vector<Vec2i> path;
    };
    
    std::string getTitleOfRepu(int reputation);
}
