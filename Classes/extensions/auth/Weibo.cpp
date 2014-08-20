//
//  Weibo.cpp
//  HelloCpp
//
//  Created by R.C on 12-10-29.
//
//

#include "Weibo.h"
#include "WeiboAuthLayer.h"
#include "URLHelpers.h"
#include "CCNative.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <platform/CCFileUtils.h>
#include <support/user_default/CCUserDefault.h>
#include <network/HttpRequest.h>
#include <network/HttpClient.h>
#include <boost/function.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/container/flat_set.hpp>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "extensions/utils.h"
#include <com/stl/puzzlehero/NativeCallback.h>
#include <com/stl/puzzlehero/CCNative.hpp>
#include "platform/android/jni/JniHelper.h"

using namespace j2cpp;
namespace stl = com::stl::puzzlehero;
#endif

#define kSinaWeiboSDKAPIDomain             "https://api.weibo.com/2"
#define kSinaWeiboSDKAuthURL               "https://api.weibo.com/oauth2/access_token"
#define kTencentWeiboSDKAPIDomain          "https://open.t.qq.com/api"

#define kSinaWeiboAppAuthURL_iPhone        "sinaweibosso://login"
#define kSinaWeiboAppAuthURL_iPad          "sinaweibohdsso://login"

#define kSinaWeiboRequestStringBoundary    "293iosfksdfkiowjksdf31jsiuwq003s02dsaffafass3qw"

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

using namespace PH::URLHelper;
using namespace std;
using namespace boost::container;
using namespace rapidjson;

