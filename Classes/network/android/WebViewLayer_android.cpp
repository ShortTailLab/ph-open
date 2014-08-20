//
//  WebViewLayer.m
//  HelloCpp
//
//  Created by R.C on 12-10-30.
//
//

#include "WebViewLayer_android.h"
#include "platform/android/jni/JniHelper.h"
#include "extensions/utils.h"
#include "Board/Common.h"
#include "CCNative.h"
#include <string>
#include <j2cpp/j2cpp.hpp>
#include <java/lang/String.hpp>
#include <android/app/Activity.hpp>
#include <android/view/ViewGroup.hpp>
#include <android/graphics/Color.hpp>
#include <android/view/View.hpp>
#include <android/view/ViewParent.hpp>
#include <android/content/Context.hpp>
#include <android/webkit/WebView.hpp>
#include <android/webkit/WebSettings.hpp>
#include <android/webkit/WebViewClient.hpp>
#include <com/stl/puzzlehero/RootActivity.hpp>
#include <com/stl/puzzlehero/NativeCallback.h>
#include <com/stl/puzzlehero/WebViewLayerClient.h>
#include <android/widget/AbsoluteLayout.hpp>
#include <android/log.h>

using namespace j2cpp;
using namespace cocos2d;
using namespace j2cpp::java::lang;
using namespace j2cpp::android::app;
using namespace j2cpp::android::content;
using namespace j2cpp::android::view;
using namespace j2cpp::android::widget;
using namespace j2cpp::android::graphics;
using namespace j2cpp::android::view::ViewGroup_;
using namespace j2cpp::android::webkit;
using namespace com::stl::puzzlehero;

#define  LOG_TAG    "puzzlehero"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

