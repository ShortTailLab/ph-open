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
#include "CCLocalization.h"
#include "CCUtils.h"
#include "CCAndroidStringsParser.h"
#include "CCLocale.h"

NS_CC_BEGIN

// init static
CCLocalization* CCLocalization::s_instance = NULL;

CCLocalization::CCLocalization() {
}

CCLocalization::~CCLocalization() {
    // release singleton
    if(s_instance) {
        s_instance->release();
        s_instance = NULL;
    }
}

CCLocalization* CCLocalization::sharedLocalization() {
    if(!s_instance) {
        s_instance = new CCLocalization();
    }
    return s_instance;
}

void CCLocalization::addAndroidStrings(const string& lan, const string& path, bool merge) {
    // basic checking
    if(path.empty()) {
        CCLOGWARN("CCLocalization::addAndroidStrings: string file path is empty");
        return;
    }
    if(lan.length() != 2) {
        CCLOGWARN("CCLocalization::addAndroidStrings: language code is not in ISO 639-1 format");
        return;
    }
    
    // register language dictionary
    CCDictionary* d = (CCDictionary*)m_lanMap.objectForKey(lan);
    if(!d) {
        d = CCDictionary::create();
        m_lanMap.setObject(d, lan);
    }
    
    // if not merge, clear first
    if(!merge) {
        d->removeAllObjects();
    }
    
    // parse it
    CCAndroidStringsParser::create()->parse(path, *d);
}

string CCLocalization::getString(const string& key) {
    // get strings, may fallback to English if not found
    string lan = CCLocale::sharedLocale()->getLanguage();
    CCDictionary* strings = (CCDictionary*)m_lanMap.objectForKey(lan);
    if(!strings) {
        if(lan != "en") {
            size_t pos = lan.find("-");
            if(pos == string::npos) {
                strings = (CCDictionary*)m_lanMap.objectForKey("en");
            } else {
                string primaryKey = lan.substr(0, pos);
                strings = (CCDictionary*)m_lanMap.objectForKey(primaryKey);
                if(!strings) {
                    strings = (CCDictionary*)m_lanMap.objectForKey("en");
                }
            }
        }
    }
    
    // find string, or return empty if not found
    if(strings) {
        CCString* s = (CCString*)strings->objectForKey(key);
        if(s) {
            return s->getCString();
        } else {
            return "!" + key + "!";
        }
    } else {
        return "!" + key + "!";
    }
}

NS_CC_END