// Copyright 2016 Mail.Ru Group. All Rights Reserved.

#include "MRGServicePrivatePCH.h"
#include "MRGS.h"

#if WITH_MRGS

#if PLATFORM_IOS
    using namespace mrgs;
#endif

/** Common Delegate */

void MRGServiceDelegate::loadServerDataDidFinished(std::string json) {
	#if PLATFORM_IOS || PLATFORM_ANDROID
		MRGSLOG("MRGServiceDelegate::loadServerDataDidFinished('%s')", json.c_str());
	#endif
}

void MRGServiceDelegate::loadPromoBannersDidFinished(std::string json) {
	#if PLATFORM_IOS || PLATFORM_ANDROID
		MRGSLOG("MRGServiceDelegate::loadPromoBannersDidFinished('%s')", json.c_str());
	#endif
}


/** Billing Delegate */

void MRGSBillingDelegate::loadProductsDidFinished(const std::vector<MRGSPurchaseItem>& items) {
    std::string value = "";
    std::string comma = "";
    for (int i = 0; i < items.size(); i++) {
        value += comma + "{sku: " + items[i].sku + "; title: " + items[i].title + "; price: " + items[i].price + "}";
        comma = ", ";
    }

	#if PLATFORM_IOS || PLATFORM_ANDROID
		MRGSLOG("MRGSBillingDelegate::loadProductsDidFinished(%s)", value.c_str());
	#endif
    if(MRGSBillingDelegate::loadProductsComplete != nullptr)
    {
//        NSLog(@"MRGSBillingDelegate::loadProductsComplete");
        MRGSBillingDelegate::loadProductsComplete();
    }
}

void MRGSBillingDelegate::purchaseComplete(const std::string& sku, const std::string& transactionId, const std::string& answer) {
	#if PLATFORM_IOS || PLATFORM_ANDROID
		MRGSLOG("MRGSBillingDelegate::purchaseComplete('%s', '%s', '%s')", sku.c_str(), transactionId.c_str(), answer.c_str());
	#endif
	
    if(MRGSBillingDelegate::purchaseCompleteCallback != nullptr)
    {
//        NSLog(@"MRGSBillingDelegate::purchaseCompleteCallback");
        MRGSBillingDelegate::purchaseCompleteCallback(sku, transactionId);
    }
}

void MRGSBillingDelegate::purchaseFail(const std::string& sku, const std::string& answer) {
	#if PLATFORM_IOS || PLATFORM_ANDROID
		MRGSLOG("MRGSBillingDelegate::purchaseFail('%s', '%s')", sku.c_str(), answer.c_str());
	#endif
    if(MRGSBillingDelegate::purchaseFailCallback != nullptr)
    {
//        NSLog(@"MRGSBillingDelegate::purchaseFailCallback");
        MRGSBillingDelegate::purchaseFailCallback(sku, answer);
    }
}



/** Notification Service Delegate */

//void MRGSNotificationServiceDelegate::clickOnNotification(int notificationId, std::string title, std::string message, bool isLocal) {
//    MRGSLOG("MRGSNotificationServiceDelegate::clickOnNotification(%d, '%s', '%s', %s)", notificationId, title.c_str(), message.c_str(), (isLocal ? "local" : "server"));
//}
//
//void MRGSNotificationServiceDelegate::receivedNotification(int notificationId, std::string title, std::string message, bool isLocal) {
//    MRGSLOG("MRGSNotificationServiceDelegate::receivedNotification(%d, '%s', '%s', %s)", notificationId, title.c_str(), message.c_str(), (isLocal ? "local" : "server"));
//}

#endif // WITH_MRGS
