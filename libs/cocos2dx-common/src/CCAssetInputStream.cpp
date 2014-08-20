/****************************************************************************
 Author: Luma (stubma@gmail.com)
 
 https://github.com/stubma/cocos2dx-comon
 
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
#include "CCAssetInputStream.h"
#include <errno.h>
#include "CCMoreMacros.h"

NS_CC_BEGIN

CCAssetInputStream::CCAssetInputStream() :
        m_endian(LITTLE_ENDIAN) {
}

CCAssetInputStream::CCAssetInputStream(const string& path) :
        m_endian(LITTLE_ENDIAN) {
}

CCAssetInputStream::~CCAssetInputStream() {
}

bool CCAssetInputStream::open() {
	return true;
}

ssize_t CCAssetInputStream::readByte(char* ret) {
	return read(ret, sizeof(char));
}

ssize_t CCAssetInputStream::readShort(short* ret) {
	*ret = 0;
	ssize_t readBytes = read((char*)ret, sizeof(short));
	if(readBytes > 0) {
		if(isBigEndian())
			*ret = betoh16(*ret);
		else
			*ret = letoh16(*ret);
	}

	return readBytes;
}

ssize_t CCAssetInputStream::readInt(int* ret) {
	*ret = 0;
	ssize_t readBytes = read((char*)ret, sizeof(int));
	if(readBytes > 0) {
		if(isBigEndian())
			*ret = betoh32(*ret);
		else
			*ret = letoh32(*ret);
	}

	return readBytes;
}

ssize_t CCAssetInputStream::readInt64(int64_t* ret) {
	*ret = 0;
	ssize_t readBytes = read((char*)ret, sizeof(int64_t));
	if(readBytes > 0) {
		if(isBigEndian())
			*ret = betoh64(*ret);
		else
			*ret = letoh64(*ret);
	}

	return readBytes;
}

void CCAssetInputStream::setBigEndian(bool big) {
	m_endian = big ? BIG_ENDIAN : LITTLE_ENDIAN;
}

bool CCAssetInputStream::isBigEndian() {
	return m_endian == BIG_ENDIAN;
}

NS_CC_END