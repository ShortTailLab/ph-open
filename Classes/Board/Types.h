//
//  Types.h
//  HelloCpp
//
//  Created by Zeyang Li on 5/22/13.
//
//

#pragma once

#include <cocos2d.h>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>
#include <map>

#include "Board/Gem.h"
#include "Board/HeroTable.h"

namespace PH
{
    USING_NS_CC;
    
	struct Level
	{
        std::string id;
		std::string name;
		int cost;
		int roundCount;
        int level;
        int cleared; 
        
        int levelLimit;
        std::vector<int> dropsHint;
        //std::string levelHint;
	};
    
    enum StageEvent
    {
        NoEvent = 0,
        SpiritHalf = 1,
        DropTwice = 2,
        DropOneDotFive = 3,
        CoinsTwice = 4,
    };
    
    struct Stage
    {
        std::string id;
        std::string name;
        
        StageEvent event;
        
        boost::posix_time::ptime visibleStart, visibleEnd;
        
        int startTimeStamp;
        int endTimeStamp;
        int stage;
        int dailyQuota;     // 每日刷关次数，-1无限
        //int progress; // -1 reserved for no limit symbol
        bool rewardAvailable; // 全S通关是否已经领取
        bool hasReward; // 是否有全S通关奖励配置
        int dailyInstanceProgress;

        GemUtils::GemColor stageColor;
        std::string hint;
        //std::vector<int> dropsHint;
        
        std::vector<Level> levelList;
        
        const Level* findLevelById(std::string& idd)
        {
            for(Level& l : levelList)
            {
                if(l.id == idd)
                    return &l;
            }
            return NULL;
        }
        
        bool isAllPass() const
        {
            for(const Level& level : levelList)
                if(level.cleared != 1)
                    return false;
            return true;
        }
    };
    typedef boost::shared_ptr<Stage> StagePtr;
    
    struct DailyStage
    {
        int progress;
        StagePtr stage;
    };
    
    struct Account
    {
        std::string type;
        std::string id;
    };
    
    enum LoginRewardType
    {
        Reward_Exp = 0,
        Reward_Coins,
        Reward_Gold,
        Reward_Hero,
        Reward_Soul,
        Reward_PercentExp,
        Reward_PercentCoins
    };
    
    struct Treasure
    {
        int type;
        int count;
        int ident;
        int level;
        
        Treasure(){};
        Treasure(int type, int count)
        {
            this->type = type;
            this->count = count;
        };
    };
    
    typedef std::vector<Treasure> LoginRewards;
    typedef std::vector<Treasure> VictoryRewards;
    typedef std::vector<Treasure> ChestRewards;
    typedef std::vector<Treasure> TaskRewards;
    
    struct Reward
    {
        int totalCoins, totalGolds;
        std::string html;
    };
    
    struct VIPPack
    {
        int pid;
        int availableAtLevel;
        int price, showPrice;
        
        std::vector<Treasure> items;
        bool purchased;
        bool isHeroesPack() const
        {
            if(items.empty())return false;
            for(const Treasure& t : items)
                if(t.type != LoginRewardType::Reward_Hero || HPT(t.ident).isMat())
                    return false;
            return true;
        }
    };
    
    enum class PlayerTaskType
    {
        NoneCond = 1,
        LevelClear = 2,
        LevelS = 3,
        UserLevel = 4,
        DailyHelperAward = 5,
        HelperAwardNum = 6,
        FriendsNum = 7,
        StorageLimit = 8,
        
        PayGolds = 18,
        PayStorage = 10,
        PaySpirit = 11,
        PayNiceHeroDraw = 12,
        PayExpItems = 13,
        PayTransItems = 14,
        PayRevive = 15,
        
        VIPLevel = 16,
        LevelSCount = 17,
        
        Spending = 9,
        ItemStorage = 19,
        
        DailyLevelClearCount = 20,
        DailyPayGoldsAmount = 21,
        Succeed3StarsInDraw10Heroes = 22,
        SucceedTransformHero = 23,
        SucceedChangeRelations = 24,
        
        MaxRelations = 25,
        
        SucceedExchangeBySoul = 26,
        SucceedPromoteEliteLevel = 27
    };
    
    enum class PlayerTaskCategory
    {
        Normal = 0,
        Daily = 1,
        Newbie = 2,
        Purchase = 3
    };
    
    // LevelClear, LevelS, LevelSCount 需要跳转到江湖
    // VIPLevel, PayGolds, PayStorage, PaySpirit, PayNiceHeroDraw, PayExpItems,
    // PayTransItems, StorageLimit 需要跳转到商店
    // 其他待定
    
    struct PlayerTask
    {
        int tid;
        PlayerTaskType type;
        PlayerTaskCategory category;
        std::string name, desc;
        int progress, total;
        int index, position;
        
        std::vector<Treasure> sample;
    };
    
    typedef std::vector<PlayerTask> PlayerTaskVec;
    
	struct Player
	{
        virtual ~Player()
        {
            for(auto& h : storage)
                if(h)
                {
                    delete h;
                    h = NULL;
                }
        }
        
        std::string uid;
        std::string name;
        
