#import "BuildConf.h"
#include "native/CCNative.h"
#import "CCCrypto.h"
#import "platform/ios/UIDevice+Hardware.h"
#import "platform/ios/CCNative_objc.h"
#import "AudioToolbox/AudioServices.h"
#import <mach/mach.h>

NS_CC_EXT_BEGIN

#pragma mark -
#pragma mark activity indicator

void CCNative::showActivityIndicator(CCActivityIndicatorViewStyle style)
{
    [[CCNative_objc sharedInstance] showActivityIndicator:static_cast<UIActivityIndicatorViewStyle>(style)];
}

void CCNative::hideActivityIndicator(void)
{
    [[CCNative_objc sharedInstance] hideActivityIndicator];
}

void CCNative::vibrate()
{
    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
}

#pragma mark -
#pragma mark alert view

void CCNative::createAlert(const char* title,
                           const char* message,
                           const char* cancelButtonTitle)
{
    NSString *title_ = [NSString stringWithUTF8String:title ? title : ""];
    NSString *message_ = [NSString stringWithUTF8String:message ? message : ""];
    NSString *cancelButtonTitle_ = cancelButtonTitle ? [NSString stringWithUTF8String:cancelButtonTitle] : nil;
    [[CCNative_objc sharedInstance] createAlertView:title_
                                         andMessage:message_
                               andCancelButtonTitle:cancelButtonTitle_];
}

int CCNative::addAlertButton(const char* buttonTitle)
{
    NSString *buttonTitle_ = [NSString stringWithUTF8String:buttonTitle ? buttonTitle : "Button"];
    return [[CCNative_objc sharedInstance] addAlertButton:buttonTitle_];
}

#if CC_LUA_ENGINE_ENABLED > 0
int CCNative::addAlertButtonLua(const char* buttonTitle)
{
    return addAlertButton(buttonTitle) + 1;
}
#endif

void CCNative::showAlert(CCAlertViewDelegate* delegate)
{
    [[CCNative_objc sharedInstance] showAlertViewWithDelegate:delegate];
}

#if CC_LUA_ENGINE_ENABLED > 0
void CCNative::showAlertLua(cocos2d::LUA_FUNCTION listener)
{
    [[CCNative_objc sharedInstance] showAlertViewWithLuaListener:listener];
}
#endif

void CCNative::cancelAlert(void)
{
    [[CCNative_objc sharedInstance] cancelAlertView];
}

void CCNative::promptForExit()
{
    // Do nothing. This method is only valid for android
}

static float asMB(unsigned bytes)
{
    return (float)bytes / (1024.0f * 1024.0f);
}

void CCNative::reportMemoryUsage()
{
    static unsigned last_resident_size=0;
    static unsigned greatest = 0;
    static unsigned last_greatest = 0;
    
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    kern_return_t kerr = task_info(mach_task_self(),
                                   TASK_BASIC_INFO,
                                   (task_info_t)&info,
                                   &size);
    if( kerr == KERN_SUCCESS ) {
        int diff = (int)info.resident_size - (int)last_resident_size;
        unsigned latest = info.resident_size;
        if( latest > greatest   )   greatest = latest;  // track greatest mem usage
        int greatest_diff = greatest - last_greatest;
        NSLog(@"Mem: %.2f MB (%.2f MB) : greatest: %.2f MB (%.2f MB)",
              asMB(info.resident_size),
              asMB(diff),
              asMB(greatest),
              asMB(greatest_diff));
    } else {
        NSLog(@"Error with task_info(): %s", mach_error_string(kerr));
    }
    last_resident_size = info.resident_size;
    last_greatest = greatest;
}