namespace PH{

using namespace cocos2d::extension;
    
WebViewLayerAndroid::WebViewLayerAndroid(const CCRect& r)
{
    mOpaque = true;
    isVisible = false;
    closeButton = NULL;
    
    topLeft = CCDirector::sharedDirector()->convertToUI(ccp(r.getMinX(), r.getMaxY()));
    bottomRight = CCDirector::sharedDirector()->convertToUI(ccp(r.getMaxX(), r.getMinY()));
}

void WebViewLayerAndroid::initWebviewIfNeeded_UIThread(j2cpp::global_ref<j2cpp::android::content::Context> ctx)
{
    if (!webview)
    {
        this->webview = (global_ref<WebView>)WebView(ctx);
        local_ref<View> asView = (local_ref<View>)this->webview;
        asView->requestFocus();
        
        global_ref<WebViewLayerClient> client = WebViewLayerClient();
        local_ref<NativeCallback> cb = addCallback(boost::function<void (const std::string&, const std::string&)>([=](const std::string& url, const std::string&)
        {
            if (!url.empty())
            {
                cocos2d::extension::CCHttpRequest req;
                req.setUrl(url.c_str());
                
                client->shouldOverrideUrlLoadingNativeRet = !beforeStartLoadRequest(req);
            }
            else
            {
                client->shouldOverrideUrlLoadingNativeRet = false;
            }
        }), true);
        cbID = cb->nativeCallbackId;
        
        client->shouldOverrideUrlLoadingNative = cb;
        webview->setWebViewClient((local_ref<WebViewClient>)client);
        if (!this->mOpaque)
        {
            webview->setBackgroundColor(0);
            (local_ref<View>(webview))->setBackgroundResource(android::graphics::Color::TRANSPARENT);
        }

        float scale = CCEGLView::sharedOpenGLView()->getScaleX();
        
        CCSize winSize = CCDirector::sharedDirector()->getWinSize();
        CCSize screenSize = CCSize(CCNative::getResolutionWidth(),
                                   CCNative::getResolutionHeight());
        
        CCPoint delta = ccp((screenSize.width - winSize.width * scale) * 0.5f,
                            (screenSize.height - winSize.height * scale) * 0.5f);
        
        root = (global_ref<AbsoluteLayout>)AbsoluteLayout(ctx);
        ((local_ref<ViewGroup>)root)->addView((local_ref<WebView>)webview,
                                              AbsoluteLayout::LayoutParams(int((bottomRight.x - topLeft.x) * scale),
                                                                           int((bottomRight.y - topLeft.y) * scale),
                                                                           int(delta.x + topLeft.x * scale),
                                                                           int(delta.y + topLeft.y * scale)));
        
        local_ref<Activity>(ctx->get_jobject())->addContentView((local_ref<AbsoluteLayout>)root,
                                                                LayoutParams(LayoutParams::FILL_PARENT,
                                                                             LayoutParams::FILL_PARENT));
    }
}

void WebViewLayerAndroid::loadHTML(const std::string& data)
{
    runOnUIThread([=](global_ref<Context> ctx)
    {
        initWebviewIfNeeded_UIThread(ctx);
        
        webview->getSettings()->setJavaScriptEnabled(true);
//        webview->getSettings()->setLayoutAlgorithm(WebSettings::LayoutAlgorithm::NORMAL);
        local_ref<String> mime = stringToJavaString("text/html");
        local_ref<String> encode = stringToJavaString("UTF-8");


        LOGD("Android webview load local html...\n");
        webview->loadDataWithBaseURL(local_ref<String>(), stringToJavaString(data), mime, encode, local_ref<String>());
//        webview->loadUrl(stringToJavaString("http://192.168.0.30:8000/static/tt.html"));
    });
    
    // create close button
    closeButton = ButtonSprite::create(GetSprite("gameui/cha.png"),
                                       GetSprite("gameui/cha.png"),
                                       UI_TOUCH_ORDER_MODAL_MENU-1);
    closeButton->setVisible(isVisible);
    closeButton->setPosition(ccp(bottomRight.x+10, bottomRight.y+10));
    closeButton->setOnClickListener([=]()
                                    {
                                        onClose();
                                    });
    this->addChild(closeButton);
}

void WebViewLayerAndroid::openURL(const std::string &url, bool ignoreCache)
{
    runOnUIThread([=](global_ref<Context> ctx)
    {
        initWebviewIfNeeded_UIThread(ctx);
        
        webview->getSettings()->setCacheMode(WebSettings::LOAD_NO_CACHE);
        webview->getSettings()->setJavaScriptEnabled(true);
        webview->loadUrl(stringToJavaString(url));
    });
    
    // create close button
    closeButton = ButtonSprite::create(GetSprite("gameui/cha.png"),
                                       GetSprite("gameui/cha.png"),
                                       UI_TOUCH_ORDER_MODAL_MENU-1);

    closeButton->setVisible(isVisible);
    closeButton->setPosition(ccp(bottomRight.x+10, bottomRight.y+10));
    closeButton->setOnClickListener([=]()
                                    {
                                        onClose();
                                    });
    this->addChild(closeButton);
}

void WebViewLayerAndroid::showCloseButton(bool show)
{
    isVisible = show;
    if(closeButton)
        closeButton->setVisible(isVisible);
}

void WebViewLayerAndroid::opaque(bool opaque)
{
    this->mOpaque = opaque;
}

bool WebViewLayerAndroid::init()
{
    return WebViewLayer::init();
}

WebViewLayerAndroid::~WebViewLayerAndroid()
{
    global_ref<View> v = root;
    int localCBID = cbID;
    
    if (v)
    {
        runOnUIThread([=](global_ref<Context> ctx)
        {
            local_ref<ViewParent> parent = v->getParent();
            ViewGroup(parent.get_jobject()).removeView(v);
            
            CCNative::focusGLView();
        });
        
        runOnMainThread([=]()
        {
            retireCallback(localCBID);
        });
    }
}

WebViewLayer* WebViewLayer::create(const CCRect& rect)
{
    WebViewLayerAndroid* ret = new WebViewLayerAndroid(rect);
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return NULL;
}

}
