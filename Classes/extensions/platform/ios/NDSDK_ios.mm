//
//  NDSDK_ios.m
//  HelloCpp
//
//  Created by Wu Zhen on 12-11-17.
//
//

#import "NDSDK_ios.h"
#import <boost/bind.hpp>
#import <boost/function.hpp>
#import <boost/uuid/uuid_generators.hpp>
#import <boost/uuid/uuid_io.hpp>
#import <NdComPlatform/NdComPlatformAPIResponse.h>
#import <NdComPlatform/NdCPNotifications.h>


using namespace boost;
using namespace boost::uuids;

@implementation NDSDK_ios
@synthesize versionUpdatCallback, loginCallback, buyInfoCallback;
@synthesize leavePlatformCallback;

- (id)init
{
    self = [super init];
    if (self)
    {
        buyInfoCallback = new std::map<NdBuyInfo*, boost::function<void (PH::ND_PLATFORM_ERRORS)>>;
    }
    return self;
}

- (void)dealloc
{
    delete buyInfoCallback;
    [super dealloc];
}

- (void)uploadChannelIdDidFinish:(int)error
{
    NSLog(@"unhandled nd callback uploadChannelIdDidFinish");
}

- (void)appVersionUpdateDidFinish:(ND_APP_UPDATE_RESULT)updateResult
{
    versionUpdatCallback((PH::ND_APP_UPDATE_RESULT)updateResult);
}

- (void)SNSLoginResult:(NSNotification *)notify
{
    NSDictionary *dict = [notify userInfo];
    BOOL bSuccess = [[dict objectForKey:@"result"] boolValue];
    int error =  PH::ND_PLATFORM_NO_ERROR;
    if (!bSuccess)
        error = [[dict objectForKey:@"error"] intValue];
    else
    {
        NdGuestAccountStatus* guestStatus = (NdGuestAccountStatus*)[dict objectForKey:@"NdGuestAccountStatus"];
        if (guestStatus && [guestStatus isGuestLogined])
            error = PH::ND_PLATFORM_NO_ERROR;
        else if (guestStatus && [guestStatus isGuestRegistered])
            error = PH::ND_PLATFORM_ERROR_GUEST_OFFICIAL_SUCCESS;
        else
            error = PH::ND_PLATFORM_NO_ERROR;
    }
    
    loginCallback((PH::ND_PLATFORM_ERRORS)error);
}

- (void)NdUniPayResult:(NSNotification*)notify
{
    NSDictionary *dict = [notify userInfo];
    BOOL bSuccess = [[dict objectForKey:@"result"] boolValue];
    int error =  PH::ND_PLATFORM_NO_ERROR;
    if (!bSuccess)
        error = [[dict objectForKey:@"error"] intValue];
    
    NdBuyInfo* buyInfo = (NdBuyInfo*)[dict objectForKey:@"buyInfo"];
    (*buyInfoCallback)[buyInfo]((PH::ND_PLATFORM_ERRORS)error);
    (*buyInfoCallback).erase(buyInfo);
    [buyInfo release];
}

- (void)SNSLeaveComplatformUI:(NSNotification*)notify
{
    leavePlatformCallback();
}

@end

using namespace PH;

ND& ND::instance()
{
    static ND_iOS* ins = NULL;
    if (!ins)
        ins = new ND_iOS;
    return *ins;
}

PH::ND_iOS::ND_iOS()
{
    delegate = [[NDSDK_ios alloc] init];
    [[NSNotificationCenter defaultCenter] addObserver:delegate
                                             selector:@selector(SNSLoginResult:)
                                                 name:(NSString *)kNdCPLoginNotification
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:delegate
                                             selector:@selector(NdUniPayResult:)
                                                 name:kNdCPBuyResultNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:delegate
                                             selector:@selector(SNSLeaveComplatformUI:)
                                                 name:(NSString *)kNdCPLeavePlatformNotification
                                               object:nil];
}

PH::ND_iOS::~ND_iOS()
{
    [[NSNotificationCenter defaultCenter] removeObserver:delegate];
    [delegate release];
}

void PH::ND_iOS::setDebugMode(int)
{
    [[NdComPlatform defaultPlatform] NdSetDebugMode:0];
}

void PH::ND_iOS::setAppIdAndAppKey(int appId, const std::string &appKey)
{
    [[NdComPlatform defaultPlatform] setAppId:appId];
    [[NdComPlatform defaultPlatform] setAppKey:[NSString stringWithUTF8String:appKey.c_str()]];
}

