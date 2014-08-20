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
#ifndef __CCAssetOutputStream_h__
#define __CCAssetOutputStream_h__

#include "cocos2d.h"
#include <string>

using namespace std;

NS_CC_BEGIN

/**
 * A cross platform output stream implementation for writing resources
 */
class CC_DLL CCAssetOutputStream : public CCObject {
protected:
	/// true indicating append file
	bool m_append;

	/// path
	string m_path;

protected:
	/**
	 * Default constructor
	 */
	CCAssetOutputStream();

	/**
	 * constructor
	 *
	 * @param path write file path
	 * @param append append file
	 */
	CCAssetOutputStream(const string& path, bool append = false);

public:
	/**
	 * static constructor
	 *
	 * @param path write file path
	 * @param append append file
	 * @return \link CCAssetOutputStream CCAssetOutputStream\endlink
	 */
	static CCAssetOutputStream* create(const string& path, bool append = false);

	virtual ~CCAssetOutputStream();

	/**
	 * open asset
	 *
	 * @return true means successful
	 */
	virtual bool open();

	/**
	 * close asset
	 */
	virtual void close() = 0;

	/**
	 * write char data
	 *
	 * @param data char data
	 * @param len character count in \c data
	 * @return byte write actually, -1 means error
	 */
	virtual ssize_t write(const char* data, size_t len) = 0;

	/**
	 * write int data
	 *
	 * @param data int data
	 * @param len count of integer in \c data
	 * @return byte write actually, -1 means error
	 */
	virtual ssize_t write(const int* data, size_t len) = 0;
    
    /**
	 * get offset from start to current position
	 *
	 * @return offset from start to current position
	 */
	virtual size_t getPosition() = 0;

	/**
	 * change pointer position
	 *
	 * @param offset offset
	 * @param mode seek mode
	 * @return new offset after changed后的实际位置
	 * \endif
	 */
	virtual size_t seek(int offset, int mode) = 0;

	/**
	 * Get asset output path, caller should not release it
	 */
	const string& getPath() { return m_path; }
};

NS_CC_END

#endif // __CCAssetOutputStream_h__
