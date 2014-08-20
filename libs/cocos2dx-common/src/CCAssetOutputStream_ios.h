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

#ifndef __CCAssetOutputStream_ios_h__
#define __CCAssetOutputStream_ios_h__

#include "CCAssetOutputStream.h"
#include <stdio.h>
#import <Foundation/Foundation.h>

NS_CC_BEGIN

/**
 * iOS implementation of output stream
 */
class CCAssetOutputStream_ios : public CCAssetOutputStream {
	friend class CCAssetOutputStream;

private:
	/// file handle in iOS
	NSFileHandle* m_handle;

    /// length of file
	int m_length;

protected:
	/**
	 * constructor
	 *
	 * @param path write file path
	 * @param append append file
	 */
	CCAssetOutputStream_ios(const string& path, bool append = false);

public:
	virtual ~CCAssetOutputStream_ios();

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

#endif // __CCAssetOutputStream_ios_h__

#endif // CC_TARGET_OS_IPHONE
