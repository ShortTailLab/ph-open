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
#ifndef __CCLocalization_h__
#define __CCLocalization_h__

#include "cocos2d.h"

using namespace std;

NS_CC_BEGIN

/**
 * localization resource manager and retriever. It can load Android format
 * strings.xml and map all strings. To get a string, just one method with a
 * string key.
 */
class CC_DLL CCLocalization : public CCObject {
private:
    /// singleton
    static CCLocalization* s_instance;
    
    /// language map, key is language ISO code, value is CCDictionary
    CCDictionary m_lanMap;
    
protected:
    CCLocalization();
    
public:
    virtual ~CCLocalization();
    static CCLocalization* sharedLocalization();
    
    /**
     * register a bunch of strings for a language. The string file must be
     * an Android string XML format file.
     *
     * \note
     * If strings of this language already exists, then      *
     * @param lan language ISO 639-1 two-letter code
     * @param path string XML file path. The path is platform-independent and
     *      it will be mapped to platform format. For example, "/sdcard/strings.xml" will
     *      be mapped to "~/Documents/sdcard/strings.xml" in iOS like system.
     * @param merge true means new strings will be merged with current strings if strings 
     *      of this language already exists. false means new strings will replace current strings
     *      if strings of this language already exists.
     */
    void addAndroidStrings(const string& lan, const string& path, bool merge = false);

    /**
     * Get a string by key, in current language. If current language is not English and
     * string is not found, it will try to fallback to English.
     *
     * @param key string key name
     * @return string, or empty if key can't be matched
     */
    string getString(const string& key);
};

/// macro for easily get strings
#define CCL(k) (CCLocalization::sharedLocalization()->getString((k)))

/// macro to get a C string
#define CCLC(k) (CCL(k).c_str())

NS_CC_END

#endif /* defined(__CCLocalization_h__) */
