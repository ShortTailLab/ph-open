//
//  CCNative.cpp
//  HelloCpp
//
//  Created by Wu Zhen on 12-11-25.
//
//

#include "CCNative.h"

NS_CC_EXT_BEGIN

std::string CCNative::device()
{
    return "Win32|Test";
}

std::string CCNative::OS()
{
    return "Win32";
}

std::string CCNative::IPAddress()
{
    return "127.0.0.1";
}

NS_CC_EXT_END
