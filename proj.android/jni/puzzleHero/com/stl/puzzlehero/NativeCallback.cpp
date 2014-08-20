//
//  NativeCallback.cpp
//  HelloCpp
//
//  Created by Wu Zhen on 12-11-27.
//
//

#include "NativeCallback.h"
#include "platform/android/jni/JniHelper.h"
#include <j2cpp/array.hpp>
#include <java/lang/Object.hpp>
#include <java/lang/Integer.hpp>
#include <boost/container/map.hpp>
#include <boost/move/move.hpp>
#include <boost/function.hpp>

namespace j2cpp {
    J2CPP_DEFINE_CLASS(NativeCallback,"com/stl/puzzlehero/NativeCallback")
    J2CPP_DEFINE_METHOD(NativeCallback,0,"<init>","(I)V")
    J2CPP_DEFINE_FIELD(NativeCallback,0,"nativeCallbackId","I")
}

using namespace j2cpp;

static boost::container::map<int, boost::function<void (jobjectArray)>> nativeCallbackMap;
static int nativeCallbackID = 1;

extern "C"{
    
JNIEXPORT void JNICALL Java_com_stl_puzzlehero_NativeCallback_call(JNIEnv *env, jobject, jint cbId, jobjectArray parameters)
{
    nativeCallbackMap[cbId](parameters);
    if (cbId > 0)
        nativeCallbackMap.erase(cbId);
}

}

j2cpp::NativeCallback::NativeCallback(jobject jobj)
: object<NativeCallback>(jobj)
{
}

j2cpp::NativeCallback::NativeCallback(jint a0)
: object<j2cpp::NativeCallback>(
                        call_new_object<
                        j2cpp::NativeCallback::J2CPP_CLASS_NAME,
                        j2cpp::NativeCallback::J2CPP_METHOD_NAME(0),
                        j2cpp::NativeCallback::J2CPP_METHOD_SIGNATURE(0)
                        >(a0)
                        )
{
    this->nativeCallbackId = a0;
}

j2cpp::NativeCallback::operator local_ref<j2cpp::java::lang::Runnable> () const
{
	return local_ref<j2cpp::java::lang::Runnable>(get_jobject());
}

static
j2cpp::local_ref<j2cpp::NativeCallback> addRawCallback(boost::function<void (jobjectArray)> cb, bool persistent)
{
    int id = nativeCallbackID++;
    if (persistent)
        id = -id;
    
    nativeCallbackMap.emplace(id, cb);
    return NativeCallback(id);
}

void j2cpp::retireCallback(int cbID)
{
    if (nativeCallbackMap.count(cbID))
        nativeCallbackMap.erase(cbID);
}

j2cpp::local_ref<j2cpp::NativeCallback> j2cpp::addCallback(boost::function<void ()> cb, bool persistent)
{
    return addRawCallback([=](jobjectArray){cb();}, persistent);
}

local_ref<NativeCallback> j2cpp::addCallback(boost::function<void (local_ref<java::lang::Object>, local_ref<java::lang::Object>)> cb,
                                             bool persistent)
{
    return addRawCallback([=](jobjectArray params)
    {
        j2cpp::array<j2cpp::local_ref<j2cpp::java::lang::Object>, 1> args(params);
        j2cpp::local_ref<j2cpp::java::lang::Object> arg0 = args[0];
        j2cpp::local_ref<j2cpp::java::lang::Object> arg1 = args[1];
        cb(arg0, arg1);
    }, persistent);
}

local_ref<NativeCallback> j2cpp::addCallback(boost::function<void (int, local_ref<java::lang::Object>)> cb, bool persistent)
{
    return addCallback(boost::function<void (local_ref<java::lang::Object>, local_ref<java::lang::Object>)>([=](local_ref<java::lang::Object> a0, local_ref<java::lang::Object> a1)
    {
        local_ref<java::lang::Integer> i0 = java::lang::Integer(a0.get_jobject());
        cb(i0->intValue(), a1);
    }), persistent);
}

local_ref<j2cpp::NativeCallback> j2cpp::addCallback(boost::function<void (int, const std::string&)> cb, bool persistent)
{
    return addCallback(boost::function<void (int, local_ref<java::lang::Object>)>([=](int a0, local_ref<java::lang::Object> a1)
    {
        cb(a0, cocos2d::JniHelper::jstring2string((jstring)a1.get_jobject()));
    }), persistent);
}

local_ref<j2cpp::NativeCallback> j2cpp::addCallback(boost::function<void (const std::string&, const std::string&)> cb, bool persistent)
{
    return addCallback(boost::function<void (local_ref<java::lang::Object>, local_ref<java::lang::Object>)>([=](local_ref<java::lang::Object> a0, local_ref<java::lang::Object> a1)
    {
        cb(cocos2d::JniHelper::jstring2string((jstring)a0.get_jobject()),
           cocos2d::JniHelper::jstring2string((jstring)a1.get_jobject()));
    }), persistent);
}
