//
//  PvPMessageHandler.cpp
//  HelloCpp
//
//  Created by R.C on 13-2-25.
//
//

#include "PvPMessageHandler.h"
#include <rapidjson/rapidjson.h>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include "extensions/utils.h"
#include "URLHelpers.h"
#include "BattleScene.h"
#include "GameScene.h"
#include "network/PuzzleRPC.h"
#include "JsonParser.h"
#include "PVPBoard.h"

using namespace boost;
using namespace rapidjson;
using namespace PH::URLHelper;

namespace PH
{
    PvPMessageHandler::PvPMessageHandler(const std::string& _domain,
                                         const boost::shared_ptr<MessageChannel>& c)
    : handle(_domain, c), seqId(0), rivalSeqId(0), inGame(NULL), waitMatch(NULL), findMatch(NULL), mIsConnected(false)
    {
    }

    void PvPMessageHandler::on_connect()
    {
        LOGD("PvP: connect\n");
        mIsConnected = true;
        if(inGame)
            inGame->onConnect();
    }

    void PvPMessageHandler::on_message(const std::string & msg)
    {
        //LOGD("PvP: %s\n", msg.c_str());
        Document msgJson;
        msgJson.Parse<0>(msg.c_str());
        
        if (!msgJson.HasParseError() && msgJson["event"].IsInt())
        {
            onMessage((GameEvents)msgJson["event"].GetInt(), msgJson);
        }
    }

    void PvPMessageHandler::on_disconnect()
    {
        LOGD("PvP: disconnect\n");
        mIsConnected = false;
        if(inGame)
            inGame->onDisconnect();
    }
    
    std::string requestToStr(GameRequests r)
    {
        switch(r)
        {
            case GameRequests::Challenge:           return "R.Challenge";
            case GameRequests::FindMatch:           return "R.FindMatch";
            case GameRequests::AcceptChallenge:     return "R.AcceptChallenge";
            case GameRequests::RejectChallenge:     return "R.RejectChallenge";
            case GameRequests::CancelGameRequest:   return "R.CancelGameRequest";
            case GameRequests::BoardMove:           return "R.BoardMove";
            case GameRequests::CastSkill:           return "R.CastSkill";
            case GameRequests::RivalShallTimeout:   return "R.RivalShallTimeout";
            case GameRequests::EndRound:            return "R.EndRound";
            case GameRequests::AckEndRound:         return "R.AckEndRound";
            case GameRequests::QuitMatch:           return "R.QuitMatch";
            case GameRequests::Reset:               return "R.Reset";
            case GameRequests::PrintState:          return "R.PrintState";
            case GameRequests::Pong:                return "R.Pong";
            default:                                return std::string("R.UnknownRequest: ") + boost::lexical_cast<std::string>((int)r);
        }
    }
    
    std::string eventToStr(GameEvents e)
    {
        switch(e)
        {
            case GameEvents::Challenge:              return "E.Challenge";
            case GameEvents::RandomMatchFound:       return "E.RandomMatchFound";
            case GameEvents::WaitRandomMatch:        return "E.WaitRandomMatch";
            case GameEvents::RivalTimeoutOrRejected: return "E.RivalTimeoutOrRejected";
            case GameEvents::SearchCancelled:        return "E.SearchCancelled";
            case GameEvents::StartGame:              return "E.StartGame";
            case GameEvents::RivalBoardMove:         return "E.RivalBoardMove";
            case GameEvents::RivalCastSkill:         return "E.RivalCastSkill";
            case GameEvents::RivalEndRound:          return "E.RivalEndRound";
            case GameEvents::AckEndRound:            return "E.AckEndRound";
            case GameEvents::RivalDisconnect:        return "E.RivalDisconnect";
            case GameEvents::RivalReconnect:         return "E.RivalReconnect";
            case GameEvents::Recover:                return "E.Recover";
            case GameEvents::RivalQuit:              return "E.RivalQuit";
            case GameEvents::TimeOut:                return "E.Timeout";
            case GameEvents::GameEnd:                return "E.GameEnd";
            case GameEvents::Error:                  return "E.Error";
            case GameEvents::Info:                   return "E.Info";
            case GameEvents::CleanStart:             return "E.CleanStart";
            case GameEvents::LevelTooLow:            return "E.LevelTooLow";
            case GameEvents::Ping:                   return "E.Ping";
            default:                                 return std::string("E.UnknownEvent: ") + boost::lexical_cast<std::string>((int)e);
        }
    }
    