namespace PH{

static const char* _sinaGetMethods[] = {
    "/users/show.json",
    "/account/end_session.json",
};
static flat_set<std::string> sinaGetMethods(_sinaGetMethods, _sinaGetMethods + sizeof(_sinaGetMethods)/sizeof(_sinaGetMethods[0]));

static const char* _tencentGetMethods[] = {
    "/user/info",
};
static flat_set<std::string> tencentGetMethods(_tencentGetMethods, _tencentGetMethods + sizeof(_tencentGetMethods)/sizeof(_tencentGetMethods[0]));


template<> std::string Weibo<Sina>::configKeyPrefix() { return "sina"; }
template<> std::string Weibo<Tencent>::configKeyPrefix() { return "tencent"; }
template<> bool PH::Weibo<Tencent>::ssoAuthorize(boost::function<void (std::auto_ptr<AuthError>)> callback);
template<> bool PH::Weibo<Sina>::ssoAuthorize(boost::function<void (std::auto_ptr<AuthError>)> callback);
    
template<typename V>
void Weibo<V>::loadConfig()
{
    std::string keyP = configKeyPrefix();
    uid = CCUserDefault::sharedUserDefault()->getStringForKey((keyP + "_uid").c_str(), "");
    accessToken = CCUserDefault::sharedUserDefault()->getStringForKey((keyP + "_token").c_str(), "");
    expirationTime = CCUserDefault::sharedUserDefault()->getIntegerForKey((keyP + "_expire").c_str(), 0);
}
    
template void Weibo<Sina>::loadConfig();
template void Weibo<Tencent>::loadConfig();

static inline
WBError* jsonErrorToWBError(const rapidjson::Value& e)
{
    if (e.HasMember("error_code"))
    {
        WBError* error = new WBError;
        return error;
    }
    else
        return NULL;
}

static inline
boost::function<void (const rapidjson::Value&)> wrapCallback(boost::function<void (std::auto_ptr<WBError>)> cb)
{
    return [=](const rapidjson::Value& r){
        cb(std::auto_ptr<WBError>(jsonErrorToWBError(r)));
    };
}
    
template<>
void Weibo<Sina>::weiboRequest(const std::string& method, const rapidjson::Value& paramsIn, boost::function<void (const rapidjson::Value&)> cb)
{
    Document params = copyValue(paramsIn);
    
    if (!accessToken.empty())
    {
        params.AddMember("access_token", accessToken, params.GetAllocator());
    }
    
    string httpMethod = sinaGetMethods.count(method) ? "GET" : "POST";
    string url;
    if (method == "/oauth2/access_token")
        url = kSinaWeiboSDKAuthURL;
    else
        url = kSinaWeiboSDKAPIDomain + method;
    auto realCB = [=](const Value& r, std::auto_ptr<Error> err){
        if (!err.get())
            cb(r);
        else
        {
            Document o;
            o.SetObject();
            
            o.AddMember("error_code", (int)err->code, o.GetAllocator());
            cb(o);
        }
    };
    
    if (httpMethod == "GET")
    {
        string p = URLHelper::toQueryString(params);
        if (!p.empty())
            url += "?" + p;
        
        call(url, httpMethod, "", realCB);
    }
    else if (httpMethod == "POST")
    {
        call(url, httpMethod, URLHelper::toQueryString(params), realCB);
    }
    else
    {
        puts("Unsupported method");
    }
}

template<>
void Weibo<Tencent>::weiboRequest(const std::string& method, const rapidjson::Value& paramsIn, boost::function<void (const rapidjson::Value&)> cb)
{
    Document params = copyValue(paramsIn);
    
    if (!accessToken.empty())
    {
        params.AddMember("oauth_consumer_key", appKey, params.GetAllocator());
        params.AddMember("access_token", accessToken, params.GetAllocator());
        params.AddMember("openid", uid, params.GetAllocator());
        params.AddMember("oauth_version", "2.a", params.GetAllocator());
        params.AddMember("scope", "all", params.GetAllocator());
        params.AddMember("clientip", cocos2d::extension::CCNative::IPAddress(), params.GetAllocator());
    }
    params.AddMember("format", "json", params.GetAllocator());
    
    string httpMethod = tencentGetMethods.count(method) ? "GET" : "POST";
    string url = kTencentWeiboSDKAPIDomain + method;
    auto realCB = [=](const Value& r, std::auto_ptr<Error> err){
        if (!err.get())
            cb(r);
        else
        {
            Document o;
            o.SetObject();
            o.AddMember("error_code", (int)err->code, o.GetAllocator());
            cb(o);
        }
    };
    
    if (httpMethod == "GET")
    {
        string p = URLHelper::toQueryString(params);
        if (!p.empty())
            url += "?" + p;
    
        call(url, httpMethod, "", realCB);
    }
    else if (httpMethod == "POST")
    {
        call(url, httpMethod, URLHelper::toQueryString(params), realCB);
    }
    else
    {
        puts("Unsupported method");
    }
}

template <>
void PH::Weibo<Sina>::authorize(boost::function<void (std::auto_ptr<AuthError>)> callback)
{
    // do not open multiple authorization webviews
    if(authLayer)
        return;
    
    if (isAuthorized())
        callback(std::auto_ptr<AuthError>());
    else if (!ssoAuthorize(callback))
    {
        Document params;
        params.SetObject();
        
        params.AddMember("client_id", appKey, params.GetAllocator());
        params.AddMember("response_type", "code", params.GetAllocator());
        params.AddMember("redirect_uri", appCBURI, params.GetAllocator());
        params.AddMember("display", "mobile", params.GetAllocator());
        params.AddMember("forcelogin", true, params.GetAllocator());
        
        authLayer = WeiboAuthLayer::create(params);
        authLayer->retain();
        runningScene()->addChild(authLayer);
        
        authLayer->auth("sina", [=](const Value& ret, std::auto_ptr<AuthError> err)
        {
            if (err.get())
            {
                callback(err);
            }
            else
            {
                Document p;
                p.SetObject();
                
                const Value& code = ret["code"];
                string codeStr(code.GetString(), code.GetStringLength());
                
                p.AddMember("client_id", this->appKey, p.GetAllocator());
                p.AddMember("client_secret", this->appSecret, p.GetAllocator());
                p.AddMember("grant_type", "authorization_code", p.GetAllocator());
                p.AddMember("redirect_uri", this->appCBURI, p.GetAllocator());
                p.AddMember("code", codeStr, p.GetAllocator());
                
                this->weiboRequest("/oauth2/access_token", p, [=](const Value& p)
                {
                    if (p.HasMember("error_code"))
                    {
                        auto_ptr<AuthError> err(new AuthError);
                        err->error = boost::lexical_cast<string>(p["error_code"].GetInt());
                        callback(err);
                    }
                    else
                    {
                        // sucessfully authorized. get auth info
                        auto_ptr<AuthError> error;
                        
                        try
                        {
                            this->accessToken = string(p["access_token"].GetString(), p["access_token"].GetStringLength());
                            this->uid = string(p["uid"].GetString(), p["uid"].GetStringLength());
                            //                        this->refresh_token = wstr2str(p[L"refresh_token"].get_str());
                            
                            long long expires_in = p["expires_in"].GetInt64();
                            this->expirationTime = time(NULL) + expires_in;
                            
                            saveConfig();
                        }
                        catch (...)
                        {
                            error.reset(new AuthError);
                            error->error = "2";
                            error->errorDesc = "Response not valid";
                        }
                        
                        callback(error);
                    }
                });
            }
            runOnMainThread([=]()
            {
                authLayer->autorelease();
                authLayer = NULL;
            });
        });
    }
}

template <>
void PH::Weibo<Tencent>::authorize(boost::function<void (std::auto_ptr<AuthError>)> callback)
{
    // do not open multiple authorization webviews
    if(authLayer)
        return;
    
    if (isAuthorized())
        callback(std::auto_ptr<AuthError>());
    else if (!ssoAuthorize(callback))
    {
        Document params;
        params.SetObject();
        
        std::string os = cocos2d::extension::CCNative::OS();
        transform(os.begin(), os.end(), os.begin(), ::tolower);
        
        params.AddMember("client_id", appKey, params.GetAllocator());
        params.AddMember("response_type", "token", params.GetAllocator());
        params.AddMember("redirect_uri", appCBURI, params.GetAllocator());
        params.AddMember("forcelogin", true, params.GetAllocator());
        
        authLayer = WeiboAuthLayer::create(params);
        authLayer->retain();
        runningScene()->addChild(authLayer);
        
        authLayer->auth("tencent", [=](const Value& p, std::auto_ptr<AuthError> err)
        {
            if (err.get())
            {
                callback(err);
            }
            else
            {
                if (p.HasMember("error_code"))
                {
                    auto_ptr<AuthError> err(new AuthError);
                    err->error = boost::lexical_cast<string>(p["error_code"].GetInt());
                    callback(err);
                }
                else
                {
                    // sucessfully authorized. get auth info
                    auto_ptr<AuthError> error;
                    
                    try
                    {
                        this->accessToken = string(p["access_token"].GetString(), p["access_token"].GetStringLength());
                        this->uid = string(p["openid"].GetString(), p["openid"].GetStringLength());
                        
                        long long expires_in = p["expires_in"].GetInt64();
                        this->expirationTime = time(NULL) + expires_in;
                        
                        saveConfig();
                    }
                    catch (...)
                    {
                        error.reset(new AuthError);
                        error->error = "2";
                        error->errorDesc = "Response not valid";
                    }
                    
                    callback(error);
                }
            }
            
            runOnMainThread([=]()
            {
                authLayer->autorelease();
                authLayer = NULL;
            });
        });
    }
}

template<>
void PH::Weibo<Sina>::postTweet(const std::string& content, boost::function<void (std::auto_ptr<WBError>)> cb)
{
    std::string method = "/statuses/update.json";
    
    Document params;
    params.SetObject();
    
    params.AddMember("status", content, params.GetAllocator());
    
    weiboRequest(method, params, wrapCallback(boost::move(cb)));
}

template<>
void PH::Weibo<Tencent>::postTweet(const std::string& content, boost::function<void (std::auto_ptr<WBError>)> cb)
{
    std::string method = "/t/add";
    
    Document params;
    params.SetObject();
    
    params.AddMember("content", content, params.GetAllocator());
    
    weiboRequest(method, params, wrapCallback(boost::move(cb)));
}

template<typename T>
void PH::Weibo<T>::_postTweetWithImage(const std::string& url, const Value& params, const std::string& boundary,
                                          cocos2d::CCImage* img, boost::function<void (std::auto_ptr<WBError>)> cb)
{
    std::string boundaryLine = "--" + boundary + "\r\n";
    
    unsigned long dataSize = 0;
    std::string snapshotPath = CCNative::getDocPath() + "snapshot.jpg";
    img->saveToFile(snapshotPath.c_str());
    unsigned char* data = cocos2d::CCFileUtils::sharedFileUtils()->getFileData(snapshotPath.c_str(), "rb", &dataSize);
    
    std::string formContent = toDispositionFormString(params, boundaryLine);
    formContent.reserve(formContent.size() + dataSize + 500);
    
    formContent += boundaryLine + "Content-Disposition: form-data; name=\"pic\"; filename=\"images.jpg\"\r\nContent-Type: image/jpeg\r\nContent-Transfer-Encoding: binary\r\n\r\n";
    std::copy(data, data + dataSize, back_inserter(formContent));
    delete [] data;
    
    formContent += "\r\n--" + boundary + "--\r\n";
    
    auto realCB = [=](const Value& r, std::auto_ptr<Error> err)
    {
        if (err.get())
        {
            WBError* error = new WBError;
            error->error = toStr(err->code);
            error->errorDesc = err->msg;
            
            cb(auto_ptr<WBError>(error));
        }
        else
        {
            cb(auto_ptr<WBError>(jsonErrorToWBError(r)));
        }
    };
    
    std::vector<string> header;
    header.push_back("Content-Type: multipart/form-data; boundary=" + boundary);
    
    call(url, "POST", formContent, header, realCB);
}

template<>
void PH::Weibo<Sina>::postTweetWithImage(const std::string& content,
                                         cocos2d::CCImage* img,
                                         boost::function<void (std::auto_ptr<WBError>)> cb)
{
    if (!this->isAuthorized())
    {
        auto_ptr<WBError> error(new WBError);
        error->error = "-1";
        error->errorDesc = "not authorized or authorization expired";
        cb(error);
        return;
    }
    
    std::string method = "/statuses/upload.json";

    Document params;
    params.SetObject();
    
    params.AddMember("access_token", accessToken, params.GetAllocator());
    params.AddMember("status", urlEncode(content), params.GetAllocator());
    
    _postTweetWithImage(kSinaWeiboSDKAPIDomain+method, params, kSinaWeiboRequestStringBoundary, img, boost::move(cb));
}

template<>
void PH::Weibo<Tencent>::postTweetWithImage(const std::string& content,
                                            cocos2d::CCImage* img,
                                            boost::function<void (std::auto_ptr<WBError>)> cb)
{
    if (!this->isAuthorized())
    {
        auto_ptr<WBError> error(new WBError);
        error->error = "-1";
        error->errorDesc = "not authorized or authorization expired";
        cb(error);
        return;
    }
    
    std::string method = "/t/add_pic";

    Document params;
    params.SetObject();
    
    params.AddMember("oauth_consumer_key", appKey, params.GetAllocator());
    params.AddMember("access_token", accessToken, params.GetAllocator());
    params.AddMember("openid", uid, params.GetAllocator());
    params.AddMember("oauth_version", "2.a", params.GetAllocator());
    params.AddMember("scope", "all", params.GetAllocator());
    params.AddMember("clientip", cocos2d::extension::CCNative::IPAddress(), params.GetAllocator());
    params.AddMember("format", "json", params.GetAllocator());
    
    params.AddMember("content", urlEncode(content), params.GetAllocator());
    
    _postTweetWithImage(kTencentWeiboSDKAPIDomain+method, params, kSinaWeiboRequestStringBoundary, img, boost::move(cb));
}

template<>
void PH::Weibo<Sina>::clearLoginInfo()
{
    if (!accessToken.empty())
    {
        Document params;
        params.SetObject();
        
        params.AddMember("access_token", accessToken, params.GetAllocator());
        
        weiboRequest("/account/end_session.json", params, [](const Value& ret) {});
    }
    
    uid = "";
    accessToken = "";
    expirationTime = 0;
    
    saveConfig();
}

template<>
void PH::Weibo<Tencent>::clearLoginInfo()
{
    uid = "";
    accessToken = "";
    expirationTime = 0;
    
    saveConfig();
}

template<typename V>
void PH::Weibo<V>::saveConfig()
{
    std::string keyP = configKeyPrefix();
    CCUserDefault::sharedUserDefault()->setStringForKey((keyP + "_uid").c_str(), uid);
    CCUserDefault::sharedUserDefault()->setStringForKey((keyP + "_token").c_str(), accessToken);
    CCUserDefault::sharedUserDefault()->setIntegerForKey((keyP + "_expire").c_str(), expirationTime);
    CCUserDefault::sharedUserDefault()->flush();
}

template<>
bool PH::Weibo<Tencent>::ssoAuthorize(boost::function<void (std::auto_ptr<AuthError>)> callback)
{
    ssoLoggingIn = false;
    return false;
}

static inline
std::string getValue(const Value& v, const char* name, const std::string& defaultValue)
{
    auto& memberValue = v[name];
    if (memberValue.IsNull())
        return defaultValue;
    else
        return std::string(memberValue.GetString(), memberValue.GetStringLength());
}

template<>
void PH::Weibo<Sina>::handleSSOResult(const std::string& result, boost::function<void (std::auto_ptr<AuthError>)> callback)
{
    if (ssoLoggingIn)
    {
        ssoLoggingIn = false;
        auto_ptr<AuthError> err;
        Document ssoRet;

        ssoRet.ParseInsitu<0>(const_cast<char*>(result.data()));
        if (ssoRet.HasMember("sso_error_user_cancelled"))
        {
            err.reset(new AuthError);
            err->error = "1";
            err->errorDesc = "user canceled";
        }
        else if (ssoRet.HasMember("sso_error_attempt_failed"))
        {
            ssoTriedAndFailed = true;
            authorize(boost::move(callback));
            return;
        }
        else if (ssoRet.HasMember("error_code"))
        {
            err.reset(new AuthError);
            err->error = getValue(ssoRet, "error", "unknown error");
            err->errorDesc = getValue(ssoRet, "desc", "");
        }
        else
        {
            this->accessToken = getValue(ssoRet, "access_token", "");
            long long expires_in = boost::lexical_cast<long long>(getValue(ssoRet, "expires_in", "0"));
            this->expirationTime = time(NULL) + expires_in;
            this->uid = getValue(ssoRet, "uid", "");
            
            if (this->accessToken.empty() || this->uid.empty() || expires_in==0)
            {
                err.reset(new AuthError);
                err->error = "2";
                err->errorDesc = "Response not valid";
            }
            else
            {
                saveConfig();
            }
        }
        callback(err);
    }
}

template<>
bool PH::Weibo<Sina>::ssoAuthorize(boost::function<void (std::auto_ptr<AuthError>)> callback)
{
    ssoLoggingIn = false;
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    
    Document params;
    params.SetObject();
    
    if (ssoCBURI.empty())
    {
        ssoCBURI = "sinaweibosso." + appKey + "://";
    }
    
    params.AddMember("client_id", appKey, params.GetAllocator());
    params.AddMember("redirect_uri", appCBURI, params.GetAllocator());
    params.AddMember("callback_uri", ssoCBURI, params.GetAllocator());
    
    std::string queryString = "?" + URLHelper::toQueryString(params);
    std::string ipadAuthURL = kSinaWeiboAppAuthURL_iPad + queryString;
    std::string iphoneAuthURL = kSinaWeiboAppAuthURL_iPhone + queryString;
    
    ssoLoggingIn = CCNative::openURL(ipadAuthURL) || CCNative::openURL(iphoneAuthURL);
    if (ssoLoggingIn)
        ssoCallback = callback;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if (!ssoTriedAndFailed)
    {
        ssoLoggingIn = stl::CCNative::ssoWeiboLogin(stringToJavaString(appKey), stringToJavaString(appCBURI), addCallback(boost::function<void ()>([=]()
        {
            local_ref<java::lang::String> ssoRet = com::stl::puzzlehero::CCNative::mSSORet;
            handleSSOResult(JniHelper::jstring2string((jstring)ssoRet.get_jobject()), boost::move(callback));
        })));
    }
#endif
    
    return ssoLoggingIn;
}

template<>
bool PH::Weibo<Sina>::handleURL(const std::string& url)
{
    if (boost::algorithm::starts_with(url, ssoCBURI))
    {
        if (ssoLoggingIn)
        {
            ssoLoggingIn = false;
            auto_ptr<AuthError> err;
            
            if (!URLHelper::getURLParameterValue(url, "sso_error_user_cancelled").empty())
            {
                err.reset(new AuthError);
                err->error = "1";
                err->errorDesc = "user canceled";
            }
            else if (!URLHelper::getURLParameterValue(url, "sso_error_invalid_params").empty())
            {
                err.reset(new AuthError);
                err->error = "1";
                err->errorDesc = "invalid params";
            }
            else if (!URLHelper::getURLParameterValue(url, "error_code").empty())
            {
                err.reset(new AuthError);
                err->error = URLHelper::getURLParameterValue(url, "error_code");
                err->errorDesc = URLHelper::getURLParameterValue(url, "error_description");
                err->errorUri = URLHelper::getURLParameterValue(url, "error_uri");
            }
            else
            {
                // sucessfully authorized. get auth info
                try
                {
                    this->accessToken = URLHelper::getURLParameterValue(url, "access_token");
                    long long expires_in = boost::lexical_cast<long long>(URLHelper::getURLParameterValue(url, "expires_in"));
                    this->expirationTime = time(NULL) + expires_in;
                    this->uid = URLHelper::getURLParameterValue(url, "uid");
                    
                    string remind_in = URLHelper::getURLParameterValue(url, "remind_in");
                    string refresh_token = URLHelper::getURLParameterValue(url, "refresh_token");
                    
                    saveConfig();
                }
                catch (...)
                {
                    err.reset(new AuthError);
                    err->error = "2";
                    err->errorDesc = "Response not valid";
                }
            }
            ssoCallback(err);
            ssoCallback = [](std::auto_ptr<AuthError>){};
        }
        
        return true;
    }
    return false;
}

template<>
bool PH::Weibo<Tencent>::handleURL(const std::string& url)
{
    return false;
}

}
