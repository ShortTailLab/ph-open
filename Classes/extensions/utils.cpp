//
//  utils.c
//  HelloCpp
//
//  Created by Wu Zhen on 12-12-10.
//
//

#include "utils.h"
#include <cocos2d.h>
#include <queue>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/make_shared.hpp>
#include <boost/smart_ptr/detail/spinlock.hpp>
#include <boost/container/vector.hpp>
#include <boost/container/slist.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/move/move.hpp>
#include <boost/bind.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>

#include "URLHelpers.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <android/app/Activity.hpp>
#include <android/content/Context.hpp>
#include <com/stl/puzzlehero/RootActivity.hpp>
#include <com/stl/puzzlehero/NativeCallback.h>
#include "support/zip_support/unzip.h"
#include "jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"

using namespace j2cpp;
using namespace j2cpp::android::app;
using namespace j2cpp::android::content;
using namespace j2cpp::android::content;
using namespace j2cpp::com::stl::puzzlehero;

#endif

namespace PH
{

USING_NS_CC;
    
class _tag_EventHandlerID
{
public:
    std::string event;
    boost::function<void (const std::string&, const std::map<std::string, std::string>&)> func;
};
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

j2cpp::local_ref<j2cpp::java::lang::String> stringToJavaString(const std::string& str)
{
    auto x = j2cpp::java::lang::String(j2cpp::raw_environment::get()->jenv()->NewStringUTF(str.c_str()));
    return x;
}
    
void runOnUIThread(boost::function<void (global_ref<Context>)> f)
{
    global_ref<Activity> ctx = (global_ref<RootActivity>)RootActivity::ctx;
    
    ctx->runOnUiThread(addCallback( boost::function<void ()>(boost::bind(f, ctx)) ));
}

#endif

class mainThreadRunner : public cocos2d::CCObject
{
public:
	mainThreadRunner()
    {
        // initialize if spin lock is use
    }

    void update(float dt)
    {
        boost::container::vector< boost::tuple<boost::function<bool ()>,
                                               boost::function<void ()>> > callbacksCopy;
        {
            LockType::scoped_lock lock(functionGuard);
            callbacksCopy.swap(functionsToRun);
        }
        
        for (auto& fp : callbacksCopy)
        {
            if (boost::tuples::get<0>( fp )())
                boost::tuples::get<1>( fp )();
            else
            {
                LockType::scoped_lock lock(functionGuard);
                functionsToRun.emplace_back(boost::move(fp));
            }
        }
        
        {
            LockType::scoped_lock lock(functionGuard);

            if (functionsToRun.empty())
                CCDirector::sharedDirector()->getScheduler()->pauseTarget(this);
        }
    }
    
    boost::container::vector< boost::tuple<boost::function<bool ()>, boost::function<void ()>> > functionsToRun;
    
    //    typedef boost::detail::spinlock LockType;
    typedef boost::mutex LockType;
    LockType functionGuard;
};
    
static mainThreadRunner* runner;
    
void runOnMainThreadWhen(boost::function<bool()> condition, boost::function<void ()> f)
{
    mainThreadRunner::LockType::scoped_lock lock(runner->functionGuard);
    runner->functionsToRun.emplace_back(boost::move(condition), boost::move(f));
    CCDirector::sharedDirector()->getScheduler()->resumeTarget(runner);
}
    
static bool AlwaysTrue() { return true; }

void runOnMainThread(boost::function<void ()> f)
{
    runOnMainThreadWhen(AlwaysTrue, f);
}

void _runOnMainThreadPrepare()
{
//    this is on main thread
    if (!runner)
    {
        runner = new mainThreadRunner;
        CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(runner, 0, true);
    }
}

class ThreadDispatchContext
{
public:
    ThreadDispatchContext()
    {
        workerThread = boost::thread(boost::bind(&ThreadDispatchContext::workerFunc, this));
    }
    
