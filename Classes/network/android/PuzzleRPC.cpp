//
//  PuzzleRPC.cpp
//  HelloCpp
//
//  Created by R.C on 12-11-26.
//
//

#include "PuzzleRPC.h"
#include "platform/android/jni/JniHelper.h"

namespace PH{

bool PuzzleRPC::isLocalWiFiAvailable(void)
{
    JniMethodInfo isNetworkAvailable;
    JniHelper::getStaticMethodInfo(isNetworkAvailable, "com/stl/puzzlehero/CCNative", "isNetworkAvailable", "()Z");
    bool ret = isNetworkAvailable.env->CallStaticBooleanMethod(isNetworkAvailable.classID, isNetworkAvailable.methodID);

    isNetworkAvailable.env->DeleteLocalRef(isNetworkAvailable.classID);
    return ret;
}

bool PuzzleRPC::isInternetConnectionAvailable(void)
{
    JniMethodInfo isWIFIAvailable;
    JniHelper::getStaticMethodInfo(isWIFIAvailable, "com/stl/puzzlehero/CCNative", "isWIFIAvailable", "()Z");
    bool ret = isWIFIAvailable.env->CallStaticBooleanMethod(isWIFIAvailable.classID, isWIFIAvailable.methodID);
    
    isWIFIAvailable.env->DeleteLocalRef(isWIFIAvailable.classID);
    return ret;
}

bool PuzzleRPC::isHostNameReachable(const std::string& hostName)
{
    return false;
}

CCNetworkStatus PuzzleRPC::getInternetConnectionStatus(void)
{
    if (isLocalWiFiAvailable()) return CCNetworkStatusReachableViaWiFi;
    if (isInternetConnectionAvailable()) return CCNetworkStatusReachableViaWWAN;
    return CCNetworkStatusNotReachable;
}

}
