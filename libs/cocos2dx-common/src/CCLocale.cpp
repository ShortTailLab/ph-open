/****************************************************************************
 Author: Luma (stubma@gmail.com)
 
 https://github.com/stubma/cocos2dx-common
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "CCLocale.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    #import <Foundation/Foundation.h>
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    #include "JniHelper.h"
#endif

NS_CC_BEGIN

CCLocale* CCLocale::s_instance = NULL;

CCLocale::CCLocale() {
}

CCLocale::~CCLocale() {
    s_instance = NULL;
}

CCLocale* CCLocale::sharedLocale() {
    if(!s_instance) {
        s_instance= new CCLocale();
    }
    return s_instance;
}

string CCLocale::getLanguage() {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    // get language
	NSString* lan = [[NSLocale preferredLanguages] objectAtIndex:0];
	if(!lan)
		return "en";
    else
        return [lan cStringUsingEncoding:NSUTF8StringEncoding];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    // get default locale
    JniMethodInfo t;
    JniHelper::getStaticMethodInfo(t, "java/util/Locale", "getDefault", "()Ljava/util/Locale;");
    jobject jLocale = t.env->CallStaticObjectMethod(t.classID, t.methodID);
    
    // get language
    JniHelper::getMethodInfo(t, "java/util/Locale", "getLanguage", "()Ljava/lang/String;");
    jstring jLan = (jstring)t.env->CallObjectMethod(jLocale, t.methodID);
    return JniHelper::jstring2string(jLan);
#else
    CCLOGERROR("CCLocale::getLanguage is not implemented for this platform, please finish it");
#endif
}

string CCLocale::getCountry() {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    // get country
	NSString* c = [[NSLocale currentLocale] objectForKey:NSLocaleCountryCode];
	if(!c)
		return "US";
    else
        return [c cStringUsingEncoding:NSUTF8StringEncoding];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    // get default locale
    JniMethodInfo t;
    JniHelper::getStaticMethodInfo(t, "java/util/Locale", "getDefault", "()Ljava/util/Locale;");
    jobject jLocale = t.env->CallStaticObjectMethod(t.classID, t.methodID);
    
    // get language
    JniHelper::getMethodInfo(t, "java/util/Locale", "getCountry", "()Ljava/lang/String;");
    jstring jCty = (jstring)t.env->CallObjectMethod(jLocale, t.methodID);
    return JniHelper::jstring2string(jCty);
#else
    CCLOGERROR("CCLocale::getLanguage is not implemented for this platform, please finish it");
#endif
}

NS_CC_END