void CCNative::clearLocalPushNotification()
{
    UIApplication *app                = [UIApplication sharedApplication];
	NSArray *oldNotifications         = [app scheduledLocalNotifications];
	app.applicationIconBadgeNumber=0;
	if ([oldNotifications count] > 0) {
		[app cancelAllLocalNotifications];
	}
}
void CCNative::registerLocalPushNotification(const char* content, int iFireDate, LocalPushRepeat iRepeat)
{
    UIApplication *app                = [UIApplication sharedApplication];
    UILocalNotification *notification = [[UILocalNotification alloc] init];
    
    if (notification != nil)
	{
        NSDateComponents *dateComps=nil;
		notification.timeZone  = [NSTimeZone systemTimeZone];
        switch (iRepeat) {
            case LocalPushRepeatDaily:
            {
                NSCalendar *calendar = [NSCalendar autoupdatingCurrentCalendar];
                NSDate *today = [NSDate date];
                
                NSDateComponents *dateComponents = [calendar components:(NSYearCalendarUnit | NSMonthCalendarUnit | NSDayCalendarUnit)
                                                               fromDate:today];
                dateComps = [[NSDateComponents alloc] init];
                
                [dateComps setDay:dateComponents.day+1];
                [dateComps setMonth:dateComponents.month];
                [dateComps setYear:dateComponents.year];
                
                [dateComps setHour:iFireDate/3600];
                [dateComps setMinute:(iFireDate%3600)/60];
                [dateComps setSecond:iFireDate%60];
                [dateComps setTimeZone:[NSTimeZone systemTimeZone]];
                NSDate  *fireDate = [calendar dateFromComponents:dateComps];
                notification.fireDate  = fireDate;
                
                notification.repeatInterval=NSDayCalendarUnit;
                break;
            }
            case LocalPushRepeatWeekly:
            {
                notification.repeatInterval=NSWeekCalendarUnit;
                break;
            }
            case LocalPushRepeatMonthly:
            {
                notification.repeatInterval=NSMonthCalendarUnit;
                break;
            }
            default:
            {
                NSDate  *fireDate = [NSDate dateWithTimeIntervalSince1970:iFireDate];
                notification.fireDate  = fireDate;
                break;
            }
        }
        
        notification.alertBody = [NSString stringWithUTF8String:content];
		notification.applicationIconBadgeNumber = 1;
		//notification.alertAction;
		[app scheduleLocalNotification:notification];
        
        if(dateComps)
            [dateComps release];
	}
	[notification release];
}

LanguageIdentifier CCNative::getLanguage()
{
    LanguageIdentifier ret;
    // get the current language and country config
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSArray *languages = [defaults objectForKey:@"AppleLanguages"];
    NSString *currentLanguage = [languages objectAtIndex:0];
    
    // get the current language code.(such as English is "en", Chinese is "zh" and so on)
    NSDictionary* temp = [NSLocale componentsFromLocaleIdentifier:currentLanguage];
    NSString * languageCode = [temp objectForKey:NSLocaleScriptCode];
    if ([languageCode isEqualToString:@"Hant"])
    {
        ret = kLanguageTraditionalChinese;
    }
    else
    {
        ret = (LanguageIdentifier)cocos2d::CCApplication::sharedApplication()->getCurrentLanguage();
    }
    
    return ret;
}

bool CCNative::openURL(const std::string& url)
{
    if (url.empty())
        return false;
    NSURL *nsurl = [NSURL URLWithString:[NSString stringWithCString:url.c_str() encoding:NSUTF8StringEncoding]];
    return [[UIApplication sharedApplication] openURL:nsurl];
}

std::string CCNative::getDocPath()
{
    // save to document folder
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    std::string strRet = [documentsDirectory UTF8String];
    strRet.append("/");
    return strRet;
}

std::string CCNative::getCachePath()
{
    // save to document folder
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    std::string strRet = [documentsDirectory UTF8String];
    strRet.append("/");
    return strRet;
}

void CCNative::copyFile(const std::string& from, const std::string& to)
{
    std::string source = CCFileUtils::sharedFileUtils()->fullPathForFilename(from.c_str());
    std::string target = CCFileUtils::sharedFileUtils()->fullPathForFilename(to.c_str());
    
    NSError* error = nil;
    [[NSFileManager defaultManager] copyItemAtPath:[NSString stringWithUTF8String:source.c_str()]
                                            toPath:[NSString stringWithUTF8String:target.c_str()]
                                             error:&error];
    if (error)
        NSLog(@"Copy file error: %@", error);
}

void CCNative::copyAssetToPath(const std::string& from, const std::string& to)
{
    CCNative::copyFile(from, to);
}

void CCNative::focusGLView()
{}

