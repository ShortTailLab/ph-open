//
//  ResourceSyncMan.h
//  HelloCpp
//
//  Created by R.C on 13-1-16.
//
//

#ifndef __HelloCpp__ResourceSyncMan__
#define __HelloCpp__ResourceSyncMan__

#include <boost/function.hpp>
#include <vector>
#include <string>
#include <map>

struct zsync_state;
struct zsync_receiver;

namespace cocos2d
{
    namespace extension
    {
        class CCHttpResponse;
    }
}

namespace PH
{
    enum SyncState
    {
        NOT_STARTED = 0,
        LOADING_CONTROL_FILE = 1,
        CONTROL_LOADED = 2,
        DATA_DOWNLOADING = 3,
        DECOMPRESS = 4,
        DONE = 5,
        NOUPDATE = 6,
        ERROR = 7,
    };
    
    class SyncProgress
    {
    public:
        SyncState state;
        size_t totalBytes, downloadedBytes;
    };
    
    class ResourceSyncMan
    {
    public:
        ResourceSyncMan(bool hdpack=true);
        ~ResourceSyncMan();
        
        void resetLastMod();
        
        void startSync(boost::function<void (const ResourceSyncMan&, const SyncProgress&)> progressCB);
        void stop(); // should not need to call stop manually in normal circumstances
        
        bool needDownloadResource();
        
        // hack. to get docpath in our implementation mkstemp. calls to rcksum* are in a bg thread
        // thus we cannot call CCFileUtils::writeablePath()
        static std::string getDocPath();
        
    protected:
        void processZsyncControl(const std::string&);
        void readSeedFile(const std::string&);
        void downloadNewFile();
        void downloadRange(zsync_receiver* receiver, int requestId);
        void decompress();
        void syncDone();
        
//        void downloadWhole();
        
        void progressHandler(cocos2d::extension::CCHttpResponse* resp, double dlnow, double dltotal);
        
    protected:
        std::string getBoundaryString(const std::vector<std::string>& headers);
        void addBlock(zsync_receiver* receiver, const std::vector<std::string>& headers, const std::vector<char>& data);
        void addBlocks(zsync_receiver* receiver, const std::vector<char>& data, const std::string& boundary);
        std::string lastPathComponent(const std::string& path);
        
    private:        
        zsync_state* state;
        SyncProgress progress;
        boost::function<void (const ResourceSyncMan&, const SyncProgress&)> progressCallback;
        std::map<void*, size_t> requestProgress;
        std::string lastModString; // set when sync done, enable recovery from partial sync
        std::string zsyncControlFileLastModKey, controlUrl;
        std::string docPath;
        size_t pendingWebRequests;
        bool resPackHighRes;
        
        static ResourceSyncMan* runningMan;
        
        std::vector<std::string> rangeList;
        int masterIndex;
    };
}

#endif /* defined(__HelloCpp__ResourceSyncMan__) */
