//
//  WebViewLayer.h
//  HelloCpp
//
//  Created by R.C on 12-10-30.
//
//

#ifndef __HelloCpp__WebViewLayer__
#define __HelloCpp__WebViewLayer__

#include <cocos2d.h>
#include <boost/function.hpp>
#include "network/HttpRequest.h"

namespace PH
{
    class WebViewLayer : public cocos2d::CCLayer
    {
    public:
        virtual void openURL(const std::string& url, bool ignoreCache=false)=0;
        virtual void loadHTML(const std::string& html) = 0;
        virtual void showCloseButton(bool show=true) = 0;
        virtual void opaque(bool opaque=true) = 0;
        virtual void setScaleToFit(bool val) {};
        
        static WebViewLayer* create(const cocos2d::CCRect& rect);

        // callback methods from webview
        boost::function<bool (cocos2d::extension::CCHttpRequest&)> beforeStartLoadRequest;
        boost::function<void ()> onClose;
    };
}

#endif /* defined(__HelloCpp__WebViewLayer__) */
