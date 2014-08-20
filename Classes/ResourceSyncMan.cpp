//
//  ResourceSyncMan.cpp
//  HelloCpp
//
//  Created by R.C on 13-1-16.
//
//

#include "ResourceSyncMan.h"
#include "zsync.h"
#include "network/HttpClient.h"
#include "cocos2d.h"
#include "extensions/utils.h"
#include "extensions/native/CCNative.h"
#include "DebugMisc.h"
#include "BuildConf.h"
#include "CCNative.h"
#include <vector>
#include <numeric>
#include <boost/lexical_cast.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/move/move.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/searching/boyer_moore.hpp>
#include <boost/tuple/tuple.hpp>

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace PH
{
    
ResourceSyncMan* ResourceSyncMan::runningMan = nullptr;

ResourceSyncMan::ResourceSyncMan(bool hdpack)
{
    state = nullptr;
    resPackHighRes = hdpack;
    docPath = CCNative::getDocPath();
    
    controlUrl = resPackHighRes
               ? "http://pt.shorttaillab.com/static/sync/iphonehd." PH_VERSION ".tar.zsync"
               : "http://pt.shorttaillab.com/static/sync/iphone." PH_VERSION ".tar.zsync";

    zsyncControlFileLastModKey = boost::algorithm::replace_all_copy(lastPathComponent(controlUrl) + ".LastModified", ".", "_");
    
    runningMan = this;
}

void ResourceSyncMan::resetLastMod()
{
    CCUserDefault::sharedUserDefault()->setStringForKey(zsyncControlFileLastModKey.c_str(), "");
    CCUserDefault::sharedUserDefault()->flush();
}

ResourceSyncMan::~ResourceSyncMan()
{
    if (state)
        zsync_end(state);
    
    runningMan = nullptr;
}

std::string ResourceSyncMan::lastPathComponent(const std::string& path)
{
    return path.substr(path.find_last_of('/')+1);
}

static bool stopDownload = false;
    
void ResourceSyncMan::startSync(boost::function<void (const ResourceSyncMan&, const SyncProgress&)> progressCB)
{
    progressCallback = boost::move(progressCB);
    if (state)
    {
        zsync_end(state);
        state = nullptr;
    }
    stopDownload = false;
    
#if PH_USE_BUNDLE_RESOURCE
    return syncDone();
#endif

    std::string zsyncControlFilePath = docPath + lastPathComponent(controlUrl);
    CCLog("resource save path: %s", zsyncControlFilePath.c_str());

    string lastModified = CCUserDefault::sharedUserDefault()->getStringForKey(zsyncControlFileLastModKey.c_str());
    
    CCHttpRequest* req = new CCHttpRequest;
    req->setRequestType(CCHttpRequest::kHttpGet);
    req->setUrl(controlUrl.c_str());
    req->setDownloadPath(zsyncControlFilePath);
    if (!lastModified.empty())
        req->addHeader("If-Modified-Since: " + lastModified);
    req->setResponseCallback([=](CCHttpResponse *resp)
    {
        if (resp->getResponseCode() == 304) // Not Modified
        {
            progress.state = NOUPDATE;
            progress.totalBytes = progress.downloadedBytes = 0;
            progressCallback(*this, this->progress);
        }
        else if (resp->getResponseCode() == 200)
        {
            std::string lastModHeader = "Last-Modified: ";
            for (const auto& header : resp->getHeaders())
                if (starts_with(header, lastModHeader))
                {
                    this->lastModString = trim_copy(header.substr(lastModHeader.length()));
                    break;
                }
            
            progress.state = LOADING_CONTROL_FILE;
            progress.totalBytes = progress.downloadedBytes = 0;
            progressCallback(*this, this->progress);
            runOnBackgroundThread([=]()
            {
                processZsyncControl(zsyncControlFilePath);
            });
        }
        else
        {
            progress.state = ERROR;
            progress.totalBytes = progress.downloadedBytes = 0;
            progressCallback(*this, this->progress);
        }
    });
    CCHttpClient::getInstance()->send(req);
    req->release();
}
    
void ResourceSyncMan::stop()
{
    progressCallback = [](const ResourceSyncMan&, const SyncProgress&){};
    stopDownload = true;
}
    
void ResourceSyncMan::processZsyncControl(const std::string& zsyncControlFilePath)
{
    FILE* f = fopen(zsyncControlFilePath.c_str(), "r");
    if ((state = zsync_begin(f)) == NULL)
        progress.state = ERROR;
    else
    {
        char* filenameC = zsync_filename(state);
        std::string file = docPath + filenameC;
        std::string temp_file=file+".temp";
        free(filenameC);

        if (!access(file.c_str(), R_OK))
            readSeedFile(file);
        if (!access(temp_file.c_str(), R_OK))
            readSeedFile(temp_file);
        if (!CCFileUtils::sharedFileUtils()->isFileExist(file) && !CCFileUtils::sharedFileUtils()->isFileExist(temp_file))
            readSeedFile(CCFileUtils::sharedFileUtils()->fullPathForFilename(resPackHighRes ? "iphonehd.Head.tar" : "iphone.Head.tar"));
        
        long long got, total;
        zsync_progress(state, &got, &total);
        progress.downloadedBytes = 0;
        progress.totalBytes = total - got;

        if (progress.totalBytes)
        {
            LOGD("zsync rename %s -> %s\n", zsync_filename(state), temp_file.c_str());
            zsync_rename_file(state, temp_file.c_str());
            progress.state = DATA_DOWNLOADING;
            runOnMainThread([=](){progressCallback(*this, this->progress);});
            downloadNewFile();
        }
        else
        {
            runOnMainThread([=]()
            {
                if (!lastModString.empty())
                {
                    CCUserDefault::sharedUserDefault()->setStringForKey(zsyncControlFileLastModKey.c_str(), lastModString);
                    CCUserDefault::sharedUserDefault()->flush();
                }
                progress.state = NOUPDATE;
                progress.totalBytes = progress.downloadedBytes = 0;
                progressCallback(*this, this->progress);
            });
        }
    }
    fclose(f);
}

void ResourceSyncMan::readSeedFile(const std::string& seed)
{
    FILE *f = fopen(seed.c_str(), "r");
    if (!f)
    {
        perror("open");
        LOGD("not using seed file %s\n", seed.c_str());
    }
    else
    {
        /* Give the contents to libzsync to read, to find any content that
         * is part of the target file. */
        zsync_submit_source_file(state, f, true);
        
        /* And close */
        fclose(f);
    }
}

void ResourceSyncMan::progressHandler(CCHttpResponse* resp, double dlnow, double dltotal)
{
    requestProgress[resp->getHttpRequest()] = dlnow;
    
    progress.downloadedBytes = 0.985 * std::accumulate(requestProgress.begin(), requestProgress.end(), 0,
                                                      [](int sum, const std::pair<void*, size_t>& req)
                                                      {
                                                          return sum + req.second;
                                                      });

    runOnMainThread([=]()
    {
        progressCallback(*this, this->progress);
    });
}
    
void ResourceSyncMan::downloadRange(zsync_receiver* receiver, int rangeId)
{
    if(rangeId >= rangeList.size())
        return;
    
    // allocate a receiver.
    int n, utype;
    const char* const* url = zsync_get_urls(state, &n, &utype);
    
    auto rangeResponseHandler = [=](CCHttpResponse *resp)
    {
        resp->retain();
        runOnBackgroundThread([=]()
        {
            auto boundary = getBoundaryString(resp->getHeaders());
            if (boundary.empty())
                addBlock(receiver, resp->getHeaders(), *resp->getResponseData());
            else
                addBlocks(receiver, *resp->getResponseData(), boundary);

            pendingWebRequests--;
            
            if (zsync_status(state) >= 2) // Done!
            {
                // free receiver
                zsync_end_receive(receiver);
                decompress();
            }
            else
            {
                long long got, total;
                zsync_progress(state, &got, &total);
                LOGD("Got %lld / %lld, rangeId %d\n", got, total, rangeId);

                if(pendingWebRequests == 0)
                {
                    zsync_end_receive(receiver);
                    LOGD("No more web request pending. but update not done yet.\n");
                    if(total - got == progress.totalBytes)   // not made any progress
                        downloadNewFile();
                    else
                        downloadNewFile();
                }
                else
                {
                    // continue to next block
                    downloadRange(receiver, masterIndex);
                    masterIndex++;
                }
            }
            resp->release();
        });
    };

    LOGD("rangeId %d, range %s\n", rangeId, rangeList[rangeId].c_str());

    CCHttpRequest* req = new CCHttpRequest;
    req->addHeader("Range: bytes=" + rangeList[rangeId]);
    
    req->setRequestType(CCHttpRequest::kHttpGet);
    req->setUrl((const char*)url);
    req->setResponseCallback(rangeResponseHandler);
    req->setProgressCallback([=](CCHttpResponse* resp, double dlnow, double dltotal)
                             {
                                 // enter from network thread
                                 this->progressHandler(resp, dlnow, dltotal);
                             });

    CCHttpClient::getInstance()->setTimeoutForRead(20);
    
    CCHttpClient::getInstance()->send(req);
    req->release();
    requestProgress[req] = 0;
}

inline std::string toRangeStr(long long start, long long end)
{
    return lexical_cast<string>(start) + "-" + lexical_cast<string>(end);
}
    
void ResourceSyncMan::downloadNewFile()
{
    LOGD("Download new file\n");
    
    int n, utype, rangeCount;
    zsync_get_urls(state, &n, &utype);
    off_t *zbyterange = zsync_needed_byte_ranges(state, &rangeCount, utype);
    
    const long long rangeSplitThreshold = 256*1024; // 256KB
    
    rangeList.clear();
    
    int rangeIndex = 0;
    
    while(rangeIndex<rangeCount)
    {
        long long leftOverSize = 0;
        std::vector<std::string> leftOver;
        
//        while(leftOverSize < 200 * 1024)
        {
            long long rangeStart = zbyterange[rangeIndex*2];
            long long rangeLen = zbyterange[rangeIndex*2+1] - rangeStart;

            while(rangeLen > rangeSplitThreshold)
            {
                rangeList.push_back(toRangeStr(rangeStart, rangeStart + rangeSplitThreshold));
                rangeStart += rangeSplitThreshold;
                rangeLen -= rangeSplitThreshold;
            }
            
            // left over
//            leftOver.push_back(toRangeStr(rangeStart, rangeStart+rangeLen));
//            leftOverSize += rangeLen;
            rangeList.push_back(toRangeStr(rangeStart, rangeStart+rangeLen));
            rangeIndex++;
        }
    
//        rangeList.push_back(join(leftOver, ","));
    }

    // allocate a receiver.
    zsync_receiver* receiver = zsync_begin_receive(state, utype);
    
    pendingWebRequests = rangeList.size();

    masterIndex = 0;
    const int MAX_CONCURRENT_REQUEST = 16;
    for(; masterIndex < MAX_CONCURRENT_REQUEST; masterIndex++)
        downloadRange(receiver, masterIndex);
    
    free(zbyterange);
}

    /*
void ResourceSyncMan::downloadWhole()
{
    int n, utype, rangeCount;
    auto url = zsync_get_urls(state, &n, &utype);
    off_t *zbyterange = zsync_needed_byte_ranges(state, &rangeCount, utype);
    zsync_receiver* receiver = zsync_begin_receive(state, utype);
    off_t rStart = zbyterange[0], rEnd = zbyterange[rangeCount * 2 -1];
    
    auto rangeResponseHandler = [this, receiver](CCHttpResponse *resp)
    {
        resp->retain();
        runOnBackgroundThread([=]()
        {
            --pendingWebRequests;
            auto boundary = getBoundaryString(resp->getHeaders());
            if (boundary.empty())
                addBlock(receiver, resp->getHeaders(), *resp->getResponseData());
            else
                addBlocks(receiver, *resp->getResponseData(), boundary);
            
            if (zsync_status(state) >= 2) // Done!
            {
                zsync_end_receive(receiver);
                decompress();
            }
            else if (pendingWebRequests == 0) // some went wrong
            {
                LOGD("Downloaded whole update region, but update not done yet.\n");
                zsync_end_receive(receiver);
                
//                long long got, total;
//                zsync_progress(state, &got, &total);
//                if (total - got == progress.totalBytes)   // not made any progress
//                {
//                    downloadWhole();
//                }
//                else    // made some progress. but not enough. try again.
                {
                    downloadNewFile();
                }
            }

            resp->release();
        });
    };

    requestProgress.clear();
    pendingWebRequests = 0;
    int reqRangeStart = rStart, reqBlock = 256 * 1024;
    CCHttpClient::getInstance()->setTimeoutForRead(300);
    while (reqRangeStart < rEnd)
    {
        int reqRangeEnd = std::min(reqRangeStart + reqBlock, (int)rEnd);
        
        CCHttpRequest* req = new CCHttpRequest;
        req->addHeader("Range: bytes=" + lexical_cast<string>(reqRangeStart) + "-" + lexical_cast<string>(reqRangeEnd));
        req->setRequestType(CCHttpRequest::kHttpGet);
        req->setUrl((const char*)url);
        req->setResponseCallback(rangeResponseHandler);
        req->setProgressCallback(boost::bind(&ResourceSyncMan::progressHandler, this, _1, _2, _3));
        CCHttpClient::getInstance()->send(req);
        req->release();

        requestProgress[req] = 0;
        reqRangeStart += reqBlock;
        ++pendingWebRequests;
    }
    free(zbyterange);
}
     */

void ResourceSyncMan::decompress()
{
    long long got, total;
    zsync_progress(state, &got, &total);
    progress.downloadedBytes = progress.totalBytes - (total - got);
    progress.state = DECOMPRESS;
    runOnMainThread([=](){progressCallback(*this, this->progress);});
    
    switch (zsync_complete(state))
    {
    case -1:
        LOGD("Aborting, download available in temp file\n");
        break;
    case 0:
        LOGD("no recognised checksum found\n");
        break;
    case 1:
        LOGD("checksum matches OK\n");
        break;
    }
    
    auto zFileName = zsync_filename(state);
    std::string file = docPath + zFileName;
    
    std::string temp_file = zsync_end(state);
    free(zFileName);
    state = NULL;
    
    if (access(temp_file.c_str(), R_OK))
        perror("access");
    
    runOnMainThread([=]()
    {
        LOGD("temp file %s\n", temp_file.c_str());
        CCNative::moveFile(temp_file, file);
        syncDone();
    });
}

void ResourceSyncMan::syncDone()
{
    if (!lastModString.empty())
    {
        CCUserDefault::sharedUserDefault()->setStringForKey(zsyncControlFileLastModKey.c_str(), lastModString);
        CCUserDefault::sharedUserDefault()->flush();
    }
    
    progress.state = DONE;
    progress.totalBytes = progress.downloadedBytes = 0;
    progressCallback(*this, this->progress);
}

std::string ResourceSyncMan::getBoundaryString(const std::vector<std::string>& headers)
{
    string boundary = "boundary=";
    for (const auto& h : headers)
        if (starts_with(h, "Content-Type: "))
        {
            if (contains(h, boundary))
            {
                auto b = h.substr(h.find(boundary) + boundary.size());
                return trim_copy(b);
            }
            break;
        }
    return "";
}

static string rangePattern = "Content-Range: bytes ";
static auto rangeFinder = make_boyer_moore(rangePattern);
template <typename RangeT>
boost::tuple<int, int> getRange(const RangeT& r)
{
    int rB=0, rE=0;
    auto rangeStart = rangeFinder(r);
    if (rangeStart != r.end())
    {
        auto findRange = make_iterator_range(rangeStart + rangePattern.size(), r.end());
        auto rangeEnd = find_first(findRange, "/");
        auto range = make_iterator_range(rangeStart + rangePattern.size(), rangeEnd.begin());
        
        std::vector<string> rangeSplit;
        string rangeStr = copy_range<string>(range);
        split(rangeSplit, rangeStr, is_any_of("-"));
        if (rangeSplit.size() == 2)
        {
            try
            {
                rB=lexical_cast<int>(rangeSplit[0]);
                rE=lexical_cast<int>(rangeSplit[1]);
            }
            catch (...)
            {
                rB = rE = 0;
            }
        }
    }
    return boost::tuples::make_tuple(rB, rE);
}

void ResourceSyncMan::addBlock(zsync_receiver* receiver, const std::vector<std::string>& headers, const std::vector<char>& data)
{
    for (const auto& h : headers)
    {
        int rB, rE, length;
        boost::tuples::tie(rB, rE) = getRange(h);
        length = rE-rB+1;
        if (rB < rE)
        {
            if (data.size() >= length)
            {
                int result = zsync_receive_data(receiver, (unsigned char*)data.data(), rB, length);
                LOGD("Single block: offset=%d, length=%zu, error: %d\n", rB, data.size(), result);
            }
            return;
        }
    }
    int result = zsync_receive_data(receiver, (unsigned char*)data.data(), 0, data.size());
    LOGD("Single block: offset=0, length=%zu, error: %d\n", data.size(), result);
}

static string emptyLinePattern = "\r\n\r\n";
static auto emptyLineFinder = make_boyer_moore(emptyLinePattern);
void ResourceSyncMan::addBlocks(zsync_receiver* receiver, const std::vector<char>& data, const std::string& boundary)
{
    typedef std::vector<char>::const_iterator dataIter;
    typedef split_iterator<dataIter> data_split_iterator;
    
    auto boundaryStr = boundary+"\r\n";
    auto bmSearch = make_boyer_moore(boundaryStr);
    auto finder = [&](dataIter begin, dataIter end) -> iterator_range<dataIter>
    {
        auto rBegin = bmSearch(begin, end);
        if (rBegin < end)
            return make_iterator_range(rBegin, rBegin + boundaryStr.size());
        else
            return make_iterator_range(rBegin, rBegin);
    };
    
    for (data_split_iterator it=make_split_iterator(data, finder);it!=data_split_iterator();++it)
    {
        int rB, rE;
        boost::tuples::tie(rB, rE) = getRange(*it);
        if (rB < rE)
        {
            int length = rE-rB+1;
            auto dataSep = emptyLineFinder(*it) + emptyLinePattern.size();
            if (std::distance(dataSep, it->end()) >= length)
            {
                auto data = make_iterator_range(dataSep, dataSep + length);
                int result = zsync_receive_data(receiver, (unsigned char*)&*data.begin(), rB, length);
                LOGD("Block offset=%d, length=%d, error: %d\n", rB, length, result);
            }
            else
            {
                LOGD("data not enough: %d/%d\n", length, std::distance(dataSep, it->end()));
            }
        }
    }
}

bool ResourceSyncMan::needDownloadResource()
{
    return progress.state >= CONTROL_LOADED && progress.totalBytes > 0 && progress.downloadedBytes < progress.totalBytes;
}

std::string ResourceSyncMan::getDocPath()
{
    return runningMan ? runningMan->docPath : "";
}
    
}