void PH::ND_iOS::checkVersionUpdate(boost::function<void (ND_APP_UPDATE_RESULT)> callback)
{
    delegate.versionUpdatCallback = callback;
    [[NdComPlatform defaultPlatform] NdAppVersionUpdate:0 delegate:delegate];
}

void PH::ND_iOS::setPlatformUIOrientation(PH::ND_PLATFORM_ORIENTATION o)
{
    UIInterfaceOrientation orient = UIInterfaceOrientationPortrait;
    if (o == ND_PLATFORM_LandscapeLeft)
        orient = UIInterfaceOrientationLandscapeLeft;
    else if (o == ND_PLATFORM_LandscapeRight)
        orient = UIInterfaceOrientationLandscapeRight;
    
    [[NdComPlatform defaultPlatform] NdSetScreenOrientation:orient];
}

void PH::ND_iOS::login(boost::function<void (ND_PLATFORM_ERRORS)> cb)
{
    delegate.loginCallback = cb;
    [[NdComPlatform defaultPlatform] NdLogin:0];
}

void PH::ND_iOS::guestLogin(boost::function<void (ND_PLATFORM_ERRORS)> cb)
{
    delegate.loginCallback = cb;
    [[NdComPlatform defaultPlatform] NdLoginEx:0];
}

void PH::ND_iOS::guestRegist(boost::function<void (ND_PLATFORM_ERRORS)> cb)
{
    delegate.loginCallback = cb;
    delegate.leavePlatformCallback = [](){};
    [[NdComPlatform defaultPlatform] NdGuestRegist:0];
}

bool PH::ND_iOS::isLogedIn()
{
    return [[NdComPlatform defaultPlatform] isLogined];
}

bool PH::ND_iOS::isGuestLogin()
{
    return [[NdComPlatform defaultPlatform] getCurrentLoginState]  == ND_LOGIN_STATE_GUEST_LOGIN;
}

std::string PH::ND_iOS::UIN()
{
    return [[NdComPlatform defaultPlatform] loginUin].UTF8String;
}

std::string PH::ND_iOS::nickName()
{
    return [[NdComPlatform defaultPlatform] nickName].UTF8String;
}

void PH::ND_iOS::logout()
{
    [[NdComPlatform defaultPlatform] NdLogout:1];
}

void PH::ND_iOS::pay(PH::StoreItem& item, boost::function<void (ND_PLATFORM_ERRORS, const std::string&)> callback)
{
    std::string serial = to_string(boost::uuids::random_generator()());
    
    NdBuyInfo* buyInfo = [[NdBuyInfo alloc] init];
    buyInfo.cooOrderSerial = [NSString stringWithUTF8String: serial.c_str()];
    buyInfo.productCount = 1;
    buyInfo.productId = [NSString stringWithUTF8String:item.productId.c_str()];
    buyInfo.productOrignalPrice = item.price;
    buyInfo.productPrice = item.price;
    buyInfo.productName = [NSString stringWithUTF8String:item.productName.c_str()];
    
    (*[delegate buyInfoCallback])[buyInfo] = boost::bind(callback, _1, serial);
    int error = [[NdComPlatform defaultPlatform] NdUniPay:buyInfo];
    if (error != 0)
    {
        printf("NdBuy argument error %d", error);
    }
}

void PH::ND_iOS::enterPlatform(boost::function<void (ND_PLATFORM_ERRORS)> cb)
{
    delegate.loginCallback = [=](ND_PLATFORM_ERRORS err){
        this->lastLoginResult = err;
        this->loginResultCalledInPlatformUI = true;
    };
    delegate.leavePlatformCallback = [=](){
        if (this->loginResultCalledInPlatformUI)
            cb(this->lastLoginResult);
        else
            cb((ND_PLATFORM_ERRORS)1);
    };
    loginResultCalledInPlatformUI = false;
    
    [[NdComPlatform defaultPlatform] NdEnterPlatform:0];
}

void PH::ND_iOS::feedback()
{
    delegate.leavePlatformCallback = [=](){};
    [[NdComPlatform defaultPlatform] NdUserFeedBack];
}

void PH::ND_iOS::queueLocalNotification(const std::string &msg, int seconds)
{
    [[NdComPlatform defaultPlatform] NdSetLocalNotification:seconds alertBody:[NSString stringWithUTF8String:msg.c_str()]];
}

void PH::ND_iOS::cancelAllLocalNotifications()
{
    [[NdComPlatform defaultPlatform] NdCancelAllLocalNotification];
}