    static std::string secTimeNow()
    {
        using namespace boost::posix_time;
        static std::locale loc(std::cout.getloc(), new time_facet("%M%S.%f"));
        
        std::basic_stringstream<char> wss;
        wss.imbue(loc);
        wss << boost::posix_time::microsec_clock::local_time();
        return wss.str();
    }
    
    void PvPMessageHandler::onMessage(GameEvents event,
                                      const Value& eventParams)
    {
        LOGD("<-- %s [%s]\n", eventToStr(event).c_str(), secTimeNow().c_str());
        
        // try waiting jobs
        // This will block ACK END ROUND
        if(processWaitJobs(event, eventParams))
        {
            LOGD("processed wait job.\n");
        }
        
        switch(event)
        {
            case GameEvents::Challenge:
            {
                Player other = PH::parseV<Player>(eventParams["rival"]);
                std::string otherUID = other.uid;
                waitMatch->onChallenge(other);
                
                break;
            }
            case GameEvents::StartGame:
            {
                seqId = eventParams["seqID_local"].GetInt();
                rivalSeqId = eventParams["seqID_remote"].GetInt();
                
                const Value& users = eventParams["users"];
                
                Player first = parseV<Player>(users[0u]);
                Player second = parseV<Player>(users[1u]);
                int seed = eventParams["seed"].GetInt();
                int round = eventParams["round"].GetInt();
                
                PvPConfig config;
                config.moveTimeout = eventParams["config"]["moveTimeout"].GetInt() + 1;
                config.maxRound = eventParams["config"]["drawRounds"].GetInt();
                config.healthBase = (float)eventParams["config"]["healthBase"].GetDouble();
                config.healthFactor = (float)eventParams["config"]["healthFactor"].GetDouble();
                
                LOGD("config %d %d, %f %f\n", config.moveTimeout, config.maxRound, config.healthBase, config.healthFactor);
                
                waitMatch->onStartGame(first, second, seed, config);

                break;
            }
            case GameEvents::RandomMatchFound:
            {
                Player other = PH::parseV<Player>(eventParams["rival"]);
                std::string otherUID = other.uid;
                waitMatch->onRandomMatchFound(other);
                
                break;
            }
            case GameEvents::Recover:
            {
                // if not in game, this is not in game recover, just return
                if(!inGame)
                {
                    // recover seqId first.
                    seqId = eventParams["status"]["seqID_local"].GetInt();
                    PuzzleRPC::getInstance()->getPvP()->sendQuitMatch();
                    break;
                }
                
                LOGD("recover content: %s\n", write(eventParams).c_str());
                
                seqId = eventParams["status"]["seqID_local"].GetInt();

                // resend messages > local seqId
                for(std::pair<int, std::string>& x : mRoundMessageQueue)
                {
                    if(x.first > seqId)
                        this->send(x.second);
                }
                
                // exec messages > rivalSeqId
                {
                    const Value& events = eventParams["missedMessages"];
                    for(SizeType i = 0; i < events.Size(); i++)
                    {
                        GameEvents e = (GameEvents)events[i]["req"].GetInt();
                        int remoteSeqId = events[i]["seqID"].GetInt();
                        
                        if(remoteSeqId > rivalSeqId)
                            onMessage(e, events[i]);
                    }
                }
                
                break;
                
                // crash recover stuff.
                int seed = eventParams["status"]["seed"].GetInt();
                int round = eventParams["status"]["round"].GetInt();
                
                const Value& users = eventParams["status"]["users"];
                Player first = parseV<Player>(users[0u]);
                Player second = parseV<Player>(users[1u]);
                
                const Value& battleState = eventParams["status"]["board"];
                
                float skillTimeout = eventParams["status"]["config"]["skillTimeout"].GetDouble();
                float moveTimeout = eventParams["status"]["config"]["moveTimeout"].GetDouble();
                
                float skillTimeleft = eventParams["timerLeft"]["skill"].GetDouble();
                float moveTimeleft = eventParams["timerLeft"]["move"].GetDouble();
                
                const Value& events = eventParams["missedMessages"];
                
                
                waitMatch->onRecover(seqId,
                                seed,
                                round,
                                first,
                                second,
                                battleState,
                                skillTimeout,
                                moveTimeout,
                                skillTimeleft,
                                moveTimeleft,
                                events);
                break;
            }
            // InGame Events
            case GameEvents::RivalEndRound:
            {
                int rivalSID = eventParams["seqID"].GetInt();
                if(rivalSID < rivalSeqId)
                    break;
                rivalSeqId = rivalSID;
                
                std::string stateHash = eventParams["stateHash"].GetString();
                PvPEndGame victory = PvPEndGame::NONE;
                if(eventParams.HasMember("victory"))
                {
                    bool isVictory = eventParams["victory"].GetBool();
                    if(isVictory)
                        victory = PvPEndGame::WIN;
                    else
                        victory = PvPEndGame::LOSS;
                }
                if(inGame)
                    inGame->onRivalEndRound(stateHash, victory);
                break;
            }
            case GameEvents::RivalBoardMove:
            {
                LOGD("message %s\n", write(eventParams).c_str());
                int rivalSID = eventParams["seqID"].GetInt();
                if(rivalSID < rivalSeqId)
                    break;
                rivalSeqId = rivalSID;
                
                const Value& move = eventParams["sequence"][0u];
                int srcx = move["srcx"].GetInt();
                int srcy = move["srcy"].GetInt();
                int dstx = move["dstx"].GetInt();
                int dsty = move["dsty"].GetInt();
                
                if(inGame)
                    inGame->onRivalBoardMove(srcx, srcy, dstx, dsty);
                
                break;
            }
            case GameEvents::RivalCastSkill:
            {
                int rivalSID = eventParams["seqID"].GetInt();
                if(rivalSID < rivalSeqId)
                    break;
                rivalSeqId = rivalSID;
                
                int heroSlotId = eventParams["hero"].GetInt();
                if(inGame)
                inGame->onRivalCastSkill(heroSlotId);
                
                break;
            }
            case GameEvents::TimeOut:
            {
                int rivalSID = eventParams["seqID"].GetInt();
                if(rivalSID < rivalSeqId)
                    break;
                rivalSeqId = rivalSID;
                if(inGame)
                    inGame->onTimeOut();
                break;
            }
            case GameEvents::GameEnd:
            {
                int result = eventParams["gameResult"].GetInt();
                int reputationGain = eventParams["reputationGain"].GetInt();
                int scoreGain = eventParams["scoreGain"].GetInt();
                if(inGame)
                    inGame->onGameEnd((PvPEndGame)result, reputationGain, scoreGain);
                break;
            }
            case GameEvents::CleanStart:
            {
                if(inGame)
                    inGame->onCleanStart();
                break;
            }
            case GameEvents::AckEndRound:
            {
                int rivalSID = eventParams["seqID"].GetInt();
                if(rivalSID < rivalSeqId)
                    break;
                rivalSeqId = rivalSID;
                if(inGame)
                    inGame->onAckEndRound();
                
                // clear message queue on ack end round
                PuzzleRPC::getInstance()->getPvP()->clearMessageQueue();
                
                break;
            }
            case GameEvents::RivalDisconnect:
            {
                int timeout = eventParams["timeout"].GetInt();
                if(inGame)
                    inGame->onRivalDisconnect(timeout);
                break;
            }
            case GameEvents::RivalReconnect:
            {
                if(inGame)
                    inGame->onRivalReconnect();
                break;
            }
            case GameEvents::Error:
            {
                std::string error = eventParams["error"].GetString();
                LOGD("    error: %s\n", error.c_str());
                if(inGame)
                    inGame->onError(error);
                break;
            }
            case GameEvents::RivalTimeoutOrRejected:
            {
                if(findMatch)
                    findMatch->onRivalTimeoutOrRejected();
                break;
            }
            case GameEvents::SearchCancelled:
            {
                if(findMatch)
                    findMatch->onSearchCancelled();
                break;
            }
            case GameEvents::Ping:
            {
                Document params;
                params.SetObject();
                
                sendMessage(GameRequests::Pong, params);
                break;
            }
            default:
            {
                //phassert(false && "request not handled");
                LOGD("Request not handled %d\n", event);
                break;
            }
        }
    }
    
