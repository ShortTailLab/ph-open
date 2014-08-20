//
//  NativeCallback.h
//  HelloCpp
//
//  Created by Wu Zhen on 12-11-27.
//
//

#ifndef __HelloCpp__NativeCallback__
#define __HelloCpp__NativeCallback__

#include <j2cpp/j2cpp.hpp>
#include <java/lang/Runnable.hpp>
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>
#include <string>

namespace j2cpp {
    class NativeCallback : public j2cpp::object<NativeCallback>
    {
    public:
        J2CPP_DECLARE_CLASS
		J2CPP_DECLARE_METHOD(0)
        J2CPP_DECLARE_FIELD(0)

        explicit NativeCallback(jobject);
        explicit NativeCallback(jint);
        
        operator local_ref<j2cpp::java::lang::Runnable>() const;

        int nativeCallbackId;
    };

    void retireCallback(int cbID);

    
    local_ref<NativeCallback> addCallback(boost::function<void ()>, bool persistent=false);
    
    
    local_ref<NativeCallback> addCallback(boost::function<void (local_ref<java::lang::Object>, local_ref<java::lang::Object>)>,
                                          bool persistent=false);
    local_ref<NativeCallback> addCallback(boost::function<void (int, local_ref<java::lang::Object>)>, bool persistent=false);
    local_ref<NativeCallback> addCallback(boost::function<void (int, const std::string&)>, bool persistent=false);
    local_ref<NativeCallback> addCallback(boost::function<void (const std::string&, const std::string&)>, bool persistent=false);
}

#endif /* defined(__HelloCpp__NativeCallback__) */
