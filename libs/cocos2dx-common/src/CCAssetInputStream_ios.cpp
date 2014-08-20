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

#include "CCAssetInputStream_ios.h"
#include <errno.h>
#include "CCUtils.h"

NS_CC_BEGIN

CCAssetInputStream* CCAssetInputStream::create(const string& path) {
	CCAssetInputStream* ais = new CCAssetInputStream_ios(path);
	return (CCAssetInputStream*)ais->autorelease();
}

CCAssetInputStream_ios::CCAssetInputStream_ios(const string& path) :
		CCAssetInputStream(path),
		m_handle(nil),
		m_length(0) {
	// get mapped path
    string mappedPath = CCUtils::mapLocalPath(path);

	// is nil?
	if(mappedPath.empty()) {
		CCLOGWARN("CCAssetInputStream: can't map android path %s", path.c_str());
	} else {
		// open file
        NSString* nsPath = [NSString stringWithCString:mappedPath.c_str()
                                              encoding:NSUTF8StringEncoding];
		m_handle = [NSFileHandle fileHandleForReadingAtPath:nsPath];
		[m_handle retain];

		// get file length
		NSFileManager* fm = [NSFileManager defaultManager];
		NSDictionary* attr = [fm attributesOfItemAtPath:nsPath error:NULL];
		m_length = [[attr objectForKey:NSFileSize] intValue];
	}
}

CCAssetInputStream_ios::~CCAssetInputStream_ios() {
	[m_handle closeFile];
	[m_handle release];
	m_handle = nil;
}

size_t CCAssetInputStream_ios::getLength() {
	return m_length;
}

size_t CCAssetInputStream_ios::getPosition() {
	return [m_handle offsetInFile];
}

size_t CCAssetInputStream_ios::available() {
	return m_length - [m_handle offsetInFile];
}

char* CCAssetInputStream_ios::getBuffer() {
	size_t len = getLength();
	char* buf = (char*)malloc(len * sizeof(char));

	NSData* data = [m_handle availableData];
	memcpy(buf, [data bytes], [data length]);

	return buf;
}

void CCAssetInputStream_ios::close() {
	[m_handle closeFile];
	[m_handle release];
	m_handle = nil;
}

ssize_t CCAssetInputStream_ios::read(char* buffer, size_t length) {
	NSData* data = [m_handle readDataOfLength:length];
	memcpy(buffer, [data bytes], [data length]);
	return [data length];
}

size_t CCAssetInputStream_ios::seek(int offset, int mode) {
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
