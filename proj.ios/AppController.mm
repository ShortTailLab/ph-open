/****************************************************************************
 Copyright (c) 2010 cocos2d-x.org
 
 http://www.cocos2d-x.org
 
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
#import <UIKit/UIKit.h>
#import "AppController.h"
#import "AppDelegate.h"
#import "cocos2d.h"
#import "EAGLView.h"
#import "RootViewController.h"
#import "Common.h"
#import "CCNative.h"
#import "DebugMisc.h"
#import "Weixin_ios.h"
#import "MobClick.h"
#import <boost/make_shared.hpp>
#import "BuildConf.h"

@implementation AppController
@synthesize window;

#pragma mark -
#pragma mark Application lifecycle

// cocos2d application instance
static AppDelegate s_sharedApplication;

- (void)copyUserDefaultsFile
{
    NSArray *dpaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [dpaths objectAtIndex:0];
    
    NSArray *cpaths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *cacheDirectory = [cpaths objectAtIndex:0];
    
    NSString *dUserDefaults = [documentsDirectory stringByAppendingPathComponent:@"UserDefault.xml"];
    NSString *cUserDefaults = [cacheDirectory stringByAppendingPathComponent:@"UserDefault.xml"];
    
    NSError* error = nil;
    [[NSFileManager defaultManager] copyItemAtPath:cUserDefaults toPath:dUserDefaults error:&error];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // UMeng
    {
        [MobClick startWithAppkey:@"53f2e6a3fd98c536af003936" reportPolicy:SEND_INTERVAL channelId:[NSString stringWithUTF8String:PH_CHANNEL_ID]];
        [MobClick setLogSendInterval:30.];
    }
    
    [self copyUserDefaultsFile];

    // Add the view controller's view to the window and display.
    window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
    EAGLView *__glView = [EAGLView viewWithFrame: [window bounds]
                                     pixelFormat: kEAGLColorFormatRGBA8
                                     depthFormat: GL_DEPTH_COMPONENT16
                              preserveBackbuffer: NO
                                      sharegroup: nil
                                   multiSampling: NO
                                 numberOfSamples: 0];
    
    // Use RootViewController manage EAGLView
    viewController = [[RootViewController alloc] initWithNibName:nil bundle:nil];
    viewController.wantsFullScreenLayout = YES;
    viewController.view = __glView;

    // Set RootViewController to window
    [window addSubview: viewController.view];
    [window makeKeyAndVisible];
    window.rootViewController = viewController;

    [[UIApplication sharedApplication] setStatusBarHidden: YES];
    
    cocos2d::CCApplication::sharedApplication()->run();
    
    UIRemoteNotificationType regNotiType = UIRemoteNotificationTypeBadge | UIRemoteNotificationTypeAlert | UIRemoteNotificationTypeSound;
    [[UIApplication sharedApplication] registerForRemoteNotificationTypes:regNotiType];
    
    return YES;
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken
{
    NSCharacterSet* trimCharacters = [NSCharacterSet characterSetWithCharactersInString:@"<> "];
    NSString* trimmedString = [[deviceToken description] stringByTrimmingCharactersInSet:trimCharacters];
    
    std::string token = trimmedString.UTF8String;
    if (!token.empty())
    {
        PH::GameConfig::shared()->pushToken = "APNS|" + token;
    }
}

- (void)application:(UIApplication *)app didFailToRegisterForRemoteNotificationsWithError:(NSError *)err
{
    NSLog(@"Error in registration. Error: %@", err);
}

- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
    cocos2d::CCDirector::sharedDirector()->pause();
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
    cocos2d::CCDirector::sharedDirector()->resume();
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
    cocos2d::CCApplication::sharedApplication()->applicationDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
    cocos2d::CCApplication::sharedApplication()->applicationWillEnterForeground();
}

- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url
{
    return [WXApi handleOpenURL:url delegate:self]
    || ((AppDelegate*)cocos2d::CCApplication::sharedApplication())->openURL(url.absoluteString.UTF8String, "");
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
    return [WXApi handleOpenURL:url delegate:self]
    || ((AppDelegate*)cocos2d::CCApplication::sharedApplication())->openURL(url.absoluteString.UTF8String, sourceApplication.UTF8String);
}

#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
    cocos2d::CCLog("Application received memory warning. Free unused textures\n");
    cocos2d::CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}


- (void)dealloc {
    [super dealloc];
}


#pragma mark - Weixin API


-(void) onReq:(BaseReq*)req
{
    PH::WeixinApi::instance().onReq(boost::make_shared<PH::WXReqWrapper>(req));
}

-(void) onResp:(BaseResp*)resp
{
    PH::WeixinApi::instance().onResp(boost::make_shared<PH::WXRespWrapper>(resp));
}

@end

