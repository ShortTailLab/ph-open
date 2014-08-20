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
#include "CCMemoryInputStream.h"
#include <stdio.h>

NS_CC_BEGIN

CCMemoryInputStream::CCMemoryInputStream(char* buffer, size_t length, bool release) : 
        m_buffer(buffer),
        m_length(length),
        m_release(release),
        m_position(0) {
}

CCMemoryInputStream::~CCMemoryInputStream() {
    if(m_release) {
        if(m_buffer) {
            free(m_buffer);
        }
    }
}

CCMemoryInputStream* CCMemoryInputStream::create(char* buffer, size_t length, bool release) {
    CCMemoryInputStream* s = new CCMemoryInputStream(buffer, length, release);
    return (CCMemoryInputStream*)s->autorelease();
}

char* CCMemoryInputStream::getBuffer() {
    return m_buffer;
}

size_t CCMemoryInputStream::getPosition() {
    return m_position;
}

size_t CCMemoryInputStream::getLength() {
    return m_length;
}

size_t CCMemoryInputStream::available() {
    return m_length - m_position;
}

void CCMemoryInputStream::close() {
    // do nothing
}

ssize_t CCMemoryInputStream::read(char* buffer, size_t length) {
    int canRead = MIN(length, available());
    memcpy(buffer, m_buffer + m_position, canRead);
    m_position += canRead;
    return canRead;
}

size_t CCMemoryInputStream::seek(int offset, int mode) {
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