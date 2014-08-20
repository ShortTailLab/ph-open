
#ifndef __CC_EXTENSION_CCSTORE_TRANSACTION_OBSERVER_H_
#define __CC_EXTENSION_CCSTORE_TRANSACTION_OBSERVER_H_

#include "cocos2dx_extensions.h"
#include "store/CCStorePaymentTransaction.h"

NS_CC_EXT_BEGIN

class CCStoreTransactionObserver
{
public:
    virtual void transactionCompleted(CCStorePaymentTransaction* transaction) = 0;
    virtual void transactionFailed(CCStorePaymentTransaction* transaction) = 0;
    virtual void transactionRestored(CCStorePaymentTransaction* transaction) = 0;
};

NS_CC_EXT_END

#endif // __CC_EXTENSION_CCSTORE_TRANSACTION_OBSERVER_H_
