//
//  EventsMessageHandler.cpp
//  HelloCpp
//
//  Created by Zhen Wu on 13-7-28.
//
//

#include "MessageChannel.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "extensions/utils.h"

using namespace boost;
using namespace rapidjson;

namespace PH
{
    void GameEventMessageHandler::on_connect()
    {
        
    }
    
    void GameEventMessageHandler::on_message(const std::string& msg)
    {
        Document msgJson;
        msgJson.ParseInsitu<0>((char*)msg.c_str());
        std::map<std::string, std::string> params;
        
        auto& eventField = msgJson["event"];
        std::string event(eventField.GetString(), eventField.GetStringLength());
        
        LOGD("event %s received\n", event.c_str());
        auto& paramField = msgJson["param"];
        if (paramField.IsObject())
        {
            for (auto kv=paramField.MemberBegin();kv!=paramField.MemberEnd();++kv)
            {
                std::string key(kv->name.GetString(), kv->name.GetStringLength());
                std::string value(kv->value.GetString(), kv->value.GetStringLength());
                
                params[key] = value;
            }
        }
        
        InProcEventCentral::signal(event, params);
    }
    
    void GameEventMessageHandler::on_disconnect()
    {
        
    }
};
