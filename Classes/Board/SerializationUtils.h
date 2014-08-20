//
//  SerializationUtils.h
//  HelloCpp
//
//  Created by 毛 靖凯 on 13-8-29.
//
//

#ifndef HelloCpp_SerializationUtils_h
#define HelloCpp_SerializationUtils_h

#include <boost/functional/hash.hpp>
#include <boost/shared_ptr.hpp>
#include <rapidjson/document.h>
#include <cocos2d.h>
#include <string>
#include <memory>

namespace PH { //tolua_export
    
    // --- hashing string ---
    inline static size_t hashString(const std::string & tar)
    {
        static boost::hash<std::string> hasher;
        return hasher(tar);
    }
    
    // --- generic object creator ---
    template <class BaseType, class SubType>
    BaseType * GenericObjectCreationFunction(void){ return new SubType; }
    
    template<class IdType, class BaseClass>
    class GenericObjectFactory
    {
        typedef BaseClass* (*ObjectCreationFunction)(void);
        std::map<IdType, ObjectCreationFunction> m_creationFunctions;
        
    public:
        static boost::shared_ptr<GenericObjectFactory<IdType, BaseClass>> getInstance()
        {
            static boost::shared_ptr<GenericObjectFactory<IdType, BaseClass>> factory;
            if(!factory)
                factory.reset(new GenericObjectFactory<IdType, BaseClass>());
            return factory;
        }
        
        template<class SubClass>
        bool reg(IdType id)
        {
            auto findIt = m_creationFunctions.find(id);
            if (findIt == m_creationFunctions.end())
            {
                m_creationFunctions[id] = &GenericObjectCreationFunction<BaseClass, SubClass>;
                // insert() is giving me compiler errors
                return true;
            }
            return false;
        }
        
        BaseClass * create(IdType id)
        {
            auto findIt = m_creationFunctions.find(id);
            if (findIt != m_creationFunctions.end())
            {
                ObjectCreationFunction pFunc = findIt->second;
                return pFunc();
            }
            return NULL;
        }
    };
    // --- end ---
    
    // --- basic object with serialization ---
    // collisions may be raised someplace with skObjectId..
    size_t ObjectGUIDGenerator();
    
#define INITIALIZE_OBJECT() \
public: \
    virtual size_t vGetId() const { return getId(); }\
    static  size_t getId()        { static size_t skObjectId = ObjectGUIDGenerator(); return skObjectId; }
    
    class IObject { //tolua_export
    public:
        INITIALIZE_OBJECT();
        virtual void vSerialize(std::ostringstream& out) const{};
        virtual void vDeserialize(std::istringstream& in){};
        
        virtual void vSerialize(rapidjson::Value& o, rapidjson::Document::AllocatorType& alloc) const = 0;
        virtual void vDeserialize(rapidjson::Value& o) = 0;
    };     //tolua_export
    // 
    // --- end ---
    
    class IObject;
    typedef boost::shared_ptr<IObject> IObjectPtr;
    typedef GenericObjectFactory<size_t, IObject> IntObjectFactory;
    typedef GenericObjectFactory<std::string, IObject> StrObjectFactory;
    
    // --- usefull macro ---
    inline static void safeRelease(cocos2d::CCNode * parent)
    {
        if(!parent) return;
        if(parent->getParent() != NULL) parent->removeFromParent();
        parent->release();
    }
}     //tolua_export
#endif