void CCNative::moveFile(const std::string& from, const std::string& to)
{
    std::string source = CCFileUtils::sharedFileUtils()->fullPathForFilename(from.c_str());
    std::string target = CCFileUtils::sharedFileUtils()->fullPathForFilename(to.c_str());
    
//    int removeRet = remove(target.c_str());
//    if (removeRet) perror("remove");
//    int renameRet = rename(source.c_str(), target.c_str());
//    if (renameRet) perror("rename");
    
    NSError* error = nil;
    [[NSFileManager defaultManager] removeItemAtPath:[NSString stringWithUTF8String:target.c_str()] error:&error];
    if (error)
        NSLog(@"remove file error: %@", error);
    [[NSFileManager defaultManager] moveItemAtPath:[NSString stringWithUTF8String:source.c_str()]
                                            toPath:[NSString stringWithUTF8String:target.c_str()]
                                             error:&error];
    if (error)
        NSLog(@"Move file error: %@", error);
}

void CCNative::removeFile(const std::string& file)
{
    NSError* error = nil;
    [[NSFileManager defaultManager] removeItemAtPath:[NSString stringWithUTF8String:file.c_str()] error:&error];
    if (error)
        NSLog(@"Remove file error: %@", error);
}

unsigned long long CCNative::getFreeDiskSpace()
{
    unsigned long long totalSpace = 0;
    unsigned long long totalFreeSpace = 0;
    
    __autoreleasing NSError *error = nil;
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSDictionary *dictionary = [[NSFileManager defaultManager] attributesOfFileSystemForPath:[paths lastObject] error: &error];
    
    if (dictionary) {
        NSNumber *fileSystemSizeInBytes = [dictionary objectForKey: NSFileSystemSize];
        NSNumber *freeFileSystemSizeInBytes = [dictionary objectForKey:NSFileSystemFreeSize];
        totalSpace = [fileSystemSizeInBytes unsignedLongLongValue];
        totalFreeSpace = [freeFileSystemSizeInBytes unsignedLongLongValue];
        NSLog(@"Memory Capacity of %llu MiB with %llu MiB Free memory available.", ((totalSpace/1024ll)/1024ll), ((totalFreeSpace/1024ll)/1024ll));
    } else {
        NSLog(@"Error Obtaining System Memory Info: Domain = %@, Code = %d", [error domain], [error code]);
    }  
    
    return totalFreeSpace;
}

#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)

bool CCNative::addSkipBackupAttributeToItem(const std::string& file)
{
    bool success = true;
    
    NSURL* URL = [NSURL fileURLWithPath:[NSString stringWithUTF8String:file.c_str()]];
    if ([[NSFileManager defaultManager] fileExistsAtPath: [URL path]] && SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(@"5.1"))
    {
        NSError *error = nil;
        success = [URL setResourceValue:[NSNumber numberWithBool: YES] forKey:@"NSURLIsExcludedFromBackupKey" error:&error];
        if(!success)
        {
            NSLog(@"Error excluding %@ from backup %@", [URL lastPathComponent], error);
        }
    }
    return success;
}

#pragma mark -
#pragma mark OpenUDID

std::string CCNative::getOpenUDID(void)
{
//    return std::string([[OpenUDID_objc value] cStringUsingEncoding:NSUTF8StringEncoding]);
    return "";
}

std::string CCNative::device()
{
    std::string ret = std::string("iOS|") + [[UIDevice currentDevice] platformString].UTF8String;
    if ([[UIDevice currentDevice] isJailBroken])
        ret += "|JailBroken";
    
    return ret;
}

int CCNative::getResolutionWidth()
{
    CCEGLView* glView = CCEGLView::sharedOpenGLView();
    CCSize frameSize = glView->getFrameSize();
    return frameSize.width;
}

int CCNative::getResolutionHeight()
{
    CCEGLView* glView = CCEGLView::sharedOpenGLView();
    CCSize frameSize = glView->getFrameSize();
    return frameSize.height;
}

std::string CCNative::OS()
{
    return "iOS";
}

std::string CCNative::getBuildNum()
{
    NSDictionary *appInfo = [[NSBundle mainBundle] infoDictionary];
    return [[appInfo objectForKey:@"CFBundleVersion"] UTF8String];
}

std::string CCNative::OSVersion()
{
    return [UIDevice currentDevice].systemVersion.UTF8String;
}

std::string CCNative::IPAddress()
{
    return [[UIDevice currentDevice] ipAddress].UTF8String;
}

std::string CCNative::MACAddress()
{
    return [[UIDevice currentDevice] macaddress].UTF8String;
}

NS_CC_EXT_END
