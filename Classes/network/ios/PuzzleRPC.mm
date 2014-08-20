//
//  PuzzleRPC.cpp
//  HelloCpp
//
//  Created by R.C on 12-10-29.
//
//

#include "PuzzleRPC.h"
#include "Reachability.h"

using namespace PH;

bool PuzzleRPC::isLocalWiFiAvailable(void)
{
    return [[Reachability reachabilityForLocalWiFi] currentReachabilityStatus] != NotReachable;
}

bool PuzzleRPC::isInternetConnectionAvailable(void)
{
    return [[Reachability reachabilityForInternetConnection] currentReachabilityStatus] != NotReachable;
}

bool PuzzleRPC::isHostNameReachable(const std::string& hostName)
{
    NSString *hostName_ = [NSString stringWithUTF8String:hostName.c_str()];
    return [[Reachability reachabilityWithHostName:hostName_] currentReachabilityStatus] != NotReachable;
}

CCNetworkStatus PuzzleRPC::getInternetConnectionStatus(void) 
{
    NetworkStatus status = [[Reachability reachabilityForInternetConnection] currentReachabilityStatus];
    if (status == ReachableViaWiFi) return CCNetworkStatusReachableViaWiFi;
    if (status == ReachableViaWWAN) return CCNetworkStatusReachableViaWWAN;
    return CCNetworkStatusNotReachable;
}
