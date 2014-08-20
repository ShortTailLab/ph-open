//
//  PuzzleRPC.h
//  HelloCpp
//
//  Created by R.C on 12-10-24.
//
//

#ifndef __HelloCpp__PuzzleRPC__
#define __HelloCpp__PuzzleRPC__

#include <memory>
#include <boost/container/map.hpp>
#include <vector>
#include <boost/function.hpp>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <cocos2d.h>
#include <network/HttpResponse.h>
#include <rapidjson/document.h>
#include "BattleScene.h"
#include "SystemInform.h"



namespace PH
{
    enum CCNetworkStatus
    {
        CCNetworkStatusNotReachable = 0,
        CCNetworkStatusReachableViaWiFi,
        CCNetworkStatusReachableViaWWAN
    };
    
    enum ErrorCode : int
    {
        Succeeded = 0,
        // LOCAL Error
        TimeOut = 1,
        JsonParseError = 2,
        NetworkError = 3,
        
        // 3-digit error codes are http error codes
        
        // SERVER Error
        RequestNotValid = 1001,
        MissingParam = 1002,
        InCorrectParamType = 1003,
        InvalidParam = 1007,
        WrongUserNamePassword = 1004,
        NotLoggedIn = 1005,
        RegClosed = 1025,
        
        Taken = 1006,
        GameNonExist = 1008,
        HeroNotOwned = 1009,
        NotEnoughCoins = 1010,
        NotEnoughGolds = 1011,
        NotEnoughSpirits = 1012,
        StorageExceeded = 1013,
        MaxLevel = 1014,
        Banned = 1015,
        PurchasePending = 1016,
        
        NoUser = 1017,
        
        BoundToOtherUser = 1018,
        AuthTypeBound = 1019,
        NotEnoughLP = 1020,
        PurchaseVerified = 1021,
        PurchaseInvalid = 1022,
        
        TutorialDone = 1023,
        
        MaxStorageLimit = 1030,
        MaxFriend       = 1031,
        NotEnoughSouls  = 1032,
        
        QuotaExceeded = 1033,
        FreeDrawNotAvailable = 1034,
        BadUsername = 1035,
        
        Bug = 2000,
    };
        
    struct Error
    {
        ErrorCode code;
        std::string msg;
        
        std::string toStr()
        {
            std::string num = boost::lexical_cast<std::string>(code);
            return num + ", " + codeToStr(code) + ": " + msg;
        }
        
        std::string toStrCN()
        {
            std::string num = boost::lexical_cast<std::string>(code);
            return codeToStrCN(code);
        }
        
        void handleErrorInform();
        
        static std::string codeToStr(ErrorCode code)
        {
            switch(code)
            {
                case Succeeded : return "Succeeded";
                case TimeOut : return "TimeOut";
                case JsonParseError : return "JsonParseError";
                case NetworkError : return "NetworkError";
                case RequestNotValid : return "RequestNotValid";
                case MissingParam : return "MissingParam";
                case InCorrectParamType : return "IncorrectParamType";
                case InvalidParam : return "InvalidParam";
                case WrongUserNamePassword : return "WrongUserNamePassword";
                case NotLoggedIn : return "NotLoggedIn";
                case Taken : return "Taken";
                case GameNonExist : return "GameNotExist";
                case HeroNotOwned : return "HeroNotOwned";
                case NotEnoughCoins : return "NotEnoughCoins";
                case NotEnoughGolds : return "NotEnoughtGolds";
                case NotEnoughSpirits : return "NotEnoughtSpirits";
                case StorageExceeded : return "StorageExceeded";
                case MaxLevel : return "MaxLevel";
                case Banned : return "Banned";
                case PurchasePending : return "PurchasePending";
                case NoUser : return "NoUser";
                case BoundToOtherUser : return "BoundToOtherUser";
                case AuthTypeBound : return "AuthTypeBound";
                case NotEnoughLP : return "NotEnoughLP";
                case PurchaseVerified : return "PurchaseVerified";
                    
                case TutorialDone: return "TutorialDone";
                case MaxStorageLimit : return "MaxStorageLimit";
                case MaxFriend : return "MaxFriend";
                    
                case NotEnoughSouls : return "NotEnoughSouls";
                case RegClosed: return "RegClosed";
                case BadUsername: return "BadUsername";
                case Bug : return "Bug";
                default:
                    return "Unkown error code";
            }
        }
        
