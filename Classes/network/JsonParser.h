//
//  JsonParser.h
//  HelloCpp
//
//  Created by R.C on 12-10-27.
//
//

#ifndef __HelloCpp__JsonParser__
#define __HelloCpp__JsonParser__

#include <memory>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

namespace PH
{
    template <class T>
    T* parse(const rapidjson::Value& v)
    {
        std::auto_ptr<T> p(new T);
        parse(v, *p);
        return p.release();
    }
    
    template <class T>
    T parseV(const rapidjson::Value& v)
    {
        T ret;
        parse(v, ret);
        return ret;
    }
    
    template <class T>
    void parse(const rapidjson::Value&, T&);
    
    
    struct Ignore{};
}

#endif /* defined(__HelloCpp__JsonParser__) */
