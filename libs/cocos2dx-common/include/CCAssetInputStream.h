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
#ifndef __CCAssetInputStream_h__
#define __CCAssetInputStream_h__

#include "cocos2d.h"

using namespace std;

NS_CC_BEGIN
    
/**
 * A cross platform input stream implementation for reading resources
 */
class CC_DLL CCAssetInputStream : public CCObject {
protected:
    /// byte order, default is little endian
    int m_endian;
    
protected:
    /**
     * empty constructor
     */
    CCAssetInputStream();
    
    /**
     * constructor
     *
     * @param path file path
     */
    CCAssetInputStream(const string& path);
    
public:
    /**
     * static constructor
     *
     * @param path file path
     */
    static CCAssetInputStream* create(const string& path);
    
    virtual ~CCAssetInputStream();
    
    /**
     * get buffer of resource, it contains all data
     *
     * @return buffer contains data of resource
     */
    virtual char* getBuffer() = 0;
    
    /**
     * get offset from start to current position
     *
     * @return offset from start to current position
     */
    virtual size_t getPosition() = 0;
    
    /**
     * get length of all data
     *
     * @return length of all data
     */
    virtual size_t getLength() = 0;
    
    /**
     * get length of available data
     *
     * @return length of available data
     */
    virtual size_t available() = 0;
    
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
     * read data into a buffer
     *
     * @param buffer buffer pointer
     * @param length byte to read at most
     * @return byte read actually, 0 means no more data, -1 means error
     */
    virtual ssize_t read(char* buffer, size_t length) = 0;
    
    /**
     * read a byte
     *
     * @param ret return the byte read
     * @return byte read in fact, 0 means no more data, -1 means error
     */
    virtual ssize_t readByte(char* ret);
    
    /**
     * read a short int
     *
     * @param ret return the byte read
     * @return byte read in fact, 0 means no more data, -1 means error
     */
    virtual ssize_t readShort(short* ret);
    
    /**
     * read a int and the read value will reflect current byte order setting
     *
     * @param ret return the int read
     * @return byte read in fact, 0 means no more data, -1 means error
     */
    virtual ssize_t readInt(int* ret);
    
    /**
     * read a int 64 and the read value will reflect current byte order setting
     *
     * @param ret return the int read
     * @return byte read in fact, 0 means no more data, -1 means error
     */
    virtual ssize_t readInt64(int64_t* ret);
    
    /**
     * change pointer position
     *
     * @param offset offset
     * @param mode seek mode
     * @return new offset after changed
     */
    virtual size_t seek(int offset, int mode) = 0;
    
    /**
     * set use big endian or not, default is little endian
     *
     * @param big true means use big endian
     */
    virtual void setBigEndian(bool big);
    
    /**
     * is big endian or not?
     *
     * @return true means current byte order is big endian
     */
    virtual bool isBigEndian();
};

NS_CC_END

#endif // __CCAssetInputStream_h__