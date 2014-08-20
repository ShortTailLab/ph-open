//
//  NDSDK_android.h
//  HelloCpp
//
//  Created by R.C on 12-11-26.
//
//

#ifndef __HelloCpp__NDSDK_android__
#define __HelloCpp__NDSDK_android__

#include "NDSDK/ND.h"

namespace PH
{
    class ND_Android : public ND
    {
    public:
        ND_Android();
        ~ND_Android();
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
        
        virtual void logout();
        virtual std::string nickName();
        
        virtual std::string UIN();
        
    public: // 91 platform UI
        virtual void feedback();
        virtual void enterPlatform(boost::function<void (ND_PLATFORM_ERRORS)>);
        
    public: // purchasing
        virtual void pay(StoreItem&, boost::function<void (ND_PLATFORM_ERRORS, const std::string&)> callback);
        
    public: // local notification
        virtual void queueLocalNotification(const std::string& msg, int seconds);
        virtual void cancelAllLocalNotifications();
    };
}

#endif /* defined(__HelloCpp__NDSDK_android__) */
