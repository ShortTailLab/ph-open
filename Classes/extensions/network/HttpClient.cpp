/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2012 greathqy
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HttpClient.h"
// #include "platform/CCThread.h"

#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <list>
#include <boost/smart_ptr/detail/spinlock.hpp>
#include <boost/thread/condition_variable.hpp>

#include "curl/curl.h"

NS_CC_EXT_BEGIN
using namespace boost;

//typedef boost::detail::spinlock                     MutexType;
typedef boost::mutex                                MutexType;
typedef std::list<CCHttpRequest*>                   RequestQueueType;
typedef std::list<CCHttpResponse*>                  ResponseQueueType;

static pthread_t        networkThread       =   NULL;
//static MutexType        requestQueueMutex   =   BOOST_DETAIL_SPINLOCK_INIT;
//static MutexType        responseQueueMutex  =   BOOST_DETAIL_SPINLOCK_INIT;

static MutexType        requestQueueMutex;
static MutexType        responseQueueMutex;

static bool need_quit = false;

static RequestQueueType         requestQueue;
static ResponseQueueType        responseQueue;

static CCHttpClient *s_pHttpClient = NULL; // pointer to singleton

static char s_errorBuffer[CURL_ERROR_SIZE];

static CURLM* curlMultiHandle = NULL;

typedef size_t (*write_callback)(void *ptr, size_t size, size_t nmemb, void *stream);


// Callback function used by libcurl for collect response data
size_t writeData(void *ptr, size_t size, size_t nmemb, void *stream)
{
    CCHttpResponse* resp = (CCHttpResponse*)stream;
    
    std::vector<char> *recvBuffer = resp->getResponseData();
    size_t sizes = size * nmemb;
    
    // add data to the end of recvBuffer
    // write data maybe called more than once in a single request
    recvBuffer->insert(recvBuffer->end(), (char*)ptr, (char*)ptr+sizes);
    
    return sizes;
}

size_t writeDataToFile(void *ptr, size_t size, size_t nmemb, void *stream)
{
    CCHttpResponse* resp = (CCHttpResponse*)stream;
    
    int written = fwrite(ptr, size, nmemb, resp->fileHandle);
    resp->downloadSize += written;
    return written;
}

size_t writeHeader(void *ptr, size_t size, size_t nmemb, void *stream)
{
    char* b = (char*)ptr;
    CCHttpResponse* resp = (CCHttpResponse*)stream;
    
    int result = 0;
    if (resp != NULL)
    {
        std::string s(b, size * nmemb);
        resp->getHeaders().push_back(s);
        result = size * nmemb;
    }
    return result;
}

int progressCallback(CCHttpResponse *response, double dltotal, double dlnow, double ultotal, double ulnow)
{
    response->getHttpRequest()->getProgressCallback()(response, dlnow, dltotal);
    return 0;
}


// Prototypes
bool configureCURL(CURL *handle);
int processGetTask(CCHttpRequest *request, write_callback callback, write_callback headerCB, CCHttpResponse* resp);
int processPostTask(CCHttpRequest *request, write_callback callback, write_callback headerCB, CCHttpResponse* resp);

static inline
void runCurlEvents()
{
    int runningHandles = 0;

    CURLMcode ret;
    do{
        ret = curl_multi_perform(curlMultiHandle, &runningHandles);
    } while (ret == CURLM_CALL_MULTI_PERFORM);
    
    {
        CURLMsg* msg = NULL;
        CCHttpResponse* resp;
        int msgLeft = 0;

        while ((msg=curl_multi_info_read(curlMultiHandle, &msgLeft)))
        {
            if (msg->msg == CURLMSG_DONE)
            {
                CURL* curl = msg->easy_handle;
                CURLcode code = msg->data.result;
                int responseCode = -1;
                curl_easy_getinfo(curl, CURLINFO_PRIVATE, &resp);
                if (code == CURLE_OK)
                {
                    code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
                    if (code != CURLE_OK || (responseCode != 200 && responseCode != 304 && responseCode != 206)) {
                        code = CURLE_HTTP_RETURNED_ERROR;
                    }
                }
                if (!resp->getHttpRequest()->getDownloadPath().empty())
                {
                    fclose(resp->fileHandle);
                    resp->fileHandle = NULL;
                }
                
                resp->setErrorBuffer(s_errorBuffer);
                resp->setSucceed(code == CURLE_OK);
                resp->setResponseCode(responseCode);
                
                curl_multi_remove_handle(curlMultiHandle, curl);
                curl_easy_cleanup(curl);
                curl_slist_free_all((curl_slist*)resp->headerList);
                
                if (code != CURLE_OK)
                    CCLog("HTTPClient: curl perform task error: %d, %s, %d", code, curl_easy_strerror(code), responseCode);
                
                MutexType::scoped_lock lock(responseQueueMutex);
                responseQueue.push_back(resp);
                
                CCDirector::sharedDirector()->getScheduler()->resumeTarget(CCHttpClient::getInstance());
            }
        }
    }
    
    if (runningHandles)
    {
        curl_multi_wait(curlMultiHandle, NULL, 0, 50, NULL);
    }
    else
    {
        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 20*1000;

        select(0, NULL, NULL, NULL, &timeout);
    }
}

