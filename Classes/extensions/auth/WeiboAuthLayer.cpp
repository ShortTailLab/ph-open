//
//  WeiboAuthLayer.cpp
//  HelloCpp
//
//  Created by R.C on 12-10-30.
//
//

#include "WeiboAuthLayer.h"
#include "Weibo.h"
#include "URLHelpers.h"
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <rapidjson/rapidjson.h>

#define kSinaWeiboSDKOAuth2APIDomain       "https://open.weibo.cn/2/oauth2/"
#define kSinaWeiboWebAuthURL               "https://open.weibo.cn/2/oauth2/authorize"


#ifdef CC_PLATFORM_IOS

#define kTCWBAuthorizeURL         "https://open.t.qq.com/cgi-bin/oauth2/authorize"

#elif defined(CC_PLATFORM_ANDROID)

#define kTCWBAuthorizeURL         "https://open.t.qq.com/cgi-bin/oauth2/authorize"

#endif

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

using namespace PH;
using namespace PH::URLHelper;
using namespace std;
using namespace cocos2d;
using namespace boost::algorithm;
using namespace rapidjson;


WeiboAuthLayer* WeiboAuthLayer::create(const rapidjson::Value& params)
{
    WeiboAuthLayer* ret = new WeiboAuthLayer;
    if (ret && ret->init())
    {
        copyValueTo(params, ret->authParams);
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return NULL;
}

bool WeiboAuthLayer::init()
{
    if (!CCLayer::init())
        return false;

    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    LOGD("create web layer\n");
    webView = WebViewLayer::create(CCRect(25,
                                          25,
                                          winSize.width-50,
                                          winSize.height-50));
    webView->setScaleToFit(true);
    webView->beforeStartLoadRequest = boost::bind(&WeiboAuthLayer::webviewShouldLoadRequest, this, _1);
    webView->onClose = boost::bind(&WeiboAuthLayer::webViewClosed, this);
    webView->showCloseButton();
    this->addChild(webView);

    return true;
}

std::string WeiboAuthLayer::authURL(const std::string& base)
{
    return base + "?" + URLHelper::toQueryString(authParams);
}

void WeiboAuthLayer::auth(const std::string& service, boost::function<void (const Value& params, std::auto_ptr<AuthError>)> callback)
{
    authCB = boost::move(callback);
    
    if (service == "sina")
    {
        webView->openURL(authURL(kSinaWeiboWebAuthURL), true);
    }
    else if (service == "tencent")
    {
        webView->openURL(authURL(kTCWBAuthorizeURL), true);
    }
}

bool WeiboAuthLayer::webviewShouldLoadRequest(cocos2d::extension::CCHttpRequest& req)
{
    string appRedirectURI = authParams["redirect_uri"].GetString();
    string siteRedirectURI = kSinaWeiboSDKOAuth2APIDomain + appRedirectURI;
    
    string url(req.getUrl());
    
    if (starts_with(url, appRedirectURI) || starts_with(url, siteRedirectURI))
    {
        runOnMainThread([url, this]()
        {
            string error_code = URLHelper::getURLParameterValue(url, "error_code");
            Document result;
            result.SetObject();
            
            auto_ptr<AuthError> error;
            
            if (error_code.empty())
            {
                string code = URLHelper::getURLParameterValue(url, "code");
                string access_token = URLHelper::getURLParameterValue(url, "access_token", '#');
                if (!code.empty())
                    result.AddMember("code", code, result.GetAllocator());
                else if (!access_token.empty())
                {
                    result.AddMember("access_token", access_token, result.GetAllocator());
                    result.AddMember("openid", URLHelper::getURLParameterValue(url, "openid", '#'), result.GetAllocator());
                    result.AddMember("openkey", URLHelper::getURLParameterValue(url, "openkey", '#'), result.GetAllocator());
                    result.AddMember("expires_in", boost::lexical_cast<int>(URLHelper::getURLParameterValue(url, "expires_in", '#')), result.GetAllocator());
                }
            }
            else
            {
                auto_ptr<AuthError> err(new AuthError);
                
                err->error = URLHelper::getURLParameterValue(url, "error");
                err->errorUri = URLHelper::getURLParameterValue(url, "error_uri");
                err->errorDesc = URLHelper::getURLParameterValue(url, "error_description");
            }
            
            authCB(result, error);
            runOnMainThread([=]()
            {
                this->removeFromParent();
            });
        });

        return false;
    }
    return true;
}

void WeiboAuthLayer::webViewClosed()
{
    runOnMainThread([=]()
    {
        auto_ptr<AuthError> err(new AuthError);
        err->error = "user canceled";
        
        authCB(Value(), err);

        runOnMainThread([=]()
        {
            this->removeFromParent();
        });
    });
}
