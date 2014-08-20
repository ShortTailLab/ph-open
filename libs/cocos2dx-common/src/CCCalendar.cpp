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
#include "CCCalendar.h"
#include "CCUtils.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    #import <Foundation/Foundation.h>
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    #include "JniHelper.h"
#endif

NS_CC_BEGIN

CCCalendar* CCCalendar::s_instance = NULL;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

static int DAY_OF_MONTH;
static int DAY_OF_WEEK;
static int MONTH;
static int YEAR;
static int HOUR_OF_DAY;
static int MINUTE;
static int SECOND;
static jmethodID CALENDAR_GET;

static void setupAndroidCalendar() {
    // get klass of calendard
	JNIEnv* env = CCUtils::getJNIEnv();
    jclass klass = env->FindClass("java/util/Calendar");
    
    // populate field
    jfieldID fid = env->GetStaticFieldID(klass, "DAY_OF_MONTH", "I");
    DAY_OF_MONTH = env->GetStaticIntField(klass, fid);
    fid = env->GetStaticFieldID(klass, "DAY_OF_WEEK", "I");
    DAY_OF_WEEK = env->GetStaticIntField(klass, fid);
    fid = env->GetStaticFieldID(klass, "MONTH", "I");
    MONTH = env->GetStaticIntField(klass, fid);
    fid = env->GetStaticFieldID(klass, "YEAR", "I");
    YEAR = env->GetStaticIntField(klass, fid);
    fid = env->GetStaticFieldID(klass, "HOUR_OF_DAY", "I");
    HOUR_OF_DAY = env->GetStaticIntField(klass, fid);
    fid = env->GetStaticFieldID(klass, "MINUTE", "I");
    MINUTE = env->GetStaticIntField(klass, fid);
    fid = env->GetStaticFieldID(klass, "SECOND", "I");
    SECOND = env->GetStaticIntField(klass, fid);
    CALENDAR_GET = env->GetMethodID(klass, "get", "(I)I");
	
	// clear
	env->DeleteLocalRef(klass);
}

static jobject createAndroidCalendar(float time) {
    // get GregorianCalendar
	JNIEnv* env = CCUtils::getJNIEnv();
	jclass klass = env->FindClass("java/util/GregorianCalendar");
    jmethodID mid = env->GetMethodID(klass, "<init>", "()V");
    jobject jCalendar = env->NewObject(klass, mid);
    
    // set time
	mid = env->GetMethodID(klass, "setTimeInMillis", "(J)V");
    env->CallVoidMethod(jCalendar, mid, (jlong)(time * 1000.0f));
    
    return jCalendar;
}

#endif // #if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

CCCalendar::CCCalendar() {
    m_time = (double)CCUtils::currentTimeMillis() / 1000.0f;
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    setupAndroidCalendar();
#endif
}

CCCalendar::~CCCalendar() {
    s_instance = NULL;
}

CCCalendar* CCCalendar::sharedCalendar() {
    if(!s_instance) {
        s_instance = new CCCalendar();
    }
    return s_instance;
}

void CCCalendar::setNow() {
    m_time = (double)CCUtils::currentTimeMillis() / 1000.0f;
}

int CCCalendar::getYear() {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    NSCalendar* c = [NSCalendar currentCalendar];
    NSDate* d = [NSDate dateWithTimeIntervalSince1970:m_time];
    NSDateComponents* dc = [c components:NSYearCalendarUnit fromDate:d];
    return dc.year;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    jobject c = createAndroidCalendar(m_time);
	JNIEnv* env = CCUtils::getJNIEnv();
    int ret = env->CallIntMethod(c, CALENDAR_GET, YEAR);
    env->DeleteLocalRef(c);
    return ret;
	return 0;
#else
    CCLOGERROR("CCCalendar::getYear is not implemented for this platform, please finish it");
    return 0;
#endif
}

