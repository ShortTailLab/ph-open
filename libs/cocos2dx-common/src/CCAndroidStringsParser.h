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
#ifndef __CCAndroidStringsParser_h__
#define __CCAndroidStringsParser_h__

#include "cocos2d.h"
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

NS_CC_BEGIN

/// a parser of Android strings.xml, and save string key-values to a dictionary
class CCAndroidStringsParser : public CCObject, public XMLVisitor {
private:
    /// hold dictionary
    CCDictionary* m_dict;
    
public:
    CCAndroidStringsParser();
    virtual ~CCAndroidStringsParser();

    /**
     * Create an instance of Android string parser
     */
    static CCAndroidStringsParser* create();
    
    /**
     * Parse android string XML and save strings into a dictionary
     *
     * @param path string XML file path. The path is platform-independent and
     *      it will be mapped to platform format. For example, "/sdcard/strings.xml" will
     *      be mapped to "~/Documents/sdcard/strings.xml" in iOS like system.
     * @param dict the dictionary to hold parsed strings
     */
    void parse(const string& path, const CCDictionary& dict);
    
    /// override XMLVisitor
    virtual bool VisitEnter(const XMLElement& element, const XMLAttribute* firstAttribute);
};

NS_CC_END

#endif /* defined(__CCAndroidStringsParser_h__) */
