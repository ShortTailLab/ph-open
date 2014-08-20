//
//  MessageChannel.cpp
//  HelloCpp
//
//  Created by R.C on 12-12-9.
//
//

#define BOOST_DATE_TIME_NO_LIB

#include "MessageChannel.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "extensions/utils.h"
#include "BuildConf.h"

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::posix_time;
using namespace websocketpp;

#define MessageChannelDebug 0

static void logError(const char* evName, const char* errorMessage)
{
#if MessageChannelDebug
    LOGD("error: %s", errorMessage);
#endif
}

static void logError(const char* evName, const boost::system::error_code& e)
{
    logError(evName, e.message().c_str());
}

static void logError(const char* evName, const boost::system::system_error& e)
{
    logError(evName, e.what());
}

namespace PH{

void simpleMessageHandler::addCallback(boost::function<void (const std::string&)> f)
{
    callbacks.push_back(f);
}

void simpleMessageHandler::on_message(const std::string & msg)
{
    for (auto& c : callbacks)
        c(msg);
}
    
void simpleMessageHandler::clearCallback()
{
    callbacks.clear();
}
    
void handle::send(const std::string& msg)
{
    try
    {
        channel.lock()->send( domain + "|" + msg );
    }
    catch (const std::exception& e)
    {
        CCLOG("ws send error: %s", e.what());
    }
}

void SystemMessageHandler::on_connect()
{
    send("{\"cmd\":\"login\",\"token\":\"" + token + "\"}");
}

void SystemMessageHandler::on_message(const std::string & msg)
{
    CCLOG("System: %s", msg.c_str());
}

void SystemMessageHandler::on_disconnect()
{
}

MessageChannel::MessageChannel()
{
    reconnectTime = 0.3f;
}

void MessageChannel::connect(const std::string& serverUri)
{
    endpoint = boost::make_shared<client>();
    
    endpoint->clear_access_channels(websocketpp::log::alevel::all);
    endpoint->clear_error_channels(websocketpp::log::elevel::all);
    
    endpoint->init_asio();
    endpoint->set_message_handler(boost::bind(&MessageChannel::on_message, shared_from_this(), _1, _2));
    endpoint->set_open_handler(boost::bind(&MessageChannel::on_open, shared_from_this(), _1));
    endpoint->set_close_handler(boost::bind(&MessageChannel::on_close, shared_from_this(), _1));
    endpoint->set_fail_handler(boost::bind(&MessageChannel::on_fail, shared_from_this(), _1));
    endpoint->set_pong_handler(boost::bind(&MessageChannel::on_pong, shared_from_this(), _1, _2));
    endpoint->set_pong_timeout_handler(boost::bind(&MessageChannel::on_pong_timeout, shared_from_this(), _1, _2));
    
    websocketpp::lib::error_code ec;
    conn = endpoint->get_connection(serverUri, ec);
    endpoint->connect(conn);
    
    this->serverUri = serverUri;
}

void MessageChannel::run()
{
    while (true)
    {
        try
        {
            endpoint->run();
        }
        catch (const boost::system::system_error& e)
        {
            logError("MessageChannel.run.error", e);
        }
        catch (...)
        {
            logError("MessageChannel.run.error", "unknown exception caught");
        }
    }
}
    
void MessageChannel::send(const std::string& msg)
{
    if (conn->get_state() != session::state::open)
    {
        boost::recursive_mutex::scoped_lock lock(handlerMutex);
        messageQueue.push_back(msg);
    }
    else
        conn->send(msg);
}
    
void MessageChannel::multiplex(const std::string& domain, const boost::shared_ptr<handle>& handler)
{
    boost::recursive_mutex::scoped_lock lock(handlerMutex);
    
    if (handlers.count(domain))
        LOGD("MessageChannel:\tduplicate handler for the same domain: %s, discarding old one\n", domain.c_str());
    handlers[domain] = handler;
}

boost::shared_ptr<handle> MessageChannel::handlerForDomain(const std::string& domain)
{
    boost::recursive_mutex::scoped_lock lock(handlerMutex);
    return handlers.count(domain) ? handlers[domain] : boost::shared_ptr<handle>();
}

void MessageChannel::on_fail(connection_hdl con)
{
#if MessageChannelDebug
    LOGD("MessageChannel:\ton_fail()\n");
#endif
    
    reconnect();
}

void MessageChannel::reconnect()
{
    reconnectTime = std::min(reconnectTime*1.5f, 5.0f);
    
    try
    {
        reconnectTimer = boost::make_shared<deadline_timer>(endpoint->get_io_service());
        reconnectTimer->expires_from_now(milliseconds(reconnectTime * 1000));
        reconnectTimer->async_wait([this](const boost::system::error_code& e)
        {
            if (!e)
            {
                try
                {
                    websocketpp::lib::error_code ec;
                    conn = endpoint->get_connection(serverUri, ec);
                    endpoint->connect(conn);
                }
                catch (const boost::system::system_error& e)
                {
                    logError("MessageChannel.reconnect.handler.error", e);
                    reconnect();
                }
            }
            else
            {
                logError("MessageChannel.reconnect.handler.error", e);
                reconnect();
            }
        });
    }
    catch (const boost::system::system_error& e)
    {
        logError("MessageChannel.reconnect.start.error", e);
    }
}

void MessageChannel::close()
{
    is_proper_shutdown = true;
    conn->close(close::status::normal, "");
    
    runOnMainThread([=](){
//        boost::recursive_mutex::scoped_lock lock(handlerMutex);
        for (auto& c : handlers)
            c.second->on_disconnect();
    });
}
    
void MessageChannel::ping(const lib::error_code&)
{
    if (conn->get_state() == session::state::open)
    {
        conn->ping("");
    }
}

void MessageChannel::on_pong(websocketpp::connection_hdl con, std::string)
{
}

void MessageChannel::on_pong_timeout(websocketpp::connection_hdl con, std::string)
{
}

void MessageChannel::on_open(connection_hdl con)
{
#if MessageChannelDebug
    LOGD("MessageChannel:\ton_open()\n");
#endif
    
    boost::recursive_mutex::scoped_lock lock(handlerMutex);
    if (!messageQueue.empty())
    {
        for (auto& msg : messageQueue)
            conn->send(msg);

        messageQueue.clear();
    }
    
    websocketpp::lib::error_code ec;
    conn->get_raw_socket().set_option(::boost::asio::ip::tcp::no_delay(true), ec);
    conn->get_raw_socket().set_option(::boost::asio::socket_base::keep_alive(true), ec);
    
    runOnMainThread([=](){
//        boost::recursive_mutex::scoped_lock lock(handlerMutex);
        for (auto& c : handlers)
            c.second->on_connect();
    });
    
    reconnectTime = 0.5f;
    is_proper_shutdown = false;
}

void MessageChannel::on_close(connection_hdl con)
{
#if MessageChannelDebug
    LOGD("MessageChannel:\ton_close()\n");
#endif
    
    if (is_proper_shutdown)
        return;
    
    runOnMainThread([=](){
//        boost::recursive_mutex::scoped_lock lock(handlerMutex);
        for (auto& c : handlers)
            c.second->on_disconnect();
    });

    reconnect();
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
    
void MessageChannel::on_message(connection_hdl con, message_ptr msg)
{
    const std::string& payload = msg->get_payload();
#if MessageChannelDebug
//    LOGD("MessageChannel:\ton_message(), msg:%s, [%s]\n", payload.c_str(), secTimeNow().c_str());
    LOGD("MessageChannel: [%s]\n", secTimeNow().c_str());
#endif
    
    int pos = payload.find('|');
    if (pos == string::npos)
        LOGD("MessageChannel:\tunsupported message format.\n");
    else
    {
        string domain(payload.begin(), payload.begin() + pos);
        string content(payload.begin() + pos + 1, payload.end());
        
        runOnMainThread([=](){
//            boost::recursive_mutex::scoped_lock lock(handlerMutex);
            if (handlers.count(domain))
                handlers[domain]->on_message(content);
            else
                LOGD("MessageChannel:\tno handler for message domain: %s\n", domain.c_str());
        });
    }
}

}
