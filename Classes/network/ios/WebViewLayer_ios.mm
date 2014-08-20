//
//  WebViewLayer.m
//  HelloCpp
//
//  Created by R.C on 12-10-30.
//
//

#import "WebViewLayer_ios.h"
#import <string>

#include "Board/Utils.h"

using namespace PH;

void WebViewLayeriOS::openURL(const std::string &url, bool ignoreCache)
{
    NSURLRequest* req = [NSURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]]
                                         cachePolicy:ignoreCache ? NSURLRequestReloadIgnoringLocalCacheData : NSURLRequestUseProtocolCachePolicy
                                     timeoutInterval:60.0f];
    [webView loadRequest:req];
}

void WebViewLayeriOS::loadHTML(const std::string& html)
{
    NSString* str = [NSString stringWithUTF8String:html.c_str()];
    [webView loadHTMLString:str baseURL:nil];
}

void WebViewLayeriOS::showCloseButton(bool show)
{
    closeBtn.hidden = NO;
}

void WebViewLayeriOS::opaque(bool opaque)
{
    webView.opaque = opaque;
    
    if (!opaque)
    {
        // set webview background to transparent,
        // no need to embed css style for transparency.
        webView.backgroundColor = [UIColor clearColor];
    }
}

bool WebViewLayeriOS::init(const cocos2d::CCRect& r)
{
    if (!WebViewLayer::init())
        return false;
    
    webView = [[UIWebView alloc] init];
    delegate = [[WebViewDelegate alloc] init];
    delegate.viewLayer = this;
    webView.delegate = delegate;
    
    UIWindow* win = [[UIApplication sharedApplication].delegate window];
    [win addSubview:webView];

    unsigned long size;
    std::string imagePath = CCFileUtils::sharedFileUtils()->fullPathForFilename("gameui/cha.png");
    unsigned char* data = CCFileUtils::sharedFileUtils()->getFileData(imagePath.c_str(), "r", &size);
    
    NSData* imgData = [NSData dataWithBytes:data length:size];
    closeBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    UIImage* image = [UIImage imageWithData:imgData];
    [closeBtn setImage:image forState:UIControlStateNormal];
    [closeBtn addTarget:delegate action:@selector(onCloseClick:) forControlEvents:UIControlEventTouchUpInside];
    closeBtn.frame = CGRectMake(0, 0, 35, 35);
    closeBtn.hidden = YES;
    
    [win addSubview:closeBtn];
    
    // resize webview
    {
        float scale = CCEGLView::sharedOpenGLView()->getScaleX() / [[UIScreen mainScreen] scale];
        
        CCSize winSize = CCDirector::sharedDirector()->getWinSize(); // cocos2d-x bug
        CGSize screenSize = [[UIScreen mainScreen] bounds].size;
        
        CCPoint delta = ccp((screenSize.width - winSize.width * scale) * 0.5f,
                            (screenSize.height - winSize.height * scale) * 0.5f);
        
        CCPoint topLeft = CCDirector::sharedDirector()->convertToUI(ccp(r.getMinX(), r.getMaxY()));
        CCPoint bottomRight = CCDirector::sharedDirector()->convertToUI(ccp(r.getMaxX(), r.getMinY()));
        
        if (r.size.width == 0 && r.size.height == 0)
            webView.frame = [webView superview].bounds;
        else
            webView.frame = CGRectMake(int(delta.x + topLeft.x * scale),
                                       int(delta.y + topLeft.y * scale),
                                       int((bottomRight.x - topLeft.x) * scale),
                                       int((bottomRight.y - topLeft.y) * scale));
        
        closeBtn.center = CGPointMake(CGRectGetMaxX(webView.frame),
                                      CGRectGetMinY(webView.frame));
    }
    
    return true;
}

void WebViewLayeriOS::setScaleToFit(bool val)
{
    webView.scalesPageToFit = val;
}

WebViewLayeriOS::~WebViewLayeriOS()
{
    [webView removeFromSuperview];
    [closeBtn removeFromSuperview];
    [webView stopLoading];
    webView.delegate = nil;
    [webView release];
    [delegate release];
}

WebViewLayer* WebViewLayer::create(const cocos2d::CCRect& rect)
{
    WebViewLayeriOS* ret = new WebViewLayeriOS();
    if (ret && ret->init(rect))
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return NULL;
}

#pragma mark webview delegate

@implementation WebViewDelegate

- (BOOL)webView:(UIWebView *)aWebView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
    cocos2d::extension::CCHttpRequest req;
    req.setUrl(request.URL.absoluteString.UTF8String);
    
    return self.viewLayer->beforeStartLoadRequest(req);
}

- (void)webViewDidFinishLoad:(UIWebView *)webView
{
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
}

- (void)onCloseClick:(UIButton*)btn
{
    self.viewLayer->onClose();
    self.viewLayer->removeFromParentAndCleanup(true);
}

@end