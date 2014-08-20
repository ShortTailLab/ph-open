//
//  NDSDK_android.cpp
//  HelloCpp
//
//  Created by R.C on 12-11-26.
//
//

#include "NDSDK_android.h"
#include "platform/android/jni/JniHelper.h"
#include "extensions/utils.h"

#include <j2cpp/j2cpp.hpp>
#include <j2cpp/raw_environment.hpp>
#include <java/lang/String.hpp>
#include <java/lang/Integer.hpp>
#include <android/app/Activity.hpp>
#include <android/content/Context.hpp>
#include <com/nd/commplatform/NdCommplatform.hpp>
#include <com/nd/commplatform/entry/NdAppInfo.hpp>
#include <com/nd/commplatform/entry/NdLoginStatus.hpp>
#include <com/nd/commplatform/entry/NdBuyInfo.hpp>
#include <com/stl/puzzlehero/NdNativeCallbacks.h>
#include <com/stl/puzzlehero/RootActivity.hpp>

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace j2cpp;
using namespace j2cpp::android::app;
using namespace j2cpp::android::content;
using namespace j2cpp::java::lang;
using namespace j2cpp::com::nd::commplatform;
using namespace j2cpp::com::nd::commplatform::entry;
using namespace j2cpp::com::stl::puzzlehero;

using namespace boost;
using namespace boost::uuids;

namespace PH{

ND& ND::instance()
{
    static ND_Android* ins = NULL;
    if (!ins)
        ins = new ND_Android;
    return *ins;
}
    
ND_Android::ND_Android()
{
    NdCommplatform::getInstance()->setRestartWhenSwitchAccount(true);
}
    
ND_Android::~ND_Android()
{
    
}
    
void ND_Android::setDebugMode(int f)
{
    NdCommplatform::getInstance()->ndSetDebugMode(f);
}

void ND_Android::setAppIdAndAppKey(int appId, const std::string &appKey)
{
    local_ref<String> key = stringToJavaString(appKey.c_str());
    local_ref<Activity> ctx = (local_ref<RootActivity>)RootActivity::ctx;
    
    global_ref<NdAppInfo> appInfo = NdAppInfo();
    appInfo->setAppId(appId);
    appInfo->setAppKey(key);
    appInfo->setCtx(ctx);

    runOnUIThread([=](global_ref<Context>){
        NdCommplatform::getInstance()->initial(0, appInfo);
    });
}

void ND_Android::checkVersionUpdate(boost::function<void (ND_APP_UPDATE_RESULT)> callback)
{
    runOnUIThread([=](global_ref<Context> ctx){
        NdCommplatform::getInstance()->ndAppVersionUpdate((local_ref<Context>)ctx, makeNdCallback([=](local_ref<java::lang::Object> code){
            callback((ND_APP_UPDATE_RESULT)java::lang::Integer(code.get_jobject()).intValue());
        }));
    });
}

void ND_Android::setPlatformUIOrientation(PH::ND_PLATFORM_ORIENTATION o)
{
    NdCommplatform::getInstance()->ndSetScreenOrientation(o);
}

void ND_Android::login(boost::function<void (ND_PLATFORM_ERRORS)> cb)
{
    runOnUIThread([=](global_ref<Context> ctx){
        NdCommplatform::getInstance()->ndLogin((local_ref<Context>)ctx, makeNdLoginCallback([=](jint result){
            cb((ND_PLATFORM_ERRORS)result);
        }));
    });
}

void ND_Android::guestLogin(boost::function<void (ND_PLATFORM_ERRORS)> cb)
{
    runOnUIThread([=](global_ref<Context> ctx){
        NdCommplatform::getInstance()->ndLoginEx((local_ref<Context>)ctx, makeNdLoginCallback([=](jint result){
            cb((ND_PLATFORM_ERRORS)result);
        }));
    });
}

void ND_Android::guestRegist(boost::function<void (ND_PLATFORM_ERRORS)> cb)
{
    runOnUIThread([=](global_ref<Context> ctx){
        NdCommplatform::getInstance()->ndGuestRegist((local_ref<Context>)ctx, stringToJavaString(""), makeNdLoginCallback([=](jint result){
            cb((ND_PLATFORM_ERRORS)result);
        }));
    });
}

bool ND_Android::isLogedIn()
{
    return NdCommplatform::getInstance()->isLogined();
}

bool ND_Android::isGuestLogin()
{
    JNIEnv* env = raw_environment::get()->jenv();
    return env->IsSameObject(
                              NdCommplatform::getInstance()->ndGetLoginStatus()->get_jobject(),
                              ((local_ref<NdLoginStatus>)NdLoginStatus::GuestLogin)->get_jobject());
}

std::string ND_Android::UIN()
{
    return JniHelper::jstring2string( (jstring)NdCommplatform::getInstance()->getLoginUin()->get_jobject() );
}
    
std::string ND_Android::nickName()
{
    return JniHelper::jstring2string( (jstring)NdCommplatform::getInstance()->getLoginNickName()->get_jobject() );
}

void ND_Android::logout()
{
    runOnUIThread([=](global_ref<Context> ctx){
        NdCommplatform::getInstance()->ndLogout(1, (local_ref<Context>)ctx);
    });
}

void ND_Android::pay(PH::StoreItem& item, boost::function<void (ND_PLATFORM_ERRORS, const std::string&)> callback)
{
    global_ref<NdBuyInfo> buyInfo = NdBuyInfo();
    
    std::string serial = to_string(boost::uuids::random_generator()());
    
    buyInfo->setSerial(stringToJavaString(serial));
    buyInfo->setProductId(stringToJavaString(item.productId));
    buyInfo->setProductName(stringToJavaString(item.productName));
    buyInfo->setProductPrice(item.price);
    buyInfo->setProductOrginalPrice(item.price);
    buyInfo->setCount(1);
    
    runOnUIThread([=](global_ref<Context> ctx){
        int error = NdCommplatform::getInstance()->ndUniPay(buyInfo, (local_ref<Context>)ctx, makeNdPayCallback([=](jint error){
            callback((ND_PLATFORM_ERRORS)error, serial);
        }));
        
        if (error)
            LOGD("pay param error: %d", error);
    });
}
    
void ND_Android::feedback()
{
    runOnUIThread([=](global_ref<Context> ctx){
        NdCommplatform::getInstance()->ndUserFeedback(ctx);
    });
}

void ND_Android::enterPlatform(boost::function<void (ND_PLATFORM_ERRORS)> cb)
{
    runOnUIThread([=](global_ref<Context> ctx){
        NdCommplatform::getInstance()->ndEnterPlatform(0, (local_ref<Context>)ctx);
    });
}

void ND_Android::queueLocalNotification(const std::string &msg, int seconds)
{
    // unsupported on android
}

void ND_Android::cancelAllLocalNotifications()
{
    // unsupported on android
}

}