void processRequest(CCHttpRequest *request)
{
    CCHttpResponse *response = new CCHttpResponse(request);
    
    // request's refcount = 2 here, it's retained by HttpRespose constructor
    request->release();
    int retValue = 0;
    
    if (!request->getDownloadPath().empty())
    {
        response->downloadSize = 0;
        response->fileHandle = fopen(request->getDownloadPath().c_str(), "wb");
    }
    
    // Process the request -> get response packet
    switch (request->getRequestType())
    {
        case CCHttpRequest::kHttpGet: // HTTP GET
            retValue = processGetTask(request,
                                      request->getDownloadPath().empty() ? writeData : writeDataToFile,
                                      writeHeader,
                                      response);
            break;
            
        case CCHttpRequest::kHttpPost: // HTTP POST
            retValue = processPostTask(request,
                                       request->getDownloadPath().empty() ? writeData : writeDataToFile,
                                       writeHeader,
                                       response);
            break;
            
        default:
            CCAssert(false, "CCHttpClient: unkown request type, only GET and POSt are supported");
            break;
    }
}

// Worker thread
/*[[noreturn]] */static
void* networkThreadFunc(void *data)
{
    curlMultiHandle = curl_multi_init();
    
    while (!need_quit)
    {
        {
            MutexType::scoped_lock lock(requestQueueMutex);
            
            for (auto req : requestQueue)
            {
                processRequest(req);
            }
            requestQueue.clear();
        }

        runCurlEvents();
    }
    
    // cleanup: if worker thread received quit signal, clean up un-completed request queue
    
    MutexType::scoped_lock lock(requestQueueMutex);
    for (auto req : requestQueue)
        req->release();
    requestQueue.clear();

    pthread_exit(NULL);
}

//Configure curl's timeout property
bool configureCURL(CURL *handle)
{
    if (!handle) {
        return false;
    }
    
    int32_t code;
    code = curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, s_errorBuffer);
    if (code != CURLE_OK) {
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
    if (code != CURLE_OK) {
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_TIMEOUT, CCHttpClient::getInstance()->getTimeoutForRead());
    if (code != CURLE_OK) {
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, CCHttpClient::getInstance()->getTimeoutForConnect());
    if (code != CURLE_OK) {
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "");
    if (code != CURLE_OK) {
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
    if (code != CURLE_OK) {
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0L);
    if (code != CURLE_OK) {
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1L);
    if (code != CURLE_OK) {
        return false;
    }
    
    return true;
}

//Process Get Request
int processGetTask(CCHttpRequest *request, write_callback callback, write_callback headerCB, CCHttpResponse* resp)
{
    CURLcode code = CURL_LAST;
    CURL *curl = curl_easy_init();
    
    do {
        if (!configureCURL(curl)) 
        {
            break;
        }
        
        /* handle custom header data */
        /* create curl linked list */
        struct curl_slist *cHeaders=NULL;
        /* get custom header data (if set) */
        for (const auto& h : request->getHeaders())
            /* append custom headers one by one */
            cHeaders=curl_slist_append(cHeaders, h.c_str());
        if (cHeaders)
        {
            code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, cHeaders);
            if (code != CURLE_OK) {
                break;
            }
        }
        resp->headerList = cHeaders;
        
        code = curl_easy_setopt(curl, CURLOPT_URL, request->getUrl());
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_PRIVATE, resp);
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, resp);
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCB);
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_WRITEHEADER, resp);
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progressCallback);
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, resp);
        CC_BREAK_IF(code != CURLE_OK);
        
        CURLMcode mcode = curl_multi_add_handle(curlMultiHandle, curl);
        if (mcode != CURLM_OK)
            CCLog("HTTPClient: curl_multi_add_handle error: %d, %s", mcode, curl_multi_strerror(mcode));
    } while (0);
    
    if (code != CURLE_OK)
        CCLog("HTTPClient: curl perform task error: %d, %s", code, curl_easy_strerror(code));
    
    return (code == CURLE_OK ? 0 : 1);
}

