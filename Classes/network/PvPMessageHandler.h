//
//  PvPMessageHandler.h
//  HelloCpp
//
//  Created by R.C on 13-2-25.
//
//

#ifndef __HelloCpp__PvPMessageHandler__
#define __HelloCpp__PvPMessageHandler__

#include "MessageChannel.h"
#include <gfx/vec2.h>
#include <rapidjson/document.h>
#include <boost/date_time/posix_time/ptime.hpp>

namespace PH
{
    std::string FormatSec(boost::posix_time::ptime now);
    
    enum class PvPEndGame : int
    {
        NONE = -2,
        LOSS = -1,
        TIE = 0,
        WIN = 1,
    };
        
    struct PvPConfig
    {
        int moveTimeout;
        int maxRound;
        float healthBase;
        float healthFactor;
    };

    enum class GameRequests : int
    {
        // Requests valid in Start state
        /*
            rival : str, rival UID
         */
        Challenge                       = 1000,
        
        FindMatch                       = 1001,
        
        /*
            rival : str, rival UID
         */
        AcceptChallenge                 = 1002,
        
        /*
            rival : str, rival UID
         */
        RejectChallenge                 = 1003,
        
        CancelGameRequest               = 1009,
        
        // Requests valid in InGame state
        /*
            sequence : [], list of moves
            seqID : int
         */
        BoardMove                       = 1010,

        /*
            hero : i
            seqID : int
         */
        CastSkill                       = 1011,
        
        /*
            clock the time the other opponent takes
            send this event when player thinks opponent is 
            going to time out.
         */
        RivalShallTimeout               = 1018,
        
        /*
            stateHash : str
            battleState: dict
            seqID : int
            victory: bool   [optional]
         */
        EndRound                        = 1019,
        
        /*
            stateHash : str
            battleState: dict
            seqID : int
            victory: bool   [optional]
         */
        AckEndRound                     = 1119,

        /*
            seqID : int
         */
        QuitMatch                       = 1030,

        
        // Debug Commands
        /*
            seqID: int
         */
        Reset                           = 1050,
        PrintState                      = 1999,
        
        Pong                            = 9999,
    };

    enum class GameEvents : int
    {
        /*
            rival : Player
         */
        Challenge                       = 100,
        RandomMatchFound                = 101,
        WaitRandomMatch                 = 102,
        RivalTimeoutOrRejected          = 103,
        SearchCancelled                 = 104,
        CleanStart                      = 110,
        
        /*
             seqID : int
             seed : int
             round : int
             users : [Player, Player]
         */
        StartGame                       = 200,
        RivalBoardMove                  = 201,
        RivalCastSkill                  = 202,
        RivalEndRound                   = 203,
        AckEndRound                     = 204,
        
        RivalDisconnect                 = 220,
        RivalReconnect                  = 221,
        /*
            status.seqID_local : int
            status.seqID_remote : int
            status.round : int
            status.users : [Player, Player]
            state : str
         */
        Recover                         = 230,
        
        RivalQuit                       = 370,
        
        TimeOut                         = 380,
        
        /*
            gameResult : int    -1, 0, 1
            reputationGain : int
            scoreGain : int
         */
        GameEnd                         = 399,
        
        Error                           = 300,
        Info                            = 400,
        LevelTooLow                     = 500,
        
        Ping                            = 999,
    };
    
    class Player;
    class PVPBoardLayer;
        
    class IPvPWaitMatchReceiver
    {
    public:
        // Receivers
        virtual bool onChallenge(const Player& other) = 0;
        virtual bool onRandomMatchFound(const Player& other) = 0;
        
        virtual bool onStartGame(const Player& first, const Player& second, int seed, const PvPConfig& config) = 0;
        virtual bool onRivalTimeoutOrRejected() = 0;
        
        virtual bool onRecover(int localSeqId,
                               int seed,
                               int round,
                               const Player& first,
                               const Player& second,
                               const rapidjson::Value& battleState,
                               float skillTimeout,
                               float moveTimeout,
                               float skillTimeLeft,
                               float moveTimeLeft,
                               const rapidjson::Value& events) = 0;
//        virtual bool onDisconnect() = 0;
    };
        
    class IPvPInGameReceiver
    {
    public:
        virtual bool onCleanStart() = 0;
        virtual bool onRivalBoardMove(int srcx, int srcy, int dstx, int dsty) = 0;
        virtual bool onRivalCastSkill(int heroSlotId) = 0;
        virtual bool onRivalEndRound(std::string stateHash, PvPEndGame victory) = 0;
        virtual bool onAckEndRound() = 0;
        
        virtual bool onTimeOut() = 0;
        virtual bool onRivalQuit() = 0;
        virtual bool onGameEnd(PvPEndGame victory, int reputationGain, int scoreGain) = 0;
        
        virtual bool onRivalDisconnect(int timeout) = 0;
        virtual bool onRivalReconnect() = 0;
        virtual bool onError(const std::string& error) = 0;
        
        virtual bool onConnect() = 0;
        virtual bool onDisconnect() = 0;
    };
        
    class IPvPFindMatchReceiver
    {
    public:
        virtual bool onRivalTimeoutOrRejected() = 0;
        virtual bool onSearchCancelled() = 0;
    };

    class PvPMessageHandler : public handle
    {
    public:
        PvPMessageHandler(const std::string& _domain, const boost::shared_ptr<MessageChannel>& c);
        
        void sendMessage(GameRequests, rapidjson::Document& commandParams);
        void sendMessage(GameRequests c, int sid, rapidjson::Document& commandParams);
        void onMessage(GameEvents, const rapidjson::Value& eventParams);
        
        // Senders
        void sendChallenge(const std::string& rivalUID);
        void sendFindMatch();
        void sendCancelGameRequest();
        
        void sendAcceptChallenge(const std::string& rivalUID);
        void sendRejectChallenge(const std::string& rivalUID);
        void sendReset();
        
        void sendMove(const Vec2i& src, const Vec2i& dst);
        void sendCastSkill(int heroSlotId);
        
        void sendEndRound(PVPBoardLayer* board, PvPEndGame victory = PvPEndGame::NONE);
        void sendAckEndRound(PVPBoardLayer* board, PvPEndGame victory = PvPEndGame::NONE);
        
        void sendRivalShallTimeout();
        void sendQuitMatch();

        // set receivers
        void setInGameReceiver(IPvPInGameReceiver* r) { inGame = r; }
        void setWaitMatchReceiver(IPvPWaitMatchReceiver* r) { waitMatch = r; }
        void setFindMatchReceiver(IPvPFindMatchReceiver* r) { findMatch = r; }

        // wait events
        typedef boost::function<void(GameEvents, const rapidjson::Value&)>  EventFunc;
        
        void waitOnEvent(GameEvents e, EventFunc f);
        bool processWaitJobs(GameEvents event, const rapidjson::Value& eventParams);
        void clearWaitJobs();
        
        void clearMessageQueue() { mRoundMessageQueue.clear(); }
        
        bool isConnected() const { return mIsConnected; }
        
    protected:
        virtual void on_connect();
        virtual void on_message(const std::string&);
        virtual void on_disconnect();
        
        int seqId;
        int rivalSeqId;
        bool mIsConnected;
        
        IPvPInGameReceiver* inGame;
        IPvPWaitMatchReceiver* waitMatch;
        IPvPFindMatchReceiver* findMatch;
        
        std::list<std::pair<GameEvents, EventFunc>> waitJobList;
        
        std::list<std::pair<int, std::string>> mRoundMessageQueue;
    };
}

#endif /* defined(__HelloCpp__PvPMessageHandler__) */