    bool PvPMessageHandler::processWaitJobs(GameEvents event,
                                            const rapidjson::Value& eventParams)
    {
        bool handled = false;
        for(auto x = waitJobList.begin(); x != waitJobList.end(); )
        {
            if(x->first == event)
            {
                x->second(event, eventParams);
                x = waitJobList.erase(x);
                handled = true;
                break;
            }
            else
                x++;
        }
        
        return handled;
    }
    
    void PvPMessageHandler::waitOnEvent(GameEvents e, EventFunc func)
    {
        waitJobList.push_back(std::pair<GameEvents, EventFunc>(e, func));
    }
    
    void PvPMessageHandler::clearWaitJobs()
    {
        waitJobList.clear();
    }
    
    //-------------------------------
    // Send requests
    //-------------------------------
    
    void PvPMessageHandler::sendMessage(GameRequests c, int sid, Document& commandParams)
    {
        LOGD(" --> %s %d [%s]\n", requestToStr(c).c_str(), sid, secTimeNow().c_str());
        commandParams.AddMember("req", (int)c, commandParams.GetAllocator());
        
        std::string raw = write(commandParams);
        
        // By force a really high seq id in the local queue, we always resend RivalShallTimeout.
        // This is to work around a server limitation.
        if(c == GameRequests::RivalShallTimeout)
            sid = INT_MAX;
        
        // record everything with a seq id
        mRoundMessageQueue.push_back(std::pair<int, std::string>(sid, raw));
        
        this->send(raw);
    }
    
