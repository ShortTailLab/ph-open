  //
//  PlayerData.cpp
//
//  Created by ZQL on 2012 11 27
//
//

#include "PlayerData.h"
#include "GameScene.h"
#include "DebugMisc.h"
#include "Tutorial.h"
#include "Common.h"

#include <algorithm>
#include <boost/unordered_set.hpp>
#include <boost/function.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <support/user_default/CCUserDefault.h>
#include <extensions/native/CCNative.h>

namespace PH
{
using namespace std;

    std::vector<Hero*> sortHeroesByTime(const std::vector<Hero*>& heroes)
    {
        std::vector<Hero*> tempStorage = heroes;
        std::reverse(tempStorage.begin(), tempStorage.end());
        return tempStorage;
    }
    
    
    std::vector<Hero*> sortHeroesByLock(const std::vector<Hero*>& heroes)
    {
        std::vector<Hero*> tempStorage = heroes;
        std::sort(tempStorage.begin() , tempStorage.end() , [=](Hero *h1 , Hero *h2){
            return h1->isLock != h2->isLock ? h1->isLock && !h2->isLock : h1->profile->tableId < h2->profile->tableId;
        });
        return tempStorage;
    }
    
    std::vector<Hero*> sortHeroesByHP(const std::vector<Hero*>& heroes)
    {
        std::vector<Hero*> tempStorage = heroes;
        std::sort(tempStorage.begin() , tempStorage.end() , [=](Hero *h1 , Hero *h2){
            return h1->health != h2->health ? h1->health > h2->health : h1->profile->tableId < h2->profile->tableId;
        });
        return tempStorage;
    }
    
    std::vector<Hero*> sortHeroesByColor(const std::vector<Hero*>& heroes)
    {
        std::vector<Hero*> tempStorage = heroes;
        std::sort(tempStorage.begin() , tempStorage.end() , [=](Hero *h1 , Hero *h2){
            return h1->profile->color != h2->profile->color ? h1->profile->color < h2->profile->color : h1->profile->tableId < h2->profile->tableId;
        });
        return tempStorage;
    }
    
    std::vector<Hero*> sortHeroesByAttack(const std::vector<Hero*>& heroes)
    {
        std::vector<Hero*> tempStorage = heroes;
        std::sort(tempStorage.begin() , tempStorage.end() , [=](Hero *h1 , Hero *h2){
            return h1->attack != h2->attack ? h1->attack > h2->attack : h1->profile->tableId < h2->profile->tableId;
        });
        return tempStorage;
    }
    
    std::vector<Hero*> sortHeroesByRegen(const std::vector<Hero*>& heroes)
    {
        std::vector<Hero*> tempStorage = heroes;
        std::sort(tempStorage.begin() , tempStorage.end() , [=](Hero *h1 , Hero *h2){
            return h1->regen != h2->regen ? h1->regen > h2->regen : h1->profile->tableId < h2->profile->tableId;
        });
        return tempStorage;
    }
    
    std::vector<Hero*> sortHeroesByStars(const std::vector<Hero*>& heroes)
    {
        std::vector<Hero*> tempStorage = heroes;
        std::sort(tempStorage.begin() , tempStorage.end() , [=](Hero *h1 , Hero *h2){
            return h1->profile->star != h2->profile->star ? h1->profile->star > h2->profile->star : h1->profile->tableId < h2->profile->tableId;
        });
        return tempStorage;
    }
    
    std::vector<Hero*> sortHeroesByTitle(const std::vector<Hero*>& heroes)
    {
        std::vector<Hero*> tempStorage = heroes;
        std::sort(tempStorage.begin() , tempStorage.end() , [=](Hero *h1 , Hero *h2){
            int index1 = heroTitleMap[h1->profile->title];
            int index2 = heroTitleMap[h2->profile->title];
            return index1 != index2 ?  index1 < index2 : h1->profile->tableId < h2->profile->tableId;
        });
        return tempStorage;
    }
    
