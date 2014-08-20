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

#include "CCAssetOutputStream_android.h"
#include <stdio.h>
#include <errno.h>

#define ERROR_RETURN -1

NS_CC_BEGIN

CCAssetOutputStream* CCAssetOutputStream::create(const string& path, bool append) {
	CCAssetOutputStream* aos = new CCAssetOutputStream_android(path, append);
	return (CCAssetOutputStream*)aos->autorelease();
}

CCAssetOutputStream_android::CCAssetOutputStream_android(const string& path, bool append) :
		CCAssetOutputStream(path, append),
		m_fp(NULL) {
	if (m_append) {
		if((m_fp = fopen(path.c_str(), "ab")) == NULL) {
			CCLOGWARN("open file %s failed: %s", path.c_str(), strerror(errno));
			m_fp = NULL;
		}
	} else {
		if((m_fp = fopen(path.c_str(), "wb")) == NULL) {
			CCLOGWARN("open file %s failed: %s", path.c_str(), strerror(errno));
			m_fp = NULL;
		}
	}
}

CCAssetOutputStream_android::~CCAssetOutputStream_android() {
	if(m_fp != NULL) {
		fclose(m_fp);
		m_fp = NULL;
	}
}

void CCAssetOutputStream_android::close() {
	if(m_fp != NULL) {
		fclose(m_fp);
		m_fp = NULL;
	}
}

ssize_t CCAssetOutputStream_android::write(const char* data, size_t len) {
	if (data == NULL)
		return ERROR_RETURN;

	if(m_fp != NULL)
		return fwrite((void*)data, sizeof(char) * len, 1, m_fp);
	else
		return ERROR_RETURN;
}

ssize_t CCAssetOutputStream_android::write(const int* data, size_t len) {
	if (data == NULL)
		return ERROR_RETURN;

	if(m_fp != NULL)
		return fwrite((void*)data, sizeof(int) * len, 1, m_fp);
	else
		return ERROR_RETURN;
}

size_t CCAssetOutputStream_android::getPosition() {
	if(m_fp != NULL)
		return ftell(m_fp);
	else
		return 0;
}

size_t CCAssetOutputStream_android::seek(int offset, int mode) {
	if(m_fp != NULL)
		return fseek(m_fp, offset, mode);
	else
		return 0;
}

NS_CC_END

#endif // #if ANDROID
