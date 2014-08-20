//
//  Weixin.h
//  HelloCpp
//
//  Created by R.C on 13-3-4.
//
//

#ifndef __HelloCpp__Weixin__
#define __HelloCpp__Weixin__

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace PH
{
    class WXBaseReq
    {
    public:
        virtual ~WXBaseReq(){}
        
    protected:
        WXBaseReq(){}
    };
    
    class WXBaseResp
    {
    public:
        virtual ~WXBaseResp(){}
        
        virtual int errorCode() = 0;
        virtual std::string errorString() = 0;
        
    protected:
        WXBaseResp(){}
    };
    
    class WXTextMessageRequest : public virtual WXBaseReq
    {
    public:
        static boost::shared_ptr<WXTextMessageRequest> create();
        
        virtual void text(const std::string&) = 0;
        virtual std::string text() = 0;
        
        virtual void sendToTimeline(bool) = 0;
    };
    
    class WXMediaMessageRequest : public virtual WXBaseReq
    {
    public:
        static boost::shared_ptr<WXMediaMessageRequest> create();
        
    public:
        virtual void text(const std::string&) = 0;
        virtual std::string text() = 0;
        
        virtual void description(const std::string&) = 0;
        virtual std::string description() = 0;

        virtual void thumbData(const std::vector<unsigned char>&) = 0;
        
        virtual void image(const std::vector<unsigned char>&) = 0;
        virtual void image(const std::string&) = 0;
        
        virtual void webpageURL(const std::string&) = 0;
        
        virtual void sendToTimeline(bool) = 0;
    };
    
    class WXMediaMessageResponse : public virtual WXBaseResp
    {
    };
    
    // Weixin Api singleton
    
    class WeixinApi
    {
    public:
        typedef boost::shared_ptr<WXBaseReq> ReqPtr;
        typedef boost::shared_ptr<WXBaseResp> RespPtr;
        
        typedef boost::function<void (ReqPtr)> ReqHandler;
        typedef boost::function<void (RespPtr)> RespHandler;
        
    public: //singleton
        static WeixinApi& instance();
        virtual ~WeixinApi(){}

    protected:
        WeixinApi(){};
        WeixinApi(const WeixinApi&);
        
    public: //api
        virtual void setRequestHandler(ReqHandler) = 0;
        virtual void setResponseHandler(RespHandler) = 0;
        
        virtual void onResp(RespPtr) = 0;
        virtual void onReq(ReqPtr) = 0;
        
        virtual bool registerApp(const std::string& appId) = 0;

        virtual bool isWXSuuportAPI() { return false; }
        
        virtual bool sendRequest(ReqPtr) = 0;
        virtual bool sendResponse(RespPtr) = 0;
    };
}

#endif /* defined(__HelloCpp__Weixin__) */
