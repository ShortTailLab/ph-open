
#include "crypto/CCCrypto.h"

#if CC_LUA_ENGINE_ENABLED > 0
extern "C" {
#include "lua.h"
#include "lapi.h"
#include "tolua_fix.h"
}
#endif

#import <CommonCrypto/CommonCryptor.h>
#include "openssl/rc4.h"
NS_CC_EXT_BEGIN

int CCCrypto::getAES256KeyLength(void)
{
    return kCCKeySizeAES256;
}

static std::string bufferToStr(const char* str, int len)
{
    std::string total;
    for(int i=0; i<len; i++)
    {
        char buffer[32];
        sprintf(buffer, "%d ", str[i]);
        total += buffer;
    }
    return total;
}

//int CCCrypto::cryptAES256(bool isDecrypt,
//                          const void* input,
//                          int inputLength,
//                          void* output,
//                          int outputBufferLength,
//                          const void* key,
//                          int keyLength)
std::string CCCrypto::cryptRC4(bool isDecrypt, const std::string& text, const std::string& key)
{
    int maxKeyLength = kCCKeySizeMaxRC4;
    unsigned char key_[maxKeyLength];
    memset(key_, 0, sizeof(key_));
    memcpy(key_, key.c_str(), key.size() > maxKeyLength ? maxKeyLength : key.size());
    
    size_t bufferSize = 0;
    size_t dataUsed = 0;
    std::string ret;
    
    CCCryptorStatus status = CCCrypt(isDecrypt ? kCCDecrypt : kCCEncrypt,
                                     kCCAlgorithmRC4,
                                     0,
                                     key_, std::min<size_t>(kCCKeySizeMaxRC4, key.size()),
                                     NULL,
                                     text.c_str(), text.size(),
                                     NULL, 0,
                                     &bufferSize);
    
    void* buffer = malloc(bufferSize);
    status = CCCrypt(isDecrypt ? kCCDecrypt : kCCEncrypt,
                     kCCAlgorithmRC4,
                     0,
                     key_, std::min<size_t>(kCCKeySizeMaxRC4, key.size()),
                     NULL,
                     text.c_str(), text.size(),
                     buffer, bufferSize,
                     &dataUsed);

    switch (status)
    {
        case kCCSuccess:
            ret = std::string((char*)buffer, bufferSize);
            break;
        case kCCParamError:
            CCLOG("[CCCrypto] cryptRC4(%s) param error", isDecrypt ? "Decrypt" : "Encrypt");
            break;
        case kCCBufferTooSmall:
            CCLOG("[CCCrypto] cryptRC4(%s) buffer too small", isDecrypt ? "Decrypt" : "Encrypt");
            break;
        case kCCMemoryFailure:
            CCLOG("[CCCrypto] cryptRC4(%s) memory failure", isDecrypt ? "Decrypt" : "Encrypt");
            break;
        case kCCAlignmentError:
            CCLOG("[CCCrypto] cryptRC4(%s) alignment error", isDecrypt ? "Decrypt" : "Encrypt");
            break;
        case kCCDecodeError:
            CCLOG("[CCCrypto] cryptRC4(%s) decode error", isDecrypt ? "Decrypt" : "Encrypt");
            break;
        case kCCUnimplemented:
            CCLOG("[CCCrypto] cryptRC4(%s) unimplemented", isDecrypt ? "Decrypt" : "Encrypt");
            break;
        default:
            CCLOG("[CCCrypto] cryptRC4(%s) unknown error", isDecrypt ? "Decrypt" : "Encrypt");
    }
    
    free(buffer);
    return ret;
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
