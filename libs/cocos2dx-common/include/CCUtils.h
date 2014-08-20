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
#ifndef __CCUtils_h__
#define __CCUtils_h__

#include "cocos2d.h"
#include <string>
#include "ccMoreTypes.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	#include <jni.h>
#endif

using namespace std;

NS_CC_BEGIN

/// custom utilities
class CC_DLL CCUtils {
public:
    typedef vector<string> StringList;
    
private:
    /// string list temp
    static StringList s_tmpStringList;
    
    /// tmp CCArray
    static CCArray s_tmpArray;
    
private:
	static unsigned char UnitScalarToByte(float x);
	
public:
    /**
	 * Copy a string
	 *
	 * @param src source string
	 * @return copied string, caller should release it
	 */
	static const char* copy(const char* src);
    
	/**
	 * Copy a string
	 *
	 * @param src source string
	 * @param start start position to copy
	 * @param len length to copy
	 * @return copied string, caller should release it. returns NULL if \c src is NULL
	 */
	static const char* copy(const char* src, int start, size_t len);
    
	/// convert string to lowercase
	static void toLowercase(string& s);
    
    /// start with sub string?
    static bool startsWith(const string& s, const string& sub);
    
    /// end with sub string?
    static bool endsWith(const string& s, const string& sub);
    
    /**
	 * Replace a char with other char
	 *
	 * @param s string reference, its content will be modified
	 * @param c char to be replaced
	 * @param sub char to be replaced to
	 */
    static void replaceChar(string& s, char c, char sub);
    
    /**
     * get digit of a number. For example: 0 returns 1, 12 returns 2,
     * if number is negative, the negate symbol is not counted
     */
    static int getNumDigits(int num);
    
    /// Get index of last slash character, if not found, returns -1
    static ssize_t lastSlashIndex(string path);
    
    /// Get index of last dot character, if not found, returns -1
    static ssize_t lastDotIndex(const string& path);
    
    /// get last segment of path
    static string lastPathComponent(const string& path);
    
    /**
	 * Delete last component in a path, for example:<br>
	 * Input					Output<br>
	 * "/tmp/scratch.tiff"      "/tmp"<br>
	 * "/tmp/scratch"           "/tmp"<br>
	 * "/tmp/"                  "/"<br>
	 * "scratch"                ""<br>
	 * "/" 						"/"<br>
	 *
	 * @param path path string
	 * @return path without last segment, caller should release returned path string
	 */
    static string deleteLastPathComponent(const string& path);
    
    /**
	 * Append a path segment to another path, for example:<br>
	 * Input					Output<br>
	 * "/tmp", "/scratch.tiff"  "/tmp/scratch.tiff"<br>
	 * "/tmp//", "/scratch"     "/tmp/scratch"<br>
	 * "/tmp", "/"              "/tmp"<br>
	 * "/", "tmp/" 				"/tmp"<br>
	 *
	 * @param path path string
	 * @param component segment to be added
	 * @return new path, caller should release returned path string
	 */
	static string appendPathComponent(const string& path, const string& component);
    
    /**
	 * Delete path extension of last path component
	 *
	 * @param path path string
	 * @return path after deleted extension, caller should release it
	 */
	static string deletePathExtension(const string& path);
	
	/**
	 * map path to different platform, for example, a path "/sdcard/a.png" will be:
	 * 1. in android, it is /sdcard/a.png
	 * 2. in iOS and MacOSX, it will be ~/Documents/sdcard/a.png
	 * 3. in Windows, it will be [Executable Dir]/sdcard/a.png
	 *
	 * for iOS and MacOSX, if path is a relative path, then it will be mapped to app folder, for example, 
	 * a path "sdcard/a.png" will be:
	 * 1. in iOS, it will be [app path]/sdcard/a.png
	 * 2. in Mac OS X, it will [app path]/Contents/Resources/sdcard/a.png
	 */
	static string mapLocalPath(string path);
	
	/// get parent path of a path
	static string getParentPath(string path);
	
	/**
	 * create intermediate folders for a path
	 *
	 * @param path must be an absolute path
	 * @return true means successful
	 */
	static bool createIntermediateFolders(string path);
	
	/**
	 * check path existence
	 *
	 * @param path must be an absolute path
	 * @return true means path is existent
	 */
	static bool isPathExistent(string path);
	
	/**
	 * create a folder at specified absolute path, its parent folder should be existent and this
	 * method won't perform checking
	 *
	 * @param path must be an absolute path
	 * @return true means successful
	 */
	static bool createFolder(string path);
	
