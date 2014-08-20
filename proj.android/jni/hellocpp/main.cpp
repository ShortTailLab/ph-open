#include "AppDelegate.h"
#include "Console.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <extensions/native/CCNative.h>
#include <j2cpp/j2cpp.hpp>
#include <j2cpp/raw_environment.hpp>


#define  LOG_TAG    "puzzlehero"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

using namespace cocos2d;

extern "C"
{

// .so loading debug utilities
static void dlopen_impl(JNIEnv* jenv, jclass clazz, jstring path)
{
    int pathlen = jenv->GetStringUTFLength(path);
    char pathb[pathlen + 1];
    jenv->GetStringUTFRegion(path, 0, pathlen, pathb);
    
    LOGD("trying dlopen of <%s>", pathb);
    void* handle = dlopen(pathb, RTLD_NOW);
    if(!handle)
        LOGD("!!! dlopen error: %s\n", dlerror());
    else
    {
        LOGD("+++ dlopen success, handle %p\n", handle);
        dlclose(handle);
    }
}

static int registerNativeMethods(JNIEnv* env, const char* className,
                                 const JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;
    
    clazz = env->FindClass(className);
    if (clazz == NULL)
        return -1;
    
    int result = 0;
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0)
        result = -1;
    
    env->DeleteLocalRef(clazz);
    return result;
}
    
jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    LOGD("JNI Load starting...\n");
    
    JniHelper::setJavaVM(vm);

    j2cpp::shared_ptr<j2cpp::raw_environment> raw_env(new j2cpp::raw_environment(vm));
    if(!j2cpp::environment::init(raw_env))
        return JNI_ERR;
    
    extension::CCNative::createClassCache();
    /*
    LOGD("JNI Load begin...\n");
    // JNI debug facilities
    {
        JNIEnv* jenv;
        vm->GetEnv((void**) &jenv, JNI_VERSION_1_4);
        
        static const JNINativeMethod m[] =
        {
            { "dlopentest", "(Ljava/lang/String;)V", (void*) &dlopen_impl },
        };
        
        registerNativeMethods(jenv, "com/stl/puzzlehero/LibLoader", m, sizeof(m)/sizeof(*m));
    } // END JNI debug facitilies
    LOGD("JNI loaded...\n");
    */
    return JNI_VERSION_1_4;
}
    
void JNI_OnUnload(JavaVM* vm, void* reserved)
{
    LOGD("call JNI_OnUnload ~~!!");
}

void Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeInit(JNIEnv*  env, jobject thiz, jint w, jint h)
{
    if (!CCDirector::sharedDirector()->getOpenGLView())
    {
        LOGD("opengl view is empty, create new native app");
        CCEGLView *view = CCEGLView::sharedOpenGLView();
        view->setFrameSize(w, h);

        cocos2d::CCDirector::sharedDirector()->setOpenGLView(view);

        AppDelegate *pAppDelegate = new AppDelegate();
        CCApplication::sharedApplication()->run();
    }
    else
    {
        LOGD("opengl view exists, try recovering");
        ccDrawInit();
        ccGLInvalidateStateCache();
        
        CCShaderCache::sharedShaderCache()->reloadDefaultShaders();
        
        //CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
        CCTextureCache::reloadAllTextures();

        CCNotificationCenter::sharedNotificationCenter()->postNotification(EVNET_COME_TO_FOREGROUND, NULL);
        CCDirector::sharedDirector()->setGLDefaultValues(); 
    }
}

}

