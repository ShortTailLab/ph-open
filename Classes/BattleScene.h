#pragma once

#include "Types.h"
#include "PvPMessageHandler.h"
#include <rapidjson/document.h>

namespace PH
{
    class PvPConfig;
    class BoardLayer;
    
	class BattleScene : public CCScene, public CCKeypadDelegate
	{
	private:
        void loadBattleResources();
        void loadBattleResourcesCont(CCObject*);
        
	public:
        virtual ~BattleScene();
		bool initWithLevel(HeroList& team, LevelInfo& level);
        
   		bool initPvP(const Player& team1, const Player& team2, const PvPConfig& config);
        
        bool initPvP(int seed,
                     int round,
                     const Player& first,
                     const Player& second,
                     const rapidjson::Value& battleState,
                     float skillTimeout,
                     float moveTimeout,
                     float skillTimeLeft,
                     float moveTimeLeft,
                     const rapidjson::Value& events);
        
        static BattleScene* create(HeroList& team, LevelInfo& level);
        
        static BattleScene* create(const Player& team1,
                                   const Player& team2,
                                   const PvPConfig& config);
        
        static BattleScene* create(int seed,
                                   int round,
                                   const Player& first,
                                   const Player& second,
                                   const rapidjson::Value& battleState,
                                   float skillTimeout,
                                   float moveTimeout,
                                   float skillTimeLeft,
                                   float moveTimeLeft,
                                   const rapidjson::Value& events);
	};
    
    CCAnimation* createAttackAnimFromColor(GemUtils::GemColor c, int numFrames);
    CCAnimation* createFlareAnim();
    void unloadBattleResources();

}