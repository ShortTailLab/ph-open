//
//  OurpalmPurchaseListener.h
//  HelloCpp
//
//  Created by Zhen Wu on 13-8-17.
//
//

#ifndef __HelloCpp__OurpalmPurchaseListener__
#define __HelloCpp__OurpalmPurchaseListener__

#include "extensions/DistroSDKs/OurPalm/OPPurchaseListener.h"

namespace PH
{
    class OurpalmPurchaseListener : public PurchaseListener
    {
    public:
        static OurpalmPurchaseListener& instance();
        
        virtual void OnPurchaseResult(bool result, const char* goodid) override;
    };
}

#endif /* defined(__HelloCpp__OurpalmPurchaseListener__) */
