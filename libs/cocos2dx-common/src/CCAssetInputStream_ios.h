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
#ifdef CC_TARGET_OS_IPHONE

#ifndef __CCAssetInputStream_ios_h__
#define __CCAssetInputStream_ios_h__

#include "CCAssetInputStream.h"
#include <stdio.h>
#import <Foundation/Foundation.h>

NS_CC_BEGIN

/**
 * iOS implementation of input stream
 */
class CCAssetInputStream_ios : public CCAssetInputStream {
	friend class CCAssetInputStream;

private:
    /// file handle in iOS
	NSFileHandle* m_handle;

	/// length of file
	size_t m_length;

protected:
	/**
	 * constructor
	 *
	 * @param path file path
	 */
	CCAssetInputStream_ios(const string& path);

public:
	virtual ~CCAssetInputStream_ios();

	/// @see CCAssetInputStream::getBuffer
	virtual char* getBuffer();

	/// @see CCAssetInputStream::getPosition
	virtual size_t getPosition();

	/// @see CCAssetInputStream::getLength
	virtual size_t getLength();

	/// @see CCAssetInputStream::available
	virtual size_t available();

	/// @see CCAssetInputStream::close
	virtual void close();

	/// @see CCAssetInputStream::read
	virtual ssize_t read(char* buffer, size_t length);

	/// @see CCAssetInputStream::seek
	virtual size_t seek(int offset, int mode);
};

NS_CC_END

#endif // __CCAssetInputStream_ios_h__

#endif // CC_TARGET_OS_IPHONE