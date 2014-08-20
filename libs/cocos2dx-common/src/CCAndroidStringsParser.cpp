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
#include "CCAndroidStringsParser.h"
#include "CCUtils.h"

NS_CC_BEGIN

CCAndroidStringsParser::CCAndroidStringsParser() {
}

CCAndroidStringsParser::~CCAndroidStringsParser() {
}

CCAndroidStringsParser* CCAndroidStringsParser::create() {
    CCAndroidStringsParser* p = new CCAndroidStringsParser();
    return (CCAndroidStringsParser*)p->autorelease();
}

void CCAndroidStringsParser::parse(const string& path, const CCDictionary& dict) {
    // map file path
    string localPath = CCUtils::mapLocalPath(path);
    
    // load file, if success, visit it
    XMLDocument* doc = new XMLDocument();
    if(doc->LoadFile(localPath.c_str()) == XML_NO_ERROR) {
        m_dict = (CCDictionary*)&dict;
        doc->Accept(this);
    }
    
    // release
    delete doc;
}

bool CCAndroidStringsParser::VisitEnter(const XMLElement& element, const XMLAttribute* firstAttribute) {
    if(!strcmp(element.Name(), "string")) {
        CCString* s = CCString::create(element.GetText());
        const char* key = element.Attribute("name");
        if(key) {
            m_dict->setObject(s, key);
        }
    }
    return true;
}

NS_CC_END