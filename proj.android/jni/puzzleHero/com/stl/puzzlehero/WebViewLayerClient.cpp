//
//  WebViewLayerClient.cpp
//  HelloCpp
//
//  Created by R.C on 12-11-28.
//
//

#include "WebViewLayerClient.h"

namespace j2cpp {
    J2CPP_DEFINE_CLASS(WebViewLayerClient,"com/stl/puzzlehero/WebViewLayerClient")
    J2CPP_DEFINE_METHOD(WebViewLayerClient,0,"<init>","()V")
    J2CPP_DEFINE_FIELD(WebViewLayerClient,0,"shouldOverrideUrlLoadingNative","Lcom/stl/puzzlehero/NativeCallback;")
    J2CPP_DEFINE_FIELD(WebViewLayerClient,1,"shouldOverrideUrlLoadingNativeRet","Z")
    J2CPP_DEFINE_FIELD(WebViewLayerClient,2,"url","Ljava/lang/String;")
}

j2cpp::WebViewLayerClient::WebViewLayerClient(jobject jobj)
: object<WebViewLayerClient>(jobj)
, shouldOverrideUrlLoadingNative(jobj)
, shouldOverrideUrlLoadingNativeRet(jobj)
, url(jobj)
{
    
}

j2cpp::WebViewLayerClient::WebViewLayerClient()
: object<j2cpp::WebViewLayerClient>(
                                call_new_object<
                                j2cpp::WebViewLayerClient::J2CPP_CLASS_NAME,
                                j2cpp::WebViewLayerClient::J2CPP_METHOD_NAME(0),
                                j2cpp::WebViewLayerClient::J2CPP_METHOD_SIGNATURE(0)
                                >()
                                )
, shouldOverrideUrlLoadingNative(get_jobject())
, shouldOverrideUrlLoadingNativeRet(get_jobject())
, url(get_jobject())
{
}

j2cpp::WebViewLayerClient::operator local_ref<j2cpp::android::webkit::WebViewClient> () const
{
	return local_ref<j2cpp::android::webkit::WebViewClient>(get_jobject());
}