    void PvPMessageHandler::sendMessage(GameRequests c,
                                        Document& commandParams)
    {
        LOGD(" --> %s [%s]\n", requestToStr(c).c_str(), secTimeNow().c_str());

        commandParams.AddMember("req", (int)c, commandParams.GetAllocator());
        this->send(write(commandParams));
    }
    
    void PvPMessageHandler::sendChallenge(const std::string& rivalUID)
    {
        Document params;
        params.SetObject();
        
        params.AddMember("rival", rivalUID, params.GetAllocator());
        
        sendMessage(GameRequests::Challenge, params);
    }
    
    void PvPMessageHandler::sendFindMatch()
    {
        Document params;
        params.SetObject();
        
        sendMessage(GameRequests::FindMatch, params);
    }
    
    void PvPMessageHandler::sendCancelGameRequest()
    {
        Document params;
        params.SetObject();
        
        sendMessage(GameRequests::CancelGameRequest, params);
    }
    
    void PvPMessageHandler::sendAcceptChallenge(const std::string& rivalUID)
    {
        Document params;
        params.SetObject();
        
        params.AddMember("rival", rivalUID, params.GetAllocator());
        
        sendMessage(GameRequests::AcceptChallenge, params);
    }

    void PvPMessageHandler::sendRejectChallenge(const std::string& rivalUID)
    {
        Document params;
        params.SetObject();
        
        params.AddMember("rival", rivalUID, params.GetAllocator());
        
        sendMessage(GameRequests::RejectChallenge, params);
    }
    
    void PvPMessageHandler::sendReset()
    {
        Document params;
        params.SetObject();
        
        params.AddMember("seqID", ++seqId, params.GetAllocator());
        
        LOGD(" prep Reset %d\n", seqId);
        sendMessage(GameRequests::Reset, seqId, params);
    }
    
