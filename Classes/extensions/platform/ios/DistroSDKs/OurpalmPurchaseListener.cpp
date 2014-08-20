//
//  OurpalmPurchaseListener.cpp
//  HelloCpp
//
//  Created by Zhen Wu on 13-8-17.
//
//

#include "OurpalmPurchaseListener.h"
#include "extensions/utils.h"
#include "BuildConf.h"

namespace PH
{
    OurpalmPurchaseListener& OurpalmPurchaseListener::instance()
    {
        static OurpalmPurchaseListener* _ins = new OurpalmPurchaseListener();
        return *_ins;
    }
    
    void OurpalmPurchaseListener::OnPurchaseResult(bool result, const char* goodid)
    {
        std::map<std::string, std::string> params;
        if (result)
        {
            params["goodid"] = goodid;
            
            InProcEventCentral::signal("purchase.order.placed", params);
        }
        else
        {
            InProcEventCentral::signal("purchase.order.cancel", params);
        }
    }
}
