//
//  WeiboAuthLayer.h
//  HelloCpp
//
//  Created by R.C on 12-10-30.
//
//

#ifndef __HelloCpp__WeiboAuthLayer__
#define __HelloCpp__WeiboAuthLayer__

#include <cocos2d.h>
#include <string>
#include <memory>
#include <boost/function.hpp>
#include <network/HttpRequest.h>
#include "WebViewLayer.h"
#include <rapidjson/document.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace PH
{
    struct AuthError;
    
    class WeiboAuthLayer : public cocos2d::CCLayer
    {
    public:
        static WeiboAuthLayer* create(const rapidjson::Value& params);
        
        bool init();
        
        void auth(const std::string& service, boost::function<void (const rapidjson::Value& params, std::auto_ptr<AuthError>)> callback);
        
    protected:
        std::string authURL(const std::string& base);
        
        bool webviewShouldLoadRequest(cocos2d::extension::CCHttpRequest&);
        void webViewClosed();
        
    private:
//        std::auto_ptr<WebViewLayer> webView;
        WebViewLayer* webView;
        rapidjson::Document authParams;
        
        boost::function<void (const rapidjson::Value& params, std::auto_ptr<AuthError>)> authCB;
    };
}

#endif /* defined(__HelloCpp__WeiboAuthLayer__) */
