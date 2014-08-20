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
#ifndef __CCMemoryOutputStream_h__
#define __CCMemoryOutputStream_h__

#include "CCAssetOutputStream.h"

NS_CC_BEGIN

/**
 * Output stream which writes to a memory data
 */
class CC_DLL CCMemoryOutputStream : public CCAssetOutputStream {
private:
    /// buffer
    char* m_buffer;

    /// buffer data length
    size_t m_length;

    /// buffer capacity
    size_t m_capacity;

    /// position
    size_t m_position;

    /// release by self or not
    bool m_release;

protected:
    /**
     * Constructor
     *
     * @param buffer memory buffer
     * @param capacity buffer capacity
     * @param release true means it will be released by memory output stream
     */
    CCMemoryOutputStream(char* buffer, size_t capacity, bool release);

    /// ensure capacity is enough after write \c len bytes
    void ensureCapacity(size_t len);

public:
    virtual ~CCMemoryOutputStream();

    /**
     * Create a default memory output stream and the output buffer is maintained by stream.
     *
     * @return output stream
     */
    static CCMemoryOutputStream* create();

    /**
     * Create a memory output stream, and buffer is created by stream but may not released by it
     *
     * @param capacity buffer byte capacity
     * @param release true means \c buffer should be released by stream, or false means \c buffer
     * 		will be released by caller
     * @return output stream
     */
    static CCMemoryOutputStream* create(size_t capacity, bool release = false);

    /// @see CCAssetOutputStream::close
	virtual void close() {}

    /// @see CCAssetOutputStream::write
	virtual ssize_t write(const char* data, size_t len);

    /// @see CCAssetOutputStream::write
	virtual ssize_t write(const int* data, size_t len);

    /// @see CCAssetOutputStream::getPosition
	virtual size_t getPosition();

    /// @see CCAssetOutputStream::seek
	virtual size_t seek(int offset, int mode);

	/**
	 * Get data written in buffer
	 *
	 * @return data length written to buffer
	 */
	size_t getLength() { return m_length; }

	/**
	 * Get buffer pointer
	 *
	 * @return buffer pointer
	 */
	const char* getBuffer();

	/**
	 * Reset stream to be ready for write new data
	 */
	void reset();
};

NS_CC_END

#endif // __CCMemoryOutputStream_h__
