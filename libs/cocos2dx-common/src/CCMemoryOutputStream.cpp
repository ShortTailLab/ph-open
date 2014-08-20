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
#include "CCMemoryOutputStream.h"
#include <stdio.h>

NS_CC_BEGIN

CCMemoryOutputStream::CCMemoryOutputStream(char* buffer, size_t capacity, bool release) :
		m_buffer(buffer),
		m_capacity(capacity),
		m_length(0),
		m_position(0),
		m_release(release) {
}

CCMemoryOutputStream::~CCMemoryOutputStream() {
	if(m_release) {
		free((void*)m_buffer);
	}
}

CCMemoryOutputStream* CCMemoryOutputStream::create() {
	char* buffer = (char*)malloc(64 * 1024 * sizeof(char));
	CCMemoryOutputStream* s = new CCMemoryOutputStream(buffer, 64 * 1024, true);
	return (CCMemoryOutputStream*)s->autorelease();
}

CCMemoryOutputStream* CCMemoryOutputStream::create(size_t capacity, bool release) {
	char* buffer = (char*)malloc(capacity);
	CCMemoryOutputStream* s = new CCMemoryOutputStream(buffer, capacity, release);
	return (CCMemoryOutputStream*)s->autorelease();
}

void CCMemoryOutputStream::ensureCapacity(size_t len) {
	if(m_position + len > m_capacity - 1) {
		m_capacity = (m_position + len) * 2;
		m_buffer = (char*)realloc(m_buffer, m_capacity * sizeof(char));
	}
}

ssize_t CCMemoryOutputStream::write(const char* data, size_t len) {
	ensureCapacity(len);
	memcpy(m_buffer + m_position, data, len);
	m_position += len;
	m_length = MAX(m_position, m_length);
	return len;
}

ssize_t CCMemoryOutputStream::write(const int* data, size_t len) {
	ensureCapacity(len * sizeof(int));
	memcpy(m_buffer + m_position, data, len * sizeof(int));
	m_position += len * sizeof(int);
	m_length = MAX(m_position, m_length);
	return len;
}

size_t CCMemoryOutputStream::getPosition() {
	return m_position;
}

void CCMemoryOutputStream::reset() {
	m_position = 0;
	m_length = 0;
}

size_t CCMemoryOutputStream::seek(int offset, int mode) {
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

const char* CCMemoryOutputStream::getBuffer() {
	// end position as a c string
	m_buffer[m_position] = 0;
	return m_buffer;
}

NS_CC_END