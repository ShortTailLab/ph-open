#include <cocos2d.h>
#include <string>
#include "CCNative.h"
#include "ResourceSyncMan.h"


using namespace std;
USING_NS_CC;
USING_NS_CC_EXT;

#define  LOG_TAG    "puzzlehero"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

extern "C"{

int mymkstemp(char* filename)
{
    string docPath = PH::ResourceSyncMan::getDocPath();
    string filenameStr = tmpnam(filename);
    string tmpFilePath = docPath + filenameStr.substr(filenameStr.find_last_of('/')+1);

    strcpy(filename, tmpFilePath.c_str());
    
    FILE* f = fopen(tmpFilePath.c_str(), "wb+");
    return f ? fileno(f) : -1;
}

}