    std::vector<Hero*> sortHeroesByFight(const std::vector<Hero*>& heroes)
    {
        std::vector<Hero*> tempStorage = heroes;
        std::sort(tempStorage.begin() , tempStorage.end() , [=](Hero *h1 , Hero *h2){
            return h1->fightSkill != h2->fightSkill ? h1->fightSkill > h2->fightSkill : h1->profile->tableId < h2->profile->tableId;
        });
        return tempStorage;
    }
    
    std::vector<Hero*> sortHeroesByStorageType(const std::vector<Hero*>& heroes, int type)
    {
        switch (type)
        {
            case 0:
                return sortHeroesByTime(heroes);
            case 1:
                return sortHeroesByLock(heroes);
            case 2:
                return sortHeroesByColor(heroes);
            case 3:
                return sortHeroesByHP(heroes);
            case 4:
                return sortHeroesByAttack(heroes);
            case 5:
                return sortHeroesByRegen(heroes);
            case 6:
                return sortHeroesByTitle(heroes);
            case 7:
                return sortHeroesByFight(heroes);
            default:
                return heroes;
        }
    }
    

    
TeamData::TeamData() : hp(0), regen(0), lp(0), maxLp(0), battleSkill(NULL), skill(NULL)
{
    memset(attack, 0, sizeof(attack));
}

void TeamData::update(const std::vector<Hero*>& team)
{
    memset(attack, 0, sizeof(attack));
    hp = lp = regen = fightSkill = 0;
    
    for(const Hero* hero : team)
    {
		if(hero == NULL)
            continue;
        
        Hero h2 = boostHeroStatsByRelations(*hero, team);
        
        hp += h2.health;
        attack[hero->profile->color] += h2.attack;
		attack[GemUtils::AllColor] += h2.attack;
		regen += h2.regen;
		lp += hero->profile->LP;
        fightSkill += h2.getFightSkill();
    }
    
	if(team.size() && team[0])
    {
        int skillId = team[0]->profile->passiveSkillId;
        skill = skillId == -1 ? NULL : &PST(skillId);
        
        int battleId = team[0]->profile->pvpPassiveSkillId;
        battleSkill = battleId == -1 ? NULL : &PST(battleId);
    }
}

// ------------------- Player Data ------------------------
    
PlayerData::PlayerData()
{
	player = NULL;
	stageChangeCallBack = []() {};
    friSortType = FriendSortType::Nor_Fri_Sort;
    hasNewMail = false;
    newMailsNum = 0;
    
    InProcEventCentral::waitForEvent("mail.new",
                                     [=](const std::string& event, const std::map<std::string, std::string>& w)
                                     {
                                         hasNewMail = true;
                                     });
}
    
PlayerData::~PlayerData()
{
    InProcEventCentral::discardAllEventHandler("mail.new");
}

PlayerData *PlayerData::getInstance()
{
	static PlayerData *data = NULL;
	if (!data)
        data = new PlayerData();
	return data;
}

void PlayerData::userLoggedOff()
{
    if (player)
    {
        delete player;
        player = NULL;
    }
}

void PlayerData::update(std::auto_ptr<Player> newPalyer, std::auto_ptr<Control> newControl)
{
    // Player
    if (!player)
    {
        player = newPalyer.release();
        updateTeamFromPlayer(player);
        storageSortType = CCUserDefault::sharedUserDefault()->getIntegerForKey("sortType" , 0);
        helpersSortType = Normal_Sort;
        soulSortType = Nor_Soul_Sort;
        setSortType(storageSortType);
        updateSpiritTime();
        drawFreeTime = player->drawFreeTime;
        drawFreeLeft = player->drawsLeft;
        drawExpLeft = player->expDrawsLeft;
        drawTransLeft = player->transDrawsLeft;
    }
    
    else
        updatePlayer(newPalyer.get());

    if (newControl.get())
    {
        setNormalStage(newControl->stages);
        setInstanceStage(newControl->fbs);
        setDailyStage(newControl->dailystages);
        drawNiceHeroPrices = newControl->drawNiceHeroPrices;
        maxLevel = newControl->maxLevel;
        tutorialStage = newControl->tutorialStage;
        storeItems = newControl->store;
        pvpStoreItems = newControl->pvpStore;
        recruitHeroImgURL = newControl->drawHeroImgURL;
        annoucement = newControl->annoucement;
        vipLevelSpent = newControl->vipLevelSpent;
        vipPacks = newControl->vipGifts;
        vipPacks.insert(vipPacks.begin(), newControl->otherVipGifts.begin(), newControl->otherVipGifts.end());
        std::sort(vipPacks.begin(), vipPacks.end(),
                  [](VIPPack p1, VIPPack p2){return p1.availableAtLevel<p2.availableAtLevel;});
        spiritRecoverSecs = newControl->spiritRecoverSecs;
        loginRewards = newControl->loginRewards;
        rewardPath = newControl->path;
        hasDrawJueDing = !newControl->showVIPDraw;
        hasTenDraw = !newControl->showVIPTenDraw;
        GameConfig::shared()->isFirstScoreAfterBattle = newControl->isFirstScoreAfterBattle;
        //isPvPOpen = newControl->pvpOptIn;
        isPvPOpen = true;
        doneTasksNum = newControl->taskHints;
        freeRelationTimes = newControl->dailyFreeRelationChangeCount;
        hasPromotion = newControl->hasPromotion;
        //freeRelationTimes = 3;
        
        boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
        delta_local_server = boost::posix_time::from_time_t(newControl->serverTime) - now;
        pvpStartTime = newControl->pvpStart;
        pvpEndTime = newControl->pvpEnd;
    }
}

void PlayerData::updateTeamFromPlayer(Player* newPlayer)
{
    // Storage is already updated from newPlayer
    if(!newPlayer->team.empty())
        player->team.swap(newPlayer->team);
    else
    {
        vector<Hero*> newTeam;
        newTeam.reserve(25);
        for (auto oldHero : player->team)
        {
            newTeam.push_back( oldHero ? player->findHero(oldHero->id) : NULL );
        } 
        player->team.swap(newTeam);
    }
    
    player->team.resize(25);
    teamData.update(getTeam());
}

void PlayerData::updatePlayer(Player *targetPlayer)
{
	assert(player->uid == targetPlayer->uid && "update user is different from original user.");
	if(!targetPlayer->name.empty())
        setName(targetPlayer->name);
 
    int levelDiff = 0;
	if(targetPlayer->level >= 0)
    {
        levelDiff = targetPlayer->level - player->level; 
        setLevel( targetPlayer->level);
    }
	if(targetPlayer->coins != player->coins)
        setCoins( targetPlayer->coins);
    //printf("\n\n target:%d now:%d\n\n",targetPlayer->golds, player->golds);
	if(targetPlayer->golds != player->golds)
        setGolds( targetPlayer->golds);
    player->spent = targetPlayer->spent;
    if(targetPlayer->vipLevel > 0)
        setVipLevel(targetPlayer->vipLevel); 
    
    {
        int currExp = targetPlayer->exp >= 0 ? targetPlayer->exp : player->exp;
        int currNextLevelExp = targetPlayer->nextLevelExp >= 0 ? targetPlayer->nextLevelExp : player->nextLevelExp;
        setExp(currExp, currNextLevelExp, levelDiff);
    }
    
    {
        player->spiritCheckTime = targetPlayer->spiritCheckTime;
        int currSpirit = targetPlayer->spirit >= 0 ? targetPlayer->spirit : player->spirit;
        int currMaxSpirit = targetPlayer->maxSpirit >= 0 ? targetPlayer->maxSpirit : player->maxSpirit;
        setSpirit(currSpirit, currMaxSpirit);
    }
     
    if (targetPlayer->lastLoginTS)
    {
        player->lastLoginTS = targetPlayer->lastLoginTS;
    }
    
	if(targetPlayer->maxLP >= 0)
        player->maxLP = targetPlayer->maxLP;
    if(targetPlayer->maxFriends >=0)
    {
        player->maxFriends = targetPlayer->maxFriends;
    }
    
	if(!targetPlayer->token.empty())
        player->token = targetPlayer->token;
	if(targetPlayer->storageLimit >=0)
        player->storageLimit = targetPlayer->storageLimit;
    
	if(targetPlayer->storage.size() > 0)
    {
        //make heroes got lately new
        if(targetPlayer->storage.size() > player->storage.size())
        {
            boost::unordered_set<std::string> hadHeroes;
            hadHeroes.reserve(player->storage.size());
            transform(player->storage.begin(), player->storage.end(), inserter(hadHeroes, hadHeroes.end()), [](Hero* h)
            {
                return h->id;
            });

            for (Hero* hero : targetPlayer->storage)
            {
                if (!hadHeroes.count(hero->id))
                    markHeroNew(hero);
            }
        }
        
		player->storage.swap(targetPlayer->storage);
        //targetPlayer storage is swaped,but team still point to the new heroes.
        updateTeamFromPlayer(targetPlayer);
	}

	if(targetPlayer->seen.size() > 0)
        player->seen = targetPlayer->seen;
    
	if(targetPlayer->owned.size() > 0)
        player->owned = targetPlayer->owned;

    if(!targetPlayer->accounts.empty())
        player->accounts = targetPlayer->accounts;
    if(targetPlayer->pvpScore > 0)
        player->pvpScore = targetPlayer->pvpScore;
    
    if (!targetPlayer->heroSouls.empty())
    {
        for(std::map<int, int>::iterator it = targetPlayer->heroSouls.begin();
            it != targetPlayer->heroSouls.end();it++)
        {
            std::map<int, int>::iterator prevIt = player->heroSouls.find(it->first);
            if(prevIt==player->heroSouls.end() || (prevIt != player->heroSouls.end() && prevIt->second<it->second))
                newSouls.push_back(it->first);
        }
        
        player->heroSouls.swap(targetPlayer->heroSouls);
    }
    
    if (targetPlayer->drawsLeft >= 0)
        drawFreeLeft = targetPlayer->drawsLeft;
    
    if (targetPlayer->expDrawsLeft >= 0)
        drawExpLeft = targetPlayer->expDrawsLeft;
    
    if (targetPlayer->transDrawsLeft >= 0)
        drawTransLeft = targetPlayer->transDrawsLeft;

    if(targetPlayer->pvpKeys >= 0)
        player->pvpKeys = targetPlayer->pvpKeys;
    if(targetPlayer->pvpRank >= 0)
        player->pvpRank = targetPlayer->pvpRank;
    if(targetPlayer->pvpReputation >= 0)
        player->pvpReputation = targetPlayer->pvpReputation;
    if(targetPlayer->pvpScore >= 0)
        player->pvpScore = targetPlayer->pvpScore;
    if(targetPlayer->pvpWinRate > 0.0)
        player->pvpWinRate = targetPlayer->pvpWinRate;
    
	updateSpiritTime();

	storageCallBack();
    
    if (targetPlayer->drawFreeTime.size() == 2)
        drawFreeTime = targetPlayer->drawFreeTime;
}

void PlayerData::addHero(Hero *hero)
{
	player->storage.push_back(hero);
    int id = hero->heroId;
    if(std::find(player->seen.begin(), player->seen.end(), id) == player->seen.end())
        player->seen.push_back(id);
    if(std::find(player->owned.begin(), player->owned.end(), id) == player->owned.end())
        player->owned.push_back(id);
    markHeroNew(hero);
}
    
void PlayerData::addHeroSoul(HeroSoulDrop soul)
{
    if(player->heroSouls.count(soul.heroId))
    {
        player->heroSouls[soul.heroId] = player->heroSouls[soul.heroId] + soul.count;
    }
    else
    {
        player->heroSouls[soul.heroId] = soul.count;
    }
}

void PlayerData::updateTeam(const std::vector<Hero*>& team)
{
    for(int i = 0; i < 5; i++)
        player->team[i+player->activeTeam*5] = team[i];

	teamData.update(getTeam());
}
    
void PlayerData::setDrawFreeTime(int type , int time)
{
    drawFreeTime[type] = time;
    
}
    
bool PlayerData::isDrawFree()
{
    return isDrawFree(0) || isDrawFree(1);
}
    
bool PlayerData::isDrawFree(int type)
{
    if(type < 0 || type >1)
        return false;
    boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time() +
        PlayerData::getInstance()->delta_local_server;
    
    auto delta = boost::posix_time::from_time_t(drawFreeTime[type]) - now;
    
    return delta.is_negative();
}

bool PlayerData::isHeroTransable(Hero *hero)
{
	return hero->profile->transformTargetHeroId != -1 && !hero->profile->transformMats.empty();
}
    
bool PlayerData::isHeroTransformMatsReady(Hero *hero)
{
    if (!isHeroTransable(hero))
        return false;

    return getUnlockHeroByIds(hero->profile->transformMats).size() == hero->profile->transformMats.size();
}

bool PlayerData::isHeroInTeam(Hero *hero)
{
    std::vector<Hero*> team = getHeroesInAllTeams();
	return std::find(team.begin() , team.end() , hero) != team.end();
}
    
bool PlayerData::isHeroInActiveTeam(const Hero* hero)
{
    const std::vector<Hero*> team = getTeam();
	return std::find(team.begin() , team.end() , hero) != team.end();
}
    
bool PlayerData::isHeroRelationReady(HeroRelation& r)
{
    
    for(int id : r.withHeroes)
    {
        int id2;
        bool has = false;
        for(id2 = id ; id2 != -1 ; id2 = HPT(id2).transformTargetHeroId)
        {
            if(getHeroByHeroId(id2))
            {
                has = true;
                break;
            }
        }
        if(!has)return false;
    }

    return true;
}
    
relationsYesNo PlayerData::heroReadyRelations(const Hero* hero)
{
    std::vector<HeroRelation> relations = hero->relations;
    std::vector<HeroRelation> ok;
    std::vector<HeroRelation> no;
    
    for(HeroRelation& r : relations)
        if(isHeroRelationReady(r))
            ok.push_back(r);
        else
            no.push_back(r);
    return boost::make_tuple(boost::move(ok) , boost::move(no));
}
    
bool PlayerData::isDailyInstance(const std::string & idd)
{
    for(auto ins : this->dailyStageList)
    {
        for(auto level : ins.stage->levelList)
        {
            if(level.id == idd)
            {
                return true;
            }
        }
    }
    return false;
}
    
bool PlayerData::isDailyInstanceByStage(const std::string &idd)
{
    for(auto ins : this->dailyStageList)
    {
        if(idd == ins.stage->id)
            return true;
    }
    return false;
}

Player *PlayerData::getPlayer()
{
	return player;
}

void PlayerData::switchSortType()
{
	storageSortType = ++storageSortType%8;

	CCUserDefault::sharedUserDefault()->setIntegerForKey("sortType" , storageSortType);
    CCUserDefault::sharedUserDefault()->flush();
}

void PlayerData::setSortType(int type)
{
	storageSortType = type;
}

int PlayerData::getSortType()
{
    return storageSortType;
}
    
const char* PlayerData::getSortTypeMsg()
{
	switch(storageSortType)
	{
	case 0:
		return "时间排序";
	case 1:
		return "加锁排序";
	case 2:
		return "属性排序";
	case 3:
		return "血量排序";
	case 4:
		return "攻击排序";
	case 5:
		return "恢复排序";
    case 6:
        return "流派排序";
    case 7:
        return "战力排序";
	default:
		return "";
	}
}

std::vector<Hero*> PlayerData::getStorage()
{
    return sortHeroesByStorageType(player->storage, getSortType());
}


    
HelpersSortType PlayerData::getHelpersSortType()
{
    return helpersSortType;
}
void PlayerData::switchHelpersSortType()
{
    int type = helpersSortType;
    helpersSortType = HelpersSortType((++type) % 6);
}
    
SoulsSortType PlayerData::getSoulSortType()
{
    return soulSortType;
}
void PlayerData::switchSoulSortType()
{
    int type = soulSortType;
    soulSortType = SoulsSortType((++type)%4);
}
    
FriendSortType PlayerData::getFriSortType()
{
    return friSortType;
}
void PlayerData::switchFriSortType()
{
    int type = friSortType;
    friSortType = FriendSortType((++type)%7);
}

void PlayerData::setActiveTeam(int i)
{
    player->activeTeam = i;
    
    teamData.update(getTeam());
}
    
int PlayerData::getActiveTeam()
{
    return player->activeTeam;
}
    
std::vector<Hero*> PlayerData::getTeam()
{
    vector<Hero*>::iterator it = player->team.begin()+player->activeTeam*5;
	return vector<Hero*>(it, it+5);
}
    
std::vector<std::string> PlayerData::getTeamIds()
{
    std::vector<std::string> teamIds;
    for(Hero* h : player->team)
    {
        std::string s = h ? h->id : "";
        teamIds.push_back(s);
    }
    
    return teamIds;
}
    
void PlayerData::markHeroNew(Hero* hero)
{
    newHeroStorage.push_back(hero->id);
}
    
std::vector<Hero*> PlayerData::getNewHeroes(const std::string& type)
{
    if(!newHeroCursorMap.count(type))
        newHeroCursorMap[type] = 0;

    int cursor = newHeroCursorMap[type];
    std::vector<Hero*> heroes;
    heroes.reserve(newHeroStorage.size() - cursor);

    for(auto it=newHeroStorage.begin()+cursor; it != newHeroStorage.end(); ++it)
    {
        Hero* hero = getHero(*it);
        if(hero)
            heroes.push_back(hero);
    }
    return heroes;
}
void PlayerData::clearNewHeroes(const std::string& type)
{
    newHeroCursorMap[type] = newHeroStorage.size();
}
    
std::vector<Hero*> PlayerData::getHeroesInAllTeams()
{
    std::vector<Hero*> heroes;
    for(Hero* hero : player->team)
        if(hero && find_if(heroes.begin(), heroes.end(), [=](Hero* h){return h->id == hero->id;}) == heroes.end())
            heroes.push_back(hero);
    
    return heroes;
}

Hero *PlayerData::getHero(const std::string& heroId)
{
	return player->findHero(heroId);
}

Hero *PlayerData::getHero(int index)
{
	return player->storage[index];
}

Hero *PlayerData::getHeroByHeroId(int id)
{
    for (auto h : player->storage)
        if (h->heroId == id)
            return h;
    return NULL;
}
    
Hero *PlayerData::getUnlockHeroById(int id)
{
    for (auto h : player->storage)
        if (h->heroId == id && !h->isLock)
            return h;
    return NULL;
}
    
std::vector<Hero*> PlayerData::getUnlockHeroByIds(const std::vector<int>& ids)
{
    std::vector<Hero*> ret;
    auto storage = getStorage();
    for (int hid : ids)
        for (auto h : storage)
            if (!h->isLock && h->profile->tableId == hid && std::find(ret.begin(), ret.end(), h) == ret.end())
            {
                ret.push_back(h);
                break;
            }
    
    return ret;
}

int PlayerData::getHeroIndex(Hero *hero)
{
	vector<Hero*> storage = getStorage();
	for(int i=0;i<storage.size();i++)
    {
		if(storage[i]->id == hero->id)
            return i;
	}
	return -1;
}

void PlayerData::updateHero(Hero *hero)
{
	//int id = getHeroIndex(hero);
	//delete player->storage[id];
	//player->storage[id] = hero;
	*getHero(hero->id) = *hero;
}

void PlayerData::lockHero(Hero *target , bool value)
{
	getHeroByHeroId(target->heroId)->isLock = value;
	storageCallBack();
}

void PlayerData::removeHero(const string& heroId)
{
	for (Hero*& h : getTeam())
    {
		if( h && h->id == heroId)
			h = NULL;
	}

    auto toRemove = remove_if(player->storage.begin(), player->storage.end(), [&](Hero* h) -> bool
    {
        phassert(h && "find NULL pointer in storage");
        if (h && h->id == heroId)
        {
            delete h;
            return true;
        }
        return false;
    });
    player->storage.erase(toRemove, player->storage.end());
}

const std::string& PlayerData::getToken()
{
	return player->token;
}

void PlayerData::setName(const string& name)
{
    auto status = GameScene::getInstance()->statusLayer();
    if (status)
        status->setName(name);
	player->name = name;
}
    
void PlayerData::setVipLevel(int vipLevel)
{
    auto status = GameScene::getInstance()->statusLayer();
    if (status)
        status->setVipLevel(vipLevel);
    player->vipLevel = vipLevel;
}

void PlayerData::setLevel(int value)
{
    auto status = GameScene::getInstance()->statusLayer();
    if (status)
        status->setLevel(value);
    if(value > player->level)
        TutorialControl::shared()->onLevelUpTutorial(value);
	player->level = value;
}

void PlayerData::setCoins(int value)
{
    auto status = GameScene::getInstance()->statusLayer();
    if (status)
        status->setCoins(player->coins, value);
	player->coins = value;
}

void PlayerData::setGolds(int value)
{
    auto status = GameScene::getInstance()->statusLayer();
    if (status)
        status->setGolds(player->golds, value);
	player->golds = value;
}
    
void PlayerData::registerLocalPushNotifications(int spiritDiff)
{
    CCNative::clearLocalPushNotification();
    
    if (spiritDiff == 0)
        spiritDiff = player->maxSpirit - player->spirit;
    
    if(spiritDiff != 0)
    {
        CCNative::registerLocalPushNotification("掌門，我們的體力恢復了，繼續闖蕩江湖吧！",
                                                player->spiritCheckTime + (spiritDiff+1)*10*60,
                                                LocalPushRepeatNone);
    }
    
    CCNative::registerLocalPushNotification("可以免費招募絕頂高手了，不要忘記喔！",
                                            drawFreeTime[1],
                                            LocalPushRepeatNone);
}

void PlayerData::setSpirit(int curr, int currMax)
{
    CCNative::clearLocalPushNotification();
    if (curr < currMax)
    {
        registerLocalPushNotifications(currMax - curr);
    }
    
    auto status = GameScene::getInstance()->statusLayer();
    if (status)
        status->setSpirit(player->spirit, player->maxSpirit, curr, currMax);
	player->spirit = curr;
    player->maxSpirit = currMax;
}

int PlayerData::getSpirit()
{
	return player->spirit;
}
    
int PlayerData::getNumOfHero(const HeroIdOfType& type)
{
    int num = 0;
    for(const Hero* h : player->storage)
        if(h->heroId == int(type))
            num++;
    return num;
}
    
void PlayerData::removeNumOfHero(const HeroIdOfType& type, int num)
{
    for(std::vector<Hero*>::iterator it = PlayerData::getInstance()->player->storage.begin();
        it != PlayerData::getInstance()->player->storage.end();)
    {
        if(num == 0)
            return;
        if(num > 0 && (*it)->heroId == int(type))
        {
            num--;
            it = PlayerData::getInstance()->player->storage.erase(it);
        }
        else
            it++;
    }
}

void PlayerData::updateSpiritTime()
{
    using namespace boost::posix_time;
    spiritCheckTime = from_time_t(player->spiritCheckTime);
//    ptime now = second_clock::universal_time();
//    ptime lastCheck = from_time_t(player->spiritCheckTime);
//
//	spiritTimeCount = 10*60 - (now - lastCheck).total_seconds();
}

void PlayerData::setExp(int curr, int currMax, int levelDiff)
{
    auto status = GameScene::getInstance()->statusLayer();
    if (status)
        status->setExp(player->exp, player->nextLevelExp, curr, currMax, levelDiff);
	player->exp = curr;
    player->nextLevelExp = currMax;
}

void PlayerData::setNormalStage(const std::vector<boost::shared_ptr<Stage>>& stages)
{
	normalStageList = stages;
	stageChangeCallBack();
}

void PlayerData::setInstanceStage(const std::vector<boost::shared_ptr<Stage>>& stages)
{
	instanceStageList = stages;
	stageChangeCallBack();
}
    
void PlayerData::setDailyStage(const std::vector<DailyStage> & stages)
{
    dailyStageList = stages;
    for(const DailyStage& s : stages)
    {
        StagePtr stage = s.stage;
        stage->dailyInstanceProgress = s.progress;
        instanceStageList.push_back(stage);
    }
}
    
void PlayerData::removeDailyStageByLevelId(const std::string &levelId)
{
    auto iter = this->instanceStageList.begin();
    for(;iter != this->instanceStageList.end();
        iter ++ )
    {
        bool flag = false;
        for(auto l : (*iter)->levelList)
        {
            if(l.id == levelId){
                flag = true;
                break;
            }
        }
        if(flag) break;
    }
    
    if(iter!=this->instanceStageList.end())
    {
        this->instanceStageList.erase(iter);
    }
}

void PlayerData::registerStageCallBack(boost::function<void ()> callBack)
{
	stageChangeCallBack = callBack;
}

void PlayerData::unRegisterStageCallBack()
{
	stageChangeCallBack = [=](){};
}

void PlayerData::registerStorageCallBack(CCNode *node , boost::function<void ()> callBack)
{
	storageChangeCallBackMap.insert(std::make_pair(node , boost::move(callBack)));
}

void PlayerData::unRegisterStorageCallBack(CCNode *node)
{
	storageChangeCallBackMap.erase(node);
}

void PlayerData::storageCallBack()
{
	std::map<cocos2d::CCNode* , boost::function<void ()> >::iterator it;
	for( it = storageChangeCallBackMap.begin() ; it != storageChangeCallBackMap.end() ; it++)
        it->second();
}

static
Level* findInStages(const std::vector<boost::shared_ptr<Stage>>& stages, const std::string& levelId)
{
    for (const auto& s : stages)
        for (const Level& l : s->levelList)
            if (l.id == levelId)
                return const_cast<Level*>(&l);
    return NULL;
}

Level* PlayerData::findLevel(const std::string& levelId)
{
    auto level = findInStages(normalStageList, levelId);
    return level ? level : findInStages(instanceStageList, levelId);
}
    
boost::shared_ptr<Stage> PlayerData::findStage(int stageId)
{
    for (const auto& s : normalStageList)
    {
        if(s->stage == stageId)
            return s;
    }
    return boost::shared_ptr<Stage>();
}
    
std::vector<Hero*> PlayerData::getHeroesById(int i)
{
    std::vector<Hero*> all;
    
    for(Hero* h : player->storage)
    {
        if(h->profile->tableId == i)
        {
            all.push_back(h);
        }
    }
    
    return all;
}

}