//
//  utils.h
//  HelloCpp
//
//  Created by R.C on 12-12-9.
//
//

#ifndef HelloCpp_utils_h
#define HelloCpp_utils_h

#include "platform/CCPlatformConfig.h"
#include <sprite_nodes/CCSprite.h>
#include <platform/CCImage.h>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
//#include <boost/random/mersenne_twister.hpp>
#include <boost/random/linear_congruential.hpp>

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#include <j2cpp/j2cpp.hpp>
#include <j2cpp/raw_environment.hpp>
#include <android/content/Context.hpp>
#include <java/lang/String.hpp>
#include <string>
#include <android/log.h>
#include "platform/android/jni/JniHelper.h"

#define  LOG_TAG    "puzzlehero"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

namespace PH {

j2cpp::local_ref<j2cpp::java::lang::String> stringToJavaString(const std::string& str);

void runOnUIThread(boost::function<void (j2cpp::global_ref<j2cpp::android::content::Context>)> f);
    
}

#else //#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#include <stdio.h>

#define LOGD(...) printf(__VA_ARGS__)

#endif //#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#if PH_DEBUG_BUILD
    #define phassert(...)    \
    {\
        CCLog("%s", __FUNCTION__); \
        assert(__VA_ARGS__);\
    }

#else
    #define phassert(...)
#endif

namespace PH {
    
    void runOnMainThread(boost::function<void ()> f);
    void runOnMainThreadWhen(boost::function<bool()> condition, boost::function<void ()> f);
    
    void runOnBackgroundThread(boost::function<void ()>);

    void _runOnMainThreadPrepare();
    
    cocos2d::CCSprite *GetSprite(const std::string& filename);
    
    bool resourcePresentForLoading(const std::string& filename);

    cocos2d::CCImage* snapshot();
    
    class _tag_EventHandlerID;
    typedef boost::shared_ptr<_tag_EventHandlerID> EventHandlerID;
    
    class InProcEventCentral
    {
    public:
        static EventHandlerID waitForEvent(const std::string& event,
                                           boost::function<void (const std::string&, const std::map<std::string, std::string>&)>);
        static void discardEventHandler(const EventHandlerID&);
        static void discardAllEventHandler(const std::string& event);
        
        static void signal(const std::string& event, const std::map<std::string, std::string>&);
        
    private:
        
    };

    // HACK: since we use UTF8 characters for name, this may result
    // in invalid characters in trimmed, which causes cocos2dx to leak
    // an image object.
    std::string trimNameCN(const std::string& s, int length = 10);
    
    class PvPRand
    {
    private:
//        static boost::mt19937 gen;
        static boost::minstd_rand gen;
        
    public:
        static void seed(int s);
        static int rand();
        static float randf(float max);
        static std::string toStr();
        static void fromStr(const std::string& str);
    };
}

#endif