	/// delete a file
	static bool deleteFile(string path);
	
	/// convert rgb to hsv
	static ccColorHSV ccc32hsv(ccColor3B c);
	
	/// convert hsv to rgb
	static ccColor3B hsv2ccc3(ccColorHSV c);
	
	/// get node origin
	static CCPoint getOrigin(CCNode* node);
	
	/// get node center
	static CCPoint getCenter(CCNode* node);
    
    /// get local center
    static CCPoint getLocalCenter(CCNode* node);
	
	/// get node position
	static CCPoint getPoint(CCNode* node, float xpercent, float ypercent);
	
	/// get node position
	static CCPoint getPoint(CCNode* node, CCPoint anchor) { return getPoint(node, anchor.x, anchor.y); }
	
	/// change opacity recursively
	static void setTreeOpacity(CCNode* n, int o);
	
	/// is a segment intersected with a box
	static bool testSegmentAABB(CCPoint p0, CCPoint p1, ccAABB b);
	
	/// perform a binary search in a int array
	static int binarySearch(int* a, size_t len, int key);
    
    /// combine two rect
    static CCRect combine(const CCRect& r1, const CCRect& r2);
    
    /// has external storage, such as sd card
    static bool hasExternalStorage();
    
    /// current time milliseconds from 1970-1-1
    static int64_t currentTimeMillis();
    
    /// verify app signature, if has, basically it is only used for android
    static bool verifySignature(void* validSign, size_t len);
    
    /**
     * split string into components by a separator
     * returned string list is const and no need to release
     * the list is shared, you must copy its content if you want to keep content for later use
     *
     * @param s string
     * @param sep separator character
     * @return a const vector, must copy it if you want to keep its content
     */
    static StringList& componentsOfString(const string& s, const char sep);
    
    /**
     * parse a CCPoint from a string in format {x,y}, such as {3.2,3.4}.
     * The brace can be replaced with bracket or parentheses, i.e., [3.2,3.4] and
     * (3.2,3.4) is also valid. Furthurmore, white space is allowed so [3.2,   3.4] is valid
     *
     * \note
     * for partial valid string, the result will be
     * {3.2} ==> ccp(3.2, 0)
     * {,3.4} ==> ccp(0, 3.4)
     * {3.2 ==> ccp(3.2, 0)
     * {,3.4 ==> ccp(0, 3.4)
     * {2,3,4} ==> ccp(2, 3)
     * {[2,3}]) ==> ccp(2,3)
     * {,} ==> ccp(0, 0)
     * 
     * @param s string
     * @return CCPoint, or a zero point if string format is invalid
     */
    static CCPoint ccpFromString(const string& s);
    
    /**
     * parse a CCSize from a string in format {x,y}, such as {3.2,3.4}.
     * The brace can be replaced with bracket or parentheses, i.e., [3.2,3.4] and
     * (3.2,3.4) is also valid. Furthurmore, white space is allowed so [3.2,   3.4] is valid
     *
     * \note
     * for partial valid string, the result will be
     * {3.2} ==> ccp(3.2, 0)
     * {,3.4} ==> ccp(0, 3.4)
     * {3.2 ==> ccp(3.2, 0)
     * {,3.4 ==> ccp(0, 3.4)
     * {2,3,4} ==> ccp(2, 3)
     * {[2,3}]) ==> ccp(2,3)
     * {,} ==> ccp(0, 0)
     *
     * @param s string
     * @return CCSize, or a zero point if string format is invalid
     */
    static CCSize ccsFromString(const string& s);
    
    /**
     * parse a CCRect from a string in format {x,y,w,h}, such as {1,2,3,4}
     * The brace can be replaced with bracket or parentheses, and whitespace
     * is allowed
     *
     * \note
     * This method has same tolerance as ccpFromString and ccsFromString
     *
     * @param s string
     * @return CCRect, or a zero point of string format is invalid
     */
    static CCRect ccrFromString(const string& s);
    
    /**
     * create a CCArray from a string in format {a1,a2,a3,...}. If the element is
     * embraced by quote or double quote, it will be a CCString object.
     * If no quote and has dot, it will be a CCFloat, otherwise it will be CCInteger
     *
     * @param s string
     * @return CCArray, you should copy it if you want to keep its content for later use
     */
    static CCArray& arrayFromString(const string& s);
    
    /**
     * convert CCArray to string format, rule is same as arrayFromString
     */
    static string arrayToString(const CCArray& array);
	
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	/// get JNIEnv
	static JNIEnv* getJNIEnv();
#endif
};

NS_CC_END

#endif // __CCUtils_h__