        static std::string codeToStrCN(ErrorCode code)
        {
            switch(code)
            {
                case Succeeded : return "Succeeded";
                case TimeOut : return "网络请求超时";
                case JsonParseError : return "JsonParseError";
                case NetworkError : return "网络连接错误";
                case RequestNotValid : return "请求无效";
                case MissingParam : return "参数缺失";
                case InCorrectParamType : return "IncorrectParamType";
                case InvalidParam : return "InvalidParam";
                case WrongUserNamePassword : return "用户名或密码错误";
                case NotLoggedIn : return "请先登录";
                case Taken : return "Taken";
                case GameNonExist : return "该关卡已经过期了";
                case HeroNotOwned : return "你没有拥有该侠客";
                case NotEnoughCoins : return "你的铜钱不足";
                case NotEnoughGolds : return "你的元宝不足";
                case NotEnoughSpirits : return "你的体力不足";
                case StorageExceeded : return "你的侠客席位不足";
                case MaxLevel : return "你的等级不足";
                case Banned : return "该操作不允许";
                case PurchasePending : return "PurchasePending";
                case NoUser : return "不存在该用户";
                case BoundToOtherUser : return "该账号已经被绑定其他用户";
                case AuthTypeBound : return "你已经绑定过该类型的账号";
                case NotEnoughLP : return "你的威望不足";
                case PurchaseVerified : return "PurchaseVerified";

                case TutorialDone: return "新手引导已完成";
                case MaxStorageLimit : return "已达到最大席位大小";
                case MaxFriend : return "好友已满，无法添加更多好友";
                    
                case NotEnoughSouls : return "魂魄不足";
                case RegClosed: return "未开放注册";
                case BadUsername: return "用户名错误或者包含敏感词";
                case Bug : return "出错";
                default:
                    return "网络连接错误";
            }
        }
    };

    class MessageChannel;
    class handle;
    class simpleMessageHandler;
    class PvPMessageHandler;
    class GameEventMessageHandler;
        
    struct AccountType
    {
        static std::string Main, Guest, QQ, SinaWeibo;
    };
        
    enum SpiritPurchaseAmount
    {
        Ten = 0,
        Half = 1,
        Full = 2,
    };
        
// auto_ptr is deprecated. however, gnustl 4.2 in ios does not provide unique_ptr
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    
    class PuzzleRPC : public cocos2d::CCObject
    {
    public:
        static void setServerURL(const std::string& server_endpoint);
        
    public:
        static PuzzleRPC* getInstance();
        
        PuzzleRPC();

        bool isLocalWiFiAvailable(void);

        bool isInternetConnectionAvailable(void);

        bool isHostNameReachable(const std::string& hostName);
        
        CCNetworkStatus getInternetConnectionStatus(void);
        
        void setNetworkTimeOut(int seconds);
        int getNetworkTimeOut();
        
    public:
        void login(const std::string& type, const std::string& udid,
                   const std::string& password, const std::string& referCode,
                   boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Control> control, std::auto_ptr<Error>)>);
        void getStoreItems(const std::string& token, boost::function<void (std::auto_ptr<Control>, std::auto_ptr<Error>)>);
        
