//
//  PlayerData.h
//
//  Created by ZQL on 2012 11 27
//
//


#pragma once

#include "BattleScene.h"

#include <boost/function.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/shared_ptr.hpp>

namespace PH
{
    
typedef boost::tuple<std::vector<HeroRelation> , std::vector<HeroRelation>> relationsYesNo;
    
typedef std::map<std::string, int> HeroStr2IntMap;
const  HeroStr2IntMap::value_type types[6] = {
            HeroStr2IntMap::value_type("博学", 1),
            HeroStr2IntMap::value_type("刀剑", 2),
            HeroStr2IntMap::value_type("拳掌", 3),
            HeroStr2IntMap::value_type("阴阳", 4),
            HeroStr2IntMap::value_type("内功", 5),
            HeroStr2IntMap::value_type("无", 6)};

static std::map<std::string, int> heroTitleMap(types, types+6);
    
enum HelpersSortType
{
    Normal_Sort = 0,
    Color_Sort,
    HP_Sort,
    Attack_Sort,
    Regen_Sort,
    Title_Sort,
};
enum SoulsSortType
{
    Nor_Soul_Sort = 0,
    Star_Soul_Sort,
    Color_Soul_Sort,
    Left_Soul_Sort
};
    
enum FriendSortType
{
    Nor_Fri_Sort = 0,
    Time_Fri_Sort,
    Color_Fri_Sort,
    Blood_Fri_Sort,
    Attack_Fri_Sort,
    Regen_Fri_Sort,
    Level_Fri_Sort
};

std::vector<Hero*> sortHeroesByTime(const std::vector<Hero*>& heroes);
std::vector<Hero*> sortHeroesByLock(const std::vector<Hero*>& heroes);
std::vector<Hero*> sortHeroesByHP(const std::vector<Hero*>& heroes);
std::vector<Hero*> sortHeroesByColor(const std::vector<Hero*>& heroes);
std::vector<Hero*> sortHeroesByAttack(const std::vector<Hero*>& heroes);
std::vector<Hero*> sortHeroesByRegen(const std::vector<Hero*>& heroes);
std::vector<Hero*> sortHeroesByStars(const std::vector<Hero*>& heroes);
std::vector<Hero*> sortHeroesByTitle(const std::vector<Hero*>& heroes);
std::vector<Hero*> sortHeroesByStorageType(const std::vector<Hero*>& heroes, int type);


struct TeamData
{
	int hp;
    float attack[GemUtils::GemColorMax];
	int regen;
	int lp;
	int maxLp;
    int fightSkill;
	const SkillInfo *skill;
    const SkillInfo *battleSkill;

	TeamData();
	void update(const std::vector<Hero*>& team);
};

enum HeroIdOfType
{
    Hero_GasPill = 7213,
    Hero_RelPill = 7214
};
    
class PlayerData
{
public:
	PlayerData();
	~PlayerData();
    
    void update(std::auto_ptr<Player>, std::auto_ptr<Control>);
    void updateTeamFromPlayer(Player*);
    
    void userLoggedOff();
    
    Level* findLevel(const std::string& levelId);
    boost::shared_ptr<Stage> findStage(int stageId);

	Player *player;
	TeamData teamData;
    boost::posix_time::ptime lastSendMailTime;
    boost::posix_time::ptime pvpStartTime;
    boost::posix_time::ptime pvpEndTime;
	int tutorialStage;
	int maxLevel;
    int spiritRecoverSecs;
    bool isPvPOpen;
    bool hasDrawJueDing;
    bool hasTenDraw;
    bool firstTimeBattleScore;
    bool hasNewMail;
    bool hasPromotion;
    int newMailsNum;
    int doneTasksNum;
    int freeRelationTimes;
    std::string annoucement;
	std::vector<boost::shared_ptr<Stage>> normalStageList;    // zhang yi
	std::vector<boost::shared_ptr<Stage>> instanceStageList;  // fu ben
    std::vector<DailyStage>               dailyStageList;
	std::vector<StoreItem> storeItems;
    std::vector<PvPStoreItem> pvpStoreItems;
	std::vector<Hero*> currentStorage;
	std::vector<PlayerPtr> friends;
    int drawFreeLeft, drawExpLeft, drawTransLeft;
	std::vector<int> drawNiceHeroPrices;
    std::vector<int> drawFreeTime;
    std::vector<int> helperRewards;
    std::vector<int> vipLevelSpent;
    std::vector<int> newSouls;
    std::vector<VIPPack> vipPacks;
	boost::function<void ()> stageChangeCallBack;
	std::map<cocos2d::CCNode* , boost::function<void ()> > storageChangeCallBackMap;
    