        int level;
        int exp;
        int coins;
        int golds;
        
        int nextLevelExp;
        
        int spirit, maxSpirit;
        int maxLP, maxFriends;
        
        int lastLoginTS;
		int spiritCheckTime;
        
        std::string token, referCode;
        
        std::vector<Account> accounts;
        std::vector<int> drawFreeTime;
        
        // team stores pointer to heroes in storage
		std::vector<Hero*> team;
        int activeTeam;
        int activeTeamScore, dailyJudgmentCount;
        
        
        std::vector<Hero*> storage;
        int storageLimit;
        
        std::vector<int> seen, owned;
        
        std::map<int, int> heroSouls;

        int pvpReputation;
        int pvpScore;
        int pvpRank, pvpKeys;
        
        double pvpWinRate;
        
        bool isFriend;
        int asHelperReward;
        
        int pendingFriendRequests;
        int vipLevel, spent;
        int drawsLeft, expDrawsLeft, transDrawsLeft;
        
        //
        VictoryRewards rewards;
        int availableRewardsCount;
        bool isFirstScoreAfterBattle;
        //
        
        Hero* findHero(const std::string& hid);
        Hero* getMainHero(){return team[activeTeam*5];};
	};
    typedef boost::shared_ptr<Player> PlayerPtr;
    
    struct Mail
    {
        std::string id;
        
        std::string sender;
        std::string message;
        std::vector<Treasure> package;
        boost::posix_time::ptime time;
        
        bool read, packageOpened;
    };
    
    struct MailBox
    {
        std::vector<Mail> mails;
        boost::posix_time::ptime mailSince;
    };
    
    struct VipUpgrade
    {
        int coins, golds, pvpKeys;
        int lastVIPLevel, newVIPLevel;
        std::vector<boost::shared_ptr<Stage>> stages, fbs;
        std::vector<Hero> expItems;
    };
    
    enum class SkillBonusChangeType
    {
        Golds = 0,
        Potions = 1
    };
    
    enum class RelationChangeType
    {
        Free = 0,
        Golds = 1,
        Potions = 2
    };
    
    struct BoolWrapper
    {
        bool ok;
    };
    
    struct RankingItem
    {
        std::string ident;
        std::string owner;
        int hostLevel, score;
        int flower, egg;
        std::vector<boost::shared_ptr<Hero>> team;
    };
    
    enum class RankingType
    {
        Team_Fire = 0,
        Team_Water = 1,
        Team_Wood = 2,
        Team_Dark = 3,
        Team_Light = 4,
        Team_All = 5,
        Team_Mix = 6,
        Player_Level = 7
    };
    
    struct RankingList
    {
        std::vector<RankingItem> table;
        int hostRank, rankDelta, hostScore;
    };
    
    typedef std::vector<RankingList> RankingListVec;
    
    struct PlayerRankList
    {
        ~PlayerRankList()
        {
            for (auto p : players)
                delete p;
        }
        
        std::vector<Player*> players;
        
        int myRank;
    };
    
    struct PvPGameLog
    {
        std::string rivalName;
        int oldRank, newRank;
        int oldRep, newRep;
        int result;
        
        boost::posix_time::ptime time;
        PvPGameLog(){};
        PvPGameLog(std::string n):rivalName(n){};
    };
    
    struct PvPGameLogList
    {
        std::vector<PvPGameLog> logs;
    };
    
    struct PlayerList
    {
        ~PlayerList()
        {
            for (auto p : players)
                delete p;
        }
        
        std::vector<Player*> players;
        
        int userCount;
    };
    
    struct StoreItem
    {
        std::string channel;
        std::string productId;
        std::string productName;
        std::string localizePrice;
        
        int amount;
        int price;
        
        int goldPerDay;
        
        float promotion;
    };
    
    enum class PvPStoreItemType : int
    {
        RelationPill = 1,
        SkillPill    = 2,
        ElitePill    = 3,
    };
    
    struct PvPStoreItem
    {
        int sid;
        PvPStoreItemType itemType;
        int price;
        int amount;
        
        std::string name, desc;
    };
    
    struct Control
    {
        std::string annoucement;
        std::vector<int> drawNiceHeroPrices;
        std::vector<int> vipLevelSpent;
        
        Reward rewards;
        LoginRewards loginRewards;
        std::vector<int> path;
        
        std::vector<StoreItem> store, monthly;
        std::vector<PvPStoreItem> pvpStore;
        std::vector<boost::shared_ptr<Stage>> stages, fbs;
        std::vector<DailyStage> dailystages;
        std::vector<VIPPack> vipGifts, otherVipGifts;
        
        int maxLevel;
        int tutorialStage;
        
        long long serverTime;
        int spiritRecoverSecs;
        
        std::string drawHeroImgURL, headVersion;
        bool forceUpgrade;
        
        bool pvpOptIn;
        bool showVIPDraw, showVIPTenDraw;
        int dailyFreeRelationChangeCount;
        bool isFirstScoreAfterBattle;
        
        std::string getRewardsString() const;
        
        int taskHints;
        
        std::string serverURL;
        bool hasPromotion;
        
        boost::posix_time::ptime pvpStart, pvpEnd;
    };
}
