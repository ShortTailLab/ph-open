//
//  CCNative.cpp
//  HelloCpp
//
//  Created by Wu Zhen on 12-11-25.
//
//

#include "CCNative.h"
#include "NativeCallback.h"
#include <platform/CCFileUtils.h>
#include "extensions/utils.h"
#include <sys/system_properties.h>
#include <fstream>
#include "platform/android/jni/JniHelper.h"
#include "BuildConf.h"

#include <jni.h>

NS_CC_EXT_BEGIN

using namespace std;

extern "C"
{
    JNIEXPORT void JNICALL Java_com_stl_puzzlehero_CCNative_freeUnusedTextures(JNIEnv* env)
    {
        LOGD("Free all unused textures\n");
        cocos2d::CCTextureCache::sharedTextureCache()->removeUnusedTextures();
    }
    
    JNIEXPORT void JNICALL Java_com_stl_puzzlehero_CCNative_endDirector(JNIEnv* env)
    {
        LOGD("CCDirector end...\n");
        cocos2d::CCDirector::sharedDirector()->end();
    }
}

static bool attachNativeThreadSafely(JNIEnv*& env)
{
    int status;
    
    status = JniHelper::getJavaVM()->GetEnv((void**)&env, JNI_VERSION_1_4);
    if(status < 0)
    {
        LOGD("assuming native thread\n");
        status = JniHelper::getJavaVM()->AttachCurrentThread(&env, NULL);
        if(status < 0)
        {
            LOGD("attachNativeThreadSafely: failed to attach current thread\n");
            return false;
        }
        return true;
    }
    
    return false;
}

static void detachNativeThreadSafely(bool isAttached)
{
    if(isAttached)
        JniHelper::getJavaVM()->DetachCurrentThread();
}

static bool getStaticMethodFromObject(JNIEnv* env,
                                      const jobject& interfaceObj,
                                      const std::string& methodName,
                                      const std::string& signature,
                                      jclass& interfaceClass,
                                      jmethodID& method)
{
    // Get class interface from class instance
    interfaceClass = env->GetObjectClass(interfaceObj);
    if(!interfaceClass)
        return false;
    
    /* Find the callBack method ID */
    method = env->GetStaticMethodID(interfaceClass,
                                    methodName.c_str(),
                                    signature.c_str());
    if(!method)
        return false;
    
    return true;
}

std::string CCNative::device()
{
    char man[PROP_VALUE_MAX + 1], mod[PROP_VALUE_MAX + 1];
    int lman = __system_property_get("ro.product.manufacturer", man);
    int lmod = __system_property_get("ro.product.model", mod);
    
    std::string ret = std::string("Android|") + man + " " + mod + "|" PH_CHANNEL_NAME;

    return ret;
}

std::string CCNative::OS()
{
    return "Android";
}

std::string CCNative::OSVersion()
{
    JniMethodInfo verAddressMethod;
    JniHelper::getStaticMethodInfo(verAddressMethod,
                                   "com/stl/puzzlehero/CCNative",
                                   "getOSVersion",
                                   "()Ljava/lang/String;");
    jstring ret = (jstring)verAddressMethod.env->CallStaticObjectMethod(verAddressMethod.classID,
                                                                        verAddressMethod.methodID);
    std::string ver = cocos2d::JniHelper::jstring2string(ret);
    verAddressMethod.env->DeleteLocalRef(verAddressMethod.classID);
    return ver;
}

std::string CCNative::IPAddress()
{
    JniMethodInfo ipAddressMethod;
    JniHelper::getStaticMethodInfo(ipAddressMethod,
                                   "com/stl/puzzlehero/CCNative",
                                   "getLocalIpAddress",
                                   "()Ljava/lang/String;");
    jstring ret = (jstring)ipAddressMethod.env->CallStaticObjectMethod(ipAddressMethod.classID,
                                                                       ipAddressMethod.methodID);
    std::string ip = cocos2d::JniHelper::jstring2string(ret);
    ipAddressMethod.env->DeleteLocalRef(ipAddressMethod.classID);
    return ip;
}

std::string CCNative::MACAddress()
{
    JniMethodInfo macAddressMethod;
    JniHelper::getStaticMethodInfo(macAddressMethod,
                                   "com/stl/puzzlehero/CCNative",
                                   "getLocalMACAddress",
                                   "()Ljava/lang/String;");
    jstring ret = (jstring)macAddressMethod.env->CallStaticObjectMethod(macAddressMethod.classID,
                                                                        macAddressMethod.methodID);

    std::string mac = cocos2d::JniHelper::jstring2string(ret);
    macAddressMethod.env->DeleteLocalRef(macAddressMethod.classID);

    return mac;
}

