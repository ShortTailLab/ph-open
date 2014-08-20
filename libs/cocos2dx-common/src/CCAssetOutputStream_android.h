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

#ifndef __CCAssetOutputStream_android_h__
#define __CCAssetOutputStream_android_h__

#include "CCAssetOutputStream.h"
#include <stdio.h>

NS_CC_BEGIN

/**
 * Android implementation of output stream
 */
class CCAssetOutputStream_android : public CCAssetOutputStream {
	friend class CCAssetOutputStream;

private:
	/// FILE pointer used for file path
	FILE* m_fp;

protected:
	/**x
	 * constructor
	 *
	 * @param path write file path
	 * @param append append file
	 */
	CCAssetOutputStream_android(const string& path, bool append = false);

public:
	virtual ~CCAssetOutputStream_android();

	/// @see CCAssetOutputStream::close
	virtual void close();

	/// @see CCAssetOutputStream::write
	virtual ssize_t write(const char* data, size_t len);

	/// @see CCAssetOutputStream::write
	virtual ssize_t write(const int* data, size_t len);

	/// @see CCAssetOutputStream::getPosition
	virtual size_t getPosition();

	/// @see CCAssetOutputStream::seek
	virtual size_t seek(int offset, int mode);
};

NS_CC_END

#endif // __CCAssetOutputStream_android_h__

#endif // #if ANDROID