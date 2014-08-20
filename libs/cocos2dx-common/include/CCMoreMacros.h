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
#ifndef __CCMoreMacros_h__
#define __CCMoreMacros_h__

#include "cocos2d.h"
USING_NS_CC;

// stl, common
#include <unistd.h>
#include <stack>
#include <vector>
#include <map>
using namespace std;

// map endian methods
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    #include <sys/stat.h>
	#include <endian.h>
    #include "JniHelper.h"

    // path separator
    #define CC_PATH_SEPARATOR '/'

	// max float
	#define MAX_FLOAT 3.4028235E38f
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    // common header
    #include <sys/stat.h>
    #import <Foundation/Foundation.h>

    // endian
	#import <CoreFoundation/CoreFoundation.h>
	#define letoh64 CFSwapInt64LittleToHost
	#define letoh32 CFSwapInt32LittleToHost
	#define letoh16 CFSwapInt16LittleToHost
	#define htole64 CFSwapInt64HostToLittle
	#define htole32 CFSwapInt32HostToLittle
	#define htole16 CFSwapInt16HostToLittle
	#define betoh64	CFSwapInt64BigToHost
	#define betoh32	CFSwapInt32BigToHost
	#define betoh16 CFSwapInt16BigToHost
	#define htobe64 CFSwapInt64HostToBig
	#define htobe32 CFSwapInt32HostToBig
	#define htobe16 CFSwapInt16HostToBig

    // path separator
    #define CC_PATH_SEPARATOR '/'

	// max float
	#define MAX_FLOAT 3.4028235E38f
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    // mkdir
    #include <direct.h>
    #define mkdir _mkdir

    // endian
	#include <io.h>
	#define ___my_swap16(x) \
		((uint16_t)( \
		(((uint16_t)(x) & (uint16_t)0x00ffU) << 8) | \
		(((uint16_t)(x) & (uint16_t)0xff00U) >> 8) ))
	#define ___my_swap32(x) \
		((uint32_t)( \
		(((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) | \
		(((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) | \
		(((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) | \
		(((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24) ))
    #define ___my_swap64(x) \
		((uint64_t)( \
		(uint64_t)(((uint64_t)(x) & (uint64_t)0x00000000000000ffULL) << 56) | \
		(uint64_t)(((uint64_t)(x) & (uint64_t)0x000000000000ff00ULL) << 40) | \
		(uint64_t)(((uint64_t)(x) & (uint64_t)0x0000000000ff0000ULL) << 24) | \
		(uint64_t)(((uint64_t)(x) & (uint64_t)0x00000000ff000000ULL) <<  8) | \
		(uint64_t)(((uint64_t)(x) & (uint64_t)0x000000ff00000000ULL) >>  8) | \
		(uint64_t)(((uint64_t)(x) & (uint64_t)0x0000ff0000000000ULL) >> 24) | \
		(uint64_t)(((uint64_t)(x) & (uint64_t)0x00ff000000000000ULL) >> 40) | \
		(uint64_t)(((uint64_t)(x) & (uint64_t)0xff00000000000000ULL) >> 56) ))
	#define letoh64(x) (x)
	#define letoh32(x) (x)
	#define letoh16(x) (x)
	#define htole64(x) (x)
	#define htole32(x) (x)
	#define htole16(x) (x)
	#define betoh64(x) ___my_swap64(x)
	#define betoh32(x) ___my_swap32(x)
	#define betoh16(x) ___my_swap16(x)
	#define htobe64(x) ___my_swap64(x)
	#define htobe32(x) ___my_swap32(x)
	#define htobe16(x) ___my_swap16(x)

    // byte order
    #define _LITTLE_ENDIAN 1234
    #define _BIG_ENDIAN 4321
    #define LITTLE_ENDIAN _LITTLE_ENDIAN
    #define BIG_ENDIAN _BIG_ENDIAN
    #define _BYTE_ORDER LITTLE_ENDIAN
    #define BYTE_ORDER _BYTE_ORDER

    // path separator
    #define CC_PATH_SEPARATOR '\\'

	// max float
	#define MAX_FLOAT FLT_MAX
#endif

// max int
#define MAX_INT 0x7fffffff

// random to max, inclusive
#define CCRANDOM_0_X_INT(x) ((int)(CCRANDOM_0_1() * (x) * 100) % ((x) + 1))
#define CCRANDOM_X_Y_INT(x, y) (CCRANDOM_0_X_INT((y) - (x)) + (x))

#endif // __CCMoreMacros_h__
