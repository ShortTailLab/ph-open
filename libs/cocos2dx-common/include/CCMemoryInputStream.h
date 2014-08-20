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
#ifndef __CCMemoryInputStream_h__
#define __CCMemoryInputStream_h__

#include "CCAssetInputStream.h"

NS_CC_BEGIN

/**
 * A stream which read a memory buffer
 */
class CC_DLL CCMemoryInputStream : public CCAssetInputStream {
private:
    /// buffer
    char* m_buffer;
    
    /// buffer data length
    size_t m_length;
    
    /// position
    size_t m_position;
    
    /// release by self or not
    bool m_release;
    
protected:
    /**
     * Constructor
     * 
     * @param buffer memory buffer
     * @param length buffer data length
     * @param release true means it will be released by memory input stream
     */
    CCMemoryInputStream(char* buffer, size_t length, bool release);
    
public:
    virtual ~CCMemoryInputStream();
    
    /**
     * Create a memory input stream
     *
     * @param buffer memory buffer
     * @param length buffer data length
     * @param release true means this buffer will be released by this stream
     * @return memory buffer input stream
     */
    static CCMemoryInputStream* create(char* buffer, size_t length, bool release = false);
    
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

#endif // __CCMemoryInputStream_h__