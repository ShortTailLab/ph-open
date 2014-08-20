#pragma once

#include "CCLuaEngine.h"

// Generic LUA argument pusher
template <typename T, size_t size, size_t i>
class CallLua
{
    static void pushval(lua_State* state, int val)
    {
        lua_pushnumber(state, val);
        lua_gettop(state);
    }
    
    static void pushval(lua_State* state, float val)
    {
        lua_pushnumber(state, val);
        lua_gettop(state);
    }
    
    static void pushval(lua_State* state, const char* val)
    {
        lua_pushstring(state, val);
        lua_gettop(state);
    }
    
    static void pushval(lua_State* state, void* val)
    {
        lua_pushlightuserdata(state, val);
        lua_gettop(state);
    }
    
    static void pushval(lua_State* state, boost::tuple<CCObject*, const char*> t)
    {
        //printf("push ccobject type\n");
        CCObject* pObject = boost::get<0>(t);
        const char* typeName = boost::get<1>(t);
        toluafix_pushusertype_ccobject(state, pObject->m_uID, &pObject->m_nLuaID, pObject, typeName);
        lua_gettop(state);
    }
    
    static void pushval(lua_State* state, boost::tuple<void*, const char*> t)
    {
        //printf("push user type\n");
        void* obj = boost::get<0>(t);
        const char* name = boost::get<1>(t);
        assert(obj != NULL && "pushed user data is nil");
        tolua_pushusertype(state, obj, name);
        lua_gettop(state);
    }
    
public:
    CallLua(lua_State* state, const char* functionName, T args)
    {
        auto ret = boost::get<size-i>(args);
        pushval(state, ret);
        CallLua<T, size, i-1>(state, functionName, args);
    }
};

template <typename T, size_t size>
class CallLua<T, size, 0>
{
public:
    CallLua(lua_State* state, const char* functionName, T args) {}
};

template <typename T>
int luacall(const char* functionName, T args, std::string& errorMessage)
{
    lua_State* m_state = CCLuaEngine::defaultEngine()->getLuaStack()->getLuaState();
    
    lua_getglobal(m_state, functionName);  /* query function by name, stack: function */
    if (!lua_isfunction(m_state, -1))
    {
        CCLOG("[LUA ERROR] name '%s' does not represent a Lua function", functionName);
        lua_pop(m_state, 1);
        return -1987;
    }
    
    CallLua<T, boost::tuples::length<T>::value, boost::tuples::length<T>::value>(m_state, functionName, args);
    
    int error = lua_pcall(m_state, boost::tuples::length<T>::value, 1, 0);         /* call function, stack: ret */
    //    lua_gc(m_state, LUA_GCCOLLECT, 0);
    
    if (error)
    {
        const char* errormsg = lua_tostring(m_state, -1);
        CCLOG("[LUA ERROR] %s", errormsg);
        errorMessage = errormsg;
        lua_pop(m_state, 1); // clean error message
        return 0;
    }
    
    // get return value
    // return value is not a number, force return 0
    if (!lua_isnumber(m_state, -1))
    {
        lua_pop(m_state, 1);
        return 0;
    }
    
    int ret = lua_tointeger(m_state, -1);
    lua_pop(m_state, 1);        /* stack: - */
    return ret;
}