int CCCalendar::getMonth() {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    NSCalendar* c = [NSCalendar currentCalendar];
    NSDate* d = [NSDate dateWithTimeIntervalSince1970:m_time];
    NSDateComponents* dc = [c components:NSMonthCalendarUnit fromDate:d];
    return dc.month;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    jobject c = createAndroidCalendar(m_time);
	JNIEnv* env = CCUtils::getJNIEnv();
    int ret = env->CallIntMethod(c, CALENDAR_GET, MONTH) + 1;
    env->DeleteLocalRef(c);
    return ret;
#else
    CCLOGERROR("CCCalendar::getMonth is not implemented for this platform, please finish it");
    return 0;
#endif
}

int CCCalendar::getDay() {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    NSCalendar* c = [NSCalendar currentCalendar];
    NSDate* d = [NSDate dateWithTimeIntervalSince1970:m_time];
    NSDateComponents* dc = [c components:NSDayCalendarUnit fromDate:d];
    return dc.day;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    jobject c = createAndroidCalendar(m_time);
	JNIEnv* env = CCUtils::getJNIEnv();
    int ret = env->CallIntMethod(c, CALENDAR_GET, DAY_OF_MONTH);
    env->DeleteLocalRef(c);
    return ret;
#else
    CCLOGERROR("CCCalendar::getDay is not implemented for this platform, please finish it");
    return 0;
#endif
}

int CCCalendar::getWeekday() {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    NSCalendar* c = [NSCalendar currentCalendar];
    NSDate* d = [NSDate dateWithTimeIntervalSince1970:m_time];
    NSDateComponents* dc = [c components:NSWeekdayCalendarUnit fromDate:d];
    return dc.weekday;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    jobject c = createAndroidCalendar(m_time);
	JNIEnv* env = CCUtils::getJNIEnv();
    int ret = env->CallIntMethod(c, CALENDAR_GET, DAY_OF_WEEK);
    env->DeleteLocalRef(c);
    return ret;
#else
    CCLOGERROR("CCCalendar::getDay is not implemented for this platform, please finish it");
    return 0;
#endif
}

int CCCalendar::getHour() {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    NSCalendar* c = [NSCalendar currentCalendar];
    NSDate* d = [NSDate dateWithTimeIntervalSince1970:m_time];
    NSDateComponents* dc = [c components:NSHourCalendarUnit fromDate:d];
    return dc.hour;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    jobject c = createAndroidCalendar(m_time);
	JNIEnv* env = CCUtils::getJNIEnv();
    int ret = env->CallIntMethod(c, CALENDAR_GET, HOUR_OF_DAY);
    env->DeleteLocalRef(c);
    return ret;
#else
    CCLOGERROR("CCCalendar::getHour is not implemented for this platform, please finish it");
    return 0;
#endif
}

int CCCalendar::getMinute() {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    NSCalendar* c = [NSCalendar currentCalendar];
    NSDate* d = [NSDate dateWithTimeIntervalSince1970:m_time];
    NSDateComponents* dc = [c components:NSMinuteCalendarUnit fromDate:d];
    return dc.minute;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    jobject c = createAndroidCalendar(m_time);
	JNIEnv* env = CCUtils::getJNIEnv();
    int ret = env->CallIntMethod(c, CALENDAR_GET, MINUTE);
    env->DeleteLocalRef(c);
    return ret;
#else
    CCLOGERROR("CCCalendar::getHour is not implemented for this platform, please finish it");
    return 0;
#endif
}

int CCCalendar::getSecond() {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    NSCalendar* c = [NSCalendar currentCalendar];
    NSDate* d = [NSDate dateWithTimeIntervalSince1970:m_time];
    NSDateComponents* dc = [c components:NSSecondCalendarUnit fromDate:d];
    return dc.second;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    jobject c = createAndroidCalendar(m_time);
	JNIEnv* env = CCUtils::getJNIEnv();
    int ret = env->CallIntMethod(c, CALENDAR_GET, SECOND);
    env->DeleteLocalRef(c);
    return ret;
#else
    CCLOGERROR("CCCalendar::getHour is not implemented for this platform, please finish it");
    return 0;
#endif
}

NS_CC_END