//Process POST Request
int processPostTask(CCHttpRequest *request, write_callback callback, write_callback headerCB, CCHttpResponse* resp)
{
    CURLcode code = CURL_LAST;
    CURL *curl = curl_easy_init();
    
    do {
        if (!configureCURL(curl)) {
            break;
        }
        
        /* handle custom header data */
        /* create curl linked list */
        struct curl_slist *cHeaders=NULL;
        /* get custom header data (if set) */
        for (const auto& h : request->getHeaders())
            /* append custom headers one by one */
            cHeaders=curl_slist_append(cHeaders, h.c_str());
        if (cHeaders)
        {
            code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, cHeaders);
            if (code != CURLE_OK) {
                break;
            }
        }
        resp->headerList = cHeaders;
        
        code = curl_easy_setopt(curl, CURLOPT_URL, request->getUrl());
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_PRIVATE, resp);
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, resp);
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_POST, 1);
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request->getRequestData());
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request->getRequestDataSize());
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCB);
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_WRITEHEADER, resp);
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progressCallback);
        CC_BREAK_IF(code != CURLE_OK);
        
        code = curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, resp);
        CC_BREAK_IF(code != CURLE_OK);
        
        CURLMcode mcode = curl_multi_add_handle(curlMultiHandle, curl);
        if (mcode != CURLM_OK)
            CCLog("HTTPClient: curl_multi_add_handle error: %d, %s", mcode, curl_multi_strerror(mcode));
    } while (0);
    
    if (code != CURLE_OK)
        CCLog("HTTPClient: curl perform task error: %d, %s", code, curl_easy_strerror(code));
    return (code == CURLE_OK ? 0 : 1);    
}

// HttpClient implementation
CCHttpClient* CCHttpClient::getInstance()
{
    if (s_pHttpClient == NULL) {
        s_pHttpClient = new CCHttpClient();
    }
    
    return s_pHttpClient;
}

void CCHttpClient::destroyInstance()
{
    CC_ASSERT(s_pHttpClient);
    CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(CCHttpClient::dispatchResponseCallbacks), s_pHttpClient);
    s_pHttpClient->release();
}

CCHttpClient::CCHttpClient()
:_timeoutForRead(60)
,_timeoutForConnect(30)
{
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(
                    schedule_selector(CCHttpClient::dispatchResponseCallbacks), this, 0, false);
    CCDirector::sharedDirector()->getScheduler()->pauseTarget(this);
}

CCHttpClient::~CCHttpClient()
{
    need_quit = true;
    s_pHttpClient = NULL;
}

//Lazy create semaphore & mutex & thread
bool CCHttpClient::lazyInitThreadSemphore()
{
    if (!networkThread)
    {
        need_quit = false;
        pthread_create(&networkThread, NULL, networkThreadFunc, NULL);
        pthread_detach(networkThread);
    }
    
    return true;
}

//Add a get task to queue
void CCHttpClient::send(CCHttpRequest* request)
{    
    if (!request || false == lazyInitThreadSemphore())
        return;
    
    request->retain();
    MutexType::scoped_lock lock(requestQueueMutex);
    requestQueue.push_back(request);
}

// Poll and notify main thread if responses exists in queue
void CCHttpClient::dispatchResponseCallbacks(float delta)
{
    // CCLog("CCHttpClient::dispatchResponseCallbacks is running");
    
    CCHttpResponse* response = NULL;
    {
        MutexType::scoped_lock lock(responseQueueMutex);
        if (!responseQueue.empty())
        {
            response = responseQueue.front();
            responseQueue.pop_front();
        }
        if (responseQueue.empty())
        {
            CCDirector::sharedDirector()->getScheduler()->pauseTarget(this);
        }
    }
    
    if (response)
    {
        CCHttpRequest *request = response->getHttpRequest();
        CCObject *pTarget = request->getTarget();
        SEL_CallFuncND pSelector = request->getSelector();

        if (pTarget && pSelector) 
        {
            (pTarget->*pSelector)((CCNode *)this, response);
        }
        
        response->release();
    }
}

NS_CC_EXT_END