    PlayerPtr lastAllyPlayer;
    
    boost::posix_time::time_duration delta_local_server;
    
    LoginRewards loginRewards;
    std::vector<int> rewardPath;

	void updatePlayer(Player *targetPlayer);
	void updateTeam(const std::vector<Hero*>& team);
    
    void setDrawFreeTime(int type , int time);
    bool isDrawFree();
    bool isDrawFree(int type);

	bool isHeroTransable(Hero *hero);
    bool isHeroTransformMatsReady(Hero *hero);
    bool isHeroInTeam(Hero *hero);
    bool isHeroInActiveTeam(const Hero* hero);
    bool isHeroRelationReady(HeroRelation& r);
    relationsYesNo heroReadyRelations(const Hero* hero);
    bool isDailyInstance(const std::string & idd);
    bool isDailyInstanceByStage(const std::string & idd);

	const std::string& getToken();
    
	void setName(const std::string& name);
    void setVipLevel(int vipLevel);
	void setLevel(int level);
	void setCoins(int value);
	void setGolds(int value);

	void setExp(int exp, int maxExp, int levelDiff);
	void setSpirit(int spirit, int maxSpirit);
	int getSpirit();
    int getNumOfHero(const HeroIdOfType& type);
    void removeNumOfHero(const HeroIdOfType& type, int num);

    boost::posix_time::ptime spiritCheckTime;
	void updateSpiritTime();
    
    void setActiveTeam(int i);
    int getActiveTeam();
	std::vector<Hero*> getTeam();
    std::vector<std::string> getTeamIds();
    std::vector<Hero*> getHeroesInAllTeams();
	
	void setNormalStage(const std::vector<boost::shared_ptr<Stage>>& stages);
	void setInstanceStage(const std::vector<boost::shared_ptr<Stage>>& stages);
    void setDailyStage(const std::vector<DailyStage>& stages);
    void removeDailyStageByLevelId(const std::string & levelId);
	void registerStageCallBack(boost::function<void ()> callBack);
	void unRegisterStageCallBack();

	void registerStorageCallBack(CCNode *node , boost::function<void ()> callBack);
	void unRegisterStorageCallBack(CCNode *node);
	void storageCallBack();

	Player *getPlayer();
	void switchSortType();
	void setSortType(int type);
    int getSortType();
	const char* getSortTypeMsg();
	std::vector<Hero*> getStorage();
    
    HelpersSortType getHelpersSortType();
    void switchHelpersSortType();
    
    SoulsSortType getSoulSortType();
    void switchSoulSortType();
    
    FriendSortType getFriSortType();
    void switchFriSortType();
    
	Hero *getHero(const std::string& heroId);
	Hero *getHero(int index);
	Hero *getHeroByHeroId(int id);
    Hero *getUnlockHeroById(int id);
    std::vector<Hero*> getUnlockHeroByIds(const std::vector<int>& id);
    void markHeroNew(Hero* hero);
    std::vector<Hero*> getNewHeroes(const std::string& type);
    void clearNewHeroes(const std::string& type);
	int getHeroIndex(Hero *hero);
	void removeHero(const std::string& heroId);
	void addHero(Hero *hero);
    void addHeroSoul(HeroSoulDrop soul);
    std::vector<Hero*> getHeroesById(int i);
	void updateHero(Hero *hero);
	void lockHero(Hero *target , bool value);
    
    void registerLocalPushNotifications(int spiritDiff = 0);

	static PlayerData *getInstance();
    
    std::string recruitHeroImgURL;
    
private:
	int storageSortType;
    HelpersSortType helpersSortType;
    SoulsSortType soulSortType;
    FriendSortType friSortType;
    std::map<std::string, int> newHeroCursorMap;
    std::vector<std::string> newHeroStorage;
};

}