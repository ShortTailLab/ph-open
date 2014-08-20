//
//  android_cpp_sdk_lib.cpp
//  HelloCpp
//
//  Created by Wu Zhen on 12-11-26.
//
//

#include "BuildConf.h"

#include <j2cpp/j2cpp.hpp>

#include <java/lang/Class.hpp>
#include <java/lang/Object.hpp>
#include <java/lang/Integer.hpp>
#include <java/lang/Comparable.hpp>
#include <java/lang/String.hpp>
#include <java/lang/System.hpp>
#include <java/nio/Buffer.hpp>
#include <java/nio/FloatBuffer.hpp>
#include <java/nio/IntBuffer.hpp>

#include <android/app/Activity.hpp>
#include <android/view/View.hpp>
#include <android/view/ViewGroup.hpp>
#include <android/graphics/Color.hpp>
#include <android/content/Context.hpp>
#include <android/webkit/WebView.hpp>
#include <android/webkit/WebSettings.hpp>
#include <android/webkit/WebViewClient.hpp>
#include <android/widget/AbsoluteLayout.hpp>

#include <com/stl/puzzlehero/RootActivity.hpp>
#include <com/stl/puzzlehero/CCNative.hpp>


#if PH_USE_WEIBO
#include <com/tencent/mm/sdk/openapi/BaseReq.hpp>
#include <com/tencent/mm/sdk/openapi/BaseResp.hpp>
#include <com/tencent/mm/sdk/openapi/WXMediaMessage.hpp>
#include <com/tencent/mm/sdk/openapi/WXTextObject.hpp>
#include <com/tencent/mm/sdk/openapi/SendMessageToWX.hpp>
#include <com/tencent/mm/sdk/openapi/IWXAPI.hpp>
#include <com/tencent/mm/sdk/openapi/SendMessageToWX.hpp>
#include <com/tencent/mm/sdk/openapi/WXAPIFactory.hpp>
#endif

#define J2CPP_INCLUDE_IMPLEMENTATION

#include <java/lang/Class.hpp>
#include <java/lang/Object.hpp>
#include <java/lang/Integer.hpp>
#include <java/lang/Comparable.hpp>
#include <java/lang/String.hpp>
#include <java/lang/System.hpp>
#include <java/nio/Buffer.hpp>
#include <java/nio/FloatBuffer.hpp>
#include <java/nio/IntBuffer.hpp>

#include <android/app/Activity.hpp>
#include <android/view/View.hpp>
#include <android/view/ViewGroup.hpp>
#include <android/graphics/Color.hpp>
#include <android/content/Context.hpp>
#include <android/webkit/WebView.hpp>
#include <android/webkit/WebSettings.hpp>
#include <android/webkit/WebViewClient.hpp>
#include <android/widget/AbsoluteLayout.hpp>

#include <com/stl/puzzlehero/RootActivity.hpp>
#include <com/stl/puzzlehero/CCNative.hpp>

#if PH_USE_WEIBO
#include <com/tencent/mm/sdk/openapi/BaseReq.hpp>
#include <com/tencent/mm/sdk/openapi/BaseResp.hpp>
#include <com/tencent/mm/sdk/openapi/WXMediaMessage.hpp>
#include <com/tencent/mm/sdk/openapi/WXTextObject.hpp>
#include <com/tencent/mm/sdk/openapi/SendMessageToWX.hpp>
#include <com/tencent/mm/sdk/openapi/IWXAPI.hpp>
#include <com/tencent/mm/sdk/openapi/SendMessageToWX.hpp>
#include <com/tencent/mm/sdk/openapi/WXAPIFactory.hpp>
#endif
