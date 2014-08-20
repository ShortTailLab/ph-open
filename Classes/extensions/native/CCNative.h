
#ifndef __CC_EXTENSION_CCNATIVE_H_
#define __CC_EXTENSION_CCNATIVE_H_

#include "cocos2dx_extensions.h"
#include "CCAlertViewDelegate.h"
#include "boost/function.hpp"
#include <string>

#if CC_LUA_ENGINE_ENABLED > 0
#include "CCLuaEngine.h"
#endif

NS_CC_EXT_BEGIN

/** @brief Activity indicator style */
typedef enum {
    CCActivityIndicatorViewStyleWhiteLarge,
    CCActivityIndicatorViewStyleWhite,
    CCActivityIndicatorViewStyleGray,
} CCActivityIndicatorViewStyle;

enum LanguageIdentifier
{
    kLanguageEnglish = 0,
    kLanguageChinese,
    kLanguageFrench,
    kLanguageItalian,
    kLanguageGerman,
    kLanguageSpanish,
    kLanguageRussian,
    kLanguageKorean,
    kLanguageJapanese,
    kLanguageHungarian,
    kLanguageTraditionalChinese
};

enum LocalPushRepeat
{
    LocalPushRepeatNone=0,
    LocalPushRepeatDaily,
    LocalPushRepeatWeekly,
    LocalPushRepeatMonthly
};

class CCNative
{
public:
    
#pragma mark -
#pragma mark activity indicator
    
    /** @brief Show activity indicator */
    static void showActivityIndicator(CCActivityIndicatorViewStyle style = CCActivityIndicatorViewStyleWhiteLarge);
    
    /** @brief Hide activity indicator */
    static void hideActivityIndicator(void);
    
#pragma mark -
#pragma mark alert view
    
    /** @brief Create alert view */
    static void createAlert(const char* title,
                            const char* message,
                            const char* cancelButtonTitle);
    static void promptForExit();
    static void exitGame();
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    static void createClassCache();
#endif
    
    static std::string device();
    static std::string OS();
    static std::string OSVersion();
    static std::string IPAddress();
    static std::string MACAddress();
    static void copyFile(const std::string& from, const std::string& to);
    static void copyAssetToPath(const std::string& from, const std::string& to);
    static void removeFile(const std::string& file);
    static void moveFile(const std::string& from, const std::string& to);
    
    static void registerLocalPushNotification(const char* content, int iFireDate, LocalPushRepeat iRepeat);
    static void clearLocalPushNotification();
    
    static bool addSkipBackupAttributeToItem(const std::string& file);
    
    static LanguageIdentifier getLanguage();
    static void vibrate();
    
    static bool fileExists(const std::string& string);
    
    // Android only stuff
    static bool isExternalStorageUsable();
    static bool isInstalledOnExternalStorage();
    
    static std::string getExternalDocPath();
    static std::string getExternalCachePath();
    static long getExternalFreeSpaceInMB();
    
    static std::string getInternalDocPath();
    static std::string getInternalCachePath();
    static long getInternalFreeSpaceInMB();

    // end of Android only stuff
    
    static std::string getDocPath();
    static std::string getCachePath();
    
    static int getResolutionWidth();
    static int getResolutionHeight();
    
    static unsigned long long getFreeDiskSpace();
    static void reportMemoryUsage();
    
    static void focusGLView();
    
    static std::string getBuildNum();

    /** @brief Add button to alert view, return button index */
    static int addAlertButton(const char* buttonTitle);
#if CC_LUA_ENGINE_ENABLED > 0
    static int addAlertButtonLua(const char* buttonTitle);
#endif        
    /** @brief Show alert view */
    static void showAlert(CCAlertViewDelegate* delegate = NULL);
#if CC_LUA_ENGINE_ENABLED > 0
    static void showAlertLua(cocos2d::LUA_FUNCTION listener);
#endif        
    /** @brief Hide and remove alert view */
    static void cancelAlert(void);
    
#pragma mark -
#pragma mark OpenUDID
    
    /** @brief Get OpenUDID value */
    static std::string getOpenUDID(void);
    
#pragma mark -
#pragma mark misc
    
    /** @brief Open a web page in the browser; create an email; or call a phone number. */
    static bool openURL(const std::string& url);
    
#pragma mark -
#pragma mark helper
    
private:
    CCNative(void) {}
};

NS_CC_EXT_END

#endif // __CC_EXTENSION_CCNATIVE_H_