    void dispatch(boost::function<void ()> f)
    {
        boost::unique_lock<boost::mutex> lock(queueGuard);
        functionsQueue.emplace_after(functionsQueue.end(), boost::move(f));
        queueSem.notify_one();
    }
    
protected:
    void workerFunc()
    {
        while (1)
        {
            boost::unique_lock<boost::mutex> lock(queueGuard);
            while (functionsQueue.empty())
            {
                queueSem.wait(lock);
            }
            boost::function<void ()> fToRun = boost::move(functionsQueue.front());
            functionsQueue.pop_front();
            
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            @autoreleasepool
            {
#endif
            fToRun();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            }
#endif
        }
    }
    
private:
    boost::thread workerThread;
    boost::mutex queueGuard;
    boost::condition_variable queueSem;
    boost::container::slist<boost::function<void ()>> functionsQueue;
};

void runOnBackgroundThread(boost::function<void ()> f)
{
    static ThreadDispatchContext* ctx = new ThreadDispatchContext;
    ctx->dispatch(boost::move(f));
}

cocos2d::CCSprite *GetSprite(const std::string& filename)
{
    CCSprite *psprite = CCSprite::create();
    
    // Search in the cache, first
    CCSpriteFrameCache *sfc = CCSpriteFrameCache::sharedSpriteFrameCache();
    CCSpriteFrame *psf = sfc->spriteFrameByName( filename.c_str() );
    
    std::string::size_type pos;
    if (!psf && (pos=filename.find_last_of('/')) != filename.npos)
    {
        std::string lastPart(filename.begin()+pos+1, filename.end());
        psf = sfc->spriteFrameByName( lastPart.c_str() );
    }
    
    if(psf)
    {
        psprite->initWithSpriteFrame( psf );
    }
    else
    {
//        CCLog("WARNING : %s not in a PACK!", filename.c_str() );
        if(!psprite->initWithFile( filename.c_str() ))
        {
            CCLog("ERROR: %s is not in resources", filename.c_str());
            return NULL;
        }
    }
    return psprite;
}

cocos2d::CCImage* snapshot()
{
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    CCRenderTexture *tex = CCRenderTexture::create(screenSize.width, screenSize.height);
    
    tex->setPosition(screenSize.width/2, screenSize.height/2);

    tex->begin();
    CCDirector::sharedDirector()->getRunningScene()->visit();
    tex->end();
    
    CCImage* img = tex->newCCImage();
    img->autorelease();
    
    return img;
}

static std::multimap<std::string, EventHandlerID> eventMap;
EventHandlerID PH::InProcEventCentral::waitForEvent(const std::string& event,
                                                    boost::function<void (const std::string&, const std::map<std::string, std::string>&)> cb)
{
    EventHandlerID handler = boost::make_shared<_tag_EventHandlerID>();
    handler->func = boost::move(cb);
    handler->event = event;
    
    eventMap.insert(std::make_pair(event, handler));
    
    return handler;
}
    
void PH::InProcEventCentral::discardEventHandler(const EventHandlerID& cb)
{
    auto range = eventMap.equal_range(cb->event);
    for (auto it=range.first;it!=range.second;++it)
        if (it->second == cb)
        {
            eventMap.erase(it);
            break;
        }
}

void PH::InProcEventCentral::discardAllEventHandler(const std::string& event)
{
    auto range = eventMap.equal_range(event);
    eventMap.erase(range.first, range.second);
}

void PH::InProcEventCentral::signal(const std::string& event, const std::map<std::string, std::string>& params)
{
    auto range = eventMap.equal_range(event);
    for (auto it=range.first;it!=range.second;++it)
    {
        auto f = it->second->func;
        runOnMainThread([f, event, params]()
        {
            f(event, boost::move(params));
        });
    }
}

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
bool resourcePresentForLoadingFromGzip(const std::string& zipFilePath, const std::string& filename)
{
    unzFile pFile = NULL;
    bool present = false;
    
    do
    {
        CC_BREAK_IF(zipFilePath.empty()|| filename.empty());
        
        pFile = unzOpen(zipFilePath.c_str());
        CC_BREAK_IF(!pFile);
        
        int nRet = unzLocateFile(pFile, filename.c_str(), 1);
        CC_BREAK_IF(UNZ_OK != nRet);
        
        present = true;
        unzCloseCurrentFile(pFile);
    } while (0);
    
    if (pFile) unzClose(pFile);

    return present;
}
#endif
    
bool resourcePresentForLoading(const std::string& filename)
{
    std::string fullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(filename.c_str());
    return CCFileUtils::sharedFileUtils()->isFileExist(fullPath);
}
    
    std::string trimNameCN(const std::string& s, int length)
    {
        std::wstring w = URLHelper::str2wstr(s);
        return URLHelper::wstr2str( w.size() < length ? w : w.substr(0, length-2).append(L"..."));
    }
    
    
    //boost::mt19937 PvPRand::gen;
    boost::minstd_rand PvPRand::gen;
    
    void PvPRand::seed(int s)
    {
        gen.seed(s);
    }
    
    int PvPRand::rand()
    {
        boost::random::uniform_int_distribution<>r(0, INT_MAX);
        return r(gen);
    }
    
    float PvPRand::randf(float max)
    {
        // BUG: uniform real is not stable cross processors.
        // ex. iphone4s produces different rand sequence than iphone5
        //boost::random::uniform_real_distribution<> r(0.0f, max);
        //float val = r(gen);
        
        boost::random::uniform_int_distribution<>r(0, INT_MAX);
        int val = r(gen);
        float xval = (float)val / INT_MAX * max;
        return xval;
    }
    
    std::string PvPRand::toStr()
    {
        std::stringstream ss;
        ss << gen;
        return ss.str();
    }

    void PvPRand::fromStr(const std::string& str)
    {
        std::stringstream ss(str);
        ss >> gen;
    }
}
