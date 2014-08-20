//
//  PuzzleRPC.cpp
//  HelloCpp
//
//  Created by R.C on 12-10-24.
//
//

#include "PuzzleRPC.h"

#include <zlib.h>
#include <cocos2d.h>
#include <string>
#include <network/HttpClient.h>
#include <network/HttpRequest.h>
#include <network/HttpResponse.h>
#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/move/move.hpp>
#include <boost/thread.hpp>
#include <boost/make_shared.hpp>

#include "MessageChannel.h"
#include "extensions/crypto/CCCrypto.h"
#include "PvPMessageHandler.h"
#include "CCNative.h"
#include "URLHelpers.h"
#include "JsonParser.h"
#include "BuildConf.h"
#include "DebugMisc.h" 
#include "Store.h"
#include "QiangHua.h"
#include "Sale.h"
#include "GameScene.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#  define COMMON_DIGEST_FOR_OPENSSL
#  include <CommonCrypto/CommonDigest.h>
#else
#  include "openssl/md5.h"
#  include "openssl/sha.h"
#endif

using namespace std;
using namespace boost;
using namespace PH;
using namespace PH::URLHelper;
using namespace cocos2d;
using namespace cocos2d::extension;
using namespace rapidjson;

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

static std::string serverUrl = "http://pt.shorttaillab.com/";
static std::string messageUrl = "ws://pt.shorttaillab.com:9000/messages";

string AccountType::Main = "main";
string AccountType::Guest = "guest";
string AccountType::QQ = "qq";
string AccountType::SinaWeibo = "weibo";

static string RPC_RC4Key = ""; // not used

static string distro = PH_CHANNEL_NAME;

void Error::handleErrorInform()
{
    switch (code) {
        case 1:
            SystemInform::alertConfirmPanel(runningScene(),
                                            "连接超时，请再试一次吧？",
                                            "警告",
                                            [](){},
                                            [](){});
            break;
        case 1011:
            SystemInform::alertConfirmPanel(runningScene(), "你的元宝不足，是否充值？", "警告", [=](){
                ContentLayer *layer = ChongZhiLayer::create();
                layer->isShowBottom = false;
                GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
            }, [=](){});
            break;
        case 1012:
            SystemInform::alertConfirmPanel(runningScene(), "你的体力不足，是否去购买？", "警告", [=](){
                GameScene::getInstance()->pushLayerWithDefaultAnimation(SpiritLayer::create());
            }, [=](){});
            break;
        case 1013:
            SystemInform::alertConfirmPanel2(runningScene(),
                                             "你的席位不足，可升级VIP，购买更多席位或者强化，出售一些侠客",
                                             "席位不足",
                                             [=]()
            {
                GameScene::getInstance()->pushLayerWithDefaultAnimation(XiweiLayer::create());
            }, [=](){
                QiangHuaLayer *layer = QiangHuaLayer::create();
                GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
            },
             [=](){
                 SaleLayer *layer = SaleLayer::create();
                 GameScene::getInstance()->pushLayerWithDefaultAnimation(layer);
             },
            [=](){
                GameScene::getInstance()->pushLayerWithDefaultAnimation(VIPLayer::create());
            });
            break;

        default:
            SystemInform::alertMessage(runningScene(), toStrCN().c_str(), "警告" , [](){});
            break;
    }

}

static void addDeviceParams(Document& params)
{
    params.AddMember("mac", CCNative::MACAddress(), params.GetAllocator());
    params.AddMember("OS", CCNative::OS(), params.GetAllocator());
    params.AddMember("OSVersion", CCNative::OSVersion(), params.GetAllocator());
    params.AddMember("UA", CCNative::device(), params.GetAllocator());
    params.AddMember("Resolution", toStr(CCNative::getResolutionWidth()) + "*" + toStr(CCNative::getResolutionHeight()), params.GetAllocator());
}

PuzzleRPC* PuzzleRPC::getInstance()
{
    static PuzzleRPC* _ins = NULL;
    if (!_ins)
    {
        _ins = new PuzzleRPC();
    }
    return _ins;
}

PuzzleRPC::PuzzleRPC()
{
}

void PuzzleRPC::setNetworkTimeOut(int seconds)
{
    CCHttpClient::getInstance()->setTimeoutForConnect(seconds);
    CCHttpClient::getInstance()->setTimeoutForRead(seconds*2);
}

int PuzzleRPC::getNetworkTimeOut()
{
    return CCHttpClient::getInstance()->getTimeoutForConnect();
}

void PuzzleRPC::setServerURL(const std::string& server_endpoint)
{
    serverUrl = "http://" + serverUrl + "/";
    messageUrl = "ws://" + serverUrl + ":9000/messages";
}

