//
//  HeroTable.h
//  HelloCpp
//
//  Created by Zeyang Li on 11/1/12.
//
//

#pragma once

#include "Gem.h"
#include "Utils.h"
#include <extensions/utils.h>

#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

namespace PH { // tolua_export
    
    struct SkillInfo
	{
        int sid;
        std::string name;
        int energy;
        std::string desc;
        std::string skillScript;
        
        int maxCooldown;
        
        SkillInfo() : sid(-1), name(""), energy(0), desc(""), skillScript(""), maxCooldown(3) {}
	};
    
    struct Polynomial
    {
        float A, B, C, D;
        int type;
        
        Polynomial(float d=0.0f) :
        A(0.0f), B(0.0f), C(0.0f), D(d), type(1) {}
        
        Polynomial(float b, float d) :
        A(0.0f), B(b), C(0.0f), D(d), type(2) {}
        
        Polynomial(float a, float b, float c, float d) :
        A(a), B(b), C(c), D(d), type(4){}
        
        float eval(float x) const
        {
            switch(type)
            {
                case 1: return               D;
                case 2: return         B*x + D;
                case 3: return A*x*x + B*x + D;
                case 4: return A*x*x + B*x + C*sqrt(x) + D;
                default:
                    break;
            }
            assert(false && "polynomial type unknown");
            return 0;
        }
    };
    
    inline std::string heroPath(const std::string& name, int star, const std::string& type)
    {
        return "heroes/" + name + "/" + boost::lexical_cast<std::string>(star) + "_" + type + ".png";
    }
    
    enum RelationEffects
    {
        HP = 1,
        STRENGTH = 2,
        REGEN = 3,
    };
    
    struct HeroRelation
    {
        std::string id;
        std::string name;
        std::vector<int> withHeroes;
        std::vector<boost::tuple<RelationEffects, double>> effects;
        
        bool operator==(const HeroRelation& r) const
        {
            return id == r.id;
        }
    };
    
    struct HeroProfile
    {
        int tableId;
        
        std::string name;
        std::string nameCN;
        std::string title;
        std::string faction;
        std::string type;
        std::string usage;
        std::string dropHint;
        
        int star;
        
        GemUtils::GemColor color;
        
        int maxLevel;
        

        Polynomial health;
        Polynomial attack;
        Polynomial regen;

        
        Polynomial exp;
        Polynomial price;
        
        Polynomial gain;
        
        Polynomial eliteHP, eliteAttack, eliteRegen;
        
        int activeSkillId;
        int passiveSkillId;
        
        int pvpActiveSkillId;
        int pvpPassiveSkillId;

		int LP;
        int costSouls;
        
        int transformTargetHeroId;
        std::vector<int> transformMats;
        int transformPrice;
        
        int correct;
        int potions;

        std::string iconPath() const
        {
            return name + "/" + boost::lexical_cast<std::string>(star) + "_icon.png";
//            std::string path = heroPath(name, star, "icon");
//            if(resourcePresentForLoading(path))
//                return path;
//            else
//                return std::string("heroes/placeholder_icon.png");
        }
        
        std::string bodyPath() const
        {
            std::string path = heroPath(name, star, "body");
            if(resourcePresentForLoading(path))
                return path;
            else
                return std::string("heroes/placeholder_body.png");
        }
        
        int getMaxStar() const;
        bool isMat() const;
        
        CCRect indexPageRegion;
    };

    struct HeroSoulDrop
    {
        int heroId;
        int count;
    };
    typedef boost::shared_ptr<HeroSoulDrop> HeroSoulDropPtr;
    
    // This class must remain as a pure value class.
    // Deep copy == Shallow copy
    struct Hero
    {
        std::string id;
        int heroId;
        
        const HeroProfile* profile;
        
        int level;
        int exp;
        
        int eliteLevel, skillBonus;
        int chance;
        
        // generated values
        int health;
        int attack;
        int regen;
        int fightSkill;
        
        int expCap;
        int maxEnergy;

		int trainPrice;
		bool isLock;
        
        std::vector<HeroRelation> relations;
        
        HeroSoulDrop soul;
        std::vector<int> next8Draws;
        int nextFreeDraw;
        float trainRate;

        Hero(){};
        Hero(std::string uid, int hid, int lvl, int ex);
        int getFightSkill() const
        {
            return float(health)/3+float(attack)/2+regen+profile->correct;
        }
    };
    typedef boost::shared_ptr<Hero> HeroPtr;
    typedef std::vector<Hero*> HeroList;
        
    struct Hero10Draw
    {
        std::vector<Hero*> heroes;
        HeroSoulDrop soul;
    };
    
    struct EnemyInfo
    {
        int hid;
        
        std::string name;
        std::string nameCN;
        GemUtils::GemColor color;
        int star;
        
        int maxHealth;
        int armor;
        int attack;
        int maxTurn;
        int initialTurn;
        
        std::string script;
        std::string passiveScript;
        std::string turnBasedScript;
        
        HeroPtr drop;
        HeroSoulDropPtr soulDrop;
        
        std::string iconPath() const
        {
            return heroPath(name, star, "icon");
//            std::string path = heroPath(name, star, "icon");
//            if(resourcePresentForLoading(path))
//                return path;
//            else
//                return std::string("heroes/placeholder_icon.png");
        }
        
        std::string bodyPath() const
        {
            std::string path = heroPath(name, star, "body");
            if(resourcePresentForLoading(path))
                return path;
            else
                return std::string("heroes/placeholder_body.png");
        }
    };
    
    typedef std::vector<EnemyInfo> RoundInfo;
    
    typedef std::vector<RoundInfo> RoundList;
    
    struct LevelInfo
    {
        int spirit;
        int golds;
        int coins;
        int exp;
        std::string bg;
        std::string id;
        int lid;
        
        int avgCombo, maxCombo;
        int refRound;
            
        std::vector<std::string> avoidGems;
        
        RoundList roundList;
    };
        
    struct LevelStats
    {
        int refAvgCombo, refMaxCombo, refRound;
        int maxCombo;
        int maxGemCount;
        int turnCount;  //
        int deathCount;
        int reviveCount; //
            
        std::vector<int> maxComboPerLevel; // todo
        std::vector<int> roundPerLevel; // todo
        int endRound; //
        int totalCombo; //
            
        int skillUseCount;
        
        float gameTimeSinceLevelStart;
            
        LevelStats() : maxCombo(0), maxGemCount(0), turnCount(0), deathCount(0),
            reviveCount(0), skillUseCount(0), gameTimeSinceLevelStart(0.0f),
            endRound(-1), totalCombo(0)
        {}
        
        int getOpLevel();
    };
        
    typedef int LevelRewardsType;
        /*
    static const int EXP    = 0;
    static const int COINS  = 1;
    static const int GOLDS  = 2;
    static const int HERO   = 3;
    static const int SOUL   = 4;
         */
        
    // type -> id + count
    //typedef std::vector<std::pair<LevelRewardsType, std::pair<int, int>>> LevelRewards;
    
    const SkillInfo& AST(int i);
    const SkillInfo& PST(int i);
    const HeroProfile& HPT(int i);
    int HPTN(const HeroProfile *profile);
    
    std::vector<HeroProfile*> heroProfiles();
    
    int addHero(HeroProfile* h);
	int addASkill(SkillInfo* skill);
	int addPSkill(SkillInfo* skill);

    bool isIdChainInTeam(int id, const std::vector<Hero*>& team);
    bool isRelationActive(const HeroRelation& relation,
                          const std::vector<Hero*>& team);
        
    Hero boostHeroStatsByRelations(const Hero& h, const HeroList& team);

} // tolua_export
