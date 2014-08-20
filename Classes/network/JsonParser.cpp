//
//  JsonParser.cpp
//  HelloCpp
//
//  Created by R.C on 12-10-27.
//
//

#include "JsonParser.h"
#include "BattleScene.h"
#include "URLHelpers.h"
#include "../Board/HeroTable.h"
#include <platform/CCPlatformConfig.h>
#include <algorithm>
#include <boost/make_shared.hpp>
#include <boost/move/move.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <support/user_default/CCUserDefault.h>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace PH;
using namespace PH::URLHelper;
using namespace rapidjson;

namespace PH{

static inline
int getValue(const Value&v, const char* name, int defaultValue)
{
    auto& value = v[name];
    return value.IsInt() ? value.GetInt() : defaultValue;
}

static inline
double getValue(const Value&v, const char* name, double defaultValue)
{
    auto& value = v[name];
    return value.IsDouble() ? value.GetDouble() : defaultValue;
}

static inline
std::string getValue(const Value& v, const char* name, const std::string& defaultValue)
{
    auto& memberValue = v[name];
    if (memberValue.IsNull())
        return defaultValue;
    else
        return std::string(memberValue.GetString(), memberValue.GetStringLength());
}
    
template<>
void parse<>(const Value& json, Ignore& ret)
{
}
    
template<>
void parse<>(const Value& json, Reward& ret)
{
//    if (json.HasMember("helpedUsers"))
//    {
//        auto& helpedUsers = json["helpedUsers"];
//        std::transform(helpedUsers.begin(), helpedUsers.end(), inserter(ret.helpedUsers, ret.helpedUsers.end()), [](Document::const_reference v)
//        {
//            return make_pair(getValue(v.first), v.second.GetInt());
//        });
//    }
    ret.totalCoins = getValue(json, "coins", 0);
    ret.totalGolds = getValue(json, "golds", 0);
    ret.html = getValue(json, "htmlStr", "");
}

template<>
void parse<>(const Value& json, Level& ret)
{
    ret.id = getValue(json, "id", "");
    ret.level = json["level"].GetInt();
    ret.name = getValue(json, "name", "");
    ret.roundCount = json["rounds"].GetInt();
    //ret.cleared = json["cleared"].GetBool();
    ret.cleared = getValue(json, "cleared", 0);
    ret.cost = json["spirit"].GetInt();
    
    ret.levelLimit = getValue(json, "lLimit", 0);
    auto & hint = json["dropsHint"];
    if(hint.IsArray())
    {
        ret.dropsHint.reserve(hint.Size());
        transform(hint.Begin(), hint.End(), back_move_inserter(ret.dropsHint), [](const Value & v)
                  {
                      return v.GetInt();
                  });
    }
}

template<>
void parse<>(const Value& json, Stage& ret)
{
    ret.stage   = json["stage"].GetInt();
    ret.id      = getValue(json, "id", "");
    ret.name    = getValue(json, "name", "");
    
    ret.event   = (StageEvent)(json.HasMember("event") ? json["event"].GetInt() : NoEvent);
    ret.startTimeStamp = json["eventStart"].GetInt();
    ret.endTimeStamp = json["eventEnd"].GetInt();
    
    ret.rewardAvailable = getValue(json, "rewardAvailable", false);
    ret.hasReward = getValue(json, "hasReward", false);
    
    ret.hint = getValue(json, "hint", "");

    int visibleStart = getValue(json, "visibleStartTS", 0);
    int visibleEnd = getValue(json, "visibleEndTS", 0);
    ret.visibleStart = visibleStart > 0 ? from_time_t(visibleStart) : not_a_date_time;
    ret.visibleEnd = visibleEnd > 0 ? from_time_t(visibleEnd) : not_a_date_time;
    
    ret.stageColor = (GemUtils::GemColor)getValue(json, "color", GemUtils::GemColor::AllColor);
    
    ret.dailyQuota = getValue(json, "quota", -1);
    //ret.progress = getValue(json, "progress", -1);
    ret.dailyInstanceProgress = -1;
    auto& jsonLevels = json["levels"];
    ret.levelList.reserve(jsonLevels.Size());
    transform(jsonLevels.Begin(), jsonLevels.End(), back_move_inserter(ret.levelList), parseV<Level>);
}

template<>
void parse<>(const Value& json, Control& ret)
{
    ret.taskHints = getValue(json, "taskHints", 0);
    
    if (json.HasMember("stages"))
    {
        auto& stages = json["stages"];
        ret.stages.reserve(stages.Size());
        transform(stages.Begin(), stages.End(), back_move_inserter(ret.stages), [](const Value& v) {
            boost::shared_ptr<Stage> ret = boost::make_shared<Stage>();
            parse<Stage>(v, *ret);
            return ret;
        });
    }
    
    if (json.HasMember("fbs"))
    {
        auto& fbs = json["fbs"];
        ret.fbs.reserve(fbs.Size());
        transform(fbs.Begin(), fbs.End(), back_move_inserter(ret.fbs), [](const Value& v) {
            boost::shared_ptr<Stage> ret = boost::make_shared<Stage>();
            parse<Stage>(v, *ret);
            return ret;
        });
    }
    
    if (json.HasMember("dailystages"))
    {
        auto & ds = json["dailystages"];
        ret.dailystages.reserve(ds.Size());
        transform(ds.Begin(), ds.End(), back_move_inserter(ret.dailystages), [](const Value & v)
        {
            DailyStage rr;
            rr.stage = boost::make_shared<Stage>();
            parse<Stage>(v["stage"], *(rr.stage));
            rr.progress = getValue(v, "progress", 1);
            return rr;
        });
    }

    auto& store = json["store"];
    if (store.IsArray())
    {
        ret.store.reserve(store.Size());
        transform(store.Begin(), store.End(), back_move_inserter(ret.store), parseV<StoreItem>);
    }
    
    auto& monthly = json["monthly"];
    if (monthly.IsArray())
    {
        ret.monthly.reserve(monthly.Size());
        transform(monthly.Begin(), monthly.End(), back_move_inserter(ret.monthly), parseV<StoreItem>);
    }
    
    if (json.HasMember("niceDrawPrices"))
    {
        auto& drawNiceHeroPrices = json["niceDrawPrices"];
        transform(drawNiceHeroPrices.Begin(), drawNiceHeroPrices.End(), back_inserter(ret.drawNiceHeroPrices),
                  [](const Value& v){return v.GetInt();});
    }

    auto& gameSettings = json["gameSettings"];
    if (!gameSettings.IsNull())
    {
        auto& vipLevel = gameSettings["vipLevelSpent"];
        if (!vipLevel.IsNull())
            transform(vipLevel.Begin(), vipLevel.End(), back_inserter(ret.vipLevelSpent), [](const Value& v){return v.GetInt();});
        ret.spiritRecoverSecs = getValue(gameSettings, "spiritRecoverSecs", 60 * 10);
    }
    
    ret.showVIPDraw = json["vipInitialDraw"].IsTrue();
    ret.showVIPTenDraw = json["vipInitialTenDraw"].IsTrue();
    ret.dailyFreeRelationChangeCount = getValue(json, "dailyFreeRelationChange", 0);
    
    ret.serverTime = json.HasMember("serverTime") ? json["serverTime"].GetInt64() : 0;
    ret.drawHeroImgURL = getValue(json, "drawHeroImgURL", "");
    ret.maxLevel = json.HasMember("maxLevel") ? json["maxLevel"].GetInt() : 1;
    ret.tutorialStage = json.HasMember("tutorialProgress") ? json["tutorialProgress"].GetInt() : 0;
    ret.annoucement = getValue(json, "announcement", "");
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    ret.headVersion = getValue(json, "headVersionAndroid", "");
    ret.forceUpgrade = json.HasMember("forceUpgradeAndroid") && json["forceUpgradeAndroid"].GetBool();
#else
    ret.headVersion = getValue(json, "headVersionIOS", "");
    ret.forceUpgrade = json.HasMember("forceUpgradeIOS") && json["forceUpgradeIOS"].GetBool();
#endif
    ret.pvpOptIn = json.HasMember("pvp") && json["pvp"].GetBool();
    
    ret.rewards.totalCoins = ret.rewards.totalGolds = 0;
    ret.serverURL = getValue(json, "serverURL", "");
    ret.isFirstScoreAfterBattle = getValue(json, "tutorialFirstScoreBattle", false);
    ret.hasPromotion = json["hasPromotion"].IsTrue();
       
    if (json["rewards"].IsObject())
    {
        parse(json["rewards"], ret.rewards);
    }

    auto & rewards = json["loginRewards"];
    if(rewards.IsArray())
    {
        ret.loginRewards.reserve(rewards.Size());
        transform(rewards.Begin(), rewards.End(), back_move_inserter(ret.loginRewards), [](const Value & v)
        {
            return parseV<Treasure>(v);
        });
    }
    
    auto & path = json["loginGamePath"];
    if(path.IsArray())
    {
        ret.path.reserve(rewards.Size());
        transform(path.Begin(), path.End(), back_move_inserter(ret.path), [](const Value & v)
        {
            return v.GetInt();
        });
    }
    
    auto& vipGifts = json["vipGifts"];
    if (vipGifts.IsArray())
    {
        ret.vipGifts.reserve(vipGifts.Size());
        transform(vipGifts.Begin(), vipGifts.End(),
                  back_inserter(ret.vipGifts), parseV<VIPPack>);
    }
    
    auto& otherVipGifts = json["otherVipGifts"];
    if( otherVipGifts.IsArray() )
    {
        ret.otherVipGifts.reserve(otherVipGifts.Size());
        transform(otherVipGifts.Begin(), otherVipGifts.End(),
                  back_inserter(ret.otherVipGifts), parseV<VIPPack>);
    }
    
    auto& pvpStore = json["pvpStore"];
    if (pvpStore.IsArray())
    {
        ret.pvpStore.reserve(pvpStore.Size());
        transform(pvpStore.Begin(), pvpStore.End(), back_inserter(ret.pvpStore), parseV<PvPStoreItem>);
    }
    
    auto& pvpOpenTime = json["pvpOpen"];
    if (pvpOpenTime.IsArray())
    {
        ret.pvpStart = from_time_t(pvpOpenTime[(rapidjson::SizeType)0].GetInt64());
        ret.pvpEnd = from_time_t(pvpOpenTime[(rapidjson::SizeType)1].GetInt64());
    }
}

template<>
void parse<>(const Value& json, PvPStoreItem& ret)
{
    ret.sid = json["sid"].GetInt();
    ret.itemType = (PvPStoreItemType)json["itemType"].GetInt();
    ret.amount = json["amount"].GetInt();
    ret.price = json["price"].GetInt();
    ret.name = getValue(json, "name", "");
    ret.desc = getValue(json, "desc", "");
}

template<>
void parse<>(const Value& json, HeroSoulDrop& ret)
{
    ret.heroId = json["heroType"].GetInt();
    ret.count = json["count"].GetInt();
}

template<>
void parse<>(const Value& json, Hero10Draw& ret)
{
    if (json["soul"].IsObject())
        parse(json["soul"], ret.soul);
    else
        ret.soul.heroId = ret.soul.count = 0;
    
    auto& heroesArray = json["heroes"];
    ret.heroes.reserve(heroesArray.Size());
    transform(heroesArray.Begin(), heroesArray.End(), back_inserter(ret.heroes), [](const Value& v){ return parse<Hero>(v);});
}

template<>
void parse<>(const Value& json, Hero& ret)
{
    ret.id = getValue(json, "id", "");
    ret.exp = json["exp"].GetInt();
    ret.level = json["level"].GetInt();
	ret.heroId  = json["heroType"].GetInt();
    ret.profile = &HPT(ret.heroId);
	ret.expCap = ret.profile->exp.eval(ret.level);
	ret.trainPrice = ret.level * 100;
    ret.maxEnergy = ret.profile->activeSkillId != -1 ? AST(ret.profile->activeSkillId).energy : 0;
	//ret.isLock = false;
	ret.isLock = CCUserDefault::sharedUserDefault()->getBoolForKey(("locked_"+ret.id).c_str());
    
    ret.eliteLevel  = getValue(json, "eliteLevel", 0);
    ret.skillBonus  = getValue(json, "skillBonus", 0);
    ret.chance      = getValue(json, "chance", 0);
    
    float eliteHp = ret.eliteLevel ? ret.profile->eliteHP.eval(ret.eliteLevel) : 0;
    float eliteAtk = ret.eliteLevel ? ret.profile->eliteAttack.eval(ret.eliteLevel) : 0;
    float eliteReg = ret.eliteLevel ? ret.profile->eliteRegen.eval(ret.eliteLevel) : 0;
    ret.health = ret.profile->health.eval(ret.level) + eliteHp;
	ret.attack = ret.profile->attack.eval(ret.level) + eliteAtk;
	ret.regen = ret.profile->regen.eval(ret.level) + eliteReg;
    ret.fightSkill = float(ret.health)/3+float(ret.attack)/2+ret.regen+ret.profile->correct;
    
    ret.nextFreeDraw = getValue(json, "nextFreeDraw", 0);
    ret.trainRate = getValue(json, "rate", 0.0);
    
    auto& relations = json["relations"];
    if (relations.IsArray())
    {
        transform(relations.Begin(), relations.End(), back_move_inserter(ret.relations), parseV<HeroRelation>);
    }
    
    auto& soulObj = json["soul"];
    if (soulObj.IsObject())
    {
        parse(soulObj, ret.soul);
    }
    else
    {
        ret.soul.heroId = 0;
        ret.soul.count = 0;
    }
    
    auto& next = json["next"];
    if (next.IsArray())
    {
        transform(next.Begin(), next.End(), back_inserter(ret.next8Draws), [](const Value& v){return v.GetInt();});
    }
}

template<>
void parse<>(const Value& json, EnemyInfo& ret)
{
    ret.hid = json["heroType"].GetInt();
    ret.maxHealth = json["hp"].GetInt();
    ret.attack = json["strength"].GetInt();
    ret.armor = json["armor"].GetInt();
    
    ret.initialTurn = json["initialTurn"].GetInt();
    ret.maxTurn = json["maxTurn"].GetInt();
    
    auto& drop = json["drop"];
    if (drop.IsObject())
    {
        ret.drop = make_shared<Hero>();
        parse<Hero>(drop, *ret.drop);
    }
    
    auto& dropSoul = json["dropSoul"];
    if (dropSoul.IsObject())
    {
        ret.soulDrop = make_shared<HeroSoulDrop>();
        parse<HeroSoulDrop>(dropSoul, *ret.soulDrop);
    }
    
    ret.script = getValue(json, "action", "");
    ret.passiveScript = getValue(json, "passiveAction", "");
    ret.turnBasedScript = getValue(json, "turnBasedAction", "");
    
    ret.name = HPT(ret.hid).name;
    ret.nameCN = HPT(ret.hid).nameCN;
    ret.color = HPT(ret.hid).color;
    ret.star = HPT(ret.hid).star;
}

template<>
void parse<>(const Value& json, Player& ret)
{
    ret.uid = getValue(json, "uid", "");
    ret.name = getValue(json, "name", "");
    ret.storageLimit = getValue(json, "storageLimit", -1);
    ret.coins = getValue(json, "coins", -1);
    ret.golds = getValue(json, "golds", -1);
    ret.exp = getValue(json, "exp", -1);
    ret.activeTeam = getValue(json, "activeTeam", 0);
    ret.vipLevel = getValue(json, "vip", 0);
    ret.spent = getValue(json, "spent", 0);
    
    ret.token = getValue(json, "token", "");
    ret.spirit = getValue(json, "spirit", -1);
    ret.maxSpirit = getValue(json, "maxSpirit", -1);
    ret.maxLP = getValue(json, "maxLP", -1);
    ret.maxFriends = getValue(json, "maxFriends", -1);
    ret.level = getValue(json, "level", -1);
    ret.nextLevelExp = getValue(json, "nextLevelExp", -1);
    ret.referCode = getValue(json, "referCode", "");
    
	ret.isFriend = json["isFriend"].IsTrue();
    ret.asHelperReward = getValue(json, "asHelperReward", 0);
    ret.pendingFriendRequests = getValue(json, "pendingFriendRequestsCount", -1);
    
    ret.pvpReputation = getValue(json, "pvpReputation", -1);
    ret.pvpScore = getValue(json, "pvpScore", -1);
    ret.pvpRank = getValue(json, "pvpRank", -1);
    ret.pvpKeys = getValue(json, "pvpKeys", -1);
    ret.pvpWinRate = getValue(json, "pvpWinRate", 0.0);
    
    ret.drawsLeft = getValue(json, "drawsLeft", -1);
    ret.expDrawsLeft = getValue(json, "expDrawsLeft", -1);
    ret.transDrawsLeft = getValue(json, "transDrawsLeft", -1);
    
    ret.activeTeamScore = getValue(json, "activeTeamScore", 0);
    ret.dailyJudgmentCount = getValue(json, "dailyJudgmentCount", 0);
    
    auto& auth = json["auth"];
    if (auth.IsArray())
    {
        ret.accounts.reserve(auth.Size());
        transform(auth.Begin(), auth.End(), back_move_inserter(ret.accounts), [](const Value& v) -> Account
        {
            Account ret;
            ret.type = getValue(v, "type", "");
            ret.id = getValue(v, "id", "");
            return ret;
        });
    }
    
    ret.lastLoginTS = getValue(json, "lastLogin", 0);
    ret.spiritCheckTime = getValue(json, "spiritCheckTime", 0);
    
    auto& jsonStorage = json["storage"];
    if (jsonStorage.IsArray())
    {
        ret.storage.reserve(jsonStorage.Size());
        transform(jsonStorage.Begin(), jsonStorage.End(), back_inserter(ret.storage),
                  [](const Value& v){return parse<Hero>(v);});
    }
    
    auto& jsonTeam = json["team25"];
    if (jsonTeam.IsArray())
    {
        ret.team.reserve(jsonTeam.Size());
        transform(jsonTeam.Begin(), jsonTeam.End(), back_inserter(ret.team),
                  [&](const Value& v) {return v.IsNull() ? NULL : ret.findHero(string(v.GetString(), v.GetStringLength()));});
    }
    
    auto& jsonSeen = json["seen"];
    if (jsonSeen.IsArray())
    {
        ret.seen.reserve(jsonSeen.Size());
        transform(jsonSeen.Begin(), jsonSeen.End(), back_inserter(ret.seen),
                  [](const Value& v){return v.GetInt();});
    }
    
    auto& jsonOwned = json["owned"];
    if (jsonOwned.IsArray())
    {
        ret.owned.reserve(jsonOwned.Size());
        transform(jsonOwned.Begin(), jsonOwned.End(), back_inserter(ret.owned),
                  [](const Value& v){return v.GetInt();});
    }
    
    auto& jsonSouls = json["souls"];
    if (jsonSouls.IsObject())
    {
        transform(jsonSouls.MemberBegin(), jsonSouls.MemberEnd(), inserter(ret.heroSouls, ret.heroSouls.end()),
                  [](const Value::Member& v){ return std::make_pair(boost::lexical_cast<int>(v.name.GetString()), v.value.GetInt()); });
    }
    
    ret.isFirstScoreAfterBattle = getValue(json, "tutorialFirstScoreBattle" , 0);
    ret.availableRewardsCount = getValue(json, "rewardsCount", 1);
    auto & rewards = json["rewards"];
    if(rewards.IsArray())
    {
        ret.rewards.reserve(rewards.Size());
        transform(rewards.Begin(), rewards.End(), back_move_inserter(ret.rewards), [](const Value & v)
        {
            return parseV<Treasure>(v);
        });
    }
    
    auto& drawFreeTime = json["drawFreeTime"];
    if (drawFreeTime.IsArray())
    {
        ret.drawFreeTime.reserve(drawFreeTime.Size());
        transform(drawFreeTime.Begin(), drawFreeTime.End(), back_inserter(ret.drawFreeTime), [](const Value& v){return v.GetInt();});
    }
}
    
template<>
void parse<>(const Value& json, Treasure& ret)
{
    ret.type = getValue(json, "type", 0);
    ret.ident = getValue(json, "ident", 0);
    ret.count = getValue(json, "count", 0);
    ret.level = getValue(json, "level", 0);
}

template<>
void parse<>(const Value& json, PlayerRankList& ret)
{
    ret.myRank = getValue(json, "myRank", 0);

    auto& users = json["leaders"];
    if (users.IsArray())
    {
        ret.players.reserve(users.Size());
        transform(users.Begin(), users.End(), back_inserter(ret.players), [](const Value& v){ return parse<Player>(v); });
    }
}

template<>
void parse<>(const Value& json, PvPGameLog& ret)
{
    typedef boost::date_time::local_adjustor<ptime, 8, no_dst> ChinaCentralTime;
    int timeTS = getValue(json, "time", 0);
    
    ret.result = getValue(json, "result", 0);
    ret.oldRank = getValue(json, "oldRank", 0);
    ret.newRank = getValue(json, "newRank", 0);
    ret.oldRep = getValue(json, "oldRep", 0);
    ret.newRep = getValue(json, "newRep", 0);
    ret.rivalName = getValue(json, "rival", "");
    ret.time = ChinaCentralTime::utc_to_local(from_time_t(timeTS));
}

template<>
void parse<>(const Value& json, PvPGameLogList& ret)
{
    auto& logs = json["logs"];
    if (logs.IsArray())
    {
        ret.logs.reserve(logs.Size());
        transform(logs.Begin(), logs.End(), back_move_inserter(ret.logs), parseV<PvPGameLog>);
    }
}
    
template<>
void parse<>(const Value& json, PlayerList& ret)
{
    ret.userCount = getValue(json, "count", 0);
    
    auto& users = json["users"];
    if (users.IsArray())
    {
        ret.players.reserve(users.Size());
        transform(users.Begin(), users.End(), back_inserter(ret.players), [](const Value& v){ return parse<Player>(v); });
    }
}

template<>
void parse<>(const Value& json, LevelInfo& ret)
{
    ret.spirit = getValue(json, "spirit", 0);
    ret.golds = getValue(json, "golds", 0);
    ret.coins = json["coins"].GetInt();
    ret.exp = json["exp"].GetInt();
    ret.bg = getValue(json, "bg", "");
    ret.id = getValue(json, "id", "");
    ret.lid = getValue(json, "lid", -1);

    ret.refRound = getValue(json, "refRound", 5);
    ret.avgCombo = getValue(json, "avgCombo", 4);
    ret.maxCombo = getValue(json, "maxCombo", 4);
    
    auto& avoidGemsArray = json["avoidGems"];
    ret.avoidGems.reserve(avoidGemsArray.Size());
    transform(avoidGemsArray.Begin(), avoidGemsArray.End(), back_move_inserter(ret.avoidGems), [](const Value& v){
        return string(v.GetString(), v.GetStringLength());
    });
    
    auto& jsonRounds = json["rounds"];
    transform(jsonRounds.Begin(), jsonRounds.End(), back_move_inserter(ret.roundList), [](const Value& v) -> RoundInfo
    {
        RoundInfo r;
        transform(v.Begin(), v.End(), back_move_inserter(r), parseV<EnemyInfo>);
        return r;
    });
}

static
void parsePoly(const Value& json, Polynomial& ret)
{
    if (json.Size() == 4)
    {
        ret.type = 4;
        
        ret.A = json[0u].GetDouble();
        ret.B = json[1u].GetDouble();
        ret.C = json[2u].GetDouble();
        ret.D = json[3u].GetDouble();
    }
}

template<>
void parse<>(const Value& json, HeroRelation& ret)
{
    ret.id = getValue(json, "id", "");
    ret.name = getValue(json, "name", "");

    auto& with = json["with"];
    transform(with.Begin(), with.End(), back_inserter(ret.withHeroes), [](const Value& v){ return v.GetInt(); });

    auto& eff = json["effects"];
    if (eff.HasMember("hp"))
        ret.effects.push_back(boost::make_tuple(HP, eff["hp"].GetDouble()));
    if (eff.HasMember("strength"))
        ret.effects.push_back(boost::make_tuple(STRENGTH, eff["strength"].GetDouble()));
    if (eff.HasMember("armor"))
        ret.effects.push_back(boost::make_tuple(REGEN, eff["armor"].GetDouble()));
}

template<>
void parse<>(const Value& json, HeroProfile& ret)
{
    ret.tableId = json["heroId"].GetInt();
    ret.nameCN = getValue(json, "name", "");
    ret.name = getValue(json, "name2", "");
    ret.title = getValue(json, "title", "");
    ret.faction = getValue(json, "faction", "");
    ret.type = getValue(json, "hType", "");
    ret.usage = getValue(json, "usage", "");
    ret.dropHint = getValue(json, "dropHint", "");
    
    ret.star = json["stars"].GetInt();
    ret.color = (GemUtils::GemColor)json["color"].GetInt();
    ret.price = Polynomial(10, json["price"].GetInt());
    ret.costSouls = getValue(json, "costSouls", 0);
    
	//add LP by zql
	ret.LP = json["lp"].GetInt();
    ret.maxLevel = json["maxLevel"].GetInt();
    ret.activeSkillId = getValue(json, "activeSkill", -1);
    ret.passiveSkillId = getValue(json, "passiveSkill", -1);
    
    ret.pvpActiveSkillId = getValue(json, "pvpActiveSkill", -1);
    ret.pvpPassiveSkillId = getValue(json, "pvpPassiveSkill", -1);
    
    ret.correct = getValue(json, "correct", 0);
    ret.potions = getValue(json, "potions", 1);
    
    parsePoly(json["healthCoeffs"], ret.health);
    parsePoly(json["levelCoeffs"], ret.exp);
    parsePoly(json["strengthCoeffs"], ret.attack);
    parsePoly(json["recoveryCoeffs"], ret.regen);
    
    parsePoly(json["eliteHP"], ret.eliteHP);
    parsePoly(json["eliteAttack"], ret.eliteAttack);
    parsePoly(json["eliteRegen"], ret.eliteRegen);
    
    auto& regionField = json["region"];
    if (regionField.IsArray())
    {
        ret.indexPageRegion = CCRectMake(regionField[(rapidjson::SizeType)0].GetDouble(), regionField[1].GetDouble(),
                                         regionField[2].GetDouble(), 0);
    }
    else
    {
        ret.indexPageRegion = CCRectMake(0, 0, 0, 0);
    }
    
    //gain, price?
    ret.gain = Polynomial(json["matExpBase"].GetInt(), 0);
    ret.transformTargetHeroId = getValue(json, "transformTarget", -1);
    if (ret.transformTargetHeroId != -1)
    {
        auto& mats = json["transformMats"];
        ret.transformMats.reserve(mats.Size());
        transform(mats.Begin(), mats.End(), back_inserter(ret.transformMats), [](const Value& v){
            return v.GetInt();
        });
        ret.transformPrice = json["transformCoins"].GetInt();
    }
}

template<>
void parse<>(const Value& json, StoreItem& ret)
{
    ret.amount = json["amount"].GetInt();
    ret.price = json["price"].GetInt();
    
    ret.channel = getValue(json, "channel", "");
    ret.productId = getValue(json, "productId", "");
    ret.productName = getValue(json, "productName", "");
    ret.goldPerDay = getValue(json, "goldPerDay", 0);
    
    ret.promotion = json.HasMember("promotion") ? json["promotion"].GetDouble() : 1;
}

template<>
void parse<>(const Value& json, SkillInfo& ret)
{
    ret.sid = json["sid"].GetInt();
    ret.energy = json["energy"].GetInt();
    ret.maxCooldown = json["cd"].GetInt();
    
    ret.name = getValue(json, "name", "");
    ret.desc = getValue(json, "desc", "");
    ret.skillScript = getValue(json, "skillScript", "");
}
    
template<>
void parse<>(const Value& json, std::vector<HeroRelation>& ret)
{
    auto& relationArray = json["relations"];
    ret.resize(relationArray.Size());
    
    int ind=0;
    for (auto it=relationArray.Begin();it<relationArray.End();++it)
    {
        parse(*it, ret[ind++]);
    }
}
    
template<>
void parse<>(const Value& json, VipUpgrade& ret)
{
    ret.coins = getValue(json, "coins", 0);
    ret.golds = getValue(json, "golds", 0);
    ret.pvpKeys = getValue(json, "pvpKeys", 0);
    ret.lastVIPLevel = getValue(json, "oldVIPLevel", 0);
    ret.newVIPLevel = getValue(json, "newVIPLevel", 0);
    
    auto& stages = json["stages"];
    if (stages.IsArray())
    {
        ret.stages.reserve(stages.Size());
        transform(stages.Begin(), stages.End(), back_move_inserter(ret.stages), [](const Value& v) {
            boost::shared_ptr<Stage> ret = boost::make_shared<Stage>();
            parse<Stage>(v, *ret);
            return ret;
        });
    }

    auto& fbs = json["fbs"];
    if (fbs.IsArray())
    {
        ret.fbs.reserve(fbs.Size());
        transform(fbs.Begin(), fbs.End(), back_move_inserter(ret.fbs), [](const Value& v) {
            boost::shared_ptr<Stage> ret = boost::make_shared<Stage>();
            parse<Stage>(v, *ret);
            return ret;
        });
    }
    
    auto& expItems = json["expItems"];
    if (expItems.IsArray())
    {
        ret.expItems.reserve(expItems.Size());
        transform(expItems.Begin(), expItems.End(), back_move_inserter(ret.expItems), parseV<Hero>);
    }
}
    
template<>
void parse<>(const Value& json, Mail& ret)
{
    ret.id = getValue(json, "id", "");
    ret.sender = getValue(json, "sender", "");
    ret.message = getValue(json, "message", "");
    ret.read = json["read"].IsTrue();
    ret.packageOpened = json["packageOpened"].IsTrue();
    ret.time = boost::posix_time::from_time_t(json["time"].GetInt64());
    
    auto& package = json["package"];
    if (package.IsArray())
    {
        ret.package.reserve(package.Size());
        transform(package.Begin(), package.End(), back_move_inserter(ret.package), parseV<Treasure>);
    }
}
    
template<>
void parse<>(const Value& json, VIPPack& ret)
{
    ret.price = getValue(json, "price", 0);
    ret.showPrice = getValue(json, "showPrice", 0);
    ret.availableAtLevel = getValue(json, "vipLevel", 0);
    ret.pid = getValue(json, "pid", 0);
    ret.purchased = json["purchased"].IsTrue();
    
    auto& items = json["items"];
    if (items.IsArray())
    {
        ret.items.reserve(items.Size());
        transform(items.Begin(), items.End(), back_move_inserter(ret.items), parseV<Treasure>);
    }
}
    
template<>
void parse<>(const Value& json, MailBox& ret)
{
    auto& mails = json["mails"];
    if (mails.IsArray())
    {
        ret.mails.reserve(mails.Size());
        transform(mails.Begin(), mails.End(), back_move_inserter(ret.mails), parseV<Mail>);
    }
    
    if (json["mailSince"].IsInt64())
    {
        ret.mailSince = boost::posix_time::from_time_t(json["mailSince"].GetInt64());
    }
}
    
// hero related
template<>
void parse<>(const Value& json, BoolWrapper& ret)
{
    //printf("json -> %d\n", getValue(json, "ok", 0));
    ret.ok = (bool)getValue(json, "ok", 0);
}
    
// ranking list

template<>
void parse<>(const Value& json, RankingList& ret)
{
    ret.hostRank = getValue(json, "hostRank", 0);
    ret.hostScore = getValue(json, "hostTeamScore", 0);
    ret.rankDelta = getValue(json, "rankDelta", 0);
    
    auto& ranks = json["ranks"];
    if (ranks.IsArray())
    {
        transform(ranks.Begin(), ranks.End(),
                  back_inserter(ret.table), [=](const Value& v)
        {
            RankingItem item;
            item.ident = getValue(v, "id", "");
            item.owner = getValue(v, "owner", "");
            item.hostLevel = getValue(v, "hostLevel", 1);
            item.flower = getValue(v, "flower", 0);
            item.egg = getValue(v, "egg", 0);
            item.score = getValue(v, "score", 0);
            
            auto& team = v["team"];
            if (team.IsArray())
            {
                item.team.reserve(team.Size());
                transform(team.Begin(), team.End(),
                          back_inserter(item.team),
                [](const Value& v)
                {
                    return v.IsNull() ? boost::shared_ptr<Hero>() : boost::shared_ptr<Hero>(parse<Hero>(v));
                });
            }
            return item;
        });
    }
}
    
// this handles getTasks
template<>
void parse<>(const Value& json, PlayerTaskVec& ret)
{
    auto& tasks = json["tasks"];
    if(tasks.IsArray())
    {
        transform(tasks.Begin(), tasks.End(), back_inserter(ret),[=](const Value & v)
        {
            PlayerTask task;
            task.tid = getValue(v, "tid", 0);
            task.type = static_cast<PlayerTaskType>(getValue(v, "type", 1));
            task.category = static_cast<PlayerTaskCategory>(getValue(v, "category", 0));
            task.name = getValue(v, "name", "");
            task.desc = getValue(v, "desc", "");
            task.progress = getValue(v, "progress", 0);
            task.total = getValue(v, "total", 1);
            
            task.index = getValue(v, "index", 0);
            task.position = getValue(v, "position", 1);
            
            auto& sample = v["treasure"];
            if(sample.IsArray())
            {
                task.sample.reserve(sample.Size());
                transform(sample.Begin(), sample.End(), back_move_inserter(task.sample), parseV<Treasure>);
            }
            
            return task;
        });
    }
}
    
// this handles purchase
template<>
void parse<>(const Value& json, boost::tuple<Player*, VipUpgrade*>& ret)
{
    tuples::get<0>(ret) = parse<Player>(json);
    
    auto& vipUpgrade = json["vipUpgrade"];
    tuples::get<1>(ret) = vipUpgrade.IsNull() ? NULL : parse<VipUpgrade>(vipUpgrade);
}

// this handles logins. it contains both
template<>
void parse<>(const Value& json, boost::tuple<Player*, Control*>& ret)
{
    tuples::get<0>(ret) = parse<Player>(json);
    tuples::get<1>(ret) = parse<Control>(json);
}

// this handles sync
template<>
void parse<>(const Value& json, boost::tuple<vector<HeroProfile*>, vector<SkillInfo*>, Control>& ret)
{
    auto& heroes = json["heroes"];
    auto& skills = json["skills"];
    
    transform(heroes.Begin(), heroes.End(), back_inserter(tuples::get<0>(ret)), [](const Value& v){
        return parse<HeroProfile>(v);
    });
    transform(skills.Begin(), skills.End(), back_inserter(tuples::get<1>(ret)), [](const Value& v){
        return parse<SkillInfo>(v);
    });
    parse(json, tuples::get<2>(ret));
}

// this handles friends
template<>
void parse<>(const Value& json, boost::tuple<vector<Player*>, int>& ret)
{
    auto& friends = json["friends"];
    
    tuples::get<1>(ret) = json["maxFriends"].GetInt();
    
    transform(friends.Begin(), friends.End(), back_inserter(tuples::get<0>(ret)), [](const Value& v){
        return parse<Player>(v);
    });
}
    
// this handles helpers/pendingfriends
template<>
void parse<>(const Value& json, vector<Player*>& ret)
{
    const Value* players;
    if (json.HasMember("pendingFriends"))
        players = &json["pendingFriends"];
    else
        players = &json["helpers"];
    
    transform(players->Begin(), players->End(), back_inserter(ret), [](const Value& v){
        return parse<Player>(v);
    });
}
    
// this handles fullhouserewards
template<>
void parse<>(const Value & json, boost::tuple<Player*, vector<Treasure>> & ret)
{
    tuples::get<0>(ret) = parse<Player>(json["user"]);
    
    auto & rewards = json["rewards"];
    if(rewards.IsArray())
    {
        transform(rewards.Begin(),
                  rewards.End(),
                  back_inserter(tuples::get<1>(ret)),
                  parseV<Treasure>);
    }
}

}