void PuzzleRPC::login(const std::string& type, const std::string& udid, const std::string& password, const std::string& referCode,
                      boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Control> control, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("accType", type, params.GetAllocator());
    params.AddMember("id", udid, params.GetAllocator());
    params.AddMember("password", password, params.GetAllocator());
    params.AddMember("clientVersion", PH_VERSION, params.GetAllocator());
    params.AddMember("clientChannel", distro, params.GetAllocator());
    
    addDeviceParams(params);
    
    if (!referCode.empty())
    {
        params.AddMember("referCode", referCode, params.GetAllocator());
    }
    
    boost::function<void (auto_ptr<boost::tuple<Player*, Control*>>, auto_ptr<Error>)> wrapped = [=](auto_ptr<boost::tuple<Player*, Control*>> r, auto_ptr<Error> e)
    {
        if (e.get())
            cb(auto_ptr<Player>(NULL), auto_ptr<Control>(NULL), e);
        else
            cb(auto_ptr<Player>(boost::tuples::get<0>(*r)), auto_ptr<Control>(boost::tuples::get<1>(*r)), e);
    };
    
    call("login", params, wrapped);
}

void PuzzleRPC::getStoreItems(const std::string& token, boost::function<void (std::auto_ptr<Control>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    params.AddMember("token", token, params.GetAllocator());
    
    call("getstoreitems", params, cb);
}

void PuzzleRPC::chooseLoginReward(const std::string& token, int position,
                                  boost::function<void (std::auto_ptr<Player>,
                                                        std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    params.AddMember("pos", position, params.GetAllocator());
    params.AddMember("token", token, params.GetAllocator());

    call("chooseLoginReward", params, cb);
}

void PuzzleRPC::getFullhouseReward(const std::string &token, const std::string &stageId,
                                   boost::function<void (std::auto_ptr<Player>,
                                                         ChestRewards,
                                                         std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("stageid", stageId, params.GetAllocator());
    params.AddMember("token", token, params.GetAllocator());
    
    boost::function<void (auto_ptr<boost::tuple<Player*, ChestRewards>>, auto_ptr<Error>)> wrapped =
    [=](auto_ptr<boost::tuple<Player*, ChestRewards>> r, auto_ptr<Error> e)
    {
        if (e.get())
            cb(auto_ptr<Player>(NULL), ChestRewards(), e);
        else
            cb(auto_ptr<Player>(boost::tuples::get<0>(*r)),
               (boost::tuples::get<1>(*r)),
               e);
    };
    
    call("getfullhousereward", params, wrapped);
}


void PuzzleRPC::registerUser(const std::string& udid, const std::string password, const std::string& referCode,
                             boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Control> control, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("udid", udid, params.GetAllocator());
    params.AddMember("password", password, params.GetAllocator());
    
    if (!referCode.empty())
    {
        params.AddMember("referCode", referCode, params.GetAllocator());
    }

    boost::function<void (auto_ptr<boost::tuple<Player*, Control*>>, auto_ptr<Error>)> wrapped = [=](auto_ptr<boost::tuple<Player*, Control*>> r, auto_ptr<Error> e)
    {
        if (e.get())
            cb(auto_ptr<Player>(NULL), auto_ptr<Control>(NULL), e);
        else
            cb(auto_ptr<Player>(boost::tuples::get<0>(*r)), auto_ptr<Control>(boost::tuples::get<1>(*r)), e);
    };
    
    call("reg", params, wrapped);
}

void PuzzleRPC::startGame(const std::string& token, const std::string& levelId, const std::string& helperUser, const std::string& helperHero,
                          boost::function<void (std::auto_ptr<LevelInfo>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("level", levelId, params.GetAllocator());
    params.AddMember("helper", helperUser, params.GetAllocator());
    params.AddMember("helperHero", helperHero, params.GetAllocator());
    
    call("startgame", params, cb);
}

void PuzzleRPC::endGame(const std::string& token,
                        const std::string& levelId,
                        const int opLevel,
                        boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("level", levelId, params.GetAllocator());
    params.AddMember("opLevel", opLevel, params.GetAllocator());
    
    call("endgame", params, cb);
}

void PuzzleRPC::redeemSoul(const std::string& token, int heroId, boost::function<void (std::auto_ptr<Hero>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("heroId", heroId, params.GetAllocator());
    
    call("redeem", params, cb);
}

void PuzzleRPC::drawHero(const std::string& token, boost::function<void (std::auto_ptr<Hero>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    
    call("drawhero", params, cb);
}

void PuzzleRPC::drawHero2(const std::string& token, int drawId, boost::function<void (std::auto_ptr<Hero>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("draw", drawId, params.GetAllocator());
    
    call("drawhero2", params, cb);
}

void PuzzleRPC::drawNiceHero(const std::string& token, int drawId, boost::function<void (std::auto_ptr<Hero>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("draw", drawId, params.GetAllocator());
    
    call("drawnicehero", params, cb);
}

void PuzzleRPC::draw10Hero(const std::string& token, boost::function<void (std::auto_ptr<Hero10Draw>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    
    call("drawhero10", params, cb);
}

void PuzzleRPC::draw10ExpItems(const std::string& token, boost::function<void (std::auto_ptr<Hero10Draw>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    
    call("drawexpitems", params, cb);
}

void PuzzleRPC::draw10TransItems(const std::string& token, boost::function<void (std::auto_ptr<Hero10Draw>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    
    call("drawtransitems", params, cb);
}

void PuzzleRPC::setFormation(const std::string& token, const std::vector<std::string>& teamIds, int activeTeam,
                             boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    Value team(kArrayType);
    for (const string& m : teamIds)
    {
        team.PushBack(m, params.GetAllocator());
    }
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("teamIds", team, params.GetAllocator());
    params.AddMember("active", activeTeam, params.GetAllocator());
    
    call("setformation", params, cb);
}

void PuzzleRPC::sellHero(const std::string& token, const std::vector<std::string>& mats,
                         boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    
    Value heroIds(kArrayType);
    for (const string& m : mats)
    {
        heroIds.PushBack(m, params.GetAllocator());
    }
    
    params.AddMember("heroIds", heroIds, params.GetAllocator());
    
    call("sell", params, cb);
}

void PuzzleRPC::trainHero(const std::string& token,
                          const std::string& target,
                          const std::vector<std::string>& mats,
                          bool wantsEliteLevelSuccess,
                          boost::function<void (std::auto_ptr<Hero>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    Value heroIds(kArrayType);
    for (const string& m : mats)
    {
        heroIds.PushBack(m, params.GetAllocator());
    }
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("target", target, params.GetAllocator());
    params.AddMember("mats", heroIds, params.GetAllocator());
    params.AddMember("wantsEliteLevelSuccess", wantsEliteLevelSuccess, params.GetAllocator());
    
    call("train", params, cb);
}

void PuzzleRPC::transformHero(const std::string& token, const std::string& hid, const std::vector<std::string>& mats,
                              boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    Value heroIds(kArrayType);
    for (const string& m : mats)
    {
        heroIds.PushBack(m, params.GetAllocator());
    }
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("target", hid, params.GetAllocator());
    params.AddMember("mats", heroIds, params.GetAllocator());
    
    call("transform", params, cb);
}

void PuzzleRPC::restoreSpirit(const std::string& token, SpiritPurchaseAmount amountId, boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("type", "spirit", params.GetAllocator());
    params.AddMember("amountId", amountId, params.GetAllocator());
    
    call("purchase", params, cb);
}

void PuzzleRPC::purchaseGolds(const std::string& token, const StoreItem& goldItem, std::string& receipt,
                              boost::function<void (std::auto_ptr<Player>, std::auto_ptr<VipUpgrade>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("type", "gold", params.GetAllocator());
    params.AddMember("amount", goldItem.amount, params.GetAllocator());
    params.AddMember("channel", goldItem.channel, params.GetAllocator());
    params.AddMember("receipt", receipt, params.GetAllocator());
    
    boost::function<void ((auto_ptr<boost::tuple<Player*, VipUpgrade*>> r, auto_ptr<Error> e))> wrapped = [=](auto_ptr<boost::tuple<Player*, VipUpgrade*>> r, auto_ptr<Error> e)
    {
        if (e.get())
            cb(auto_ptr<Player>(NULL), auto_ptr<VipUpgrade>(NULL), e);
        else
            cb(auto_ptr<Player>(boost::tuples::get<0>(*r)), auto_ptr<VipUpgrade>(boost::tuples::get<1>(*r)), e);
    };
    
    call("purchase", params, wrapped);
}

void PuzzleRPC::purchaseStorageRoom(const std::string& token, int storage, boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("type", "storage", params.GetAllocator());
    params.AddMember("storageType", storage, params.GetAllocator());
    
    call("purchase", params, cb);
}

void PuzzleRPC::purchaseResetStage(const std::string& token, int stage, boost::function<void (std::auto_ptr<Stage>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("type", "stage", params.GetAllocator());
    params.AddMember("stageID", stage, params.GetAllocator());
    
    call("purchase", params, cb);
}

void PuzzleRPC::purchaseVIPPack(const std::string& token, int vipLevel,
                                boost::function<void (std::auto_ptr<Player>, std::vector<Treasure>, std::auto_ptr<Error>)> cb)
{
    boost::function<void (auto_ptr<boost::tuple<Player*, vector<Treasure>>>, auto_ptr<Error>)> wrapped = [=](auto_ptr<boost::tuple<Player*, vector<Treasure>>> r, auto_ptr<Error> e)
    {
        if (e.get())
            cb(auto_ptr<Player>(NULL), vector<Treasure>(), e);
        else
            cb(auto_ptr<Player>(boost::tuples::get<0>(*r)), boost::tuples::get<1>(*r), e);
    };
    
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("type", "vip", params.GetAllocator());
    params.AddMember("vipGiftLevel", vipLevel, params.GetAllocator());
    
    call("purchase", params, wrapped);
}

void PuzzleRPC::purchaseVIPOtherPack(const std::string &token, int packid,
                                     boost::function<void (std::auto_ptr<Player>, std::vector<Treasure>, std::auto_ptr<Error>)> cb)
{
    boost::function<void (auto_ptr<boost::tuple<Player*, vector<Treasure>>>, auto_ptr<Error>)> wrapped = [=](auto_ptr<boost::tuple<Player*, vector<Treasure>>> r, auto_ptr<Error> e)
    {
        if (e.get())
            cb(auto_ptr<Player>(NULL), vector<Treasure>(), e);
        else
            cb(auto_ptr<Player>(boost::tuples::get<0>(*r)), boost::tuples::get<1>(*r), e);
    };
    
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("type", "ovip", params.GetAllocator());
    params.AddMember("vipGiftId", packid, params.GetAllocator());
    
    call("purchase", params, wrapped);
}

void PuzzleRPC::purchasePvPItem(const std::string& token, int pvpItemID, boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("type", "pvpItem", params.GetAllocator());
    params.AddMember("pvpItemID", pvpItemID, params.GetAllocator());
    
    call("purchase", params, cb);
}

void PuzzleRPC::revive(const std::string& token, boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("type", "revive", params.GetAllocator());
    
    call("purchase", params, cb);
}

void PuzzleRPC::friends(const std::string& token, boost::function<void (std::vector<Player*>, int maxFriends, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    params.AddMember("token", token, params.GetAllocator());
    
    boost::function<void (auto_ptr<boost::tuple<vector<Player*>, int>>, auto_ptr<Error>)> wrapped = [=](auto_ptr<boost::tuple<vector<Player*>, int>> r, auto_ptr<Error> e){
        if (e.get())
            cb(vector<Player*>(), 0, e);
        else
            cb(boost::tuples::get<0>(*r), boost::tuples::get<1>(*r), e);
    };
    
    call("friends", params, wrapped);
}

void PuzzleRPC::pendingFriends(const std::string& token, boost::function<void (std::vector<Player*>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    params.AddMember("token", token, params.GetAllocator());
    
    boost::function<void (auto_ptr<vector<Player*>>, auto_ptr<Error>)> wrapped = [=](auto_ptr<vector<Player*>> r, auto_ptr<Error> e){
        if (e.get())
            cb(vector<Player*>(), e);
        else
            cb(*r.get(), e);
    };
    
    call("pendings", params, wrapped);
}

void PuzzleRPC::helpers(const std::string& token, const std::string& level, boost::function<void (std::vector<Player*>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    params.AddMember("token", token, params.GetAllocator());
    
    boost::function<void (auto_ptr<vector<Player*>>, auto_ptr<Error>)> wrapped = [=](auto_ptr<vector<Player*>> r, auto_ptr<Error> e){
        if (e.get())
            cb(vector<Player*>(), e);
        else
            cb(*r.get(), e);
    };
    
    call("helpers", params, wrapped);
}

void PuzzleRPC::findFriendByName(const std::string& token, const std::string& userName, int skip, int limit,
                                 boost::function<void (std::auto_ptr<PlayerList>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("name", userName, params.GetAllocator());
    params.AddMember("skip", skip, params.GetAllocator());
    
    call("finduser", params, cb);
}

void PuzzleRPC::addFriend(const std::string& token, const std::string& userId, boost::function<void (std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("targetId", userId, params.GetAllocator());
    
    boost::function<void (std::auto_ptr<Ignore>, std::auto_ptr<Error>)> wrapped = [=](std::auto_ptr<Ignore>, std::auto_ptr<Error> err){
        cb(err);
    };
    
    call("addfriend", params, wrapped);
}

void PuzzleRPC::deleteFriend(const std::string& token, const std::string& userId, boost::function<void (std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("action", "delete", params.GetAllocator());
    params.AddMember("target", userId, params.GetAllocator());
    
    boost::function<void (std::auto_ptr<Ignore>, std::auto_ptr<Error>)> wrapped = [=](std::auto_ptr<Ignore>, std::auto_ptr<Error> err){
        cb(err);
    };
    
    call("friends", params, wrapped);
}

void PuzzleRPC::rejectFriendRequest(const std::string& token, const std::string& userId, boost::function<void (std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("action", "reject", params.GetAllocator());
    params.AddMember("target", userId, params.GetAllocator());
    
    boost::function<void (std::auto_ptr<Ignore>, std::auto_ptr<Error>)> wrapped = [=](std::auto_ptr<Ignore>, std::auto_ptr<Error> err){
        cb(err);
    };
    
    call("pendings", params, wrapped);
}

void PuzzleRPC::acceptFriendRequest(const std::string& token, const std::string& userId, boost::function<void (std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("action", "approve", params.GetAllocator());
    params.AddMember("target", userId, params.GetAllocator());
    
    boost::function<void (std::auto_ptr<Ignore>, std::auto_ptr<Error>)> wrapped = [=](std::auto_ptr<Ignore>, std::auto_ptr<Error> err){
        cb(err);
    };
    
    call("pendings", params, wrapped);
}

void PuzzleRPC::bindAccount(const std::string& token, const std::string& type, const std::string& id, const std::string& password,
                            boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("type", type, params.GetAllocator());
    params.AddMember("id", id, params.GetAllocator());
    params.AddMember("password", password, params.GetAllocator());

    call("bind", params, cb);
}

void PuzzleRPC::setTutorialProgress(const std::string& token, int progress, boost::function<void (std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("progress", progress, params.GetAllocator());
    
    boost::function<void (std::auto_ptr<Ignore>, std::auto_ptr<Error>)> wrapped = [=](std::auto_ptr<Ignore>, std::auto_ptr<Error> err){
        cb(err);
    };
    
    call("tutorialprogress", params, wrapped);
}

void PuzzleRPC::pickInitialHero(const std::string& token, int heroId, boost::function<void (std::auto_ptr<Hero>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("heroId", heroId, params.GetAllocator());
    
    call("tutorialpick", params, cb);
}

void PuzzleRPC::launch()
{
    boost::function<void (std::auto_ptr<Ignore>, std::auto_ptr<Error>)> wrapped = [](std::auto_ptr<Ignore>, std::auto_ptr<Error> err)
    {
    };
    
    Document params;
    params.SetObject();
    
    params.AddMember("clientChannel", distro, params.GetAllocator());
    addDeviceParams(params);
    
    call("launch", params, wrapped);
}

void PuzzleRPC::sync(boost::function<void (std::vector<HeroProfile*>, std::vector<SkillInfo*>, Control, std::auto_ptr<Error>)> cb)
{
    boost::function<void (auto_ptr<boost::tuple<vector<HeroProfile*>, vector<SkillInfo*>, Control>>, auto_ptr<Error>)> wrapped = [=](auto_ptr<boost::tuple<vector<HeroProfile*>, vector<SkillInfo*>, Control>> r, auto_ptr<Error> e)
    {
        if (e.get())
            cb(vector<HeroProfile*>(), vector<SkillInfo*>(), Control(), e);
        else
            cb(tuples::get<0>(*r), tuples::get<1>(*r), tuples::get<2>(*r), e);
    };
    
    Document params;
    params.SetObject();

    params.AddMember("clientChannel", distro, params.GetAllocator());
    addDeviceParams(params);
    
    call("sync", params, wrapped);
}

void PuzzleRPC::updateUserProfile(const std::string& token, const std::string& name, const std::string& pushToken,
                                  boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)> cb)
{
    if (name.empty() && pushToken.empty())
        return;

    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    if (!name.empty())
        params.AddMember("name", name, params.GetAllocator());
    if (!pushToken.empty())
        params.AddMember("pushToken", pushToken, params.GetAllocator());
    
    call("profile", params, cb);
}

void PuzzleRPC::getRelations(const std::string& token, int heroId, boost::function<void (std::vector<HeroRelation>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("heroId", heroId, params.GetAllocator());
    
    boost::function<void (auto_ptr<vector<HeroRelation>>, auto_ptr<Error>)> wrapped = [=](auto_ptr<vector<HeroRelation>> r, auto_ptr<Error> e) {
        if (e.get())
            cb(std::vector<HeroRelation>(), e);
        else
            cb(*r.get(), e);
    };
    
    call("relation", params, wrapped);
}

void PuzzleRPC::changeRelations(const std::string& token,
                                const std::string& hero,
                                std::vector<std::string> lockRelations,
                                RelationChangeType type,
                                boost::function<void (std::auto_ptr<Hero>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    Value locked(kArrayType);
    for (auto& r : lockRelations)
        locked.PushBack(r, params.GetAllocator());
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("hero", hero, params.GetAllocator());
    params.AddMember("lockRelations", locked, params.GetAllocator());
    params.AddMember("type", static_cast<int>(type), params.GetAllocator());
    //params.AddMember("freeFlag", true, params.GetAllocator());
    
    call("changerelation", params, cb);
}

void PuzzleRPC::pvpMyRanks(const std::string& token, boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    
    call("pvpmyrank", params, cb);
}

void PuzzleRPC::pvpTopRanks(const std::string& token, boost::function<void (std::auto_ptr<PlayerRankList>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    
    call("pvpuserrank", params, cb);
}

void PuzzleRPC::pvpGameLog(const std::string& token, int before, boost::function<void (std::auto_ptr<PvPGameLogList>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("before", before, params.GetAllocator());
    
    call("pvpgamelog", params, cb);
}

void PuzzleRPC::checkMail(const std::string& token, boost::function<void (std::auto_ptr<MailBox>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    
    call("mail/check", params, cb);
}

void PuzzleRPC::markAsRead(const std::string& token, const std::string& mailID, boost::function<void (std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("mailID", mailID, params.GetAllocator());
    
    boost::function<void (std::auto_ptr<Ignore>, std::auto_ptr<Error>)> wrapped = [=](std::auto_ptr<Ignore>, std::auto_ptr<Error> err){
        cb(err);
    };
    
    call("mail/read", params, wrapped);
}

void PuzzleRPC::sendMailToGM(const std::string& token, const std::string& replyTo, const std::string& message,
                             boost::function<void (std::auto_ptr<Mail>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    if (!replyTo.empty())
    {
        params.AddMember("reply", replyTo, params.GetAllocator());
    }
    params.AddMember("message", message, params.GetAllocator());
    
    call("mail/send", params, cb);
}

void PuzzleRPC::openPackage(const std::string& token, const std::string& mailID,
                            boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("mailID", mailID, params.GetAllocator());
    
    call("mail/open", params, cb);
}

// ranking list
void PuzzleRPC::getRankingList(const std::string& token,
                               RankingType type,
                               boost::function<void (std::auto_ptr<RankingList>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    //printf("%s\n", token.c_str());
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("type", static_cast<int>(type), params.GetAllocator());
    
    call("ranking/list", params, cb);
}

void PuzzleRPC::rankingComment(const std::string& token,
                               const std::string& objId,
                               RankingType type,
                               bool isFlower,
                               boost::function<void (std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("type", static_cast<int>(type), params.GetAllocator());
    params.AddMember("objId", objId, params.GetAllocator());
    params.AddMember("isFlower", isFlower, params.GetAllocator());
    
    boost::function<void (std::auto_ptr<Ignore>, std::auto_ptr<Error>)> wrapped = [=](std::auto_ptr<Ignore>, std::auto_ptr<Error> err){
        cb(err);
    };
    
    call("ranking/comment", params, wrapped);
}

// skill bonus
void PuzzleRPC::changeSkillBonus(const std::string &token,
                                 const std::string &hero,
                                 SkillBonusChangeType type,
                                 boost::function<void (std::auto_ptr<BoolWrapper>, std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("hero", hero, params.GetAllocator());
    params.AddMember("type", static_cast<int>(type), params.GetAllocator());
    
    call("hero/energybonus", params, cb);
}

// player tasks

void PuzzleRPC::getTasks(const std::string& token,
                         boost::function<void (std::auto_ptr<PlayerTaskVec>,
                                               std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    
    call("task/list", params, cb);
}

void PuzzleRPC::receivePlayerTaskReward(const std::string& token, int taskId,
                                        boost::function<void (std::auto_ptr<Player>,
                                                              std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    params.AddMember("taskId", taskId, params.GetAllocator());
    
    call("task/rewards", params, cb);
}

// stats

void PuzzleRPC::shareMsgSucceed(const std::string &token,
                                boost::function<void (std::auto_ptr<Error>)> cb)
{
    Document params;
    params.SetObject();
    
    params.AddMember("token", token, params.GetAllocator());
    
    boost::function<void (std::auto_ptr<Ignore>, std::auto_ptr<Error>)> wrapped = [=](std::auto_ptr<Ignore>, std::auto_ptr<Error> err){
        cb(err);
    };
    
    call("share/msg", params, wrapped);
}

#pragma mark - websocket handling

bool PuzzleRPC::messageServiceRunning()
{
    return (bool)channel;
}

void PuzzleRPC::connectMessageService(const std::string& userToken)
{
    disconnect();
    
    channel = boost::make_shared<MessageChannel>();
    auto sysHandler = boost::make_shared<SystemMessageHandler>("system", channel);
    sysHandler->setUserToken(userToken);
    
    pvpHandler = boost::make_shared<PvPMessageHandler>("pvp", channel);
    eventsHandler = boost::make_shared<GameEventMessageHandler>("events", channel);
    
    channel->multiplex("pvp", pvpHandler);
    channel->multiplex("system", sysHandler);
    channel->multiplex("events", eventsHandler);
    
    channel->connect(messageUrl);
    
    wsThread = boost::move(thread(boost::bind(&MessageChannel::run, channel.get())));
}

void PuzzleRPC::disconnect()
{
    if (messageServiceRunning())
    {
        channel->close();
        channel.reset();
    }
}

void PuzzleRPC::handlerMessagesForServiceDomain(const std::string& domain, boost::function<void (const std::string&)> f)
{
    auto msgHandler = channel->handlerForDomain(domain);
    if (!msgHandler)
    {
        msgHandler = boost::make_shared<simpleMessageHandler>(domain, channel);
        channel->multiplex(domain, msgHandler);
    }

    dynamic_pointer_cast<simpleMessageHandler>(msgHandler)->addCallback(f);
}

void PuzzleRPC::removeAllHandlersForDomain(const std::string& domain)
{
    auto msgHandler = channel->handlerForDomain(domain);
    if (!msgHandler)
    {
        msgHandler = boost::make_shared<simpleMessageHandler>(domain, channel);
        channel->multiplex(domain, msgHandler);
    }
    
    dynamic_pointer_cast<simpleMessageHandler>(msgHandler)->clearCallback();
}

void PuzzleRPC::sendMessage(const std::string& domain, const std::string& message)
{
    auto msgHandler = channel->handlerForDomain(domain);
    if (!msgHandler)
    {
        msgHandler = boost::make_shared<simpleMessageHandler>(domain, channel);
        channel->multiplex(domain, msgHandler);
    }
    
    msgHandler->send(message);
}

boost::shared_ptr<handle> PuzzleRPC::handlerForServiceDomain(const std::string& domain)
{
    return channel->handlerForDomain(domain);
}

#pragma mark - http calling and response handling

void PuzzleRPC::call(const std::string& method, const std::string& httpMethod, const Document& reqJson, RPCCallback callback)
{
    string json = write(reqJson);
    string url;
    std::vector<string> headers;
    
    if (boost::starts_with(method, "http"))
        url = method;
    else
    {
        url = serverUrl+method;
//        headers.push_back("Content-Type: STL-Request");
    }
    
    unsigned char md5[MD5_DIGEST_LENGTH] = {0};
    unsigned char sha1[SHA_DIGEST_LENGTH] = {0};

    MD5_CTX ctx5;
    MD5_Init(&ctx5);
    MD5_Update(&ctx5, json.c_str(), json.size());
    MD5_Final(md5, &ctx5);
    
    std::string md5String(md5, md5+MD5_DIGEST_LENGTH);
    std::string codedJson = json + md5String;
    
    SHA_CTX ctx_sha1;
    SHA1_Init(&ctx_sha1);
    SHA1_Update(&ctx_sha1, codedJson.c_str(), codedJson.size());
    SHA1_Final(sha1, &ctx_sha1);
    
    std::string sha1String(sha1, sha1+SHA_DIGEST_LENGTH);
    
    headers.push_back("STL-Secret-Encode: " + boost::algorithm::hex(sha1String));
    
//    auto payload = CCCrypto::encryptRC4(json, RPC_RC4Key);
    auto payload = json;
    call(url, httpMethod, payload, headers, boost::move(callback));
}

void PuzzleRPC::call(const std::string& url, const std::string& httpMethod, const std::string& data, RPCCallback callback)
{
    std::vector<string> headers;
    call(url, httpMethod, data, headers, boost::move(callback));
}

void PuzzleRPC::call(const std::string& url, const std::string& httpMethod, const std::string& data,
                     const std::vector<std::string>& customHeaders, RPCCallback callback)
{
    CCHttpRequest* req = new CCHttpRequest();
    req->setUrl(url.c_str());
    
    if (!data.empty())
        req->setRequestData(data.c_str(), data.size());
    
    if (httpMethod == "POST")
        req->setRequestType(CCHttpRequest::kHttpPost);
    else
        req->setRequestType(CCHttpRequest::kHttpGet);
    
    for (const auto& h : customHeaders)
    {
        req->addHeader(h);
    }

    req->setResponseCallback([this, callback](CCHttpResponse* resp){ this->handleResponse(resp, callback); });

    CCHttpClient::getInstance()->send(req);
    req->release();
}

static std::string inflate(const std::string& src)
{
    int ret;
    bool done = false;
    z_stream strm = {0};
    const size_t tmpBufferSize = 4096;
    Bytef tmpBuffer[tmpBufferSize] = {0};
    size_t guess_dstSize = src.size() * 2;

	strm.next_in = (Bytef*)src.c_str();
	strm.avail_in = src.size();
	strm.total_out = 0;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
    
	if (inflateInit(&strm) != Z_OK)
        return "";
    
    std::vector<char> retBuffer;
    retBuffer.reserve(guess_dstSize);
	while (!done)
	{
		// Inflate another chunk.
        strm.next_out = tmpBuffer;
        strm.avail_out = sizeof(tmpBuffer) / sizeof(tmpBuffer[0]);
		ret = inflate(&strm, Z_SYNC_FLUSH);
        copy(tmpBuffer, tmpBuffer + tmpBufferSize - strm.avail_out, back_inserter(retBuffer));
		if (ret == Z_STREAM_END)
            done = true;
		else if (ret != Z_OK)
            break;
	}
	if (inflateEnd(&strm) != Z_OK)
        return "";
    
	// Set real length.
    string retString;
	if (done)
	{
        retString = string(retBuffer.data(), retBuffer.size());
	}
	return retString;
}

void PuzzleRPC::handleResponse(CCHttpResponse* resp, RPCCallback callback)
{
    ErrorCode errCode = Succeeded;
    std::string errorMessage;
    std::string jsonStr;
    Document v;
    
    if (resp->getResponseCode() == 200)
    {
        std::string encryptedStr((const char*)resp->getResponseData()->data(), resp->getResponseData()->size());
        bool isEncrypted = false;
        for (auto& header : resp->getHeaders())
        {
            if (starts_with(header, "Content-Type: ") && find_first(header, "STL-Response"))
            {
                isEncrypted = true;
                break;
            }
        }
        if (false)
        {
            jsonStr = inflate(CCCrypto::decryptRC4(encryptedStr, RPC_RC4Key));
        }
        else
            jsonStr.swap(encryptedStr);
        //LOGD("inflated json: %s\n", jsonStr.c_str());
        v.ParseInsitu<0>((char*)jsonStr.c_str());
        if (v.HasParseError())
        {
            errCode = JsonParseError;
            errorMessage = v.GetParseError();
        }
    }
    else if (resp->getResponseCode() == -1)
        errCode = NetworkError;
    else
    {
        errCode = (ErrorCode)resp->getResponseCode();
        resp->getResponseData()->push_back('\0');
        v.ParseInsitu<0>(resp->getResponseData()->data());
    }
    
    try
    {
        if (!errCode)
            callback(v, auto_ptr<Error>(NULL));
        else
        {
            Error *err = new Error;
            err->code = errCode;
            err->msg = errorMessage;
            callback(v, auto_ptr<Error>(err));
        }
    }
    catch (...)
    {
    }
}

template <class ResultType>
void PuzzleRPC::call(const std::string& method,
          const rapidjson::Document& parameters,
          boost::function<void (std::auto_ptr<ResultType>, std::auto_ptr<Error>)> cb)
{
    call(method, "POST", parameters, [=](const Document& obj, std::auto_ptr<Error> err)
    {
        if (!err.get())
        {
            // parse resulting json into stongly typed struct
            if (obj.HasMember("errorCode"))
            {
                int code;
                std::string errMsg;
                
                try
                {
                    code = obj["errorCode"].GetInt();
                    if (obj["message"].IsString())
                        errMsg = string(obj["message"].GetString(), obj["message"].GetStringLength());
                }
                catch (...)
                {
                    code = JsonParseError;
                }
                Error* err = new Error();
                err->code = (ErrorCode)code;
                err->msg = errMsg;
                cb(std::auto_ptr<ResultType>(NULL), std::auto_ptr<Error>(err));
            }
            else
            {
                ErrorCode code = Succeeded;
                std::auto_ptr<ResultType> ret;
                
                try
                {
                    ret.reset(parse<ResultType>(obj));
                }
                catch (...)
                {
                    code = JsonParseError;
                }
                
                if (code)
                {
                    Error* err = new Error();
                    err->code = (ErrorCode)code;
                    cb(std::auto_ptr<ResultType>(NULL), std::auto_ptr<Error>(err));
                }
                else
                    cb(ret, std::auto_ptr<Error>(NULL));
            }
        }
        else
            cb(std::auto_ptr<ResultType>(NULL), err);
    });
}
