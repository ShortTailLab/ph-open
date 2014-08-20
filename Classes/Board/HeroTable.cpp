//
//  HeroTable.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 11/1/12.
//
//

#include "HeroTable.h"
#include "DebugMisc.h"
#include <string>
#include <algorithm>
#include <boost/container/flat_map.hpp>

namespace PH
{
    // Hero Profile
    int HeroProfile::getMaxStar() const
    {
        int maxStar = star;
        int transNum = transformTargetHeroId;
        
        for (;;)
        {
            if(transNum == -1)
                break;
            
            const HeroProfile& next = HPT(transNum);
            transNum = next.transformTargetHeroId;
            maxStar++;
        }
        return maxStar;
    }
    
    bool HeroProfile::isMat() const
    {
        return tableId < 10000;
    }
    
    int LevelStats::getOpLevel()
    {
        ////
        // ---------- Accounting after battle ----------
        ////
        float score =0.f;
        
        this->refMaxCombo   = std::max(this->refMaxCombo, 1);
        this->refAvgCombo   = std::max(this->refAvgCombo, 1);
        this->refRound      = std::max(this->refRound, 1);
        
        // A
        float tmp = this->maxCombo;
        tmp /= (float)this->refMaxCombo;
        tmp *= 12;
        tmp = std::min(30.f, tmp);
        score += tmp;
        
        // B
        tmp = (float)this->totalCombo;
        tmp /= (float)this->turnCount;
        tmp /= (float)this->refAvgCombo;
        tmp *= 15;
        tmp = std::min(30.f, tmp);
        score += tmp;
        
        // C
        float tmp2 = 0.f;
        for( auto val : this->roundPerLevel)
            tmp2 += val;
        tmp2 /= (float)this->roundPerLevel.size();
        tmp2 = std::max(1.f, tmp2);
        tmp = (float)this->refRound;
        tmp /= tmp2;
        tmp *= 25;
        tmp = std::min(60.f, tmp);
        score += tmp; 
        
        // D
        tmp = 4.f;
        tmp /= (this->reviveCount+4);
        tmp *= 40.f;
        tmp = std::min(40.f, tmp);
        score += tmp;
        
        int opLevel = 5;
        
        if(score > 61) opLevel = 4;
        if (score > 80) opLevel = 3;
        if (score > 100) opLevel = 2;
        if (score > 130) opLevel = 1;
        
        return opLevel;
        //return 1;
    }
    
    bool isIdChainInTeam(int id,
                         const std::vector<Hero*>& team)
    {
        for(int id2 = id; id2 != -1; id2 = HPT(id2).transformTargetHeroId)
        {
            for(Hero* h : team)
                if(h && h->heroId == id2)
                    return true;
        }
        return false;
    }
    
    bool isRelationActive(const HeroRelation& relation,
                          const std::vector<Hero*>& team)
    {
        for(int id : relation.withHeroes)
        {
            if(!isIdChainInTeam(id, team))
                return false;
        }
        
        return true;
    }

    Hero boostHeroStatsByRelations(const Hero& refH, const HeroList& team)
    {
        Hero h = refH;
        
        for(const HeroRelation& r: h.relations)
        {
            if(!isRelationActive(r, team))
                continue;
            
            for(auto& pair : r.effects)
            {
                RelationEffects effect;
                double val;
                boost::tie(effect, val) = pair;
                
                if(effect == HP)
                    h.health += refH.health * val;
                else if(effect == STRENGTH)
                    h.attack += refH.attack * val;
                else if(effect == REGEN)
                    h.regen += refH.regen * val;
                else
                    LOGD("Unknown relation effect %d", effect);
            }
        }
        return h;
    }
    
    //---------------
    // Hero
    //---------------
    Hero::Hero(std::string uid, int hid, int lvl, int ex) :id(uid), heroId(hid), level(lvl), exp(ex)
    {
        profile = &HPT(hid);
        
        eliteLevel = 0;
        health = profile->health.eval(level);
        attack = profile->attack.eval(level);
        regen  = profile->regen.eval(level);
        expCap = level > profile->maxLevel ? -1 : profile->exp.eval(level);
        maxEnergy = profile->activeSkillId != -1 ? AST(profile->activeSkillId).energy : 0;
        skillBonus = 0;
        trainPrice = lvl * 100;
    }
    
    static boost::container::flat_map<int, HeroProfile*> globalHeroTable;
	static boost::container::flat_map<int, SkillInfo*> globalSkillTable;
    
    const SkillInfo& AST(int i)
    {
        assert(globalSkillTable.count(i) && "Skill does not exist");
        return *globalSkillTable[i];
    }
    
    const SkillInfo& PST(int i)
    {
        assert(globalSkillTable.count(i) && "Skill does not exist");
        return *globalSkillTable[i];
    }
    
    const HeroProfile& HPT(int i)
    {
        assert(globalHeroTable.count(i) && "HeroProfile does not exist");
        return *globalHeroTable[i];     }
    
    int HPTN(const HeroProfile *profile)
    {
        std::vector<HeroProfile*> profiles = heroProfiles();
        for(int i = 0 ; i < profiles.size() ; i++)
        {
            if(profiles[i] && profiles[i]->tableId == profile->tableId)return i;
        }
        return -1;
    }
    
    std::vector<HeroProfile*> heroProfiles()
    {
        std::vector<HeroProfile*> profiles;
        profiles.reserve(globalHeroTable.size());
        
        transform(globalHeroTable.begin(), globalHeroTable.end(), back_inserter(profiles),
                  [](boost::container::flat_map<int, HeroProfile*>::const_reference value){ return value.second; });
        
        std::sort(profiles.begin(), profiles.end(), [](HeroProfile* first, HeroProfile* second){
            return (first->tableId-9999.5)*(second->tableId-9999.5) < 0 ? first->tableId >= 10000 : first->tableId < second->tableId; });
        
        return profiles;
    }
    
    int addHero(HeroProfile* h)
    {
        assert(globalHeroTable.count(h->tableId)==0 && "duplicate HeroProfile");
        globalHeroTable[h->tableId] = h;
        
        return globalHeroTable.size();
    }

	int addASkill(SkillInfo* skill)
	{
		assert(globalSkillTable.count(skill->sid)==0 && "duplicate HeroProfile");
        globalSkillTable[skill->sid] = skill;
        
        return globalSkillTable.size();
	}

	int addPSkill(SkillInfo* skill)
	{
		assert(globalSkillTable.count(skill->sid)==0 && "duplicate HeroProfile");
        globalSkillTable[skill->sid] = skill;
        
        return globalSkillTable.size();
	}
    
    
}