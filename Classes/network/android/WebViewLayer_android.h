//
//  WebViewLayer.h
//  HelloCpp
//
//  Created by R.C on 12-10-30.
//
//

#ifndef __HelloCpp__WebViewLayer__ANDROID__
#define __HelloCpp__WebViewLayer__ANDROID__

#include <string>
#include "WebViewLayer.h"
#include <j2cpp/j2cpp.hpp>
#include <android/webkit/webview.hpp>
#include <android/content/Context.hpp>

#include "ButtonSprite.h"

namespace PH
{
    class WebViewLayerAndroid : public WebViewLayer
    {
    public:
        WebViewLayerAndroid(const cocos2d::CCRect& rect);
        
        virtual bool init();
        virtual ~WebViewLayerAndroid();
        
        virtual void openURL(const std::string& url, bool ignoreCache=false);
        virtual void loadHTML(const std::string& html);
        virtual void showCloseButton(bool show=true);
        virtual void opaque(bool opaque=true);
        
    private:
        void initWebviewIfNeeded_UIThread(j2cpp::global_ref<j2cpp::android::content::Context>);
        
    private:
        j2cpp::global_ref<j2cpp::android::webkit::WebView> webview;
        j2cpp::global_ref<j2cpp::android::widget::AbsoluteLayout> root;
        cocos2d::CCPoint topLeft, bottomRight;
        int cbID;
        bool mOpaque;
        bool isVisible;
        
        ButtonSprite* closeButton;
    };
}


#endif