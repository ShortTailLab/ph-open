//
//  Weibo.h
//  HelloCpp
//
//  Created by R.C on 12-10-29.
//
//

#ifndef __HelloCpp__Weibo__
#define __HelloCpp__Weibo__

#include <rapidjson/document.h>
#include "PuzzleRPC.h"
#include "URLHelpers.h"
#include <ctime>
#include <boost/function/function_fwd.hpp>
#include <memory>
#include <vector>
#include <platform/CCImage.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

namespace PH
{
    class WeiboAuthLayer;
    class Sina {};
    class Tencent {};
    
    struct WBError
    {
        std::string error, errorUri, errorDesc;        
    };
    
    struct AuthError : public WBError
    {

    };
    
    template <class V>
    class Weibo : protected PuzzleRPC
    {
    public:
        static Weibo& instance(const std::string& appKey=std::string(),
                               const std::string& appSecret=std::string(),
                               const std::string& callbackURI=std::string())
        {
            static Weibo* _ins = nullptr;
            if (!_ins)
                _ins = Weibo<V>::create(appKey, appSecret, callbackURI);
            
            return *_ins;
        }
        
        static Weibo* create(const std::string& appKey, const std::string& appSecret, const std::string& callbackURI)
        {
            Weibo* w = new Weibo;
            w->appKey = appKey;
            w->appSecret = appSecret;
            w->appCBURI = callbackURI;
            w->ssoLoggingIn = false;
            w->ssoTriedAndFailed = false;
            w->ssoCallback = [](std::auto_ptr<AuthError>){};
            
            w->loadConfig();
            
            w->authLayer = NULL;
            
            return w;
        }
        
        void authorize(boost::function<void (std::auto_ptr<AuthError>)> callback);
        
        void weiboRequest(const std::string& method, const rapidjson::Value& params, boost::function<void (const rapidjson::Value&)>);
        
        void clearLoginInfo();
        
        bool isLoggedIn() const
        {
            return !uid.empty() && !accessToken.empty() && expirationTime;
        }
        
        bool isAuthorized() const
        {
            return isLoggedIn() && !isAuthorizationExpired();
        }
        
        bool isAuthorizationExpired() const
        {
            return std::time(NULL) >= expirationTime;
        }
        
        std::string token() const
        {
            return accessToken;
        }
        
        std::string userId() const
        {
            return uid;
        }
        
        std::string ssoCallbackScheme() const
        {
            return ssoCBURI;
        }
        
        void setSSOCallbackScheme(const std::string& cbScheme)
        {
            ssoCBURI = cbScheme;
        }
        
    public:  // helper methods that wraps around weiboRequest
        void postTweet(const std::string& content, boost::function<void (std::auto_ptr<WBError>)>);
        void postTweetWithImage(const std::string& content, cocos2d::CCImage*, boost::function<void (std::auto_ptr<WBError>)>);
        
    public:
        bool handleURL(const std::string& url);
        
    protected:
        bool ssoAuthorize(boost::function<void (std::auto_ptr<AuthError>)> callback);
        void _postTweetWithImage(const std::string& url, const rapidjson::Value& params, const std::string& boundary,
                                 cocos2d::CCImage* img, boost::function<void (std::auto_ptr<WBError>)> cb);
        void handleSSOResult(const std::string& result, boost::function<void (std::auto_ptr<AuthError>)> callback);
        
        void loadConfig();
        void saveConfig();
        std::string configKeyPrefix();
        
    private:
        std::string appKey, appSecret, appCBURI, ssoCBURI;
        std::string accessToken, uid;
        long long expirationTime;
        WeiboAuthLayer* authLayer;
        bool ssoLoggingIn, ssoTriedAndFailed;
        boost::function<void (std::auto_ptr<AuthError>)> ssoCallback;
    };
}

#pragma GCC diagnostic pop

#endif /* defined(__HelloCpp__Weibo__) */