std::string CCNative::getDocPath()
{
    return isInstalledOnExternalStorage() ? getExternalDocPath() : getInternalDocPath();
}

std::string CCNative::getCachePath()
{
    return isInstalledOnExternalStorage() ? getExternalCachePath() : getInternalCachePath();
}

static void initClassHelper(JNIEnv *env, const char *path, jobject *objptr)
{
    jclass cls = env->FindClass(path);
    if(!cls)
    {
        LOGD("initClassHelper: failed to get %s class reference", path);
        return;
    }
    jmethodID constr = env->GetMethodID(cls, "<init>", "()V");
    if(!constr)
    {
        LOGD("initClassHelper: failed to get %s constructor", path);
        return;
    }
    jobject obj = env->NewObject(cls, constr);
    if(!obj)
    {
        LOGD("initClassHelper: failed to create a %s object", path);
        return;
    }
    (*objptr) = env->NewGlobalRef(obj);
}

static jobject nativeInterface;

void CCNative::createClassCache()
{
    JNIEnv *env;
    JniHelper::getJavaVM()->GetEnv((void**)&env, JNI_VERSION_1_4);
    
    initClassHelper(env, "com/stl/puzzlehero/CCNative", &nativeInterface);
    
    LOGD("JNI cache CCNative object done...\n");
}

bool CCNative::fileExists(const std::string& path)
{
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "fileExists",
                                   "(Ljava/lang/String;)Z");
    
    jboolean val = method.env->CallStaticBooleanMethod(method.classID,
                                                       method.methodID,
                                                       PH::stringToJavaString(path)->get_jobject());
    return val;
}

void CCNative::copyFile(const std::string& from, const std::string& to)
{
    ifstream source(from, ios::binary);
    ofstream dest(to, ios::binary);
    
    istreambuf_iterator<char> begin_source(source);
    istreambuf_iterator<char> end_source;
    ostreambuf_iterator<char> begin_dest(dest);
    copy(begin_source, end_source, begin_dest);
}

void CCNative::copyAssetToPath(const std::string& from, const std::string& to)
{
    JNIEnv* env;
    bool isAttached = attachNativeThreadSafely(env);
    
    {
        jclass interface;
        jmethodID method;
        
        bool okay = getStaticMethodFromObject(env,
                                              nativeInterface,
                                              "copyAssetToPath",
                                              "(Ljava/lang/String;Ljava/lang/String;)V",
                                              interface,
                                              method);
        if(!okay)
        {
            detachNativeThreadSafely(isAttached);
            return;
        }
        
        env->CallStaticVoidMethod(interface,
                                  method,
                                  PH::stringToJavaString(from)->get_jobject(),
                                  PH::stringToJavaString(to)->get_jobject());
    }
    detachNativeThreadSafely(isAttached);
}

int CCNative::getResolutionWidth()
{
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "getResolutionWidth",
                                   "()I");
    
    jint val = method.env->CallStaticIntMethod(method.classID, method.methodID);
    return val;
}

int CCNative::getResolutionHeight()
{
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "getResolutionHeight",
                                   "()I");
    
    jint val = method.env->CallStaticIntMethod(method.classID, method.methodID);
    return val;
}

void CCNative::moveFile(const std::string& from, const std::string& to)
{
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "moveFile",
                                   "(Ljava/lang/String;Ljava/lang/String;)V");
    
    method.env->CallStaticVoidMethod(method.classID, method.methodID,
                                     PH::stringToJavaString(from)->get_jobject(),
                                     PH::stringToJavaString(to)->get_jobject());
}

void CCNative::removeFile(const std::string& file)
{
    ::unlink(file.c_str());
}

bool CCNative::isExternalStorageUsable()
{
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "isExternalStorageUsable",
                                   "()Z");
    
    jboolean val = method.env->CallStaticBooleanMethod(method.classID, method.methodID);
    return val;
}

bool CCNative::isInstalledOnExternalStorage()
{
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "isInstalledOnExternalStorage",
                                   "()Z");
    
    jboolean val = method.env->CallStaticBooleanMethod(method.classID, method.methodID);
    return val;
}

