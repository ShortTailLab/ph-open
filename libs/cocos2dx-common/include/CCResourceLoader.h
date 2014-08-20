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
#ifndef __CCResourceLoader_h__
#define __CCResourceLoader_h__

#include "cocos2d.h"
#include "CCResourceLoaderListener.h"
#include "CCLocalization.h"

using namespace std;

NS_CC_BEGIN

/**
 * a self-retain class for resource loading
 */
class CC_DLL CCResourceLoader : public CCObject {
public:
	/// load parameter
    struct LoadTask {
        /// idle time after loaded
        float idle;
        
        LoadTask() :
		idle(0.1f) {
        }
        
        virtual ~LoadTask() {}
        
        /// do loading
        virtual void load() {}
    };
	
private:
    /// type of load operation
    enum ResourceType {
        IMAGE,
        ZWOPTEX,
        ANIMATION
    };
    
    /// android string load task
    struct AndroidStringLoadTask : public LoadTask {
        /// language
        string lan;
        
        /// file path
        string path;
        
        /// merge or not
        bool merge;
        
        virtual ~AndroidStringLoadTask() {}
        
        virtual void load() {
            CCLocalization::sharedLocalization()->addAndroidStrings(lan, path, merge);
        }
    };
    
    /// image load parameter
    struct ImageLoadTask : public LoadTask {
        /// image name
        string name;
        
        virtual ~ImageLoadTask() {}
        
        virtual void load() {
            CCTextureCache::sharedTextureCache()->addImage(name.c_str());
        }
    };
    
    /// zwoptex load parameter
    struct ZwoptexLoadTask : public LoadTask {
        /// plist name
        string name;
        
        virtual ~ZwoptexLoadTask() {}
        
        virtual void load() {
            CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(name.c_str());
        }
    };
    
    /// zwoptex animation load parameter
    struct ZwoptexAnimLoadTask : public LoadTask {
        /// frame list
        typedef vector<string> StringList;
        StringList frames;
        
        /// animation name
        string name;
        
        /// animation unit delay
        float unitDelay;
		
		/// restore original frame when animate is done
		bool restoreOriginalFrame;
		
		ZwoptexAnimLoadTask() :
				unitDelay(0),
				restoreOriginalFrame(false) {
		}
        
        virtual ~ZwoptexAnimLoadTask() {}
        
        virtual void load() {
			if(!CCAnimationCache::sharedAnimationCache()->animationByName(name.c_str())) {
				CCSpriteFrameCache* cache = CCSpriteFrameCache::sharedSpriteFrameCache();
				CCArray* array = CCArray::create();
				for(StringList::iterator iter = frames.begin(); iter != frames.end(); iter++) {
					CCSpriteFrame* f = cache->spriteFrameByName(iter->c_str());
					array->addObject(f);
				}
				CCAnimation* anim = CCAnimation::createWithSpriteFrames(array, unitDelay);
				anim->setRestoreOriginalFrame(restoreOriginalFrame);
				CCAnimationCache::sharedAnimationCache()->addAnimation(anim, name.c_str());
			}
        }
    };
 
private:
	/// listener
	CCResourceLoaderListener* m_listener;

	/// remaining delay time
    float m_remainingIdle;
    
    /// next loading item
    int m_nextLoad;
    
    /// load list
    typedef vector<LoadTask*> LoadTaskPtrList;
    LoadTaskPtrList m_loadTaskList;

private:
	/// perform loading
	void doLoad(float delta);

public:
    CCResourceLoader(CCResourceLoaderListener* listener);
	virtual ~CCResourceLoader();
    
    /// start loading
    void run();
    
    /// directly add a load task
    void addLoadTask(LoadTask* t);
    
    /**
     * add an Android string loading task
     *
     * @param lan language ISO 639-1 code
     * @param path string XML file platform-independent path
     * @param merge true means merge new strings, or false means replace current strings
     */
    void addAndroidStringTask(const string& lan, const string& path, bool merge = false);
	
	/// add a image loading task
	void addImageTask(const string& name, float idle = 0);
	
	/// add a zwoptex image loading task
	void addZwoptexTask(const string& name, float idle = 0);
	
	/// add a zwoptex animation loading task
	/// the endIndex is inclusive
	void addZwoptexAnimTask(const string& name,
							float unitDelay,
							const string& pattern,
							int startIndex,
							int endIndex,
							bool restoreOriginalFrame = false,
							float idle = 0);
	
	/// add a zwoptex animation loading task
	/// the endIndex is inclusive
	/// this method can specify two sets of start/end index so the
	/// animation can have two stage
	void addZwoptexAnimTask(const string& name,
							float unitDelay,
							const string& pattern,
							int startIndex,
							int endIndex,
							int startIndex2,
							int endIndex2,
							bool restoreOriginalFrame = false,
							float idle = 0);
	
	/// delay time before start to load
	CC_SYNTHESIZE(float, m_delay, Delay);
};

NS_CC_END

#endif // __CCResourceLoader_h__