    void PvPMessageHandler::sendMove(const Vec2i& src, const Vec2i& dst)
    {
        Document params;
        params.SetObject();
        
        Value seq(kArrayType);
        
        {
            Value move;
            move.SetObject();
            move.AddMember("srcx", src[0], params.GetAllocator());
            move.AddMember("srcy", src[1], params.GetAllocator());
            move.AddMember("dstx", dst[0], params.GetAllocator());
            move.AddMember("dsty", dst[1], params.GetAllocator());
            
            seq.PushBack(move, params.GetAllocator());
        }
        params.AddMember("sequence", seq, params.GetAllocator());
        
        int ss = ++seqId;
        params.AddMember("seqID", ss, params.GetAllocator());
        
        LOGD(" prep BoardMove %d: (%d, %d) (%d, %d)\n", seqId, src[0], src[1], dst[0], dst[1]);
        sendMessage(GameRequests::BoardMove, seqId, params);
    }
    
    void PvPMessageHandler::sendCastSkill(int heroSlotId)
    {
        Document params;
        params.SetObject();
        
        params.AddMember("seqID", ++seqId, params.GetAllocator());
        params.AddMember("hero", heroSlotId, params.GetAllocator());

        LOGD(" prep CastSkill %d: hero slot id %d\n", seqId, heroSlotId);
        sendMessage(GameRequests::CastSkill, seqId, params);
    }
    
    void PvPMessageHandler::sendEndRound(PVPBoardLayer* board, PvPEndGame victory)
    {
        Document params;
        params.SetObject();
        
        // assume params already has an entry called "battleState"
        params.AddMember("seqID", ++seqId, params.GetAllocator());
        
        Value battleState(kObjectType);
        board->toJson(battleState, params.GetAllocator());

        std::string stateHash = sha1FromJson(battleState);
        params.AddMember("stateHash",  stateHash, params.GetAllocator());
        
        if(victory == PvPEndGame::LOSS)
            params.AddMember("victory", false, params.GetAllocator());
        else if(victory == PvPEndGame::WIN)
            params.AddMember("victory", true, params.GetAllocator());
        else {} // victory value is optional
        
        std::string state = URLHelper::write(battleState);
        params.AddMember("battleState", battleState, params.GetAllocator());
//        params.AddMember("battleState", "", params.GetAllocator());
        LOGD(" prep EndRound %d, victory %d\nbattle state: \n %s\n", seqId, victory, state.c_str());
//        LOGD("     data: %s\n", URLHelper::write(params).c_str());
        sendMessage(GameRequests::EndRound, seqId, params);
    }
    
    void PvPMessageHandler::sendAckEndRound(PVPBoardLayer* board, PvPEndGame victory)
    {
        Document params;
        params.SetObject();

        params.AddMember("seqID", ++seqId, params.GetAllocator());
        
        
        Value battleState(kObjectType);
        board->toJson(battleState, params.GetAllocator());
        params.AddMember("battleState", battleState, params.GetAllocator());
//        params.AddMember("battleState", "", params.GetAllocator());
        
        if(victory == PvPEndGame::WIN)
            params.AddMember("victory", true, params.GetAllocator());
        else if(victory == PvPEndGame::LOSS)
            params.AddMember("victory", false, params.GetAllocator());
        else {} // victory value is optional
        
        LOGD(" prep AckEndRound %d\n", seqId);
        //LOGD("     data: %s\n", URLHelper::write(params).c_str());
        
        sendMessage(GameRequests::AckEndRound, seqId, params);
    }
    
    void PvPMessageHandler::sendQuitMatch()
    {
        Document params;
        params.SetObject();
        
        params.AddMember("seqID", ++seqId, params.GetAllocator());
        LOGD(" prep QuitMatch %d\n", seqId);
        sendMessage(GameRequests::QuitMatch, seqId, params);
    }
    
    void PvPMessageHandler::sendRivalShallTimeout()
    {
        Document params;
        params.SetObject();
        
        params.AddMember("seqID", ++seqId, params.GetAllocator());
        
        sendMessage(GameRequests::RivalShallTimeout, seqId, params);
    }
}
