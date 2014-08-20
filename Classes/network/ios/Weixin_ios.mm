//
//  Weixin_ios.m
//  HelloCpp
//
//  Created by R.C on 13-3-4.
//
//

#import "Weixin_ios.h"

using namespace PH;

WeixinIOS::~WeixinIOS()
{
    
}

void WeixinIOS::setRequestHandler(ReqHandler handler)
{
    reqHandler = handler;
}

void WeixinIOS::setResponseHandler(RespHandler handler)
{
    respHandler = handler;
}

void WeixinIOS::onReq(boost::shared_ptr<WXBaseReq> _req)
{
//    BaseReq* req = boost::shared_dynamic_cast<WXReqWrapper>(_req)->req;
    if (reqHandler)
        reqHandler(_req);
}

void WeixinIOS::onResp(boost::shared_ptr<WXBaseResp> _resp)
{
    BaseResp* resp = boost::shared_dynamic_cast<WXRespWrapper>(_resp)->resp;
    if([resp isKindOfClass:[SendMessageToWXResp class]])
    {
        _resp.reset(new WXMediaRespIOS((SendMessageToWXResp*)resp));
    }
    
    if (respHandler)
        respHandler(_resp);
}

bool WeixinIOS::registerApp(const std::string& appId)
{
    return [WXApi registerApp:[NSString stringWithUTF8String:appId.c_str()]];
}

bool WeixinIOS::isWXSuuportAPI()
{
    return [WXApi isWXAppInstalled] && [WXApi isWXAppSupportApi];
}

bool WeixinIOS::sendRequest(boost::shared_ptr<WXBaseReq> _req)
{
    BaseReq* req = boost::shared_dynamic_cast<WXReqWrapper>(_req)->req;
    return [WXApi sendReq:req];
}

bool WeixinIOS::sendResponse(boost::shared_ptr<WXBaseResp> _resp)
{
    BaseResp* resp = boost::shared_dynamic_cast<WXRespWrapper>(_resp)->resp;
    return [WXApi sendResp:resp];
}

WeixinApi& WeixinApi::instance()
{
    static WeixinApi* _ins = NULL;
    if (!_ins)
        _ins = new WeixinIOS();
    return *_ins;
}

#pragma mark - WXTextMessageRequest

boost::shared_ptr<WXTextMessageRequest> WXTextMessageRequest::create()
{
    return boost::make_shared<WXTextReqIOS>([[[SendMessageToWXReq alloc] init] autorelease]);
}

WXTextReqIOS::WXTextReqIOS(SendMessageToWXReq* req)
: WXReqWrapper(req)
{
    request().bText = YES;
}

void WXTextReqIOS::text(const std::string& t)
{
    request().text = [NSString stringWithUTF8String:t.c_str()];
}

std::string WXTextReqIOS::text()
{
    return request().text.UTF8String;
}

void WXTextReqIOS::sendToTimeline(bool value)
{
    request().scene = value ? WXSceneTimeline : WXSceneSession;
}

#pragma mark - WXMediaMessageRequest

boost::shared_ptr<WXMediaMessageRequest> WXMediaMessageRequest::create()
{
    return boost::make_shared<WXMediaReqIOS>([[[SendMessageToWXReq alloc] init] autorelease]);
}

WXMediaReqIOS::WXMediaReqIOS(SendMessageToWXReq* req)
: WXReqWrapper(req)
{
    request().bText = NO;
    request().message = [WXMediaMessage message];
}

void WXMediaReqIOS::text(const std::string& t)
{
    request().text = [NSString stringWithUTF8String:t.c_str()];
}

std::string WXMediaReqIOS::text()
{
    return request().text.UTF8String;
}

void WXMediaReqIOS::description(const std::string& desc)
{
    request().message.description = [NSString stringWithUTF8String:desc.c_str()];
}

std::string WXMediaReqIOS::description()
{
    return request().message.description.UTF8String;
}

void WXMediaReqIOS::thumbData(const std::vector<unsigned char>& d)
{
    NSData* thumb = [NSData dataWithBytes:d.data() length:d.size()];
    request().message.thumbData = thumb;
}

void WXMediaReqIOS::image(const std::vector<unsigned char>& d)
{
    NSData* image = [NSData dataWithBytes:d.data() length:d.size()];
    WXImageObject* obj = [WXImageObject object];
    obj.imageData = image;
    request().message.mediaObject = obj;
}

void WXMediaReqIOS::image(const std::string& url)
{
    WXImageObject* obj = [WXImageObject object];
    obj.imageUrl = [NSString stringWithUTF8String:url.c_str()];
    request().message.mediaObject = obj;
}

void WXMediaReqIOS::webpageURL(const std::string& url)
{
    WXWebpageObject* obj = [WXWebpageObject object];
    obj.webpageUrl = [NSString stringWithUTF8String:url.c_str()];
    request().message.mediaObject = obj;
}

void WXMediaReqIOS::sendToTimeline(bool value)
{
    request().scene = value ? WXSceneTimeline : WXSceneSession;
}