std::string CCNative::getExternalDocPath()
{
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "getExternalDocPath",
                                   "()Ljava/lang/String;");
    jstring ret = (jstring)method.env->CallStaticObjectMethod(method.classID,
                                                              method.methodID);
    
    const char* val = method.env->GetStringUTFChars(ret, NULL);
    std::string path(val);
    method.env->ReleaseStringUTFChars(ret, val);
    method.env->DeleteLocalRef(method.classID);
    return path;
}

std::string CCNative::getExternalCachePath()
{
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "getExternalCachePath",
                                   "()Ljava/lang/String;");
    jstring ret = (jstring)method.env->CallStaticObjectMethod(method.classID,
                                                              method.methodID);
    
    const char* val = method.env->GetStringUTFChars(ret, NULL);
    std::string path(val);
    method.env->ReleaseStringUTFChars(ret, val);
    method.env->DeleteLocalRef(method.classID);
    return path;
}

std::string CCNative::getInternalDocPath()
{
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "getInternalDocPath",
                                   "()Ljava/lang/String;");
    jstring ret = (jstring)method.env->CallStaticObjectMethod(method.classID,
                                                              method.methodID);
    
    const char* val = method.env->GetStringUTFChars(ret, NULL);
    std::string path(val);
    method.env->ReleaseStringUTFChars(ret, val);
    method.env->DeleteLocalRef(method.classID);
    return path;
}

std::string CCNative::getInternalCachePath()
{
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "getInternalCachePath",
                                   "()Ljava/lang/String;");
    jstring ret = (jstring)method.env->CallStaticObjectMethod(method.classID,
                                                              method.methodID);
    
    const char* val = method.env->GetStringUTFChars(ret, NULL);
    std::string path(val);
    method.env->ReleaseStringUTFChars(ret, val);
    method.env->DeleteLocalRef(method.classID);
    return path;
}

long CCNative::getInternalFreeSpaceInMB()
{
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "getInternalFreeSpaceInMB",
                                   "()J");
    
    jlong val = method.env->CallStaticIntMethod(method.classID, method.methodID);
    return val;
}

long CCNative::getExternalFreeSpaceInMB()
{
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "getExternalFreeSpaceInMB",
                                   "()J");
    
    jlong val = method.env->CallStaticIntMethod(method.classID, method.methodID);
    return val;
}

void CCNative::createAlert(const char* title,
                           const char* message,
                           const char* cancelButtonTitle)
{
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "createAlert",
                                   "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

    method.env->CallStaticVoidMethod(method.classID,
                                     method.methodID,
                                     PH::stringToJavaString(title)->get_jobject(),
                                     PH::stringToJavaString(message)->get_jobject(),
                                     PH::stringToJavaString(cancelButtonTitle)->get_jobject());
}

void CCNative::promptForExit()
{
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "promptForExit",
                                   "()V");
    
    method.env->CallStaticVoidMethod(method.classID, method.methodID);
}

void CCNative::exitGame()
{
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "exitGame",
                                   "()V");
    
    method.env->CallStaticVoidMethod(method.classID, method.methodID);
}

void CCNative::focusGLView()
{
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "focusGLView",
                                   "()V");
    
    method.env->CallStaticVoidMethod(method.classID, method.methodID);
}

void CCNative::clearLocalPushNotification()
{}

void CCNative::registerLocalPushNotification(const char* content, int iFireDate, LocalPushRepeat iRepeat)
{}

void CCNative::showAlert(CCAlertViewDelegate* delegate) {}

bool CCNative::addSkipBackupAttributeToItem(const std::string& file)
{
    return false;
}

std::string CCNative::getBuildNum()
{
    return "";
}

void CCNative::reportMemoryUsage()
{
    JniMethodInfo method;
    // report memory usage
    JniHelper::getStaticMethodInfo(method,
                                   "com/stl/puzzlehero/CCNative",
                                   "reportMemoryUsage",
                                   "()V");
    method.env->CallStaticVoidMethod(method.classID, method.methodID);
}

bool CCNative::openURL(const std::string& url)
{
    JNIEnv* env;
    bool isAttached = attachNativeThreadSafely(env);
    
    {
        jclass interface;
        jmethodID method;
        
        bool okay = getStaticMethodFromObject(env,
                                              nativeInterface,
                                              "openURL",
                                              "(Ljava/lang/String;)V",
                                              interface,
                                              method);
        if(!okay)
        {
            detachNativeThreadSafely(isAttached);
            return false;
        }
        
        env->CallStaticVoidMethod(interface,
                                  method,
                                  PH::stringToJavaString(url)->get_jobject());
    }
    detachNativeThreadSafely(isAttached);
    
    return true;
}

NS_CC_EXT_END
