//
//  WebViewLayerClient.h
//  HelloCpp
//
//  Created by R.C on 12-11-28.
//
//

#ifndef HelloCpp_WebViewLayerClient_h
#define HelloCpp_WebViewLayerClient_h

#include <j2cpp/j2cpp.hpp>
#include <java/lang/String.hpp>
#include <android/webkit/WebViewClient.hpp>
#include "NativeCallback.h"

namespace j2cpp {
    class WebViewLayerClient : public j2cpp::object<WebViewLayerClient>
    {
    public:
        J2CPP_DECLARE_CLASS
		J2CPP_DECLARE_METHOD(0)
        J2CPP_DECLARE_FIELD(0)
        J2CPP_DECLARE_FIELD(1)
        J2CPP_DECLARE_FIELD(2)
        
        explicit WebViewLayerClient(jobject jobj);
        WebViewLayerClient();
        
        operator local_ref<j2cpp::android::webkit::WebViewClient> () const;
        
        j2cpp::field< J2CPP_CLASS_NAME, J2CPP_FIELD_NAME(0), J2CPP_FIELD_SIGNATURE(0), local_ref<NativeCallback> > shouldOverrideUrlLoadingNative;
        j2cpp::field< J2CPP_CLASS_NAME, J2CPP_FIELD_NAME(1), J2CPP_FIELD_SIGNATURE(1), jboolean > shouldOverrideUrlLoadingNativeRet;
        j2cpp::field< J2CPP_CLASS_NAME, J2CPP_FIELD_NAME(2), J2CPP_FIELD_SIGNATURE(2), local_ref<j2cpp::java::lang::String> > url;
    };
}

#endif
