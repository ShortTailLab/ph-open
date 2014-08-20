//
//  MessageChannel.h
//  HelloCpp
//
//  Created by R.C on 12-12-9.
//
//

#ifndef __HelloCpp__MessageChannel__
#define __HelloCpp__MessageChannel__

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <string>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace PH {
    class MessageChannel;
    
    class handle
    {
    public:
        handle(const std::string& _domain, const boost::shared_ptr<MessageChannel>& c) : domain(_domain), channel(c) {};
        virtual ~handle() {}
    public:
        void send(const std::string&);
        
        virtual void on_connect() = 0;
        virtual void on_message(const std::string&) = 0;
        virtual void on_disconnect() = 0;
        
    protected:
        std::string domain;
        boost::weak_ptr<MessageChannel> channel;
    };
    
    
    class SystemMessageHandler : public handle
    {
    public:
        SystemMessageHandler(const std::string& _domain, const boost::shared_ptr<MessageChannel>& c) : handle(_domain, c) {};
    public:
        virtual void on_connect();
        virtual void on_message(const std::string&);
        virtual void on_disconnect();
        
        void setUserToken(const std::string& token) { this->token = token; }
    private:
        std::string token;
    };
    
    
    class GameEventMessageHandler : public handle
    {
    public:
        GameEventMessageHandler(const std::string& _domain, const boost::shared_ptr<MessageChannel>& c) : handle(_domain, c) {};
    public:
        virtual void on_connect();
        virtual void on_message(const std::string&);
        virtual void on_disconnect();
    };
    
    
    class simpleMessageHandler : public handle
    {
    public:
        simpleMessageHandler(const std::string& _domain, const boost::shared_ptr<MessageChannel>& c) : handle(_domain, c) {};
    public:
        virtual void on_connect() {}
        virtual void on_message(const std::string&);
        virtual void on_disconnect() {}
        
        void addCallback(boost::function<void (const std::string&)> f);
        void clearCallback();
        
    protected:
        std::vector<boost::function<void (const std::string&)>> callbacks;
    };

    
    class MessageChannel : public boost::enable_shared_from_this<MessageChannel>
    {
        typedef websocketpp::client<websocketpp::config::asio_client> client;
        typedef client::connection_type::timer_ptr timer_ptr;
        typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
    public:
        MessageChannel();
        virtual ~MessageChannel() {}
        
        void connect(const std::string& serverUri);
        void run();
        
        void send(const std::string& msg);
        void ping(const websocketpp::lib::error_code&);
        
        void multiplex(const std::string& domain, const boost::shared_ptr<handle>& handler);
        boost::shared_ptr<handle> handlerForDomain(const std::string& domain);
        
        void close();
        
    protected:
        void on_fail(websocketpp::connection_hdl con);

        void on_open(websocketpp::connection_hdl con);

        void on_close(websocketpp::connection_hdl con);

        void on_message(websocketpp::connection_hdl con, message_ptr msg);
        
        void on_pong(websocketpp::connection_hdl con, std::string);
        
        void on_pong_timeout(websocketpp::connection_hdl con, std::string);
        
        void reconnect();

    private:
        client::connection_ptr conn;
        std::vector<std::string> messageQueue;
        boost::shared_ptr<client> endpoint;
        boost::recursive_mutex handlerMutex;
        std::map<std::string, boost::shared_ptr<handle>> handlers;
        
        std::string serverUri, authToken;
        float reconnectTime;
        bool is_proper_shutdown;
        boost::shared_ptr<boost::asio::deadline_timer> reconnectTimer;
    };
}

#endif /* defined(__HelloCpp__MessageChannel__) */
