//
//  Weixin_ios.h
//  HelloCpp
//
//  Created by R.C on 13-3-4.
//
//

#import <Foundation/Foundation.h>
#import "Weixin.h"
#import "WXApi.h"
#import <string>

namespace PH
{
    class WXReqWrapper : public WXBaseReq
    {
    public:
        WXReqWrapper(BaseReq* req) { this->req = req; [req retain]; }
        
        virtual ~WXReqWrapper()
        {
            [req release];
        }

        BaseReq* req;
    };
    
    class WXRespWrapper : public virtual WXBaseResp
    {
    public:
        WXRespWrapper(BaseResp* resp) { this->resp = resp; [resp retain]; }
        
        virtual ~WXRespWrapper()
        {
            [resp release];
        }
        
        int errorCode() { return resp.errCode; }
        std::string errorString() { return resp.errStr ? resp.errStr.UTF8String : "" ; }

        BaseResp* resp;
    };
    
    class WXTextReqIOS : public WXReqWrapper, public WXTextMessageRequest
    {
    public:
        WXTextReqIOS(SendMessageToWXReq* req);
        
        virtual void text(const std::string&);
        virtual std::string text();
        
        virtual void sendToTimeline(bool);
        
        SendMessageToWXReq* request() const
        {
            return (SendMessageToWXReq*)this->req;
        }
    };
    
    class WXMediaReqIOS : public WXReqWrapper, public WXMediaMessageRequest
    {
    public:
        WXMediaReqIOS(SendMessageToWXReq* req);
        
        virtual void text(const std::string&);
        virtual std::string text();
        
        virtual void description(const std::string&);
        virtual std::string description();
        
        virtual void thumbData(const std::vector<unsigned char>&);
        
        virtual void image(const std::vector<unsigned char>&);
        virtual void image(const std::string&);
        
        virtual void webpageURL(const std::string&);
        
        virtual void sendToTimeline(bool);
        
        SendMessageToWXReq* request() const
        {
            return (SendMessageToWXReq*)this->req;
        }
    };
    
    class WXMediaRespIOS : public WXRespWrapper, public WXMediaMessageResponse
    {
    public:
        WXMediaRespIOS(SendMessageToWXResp* resp) : WXRespWrapper(resp) {}
    };
    
    class WeixinIOS : public WeixinApi
    {
        friend class WeixinApi;
    public:
        ~WeixinIOS();
        
        void setRequestHandler(ReqHandler);
        void setResponseHandler(RespHandler);
        
        void onReq(boost::shared_ptr<WXBaseReq>);
        void onResp(boost::shared_ptr<WXBaseResp>);
        
        bool registerApp(const std::string& appId);

        bool isWXSuuportAPI();
        
        bool sendRequest(boost::shared_ptr<WXBaseReq>);
        bool sendResponse(boost::shared_ptr<WXBaseResp>);
        
    protected:
        WeixinIOS(){}
        
    private:
        ReqHandler reqHandler;
        RespHandler respHandler;
    };
}
