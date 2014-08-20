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

#include "CCAssetOutputStream_ios.h"
#include <stdio.h>
#include <errno.h>
#include "CCUtils.h"

NS_CC_BEGIN

CCAssetOutputStream* CCAssetOutputStream::create(const string& path, bool append) {
	CCAssetOutputStream* aos = new CCAssetOutputStream_ios(path, append);
	return (CCAssetOutputStream*)aos->autorelease();
}

CCAssetOutputStream_ios::CCAssetOutputStream_ios(const string& path, bool append) :
		CCAssetOutputStream(path, append),
		m_handle(nil) {
    // get mapped path
    string mappedPath = CCUtils::mapLocalPath(path);

	// is nil?
	if(mappedPath.empty()) {
		CCLOGERROR("CCAssetOutputStream: can't map android path %s", path.c_str());
	} else {
        NSString* nsPath = [NSString stringWithCString:mappedPath.c_str()
                                              encoding:NSUTF8StringEncoding];
		m_handle = [NSFileHandle fileHandleForWritingAtPath:nsPath];
		if (m_append) {
			[m_handle seekToEndOfFile];
		}
		[m_handle retain];

        // get file length
		NSFileManager* fm = [NSFileManager defaultManager];
		NSDictionary* attr = [fm attributesOfItemAtPath:nsPath error:NULL];
		m_length = [[attr objectForKey:NSFileSize] intValue];
	}
}

CCAssetOutputStream_ios::~CCAssetOutputStream_ios() {
	[m_handle closeFile];
	[m_handle release];
	m_handle = nil;
}

void CCAssetOutputStream_ios::close() {
	[m_handle closeFile];
	[m_handle release];
	m_handle = nil;
}

ssize_t CCAssetOutputStream_ios::write(const char* data, size_t len) {
	NSData *nData = [NSData dataWithBytes:data length:len];
	[m_handle writeData:nData];
	return [nData length];
}

ssize_t CCAssetOutputStream_ios::write(const int* data, size_t len) {
	NSData *nData = [NSData dataWithBytes:data length:len];
	[m_handle writeData:nData];
	return [nData length];
}

size_t CCAssetOutputStream_ios::getPosition() {
	return [m_handle offsetInFile];
}

size_t CCAssetOutputStream_ios::seek(int offset, int mode) {
	switch (mode) {
		case SEEK_CUR:
			[m_handle seekToFileOffset:getPosition() + offset];
			break;
		case SEEK_END:
			[m_handle seekToFileOffset:m_length + offset];
			break;
		case SEEK_SET:
			[m_handle seekToFileOffset:offset];
			break;
	}

	return [m_handle offsetInFile];
}

NS_CC_END

#endif // CC_TARGET_OS_IPHONE
