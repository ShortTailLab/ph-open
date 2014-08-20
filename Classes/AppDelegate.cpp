#include "AppDelegate.h"
#include "DebugMisc.h"
#include "BuildConf.h"

#include "LoadingScene.h"
#include "GameScene.h"
#include "BattleScene.h"
#include "Board/Common.h"
#include "SimpleAudioEngine.h"
#include "SystemInform.h"
#include "CCCallLambda.h"

#include "extensions/utils.h"
#include "extensions/native/CCNative.h"

#include "../libs/scripting/lua/cocos2dx_support/CCLuaEngine.h"
#include "LuaBinding.h"

#include <curl/curl.h>

#if PH_USE_WEIBO
#include <auth/Weibo.h>
#include <auth/Weixin.h>
#endif

USING_NS_CC;
using namespace std;

#define SinaAppKey          ""
#define SinaAppSecret       ""
#define SinaCBUri           ""

#define TencentAppKey       ""
#define TencentAppSecret    ""
#define TencentCBUri        ""

#define WECHAT_KEY          ""

struct ResType
{
    CCSize size;
    std::string directory;
};

static ResType LowResConfig  =  { CCSize(320, 480), "iphone" };
static ResType HighResConfig =  { CCSize(640, 960), "iphonehd" };
static CCSize designRes = CCSize(640, 960);

extern bool isInLoginScreen;

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {}

bool AppDelegate::applicationDidFinishLaunching()
{
    curl_global_init(CURL_GLOBAL_ALL);
    
    // initialize director
    CCDirector *director = CCDirector::sharedDirector();
    director->setOpenGLView(CCEGLView::sharedOpenGLView());
    
    // Lua init stuff, must run before any lua invocation
    {
        CCLuaEngine* luaEngine = CCLuaEngine::defaultEngine();
        CCScriptEngineManager::sharedManager()->setScriptEngine(luaEngine);
        
        // register lua scripts
        luaopen_Grid2D(luaEngine->getLuaStack()->getLuaState());
        luaopen_Gem(luaEngine->getLuaStack()->getLuaState());
        luaopen_EnemyControl(luaEngine->getLuaStack()->getLuaState());
        luaopen_PlayerControl(luaEngine->getLuaStack()->getLuaState());
        luaopen_HeroControl(luaEngine->getLuaStack()->getLuaState());
        luaopen_EnemySkill(luaEngine->getLuaStack()->getLuaState());
        luaopen_SkillContext(luaEngine->getLuaStack()->getLuaState());

        // Preload lua script
        const char* files[] = {"base.lua"};
        for(int i=0; i<sizeof(files) / sizeof(const char*); ++i)
        {
            std::string path = CCFileUtils::sharedFileUtils()->fullPathForFilename(files[i]);
            // prime run to load all functions in script
            luaEngine->executeScriptFile(path.c_str());
        }
    }
    
    CCEGLView* glView = CCEGLView::sharedOpenGLView();
    glView->setDesignResolutionSize(designRes.width, designRes.height, kResolutionShowAll);
    CCSize frameSize = glView->getFrameSize();

    // Disable depth test must happen after setDesignResolutionSize
    //director->setDepthTest(false);
    
	PH::_runOnMainThreadPrepare();
    
    LOGD("* GL frame size %.1f %.1f\n", frameSize.width, frameSize.height);
    
    bool isScreenHD = frameSize.height > LowResConfig.size.height;
    bool useHighRes = isScreenHD;
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    /*
        Android could carry either SD or HD res tar. 
        Auto detects the appropriate tar.
     */
    {
        // check if hd res is present
        std::string HDTar = PH::CCNative::getDocPath() + "iphonehd." PH_VERSION ".tar";
        std::string HDHeadTar = "assets/iphonehd.Head.tar";
        bool hasHDRes = PH::CCNative::fileExists(HDTar) ||
                        PH::CCNative::fileExists(HDHeadTar);
        
        LOGD("HD exists %d\n", PH::CCNative::fileExists(PH::CCNative::getDocPath() + "iphonehd." PH_VERSION ".tar"));
        
        std::string SDTar = PH::CCNative::getDocPath() + "iphone." PH_VERSION ".tar";
        std::string SDHeadTar = "assets/iphone.Head.tar";
        bool hasSDRes = PH::CCNative::fileExists(SDTar) ||
                        PH::CCNative::fileExists(SDHeadTar);

        if(!isScreenHD && !hasSDRes)
        {
            PH::CCNative::createAlert("游戏版本错误", "游戏为高分辨率资版本，但是您的设备不支持高分辨率，请下载低分辨率版本的游戏。", "知道了");
            return true;
        }
        
        if(isScreenHD)
        {
            LOGD("screen is HD\n");
            // if only HD res is available, force HD
            if(hasHDRes)
            {
                LOGD("has HD res\n");
                if(!hasSDRes)
                    useHighRes = true;
                else
                {
                    // if user overwirte is HD
//                    if(PH::GameConfig::shared()->isAndroidHDEnabled())
//                        useHighRes = true;
//                    else
//                        useHighRes = false;
                }
            }
            else
                useHighRes = false;
        }
        else
        {
            useHighRes = false;
        }

        PH::GameConfig::shared()->setAndroidHDEnabled(useHighRes);
    }
#else
//    useHighRes = GameConfig::shared()->useHighRes;
//    useHighRes = false;
#endif
    
    std::string docRoot = PH::CCNative::getDocPath();

    ResType resType = useHighRes ? HighResConfig : LowResConfig;
    
    // Set search path to DOC_ROOT/iphone[hd] and boot/iphone[hd]
    std::vector<std::string> searchList = CCFileUtils::sharedFileUtils()->getSearchPaths();
    searchList.push_back(docRoot + resType.directory);
    searchList.push_back(std::string("boot/"));
    searchList.push_back(std::string("boot/") + resType.directory);
    CCFileUtils::sharedFileUtils()->setSearchPaths(searchList);
    
    for(const std::string& path : searchList)
        LOGD("* search path %s\n", path.c_str());
    
    // Scale scene by a factor
    // TODO: Should consider landscape.
    director->setContentScaleFactor(resType.size.height/designRes.height);

#if PH_DEBUG_BUILD
    // turn on display FPS
    director->setDisplayStats(true);
#endif
    
#if PH_USE_WEIBO
    PH::Weibo<PH::Sina>::instance(SinaAppKey, SinaAppSecret, SinaCBUri);
    PH::Weibo<PH::Tencent>::instance(TencentAppKey, TencentAppSecret, TencentCBUri);
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    PH::WeixinApi::instance().registerApp(WECHAT_KEY);
#endif
#endif

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60.0f);

