//
//  Weixin_android.h
//  HelloCpp
//
//  Created by R.C on 13-3-6.
//
//

#ifndef __HelloCpp__Weixin_android__
#define __HelloCpp__Weixin_android__

#include "extensions/auth/Weixin.h"
#include "platform/android/jni/JniHelper.h"
#include <string>
#include <j2cpp/j2cpp.hpp>
#include <java/lang/String.hpp>
#include <com/tencent/mm/sdk/openapi/IWXAPI.hpp>
#include <com/tencent/mm/sdk/openapi/BaseReq.hpp>
#include <com/tencent/mm/sdk/openapi/BaseResp.hpp>
#include <boost/make_shared.hpp>

namespace PH
{
    class WXReqAndroid : public virtual WXBaseReq
    {
    public:
        
        
        j2cpp::global_ref<j2cpp::com::tencent::mm::sdk::openapi::BaseReq> req;
    };
    
    class WXRespAndroid : public virtual WXBaseResp
    {
    public:
        int errorCode() { return resp->errCode; }
        std::string errorString()
        {
            j2cpp::local_ref<j2cpp::java::lang::String> jErrString = resp->errStr;
            return cocos2d::JniHelper::jstring2string((jstring)jErrString->get_jobject());
        }
        

        j2cpp::global_ref<j2cpp::com::tencent::mm::sdk::openapi::BaseResp> resp;
    };
    
    class WXTextMessageRequestAndroid : public WXReqAndroid, public WXTextMessageRequest
    {
    public:
        WXTextMessageRequestAndroid();
        virtual ~WXTextMessageRequestAndroid(){}
        
        virtual void text(const std::string&);
        virtual std::string text();
        
        virtual void sendToTimeline(bool);
    };

    class WXMediaMessageRequestAndroid : public virtual WXReqAndroid, public WXMediaMessageRequest
    {
    public:
        //WXMediaReqAndroid(SendMessageToWXReq* req){}
        
        virtual void text(const std::string&) {}
        virtual std::string text() { return ""; }
        
        virtual void description(const std::string&) {}
        virtual std::string description() { return ""; }
        
        virtual void thumbData(const std::vector<unsigned char>&) {}
        
        virtual void image(const std::vector<unsigned char>&) {}
        virtual void image(const std::string&) {}
        
        virtual void webpageURL(const std::string&) {}
        
        virtual void sendToTimeline(bool) {}
    };
    
    class WeixinAndroid : public WeixinApi
    {
        friend class WeixinApi;
    public:
        
        void setRequestHandler(ReqHandler);
        void setResponseHandler(RespHandler);
        
        void onReq(boost::shared_ptr<WXBaseReq>);
        void onResp(boost::shared_ptr<WXBaseResp>);
        
        bool registerApp(const std::string& appId);
        
        bool isWXSuuportAPI();
        
        bool sendRequest(boost::shared_ptr<WXBaseReq>);
        bool sendResponse(boost::shared_ptr<WXBaseResp>);
        
    protected:
        WeixinAndroid();
        
    private:
        ReqHandler reqHandler;
        RespHandler respHandler;
        
        j2cpp::global_ref<j2cpp::com::tencent::mm::sdk::openapi::IWXAPI> wx;
    };
}

#endif /* defined(__HelloCpp__Weixin_android__) */
