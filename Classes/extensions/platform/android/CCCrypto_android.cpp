
#include "crypto/CCCrypto.h"

#if CC_LUA_ENGINE_ENABLED > 0
extern "C" {
#include "lua.h"
#include "lapi.h"
#include "tolua_fix.h"
}
#endif

#include "openssl/rc4.h"

NS_CC_EXT_BEGIN

int CCCrypto::getAES256KeyLength(void)
{
    assert(false && "method not implemented");
    return 0;
}

#include <android/log.h>
#define  LOG_TAG    "puzzlehero"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

std::string CCCrypto::cryptRC4(bool isDecrypt, const std::string& text, const std::string& key)
{
    RC4_KEY rc4Key;
    RC4_set_key(&rc4Key, key.size(), (const unsigned char*)key.c_str());
    
    char* out = (char*)malloc(text.size()+1);
    memset(out, 0, text.size()+1);
    
    RC4(&rc4Key, text.size(), (const unsigned char*)text.c_str(), (unsigned char*)out);
    std::string outstr((char*)out, text.size());
    
    free(out);
    return outstr;
}

#if CC_LUA_ENGINE_ENABLED > 0

cocos2d::LUA_STRING CCCrypto::cryptAES256Lua(bool isDecrypt,
                                             const void* input,
                                             int inputLength,
                                             const void* key,
                                             int keyLength)
{
    int bufferSize = inputLength + getAES256KeyLength();
    void* buffer = malloc(bufferSize);
    int dataUsed = cryptAES256(isDecrypt, input, inputLength, buffer, bufferSize, key, keyLength);
    CCLuaEngine* engine = CCLuaEngine::defaultEngine();
    engine->cleanStack();
    lua_State* L = engine->getLuaState();
    if (dataUsed > 0)
    {
        lua_pushlstring(L, static_cast<const char*>(buffer), dataUsed);
    }
    else
    {
        lua_pushnil(L);
    }
    free(buffer);
    return 1;
}

#endif

NS_CC_EXT_END
