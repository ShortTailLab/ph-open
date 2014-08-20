/****************************************************************************
 Author: Luma (stubma@gmail.com)
 
 https://github.com/stubma/cocos2dx-common
 
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
#include "CCResourceLoader.h"

NS_CC_BEGIN

CCResourceLoader::CCResourceLoader(CCResourceLoaderListener* listener) :
		m_listener(listener),
		m_delay(0),
		m_remainingIdle(0),
        m_nextLoad(0) {
}

CCResourceLoader::~CCResourceLoader() {
    for(LoadTaskPtrList::iterator iter = m_loadTaskList.begin(); iter != m_loadTaskList.end(); iter++) {
        delete *iter;
    }
}

void CCResourceLoader::run() {
	CCScheduler* scheduler = CCDirector::sharedDirector()->getScheduler();
	scheduler->scheduleSelector(schedule_selector(CCResourceLoader::doLoad), this, 0, kCCRepeatForever, m_delay, false);
}

void CCResourceLoader::addAndroidStringTask(const string& lan, const string& path, bool merge) {
    AndroidStringLoadTask* t = new AndroidStringLoadTask();
    t->lan = lan;
    t->path = path;
    t->merge = merge;
    addLoadTask(t);
}

void CCResourceLoader::addImageTask(const string& name, float idle) {
	ImageLoadTask* t = new ImageLoadTask();
    t->idle = idle;
    t->name = name;
    addLoadTask(t);
}

void CCResourceLoader::addZwoptexTask(const string& name, float idle) {
    ZwoptexLoadTask* t = new ZwoptexLoadTask();
    t->idle = idle;
    t->name = name;
    addLoadTask(t);
}

void CCResourceLoader::addZwoptexAnimTask(const string& name,
										  float unitDelay,
										  const string& pattern,
										  int startIndex,
										  int endIndex,
										  bool restoreOriginalFrame,
										  float idle) {
	ZwoptexAnimLoadTask* t = new ZwoptexAnimLoadTask();
	t->name = name;
	t->unitDelay = unitDelay;
	t->restoreOriginalFrame = restoreOriginalFrame;
	t->idle = idle;
	char buf[256];
	for(int i = startIndex; i <= endIndex; i++) {
		sprintf(buf, pattern.c_str(), i);
		t->frames.push_back(buf);
	}
	addLoadTask(t);
}

void CCResourceLoader::addZwoptexAnimTask(const string& name,
										  float unitDelay,
										  const string& pattern,
										  int startIndex,
										  int endIndex,
										  int startIndex2,
										  int endIndex2,
										  bool restoreOriginalFrame,
										  float idle) {
	ZwoptexAnimLoadTask* t = new ZwoptexAnimLoadTask();
	t->name = name;
	t->unitDelay = unitDelay;
	t->restoreOriginalFrame = restoreOriginalFrame;
	t->idle = idle;
	char buf[256];
	for(int i = startIndex; i <= endIndex; i++) {
		sprintf(buf, pattern.c_str(), i);
		t->frames.push_back(buf);
	}
	for(int i = startIndex2; i <= endIndex2; i++) {
		sprintf(buf, pattern.c_str(), i);
		t->frames.push_back(buf);
	}
	addLoadTask(t);
}

void CCResourceLoader::addLoadTask(LoadTask* t) {
    m_loadTaskList.push_back(t);
}

void CCResourceLoader::doLoad(float delta) {
    if(m_remainingIdle > 0) {
        m_remainingIdle -= delta;
    } else if(m_loadTaskList.size() <= m_nextLoad) {
        if(m_listener)
            m_listener->onResourceLoadingDone();
        
        CCScheduler* scheduler = CCDirector::sharedDirector()->getScheduler();
        scheduler->unscheduleSelector(schedule_selector(CCResourceLoader::doLoad), this);
        autorelease();
    } else {
        LoadTask* lp = m_loadTaskList.at(m_nextLoad++);
        m_remainingIdle = lp->idle;
        
        lp->load();
        if(m_listener)
            m_listener->onResourceLoadingProgress(m_nextLoad * 100 / m_loadTaskList.size(), delta);
    }
}

NS_CC_END