        void chooseLoginReward(const std::string& token, int pos,
                               boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)>);
        
        void getFullhouseReward(const std::string& token, const std::string & stageId,
                                boost::function<void (std::auto_ptr<Player>,
                                                      ChestRewards,
                                                      std::auto_ptr<Error>)>);
        
        void registerUser(const std::string& udid, const std::string password, const std::string& referCode,
                          boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Control> control, std::auto_ptr<Error>)>);
        
        void startGame(const std::string& token, const std::string& levelId, const std::string& helperUser, const std::string& helperHero,
                       boost::function<void (std::auto_ptr<LevelInfo>, std::auto_ptr<Error>)>);
        void endGame(const std::string& token, const std::string& levelId, const int opLevel,
                     boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)>);
        
        void drawHero(const std::string& token, boost::function<void (std::auto_ptr<Hero>, std::auto_ptr<Error>)>);
        void drawHero2(const std::string& token, int drawId, boost::function<void (std::auto_ptr<Hero>, std::auto_ptr<Error>)>);
        void drawNiceHero(const std::string& token, int drawId, boost::function<void (std::auto_ptr<Hero>, std::auto_ptr<Error>)>);
        void draw10Hero(const std::string& token, boost::function<void (std::auto_ptr<Hero10Draw>, std::auto_ptr<Error>)>);
        
        void draw10ExpItems(const std::string& token, boost::function<void (std::auto_ptr<Hero10Draw>, std::auto_ptr<Error>)>);
        void draw10TransItems(const std::string& token, boost::function<void (std::auto_ptr<Hero10Draw>, std::auto_ptr<Error>)>);
        
        void redeemSoul(const std::string& token, int heroId, boost::function<void (std::auto_ptr<Hero>, std::auto_ptr<Error>)>);
        
        void setFormation(const std::string& token, const std::vector<std::string>& teamIds, int activeTeam,
                          boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)>);
        void sellHero(const std::string& token, const std::vector<std::string>& mats,
                      boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)>);
        void trainHero(const std::string& token, const std::string& target, const std::vector<std::string>& mats, bool wantsEliteLevelSuccess,
                       boost::function<void (std::auto_ptr<Hero>, std::auto_ptr<Error>)>);
        void transformHero(const std::string& token, const std::string& hid, const std::vector<std::string>& mats,
                           boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)>);
        
        void findFriendByName(const std::string& token, const std::string& userName, int skip, int limit,
                              boost::function<void (std::auto_ptr<PlayerList>, std::auto_ptr<Error>)> cb);
        
        void friends(const std::string& token, boost::function<void (std::vector<Player*>, int maxFriends, std::auto_ptr<Error>)>);
        void pendingFriends(const std::string& token, boost::function<void (std::vector<Player*>, std::auto_ptr<Error>)>);
        void helpers(const std::string& token, const std::string& level, boost::function<void (std::vector<Player*>, std::auto_ptr<Error>)>);
        void addFriend(const std::string& token, const std::string& userId, boost::function<void (std::auto_ptr<Error>)>);
        void deleteFriend(const std::string& token, const std::string& userId, boost::function<void (std::auto_ptr<Error>)>);
        void rejectFriendRequest(const std::string& token, const std::string& userId, boost::function<void (std::auto_ptr<Error>)>);
        void acceptFriendRequest(const std::string& token, const std::string& userId, boost::function<void (std::auto_ptr<Error>)>);
        
        void restoreSpirit(const std::string& token, SpiritPurchaseAmount amountId,
                           boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)>);
        void purchaseGolds(const std::string& token, const StoreItem& goldItem, std::string& receipt,
                           boost::function<void (std::auto_ptr<Player>, std::auto_ptr<VipUpgrade>, std::auto_ptr<Error>)>);
        void purchaseStorageRoom(const std::string& token, int storageType,
                                 boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)>);
        void purchaseResetStage(const std::string& token, int stage, boost::function<void (std::auto_ptr<Stage>, std::auto_ptr<Error>)>);
        void purchaseVIPPack(const std::string& token, int vipLevel,
                             boost::function<void (std::auto_ptr<Player>, std::vector<Treasure>, std::auto_ptr<Error>)>);
        void purchaseVIPOtherPack(const std::string& token, int packid,
                                  boost::function<void (std::auto_ptr<Player>, std::vector<Treasure>, std::auto_ptr<Error>)>);
        void purchasePvPItem(const std::string& token, int pvpItemID, boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)>);
        
        void revive(const std::string& token, boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)>);
        
        void bindAccount(const std::string& token, const std::string& type, const std::string& id, const std::string& password,
                         boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)>);
        
        void setTutorialProgress(const std::string& token, int progress, boost::function<void (std::auto_ptr<Error>)>);
        void pickInitialHero(const std::string& token, int heroId, boost::function<void (std::auto_ptr<Hero>, std::auto_ptr<Error>)>);
        
        void launch();
        void sync(boost::function<void (std::vector<HeroProfile*>, std::vector<SkillInfo*>, Control, std::auto_ptr<Error>)>);
        
        void updateUserProfile(const std::string& token, const std::string& name, const std::string& pushToken,
                               boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)>);
        
        void getRelations(const std::string& token, int heroId, boost::function<void (std::vector<HeroRelation>, std::auto_ptr<Error>)>);
        void changeRelations(const std::string& token,
                             const std::string& hero,
                             std::vector<std::string> lockRelations,
                             RelationChangeType type,
                             boost::function<void (std::auto_ptr<Hero>, std::auto_ptr<Error>)>);
        
        void pvpMyRanks(const std::string& token, boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)> cb);
        void pvpTopRanks(const std::string& token, boost::function<void (std::auto_ptr<PlayerRankList>, std::auto_ptr<Error>)> cb);
        void pvpGameLog(const std::string& token, int before, boost::function<void (std::auto_ptr<PvPGameLogList>, std::auto_ptr<Error>)> cb);
        
    public: // ranking list
        void getRankingList(const std::string& token,
                            RankingType type,
                            boost::function<void (std::auto_ptr<RankingList>, std::auto_ptr<Error>)> cb);
        void rankingComment(const std::string& token,
                            const std::string& objId,
                            RankingType type,
                            bool isFlower,
                            boost::function<void (std::auto_ptr<Error>)> cb);
        
    public: // hero related
        void changeSkillBonus(const std::string& token,
                              const std::string& hero,
                              SkillBonusChangeType type,
                              boost::function<void (std::auto_ptr<BoolWrapper>,
                                                    std::auto_ptr<Error>)> cb);

    public: // player task
        void getTasks(const std::string& token,
                      boost::function<void (std::auto_ptr<PlayerTaskVec>, std::auto_ptr<Error>)>);
        void receivePlayerTaskReward(const std::string& token,
                                     int taskID,
                                     boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)>);
        
    public: // mailing
        void checkMail(const std::string& token, boost::function<void (std::auto_ptr<MailBox>, std::auto_ptr<Error>)> cb);
        void markAsRead(const std::string& token, const std::string& mailID, boost::function<void (std::auto_ptr<Error>)> cb);
        void sendMailToGM(const std::string& token, const std::string& replyTo, const std::string& message,
                          boost::function<void (std::auto_ptr<Mail>, std::auto_ptr<Error>)> cb);
        void openPackage(const std::string& token, const std::string& mailID,
                         boost::function<void (std::auto_ptr<Player>, std::auto_ptr<Error>)> cb);
        
    public: // realtime messaging service for message broadcasting and PvP
        bool messageServiceRunning();
        void connectMessageService(const std::string& userToken);
        void disconnect();
        
        void handlerMessagesForServiceDomain(const std::string& domain, boost::function<void (const std::string&)>);
        void removeAllHandlersForDomain(const std::string& domain);
        void sendMessage(const std::string& domain, const std::string& message);
        
        boost::shared_ptr<handle> handlerForServiceDomain(const std::string& domain);
        boost::shared_ptr<PvPMessageHandler> getPvP()
        {
            return pvpHandler;
        }
        
    public: //stats
        void shareMsgSucceed(const std::string& token,
                             boost::function<void (std::auto_ptr<Error>)> cb);
        
    protected:
        typedef boost::function<void (const rapidjson::Document&, std::auto_ptr<Error>)> RPCCallback;
        
        template <class ResultType>
        void call(const std::string& method,
                  const rapidjson::Document& parameters,
                  boost::function<void (std::auto_ptr<ResultType>, std::auto_ptr<Error>)> cb);
        void call(const std::string& method, const std::string& httpMethod, const rapidjson::Document&, RPCCallback callback);
        void call(const std::string& url, const std::string& httpMethod, const std::string& data, RPCCallback callback);
        void call(const std::string& url, const std::string& httpMethod, const std::string& data, const std::vector<std::string>& customHeaders, RPCCallback callback);
        
        void handleResponse(CCHttpResponse* resp, RPCCallback callback);
        
    private:
        boost::thread wsThread;
        boost::shared_ptr<MessageChannel> channel;
        boost::shared_ptr<PvPMessageHandler> pvpHandler;
        boost::shared_ptr<GameEventMessageHandler> eventsHandler;
    };
        
#pragma GCC diagnostic pop
}

#endif /* defined(__HelloCpp__PuzzleRPC__) */
