//
//  Weixin_android.cpp
//  HelloCpp
//
//  Created by R.C on 13-3-6.
//
//

#include "Weixin_android.h"
#include "extensions/utils.h"
#include "platform/android/jni/JniHelper.h"
#include <java/lang/String.hpp>
#include <java/lang/System.hpp>
#include <com/tencent/mm/sdk/openapi/WXAPIFactory.hpp>
#include <com/tencent/mm/sdk/openapi/SendMessageToWX.hpp>
#include <com/tencent/mm/sdk/openapi/WXTextObject.hpp>
#include <com/tencent/mm/sdk/openapi/WXMediaMessage.hpp>
#include <android/content/Context.hpp>

using namespace PH;
using namespace boost;
using namespace std;
using namespace j2cpp;
using namespace android::content;
using namespace com::tencent::mm::sdk::openapi;


WeixinAndroid::WeixinAndroid()
{

}

void WeixinAndroid::setRequestHandler(ReqHandler handler)
{
    reqHandler = handler;
}

void WeixinAndroid::setResponseHandler(RespHandler handler)
{
    respHandler = handler;
}

void WeixinAndroid::onReq(boost::shared_ptr<WXBaseReq> _req)
{
    if (reqHandler)
        reqHandler(_req);
}

void WeixinAndroid::onResp(boost::shared_ptr<WXBaseResp> _resp)
{
    if (respHandler)
        respHandler(_resp);
}

bool WeixinAndroid::registerApp(const std::string& appId)
{
    runOnUIThread([=](global_ref<Context> ctx){
        local_ref<java::lang::String> aid = stringToJavaString(appId);
        
        this->wx = WXAPIFactory::createWXAPI(ctx, local_ref<java::lang::String>(), false);
        bool regResult = this->wx->registerApp(aid);
    });
    return true;
}

bool WeixinAndroid::isWXSuuportAPI()
{
    return wx->isWXAppInstalled() && wx->isWXAppSupportAPI();
}

bool WeixinAndroid::sendRequest(boost::shared_ptr<WXBaseReq> _req)
{
    runOnUIThread([=](global_ref<Context> ctx){
        auto req = boost::dynamic_pointer_cast<WXReqAndroid>(_req);
        if (req)
        {
            wx->sendReq(req->req);
        }
    });

    return isWXSuuportAPI();
}

bool WeixinAndroid::sendResponse(boost::shared_ptr<WXBaseResp> _resp)
{
    runOnUIThread([=](global_ref<Context> ctx){
        auto resp = boost::dynamic_pointer_cast<WXRespAndroid>(_resp);
        if (resp)
        {
            wx->sendResp(resp->resp);
        }
    });

    return isWXSuuportAPI();
}

WeixinApi& WeixinApi::instance()
{
    static WeixinApi* _ins = new WeixinAndroid();
    return *_ins;
}

#pragma mark - WXMediaMessageRequest

boost::shared_ptr<WXMediaMessageRequest> WXMediaMessageRequest::create()
{
    return boost::make_shared<WXMediaMessageRequestAndroid>();
}

#pragma mark - WXTextMessageRequest

WXTextMessageRequestAndroid::WXTextMessageRequestAndroid()
{
    this->req = SendMessageToWX::Req();
}

boost::shared_ptr<WXTextMessageRequest> WXTextMessageRequest::create()
{
    return boost::make_shared<WXTextMessageRequestAndroid>();
}

void WXTextMessageRequestAndroid::text(const std::string& text)
{
    local_ref<WXMediaMessage> msg = WXMediaMessage();
    local_ref<WXTextObject> tObj = WXTextObject();
    
    tObj->text = stringToJavaString(text.c_str());
    msg->mediaObject = tObj;
    
    ((local_ref<SendMessageToWX::Req>)SendMessageToWX::Req(this->req->get_jobject()))->message = msg;
    this->req->transaction = java::lang::String::valueOf(java::lang::System::currentTimeMillis());
}

std::string WXTextMessageRequestAndroid::text()
{
    local_ref<WXMediaMessage> msg = ((local_ref<SendMessageToWX::Req>)SendMessageToWX::Req(this->req->get_jobject()))->message;
    local_ref<WXMediaMessage::IMediaObject> mObj = msg->mediaObject;
    local_ref<java::lang::String> text = ((local_ref<WXTextObject>)WXTextObject(mObj->get_jobject()))->text;
    return cocos2d::JniHelper::jstring2string((jstring)text->get_jobject());
}

void WXTextMessageRequestAndroid::sendToTimeline(bool tl)
{
    ((local_ref<SendMessageToWX::Req>)SendMessageToWX::Req(this->req->get_jobject()))->scene = tl ? 1 : 0;
}
