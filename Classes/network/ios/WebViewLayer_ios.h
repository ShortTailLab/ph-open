//
//  WebViewLayer.h
//  HelloCpp
//
//  Created by R.C on 12-10-30.
//
//

#import <UIKit/UIKit.h>
#import <string>
#import "WebViewLayer.h"

@class WebViewDelegate;

namespace PH
{
    class WebViewLayeriOS : public WebViewLayer
    {
    public:
        bool init(const cocos2d::CCRect& rect);
        virtual ~WebViewLayeriOS();
        
        virtual void openURL(const std::string& url, bool ignoreCache=false);
        virtual void loadHTML(const std::string& html);
        virtual void showCloseButton(bool show=true);
        virtual void opaque(bool opaque=true);
        
        virtual void setScaleToFit(bool val);
        
    private:
        UIWebView* webView;
        WebViewDelegate* delegate;
    public:
        UIButton* closeBtn;
    };
}


@interface WebViewDelegate : NSObject<UIWebViewDelegate>

@property (nonatomic, assign) PH::WebViewLayeriOS* viewLayer;

@end