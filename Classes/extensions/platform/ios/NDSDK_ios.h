//
//  NDSDK_ios.h
//  HelloCpp
//
//  Created by Wu Zhen on 12-11-17.
//
//

#import <Foundation/Foundation.h>
#import <NdComPlatform/NdComPlatform.h>
#import "ND.h"
#include <map>

namespace PH {
    class ND_iOS;
}

@interface NDSDK_ios : NSObject<NdComPlatformUIProtocol>

@property (nonatomic, assign) boost::function<void (PH::ND_APP_UPDATE_RESULT)> versionUpdatCallback;
@property (nonatomic, assign) boost::function<void (PH::ND_PLATFORM_ERRORS)> loginCallback;
@property (nonatomic, assign) boost::function<void ()> leavePlatformCallback;
@property (nonatomic, assign) std::map<NdBuyInfo*, boost::function<void (PH::ND_PLATFORM_ERRORS)>>* buyInfoCallback;
@end

namespace PH {
    class ND_iOS : public ND
    {
    public:
        ND_iOS();
        ~ND_iOS();
    public:
        virtual void setAppIdAndAppKey(int appId, const std::string& appKey);
        virtual void setDebugMode(int flag=0);
        
        virtual void checkVersionUpdate(boost::function<void (ND_APP_UPDATE_RESULT)> callback);
        
        virtual void setPlatformUIOrientation(ND_PLATFORM_ORIENTATION);
        
    public: // Login related methods
        virtual void login(boost::function<void (ND_PLATFORM_ERRORS)>);
        
        virtual void guestLogin(boost::function<void (ND_PLATFORM_ERRORS)>);
        virtual void guestRegist(boost::function<void (ND_PLATFORM_ERRORS)>);
        
        virtual bool isLogedIn();
        virtual bool isGuestLogin();
        
        virtual std::string UIN();
        virtual std::string nickName();
        
        virtual void logout();
        
    public: // purchasing
        virtual void pay(StoreItem&, boost::function<void (ND_PLATFORM_ERRORS, const std::string&)> callback);
        
    public: // 91 platform UI
        virtual void feedback();
        virtual void enterPlatform(boost::function<void (ND_PLATFORM_ERRORS)>);

    public: // local notification
        virtual void queueLocalNotification(const std::string& msg, int seconds);
        virtual void cancelAllLocalNotifications();
        
    private:
        NDSDK_ios* delegate;
        bool loginResultCalledInPlatformUI;
        ND_PLATFORM_ERRORS lastLoginResult;
    };
}