#if PH_DEBUG_BUILD
    /*
    DebugHelper* helper = new DebugHelper;
    director->getScheduler()->scheduleSelector(schedule_selector(DebugHelper::reportMemoryUsage),
                                               helper,
                                               3.0f,
                                               false);
     */
    PH::CCNative::reportMemoryUsage();
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    LOGD("Internal free %lu MB, SD Card free %lu MB\n",
         PH::CCNative::getInternalFreeSpaceInMB(),
         PH::CCNative::getExternalFreeSpaceInMB());

    long needMB = useHighRes ? 17 : 30;
    
    if(PH::CCNative::isInstalledOnExternalStorage())
    {
        // check SD card is available and writable
        if(!PH::CCNative::isExternalStorageUsable())
        {
            PH::CCNative::createAlert("提示",
                                      "SD卡不存在或者不可写，请检查SD卡状态，或者将游戏安装到设备内存。",
                                      "确定");
            return true;
        }
        
        /*
         Potential bug: checking for bigger space than necessary when tar is copied and untarred.
                        Then set a limit on total file size.
         */
        if(PH::CCNative::getExternalFreeSpaceInMB() < needMB)
        {
            PH::CCNative::createAlert("提示",
                                      "SD卡空间不足，至少需要30M的空闲空间。请删除一些不需要的应用，或者将游戏安装至设备内存。",
                                      "确定");
            return true;
        }
    }
    else
    {
        if(PH::CCNative::getInternalFreeSpaceInMB() < needMB)
        {
            PH::CCNative::createAlert("提示",
                                      "设备空间不足，至少需要30M的空闲空间。请删除一些不需要的应用，或者将游戏安装至SD卡。",
                                      "确定");
            return true;
        }
    }
#endif

    director->runWithScene(PH::SplashScene::create());
    
    // Delay the excution of loading screen.
    // This avoids loading everything in applicationDidFinishLaunching
    PH::runOnMainThread([=](){
        PH::runOnMainThread([=]()
        {
            // UI Scene
            director->replaceScene(PH::LoadingScreen::create(useHighRes));
        });
    });
    PH::CCNative::clearLocalPushNotification();

    return true;
}

void DebugHelper::reportMemoryUsage(cocos2d::CCObject *o)
{
    PH::CCNative::reportMemoryUsage();
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    LOGD("Application did enter background\n");
    
//    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
    
    CCDirector::sharedDirector()->stopAnimation();
    CCDirector::sharedDirector()->pause();
    
    // if you use SimpleAudioEngine, it must be pause
    CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    LOGD("Application will enter foreground, GameScene isInitialized %d\n", PH::GameScene::isInitialized());
    static bool isShown = false;
    
    CCDirector::sharedDirector()->startAnimation();
    CCDirector::sharedDirector()->resume();
    
    PH::CCNative::clearLocalPushNotification();
    
    PH::runOnMainThread([]()
    {
        PH::runOnMainThread([]()
        {
            // make sure GameScene is initialized.
            if(PH::GameScene::isInitialized() &&
               CCDirector::sharedDirector()->getRunningScene() == PH::GameScene::getInstance())
            {
                PH::GameScene::getInstance()->checkNeedRelogin();
                if(PH::GameScene::getInstance()->zimu)
                    PH::GameScene::getInstance()->zimu->show("");
            }
            
            if(PH::GameScene::isInitialized() && !isShown && !isInLoginScreen)
            {
                isShown = true;
                PH::SystemInform::alertMessage(PH::runningScene(), "点击确认继续游戏", "继续游戏", [=]()
                {
                    isShown = false;
                    
                    // if you use SimpleAudioEngine, it must resume here
                    if(PH::GameConfig::shared()->isBGMEnabled())
                        CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
                });
            }
            else if(PH::GameScene::isInitialized() && !isShown && isInLoginScreen)
            {
                isShown = true;
                PH::SystemInform::alertMessage(PH::runningScene(), "点击确认继续游戏", "继续游戏", [=]()
               {
                   isShown = false;
                   
                   // if you use SimpleAudioEngine, it must resume here
                   if(PH::GameConfig::shared()->isBGMEnabled())
                       CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
               });
            }
        });
    });
}

bool AppDelegate::openURL(const std::string& url, const std::string& sourceApplication)
{
    return false
#if PH_USE_WEIBO
    || PH::Weibo<PH::Sina>::instance(SinaAppKey, SinaAppSecret, SinaCBUri).handleURL(url)
#endif
    ;
}
