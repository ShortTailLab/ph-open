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
#ifdef ANDROID

#include "CCAssetInputStream_android.h"
#include <errno.h>

NS_CC_BEGIN

CCAssetInputStream* CCAssetInputStream::create(const string& path) {
	CCAssetInputStream* ais = new CCAssetInputStream_android(path);
	return (CCAssetInputStream*)ais->autorelease();
}

CCAssetInputStream_android::CCAssetInputStream_android(const string& path) :
		CCAssetInputStream(path),
        m_buffer(NULL),
        m_position(0),
		m_length(0) {
    unsigned long len;
    m_buffer = (char*)CCFileUtils::sharedFileUtils()->getFileData(path.c_str(), "rb", &len);
    m_length = (size_t)len;
}

CCAssetInputStream_android::~CCAssetInputStream_android() {
}

size_t CCAssetInputStream_android::getLength() {
	return m_length;
}

size_t CCAssetInputStream_android::getPosition() {
    return m_position;
}

size_t CCAssetInputStream_android::available() {
    return m_length - m_position;
}

char* CCAssetInputStream_android::getBuffer() {
    return m_buffer;
}

void CCAssetInputStream_android::close() {
    if(m_buffer) {
        free(m_buffer);
        m_buffer = NULL;
        m_length = 0;
        m_position = 0;
    }
}

ssize_t CCAssetInputStream_android::read(char* buffer, size_t length) {
    int canRead = MIN(length, available());
    memcpy(buffer, m_buffer + m_position, canRead);
    m_position += canRead;
    return canRead;
}

size_t CCAssetInputStream_android::seek(int offset, int mode) {
    switch(mode) {
        case SEEK_CUR:
            m_position = clampf(m_position + offset, 0, m_length);
            break;
        case SEEK_END:
            m_position = clampf(m_length + offset, 0, m_length);
            break;
        case SEEK_SET:
            m_position = clampf(offset, 0, m_length);
            break;
    }
	
	return m_position;
}

NS_CC_END

#endif // ANDROID
