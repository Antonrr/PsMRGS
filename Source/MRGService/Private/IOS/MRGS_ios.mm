// Copyright 2016 Mail.Ru Group. All Rights Reserved.

#include "MRGServicePrivatePCH.h"

#if WITH_MRGS

#if PLATFORM_IOS

#include <map>

using namespace std;
using namespace mrgs;

MRGSBillingDelegate* MRGSBilling::delegate = NULL;
//MRGSNotificationServiceDelegate* MRGSNotificationService::delegate = NULL;
MRGServiceDelegate* MRGService::delegate = NULL;
vector<MRGSPurchaseItem> MRGSBilling::PurchaseItems;

namespace mrgs {
    
    void MRGSLOG(const char* format, ...) {
        va_list argptr;
        va_start(argptr, format);
        vfprintf(stderr, format, argptr);
        va_end(argptr);
    }
    
/** Flurry */
    
    void MRGService::flurryEvent(const std::string &event) {
        NSLog(@"MRGService::flurryEvent");
        NSString *e = [NSString stringWithUTF8String:event.c_str()];
        [MRGSFlurry logEvent:e withParameters:nil];
    }
    
    void MRGService::flurryEvent(const std::string &event, const std::map<std::string, std::string> params) {
        NSString *e = [NSString stringWithUTF8String:event.c_str()];
        
        NSMutableDictionary *p = [NSMutableDictionary dictionary];
        for (map<string, string>::const_iterator it = params.begin(); it != params.end(); it++) {
            NSString *key = [NSString stringWithUTF8String:(*it).first.c_str()];
            NSString *value = [NSString stringWithUTF8String:(*it).second.c_str()];
            if (key && value) p[key] = value;
        }
        
        [MRGSFlurry logEvent:e withParameters:p];
    }
    
    void MRGService::flurryEventStart(const std::string &event, const std::map<std::string, std::string> params) {
        NSString *e = [NSString stringWithUTF8String:event.c_str()];
        
        NSMutableDictionary *p = [NSMutableDictionary dictionary];
        for (map<string, string>::const_iterator it = params.begin(); it != params.end(); it++) {
            NSString *key = [NSString stringWithUTF8String:(*it).first.c_str()];
            NSString *value = [NSString stringWithUTF8String:(*it).second.c_str()];
            if (key && value) p[key] = value;
        }
        
        [MRGSFlurry logEvent:e withParameters:p timed:YES];
    }
    
    void MRGService::flurryEventStop(const std::string &event, const std::map<std::string, std::string> params) {
        NSString *e = [NSString stringWithUTF8String:event.c_str()];
        
        NSMutableDictionary *p = [NSMutableDictionary dictionary];
        for (map<string, string>::const_iterator it = params.begin(); it != params.end(); it++) {
            NSString *key = [NSString stringWithUTF8String:(*it).first.c_str()];
            NSString *value = [NSString stringWithUTF8String:(*it).second.c_str()];
            if (key && value) p[key] = value;
        }
        
        [MRGSFlurry endTimedEvent:e withParameters:p];
    }
    
/** Google Analytics */
    
    void MRGService::sendGAEvent(const std::string &category, const std::string &action, const std::string &label, int value) {
        NSString *c = [NSString stringWithUTF8String:category.c_str()];
        NSString *a = [NSString stringWithUTF8String:action.c_str()];
        NSString *l = [NSString stringWithUTF8String:label.c_str()];
        
        NSLog(@"cat: %@ action: %@ label: %@", c, a, l);
        [MRGSGoogleAnalytics createEventWithCategory:@"cat" action:@"act" label:@"lbl" value:[NSNumber numberWithInt:1]/*@(value)*/];
    }
    
    void MRGService::sendGAScreen(const std::string &screenName) {
        NSString *sn = [NSString stringWithUTF8String:screenName.c_str()];
        [MRGSGoogleAnalytics setNewScreenName:sn];
    }
    
    void MRGService::sendGASocEvent(const std::string &network, const std::string &action, const std::string &target) {
        NSString *n = [NSString stringWithUTF8String:network.c_str()];
        NSString *a = [NSString stringWithUTF8String:action.c_str()];
        NSString *t = [NSString stringWithUTF8String:target.c_str()];
        [MRGSGoogleAnalytics createSocialWithNetwork:n action:a target:t];
    }
    
    void MRGService::sendGATimings(const std::string &category, long long interval, const std::string &name, const std::string &label) {
        NSString *c = [NSString stringWithUTF8String:category.c_str()];
        NSString *n = [NSString stringWithUTF8String:name.c_str()];
        NSString *l = [NSString stringWithUTF8String:label.c_str()];
        [MRGSGoogleAnalytics createTimingWithCategory:c interval:@(interval) name:n label:l];
    }
    
/** AppsFlyer */
    
    void MRGService::sendAFEvent(const std::string &eventName, const std::string &value) {
        NSLog(@"MRGService::sendAFEvent");
        NSString *e = [NSString stringWithUTF8String:eventName.c_str()];
        NSString *v = [NSString stringWithUTF8String:value.c_str()];
        [MRGSAppsFlyer notifyEvent:e eventValue:v];
    }
    
    void MRGService::sendAFEventIos(const std::string &eventName, const std::string &value) {
        NSLog(@"MRGService::sendAFEventIos");
        NSString *e = [NSString stringWithUTF8String:eventName.c_str()];
        NSString *v = [NSString stringWithUTF8String:value.c_str()];
        [MRGSAppsFlyer notifyEvent:e eventValue:v];
    }
    
/** Handle Exception */
    
    void MRGService::sendHandleException(const std::string &description) {
        NSString *s = [NSString stringWithUTF8String:description.c_str()];
        [MRGServiceInit sendHandleException:s];
    }
    
    void MRGService::sendHandleException(const std::string &description, const std::string &reason) {
        NSString *s = [NSString stringWithUTF8String:description.c_str()];
        NSString *r = [NSString stringWithUTF8String:reason.c_str()];
        [MRGServiceInit sendHandleException:s reason:r];
    }
    
/** Mark as cheater */
    
    void MRGService::markUserAsCheater(int want, int have) {
        MRGService::markUserAsCheater(want, have, "");
    }
    
    void MRGService::markUserAsCheater(int want, int have, const std::string &comment) {
        NSString *c = [NSString stringWithUTF8String:comment.c_str()];
        [[MRGSUsers sharedInstance] markCheaterWithFailInt:want andTrue:have andComment:c];
    }
    
/** MRGSBilling */
    
    void MRGSBilling::getProductsInfo(const std::vector<std::string> &skuList)
	{
		NSMutableArray* objects = [[NSMutableArray alloc] init];
        for(vector<string>::const_iterator it = skuList.begin(); it != skuList.end(); ++it)
		{
            NSString *sku = [NSString stringWithUTF8String:(*it).c_str()];
			
            [objects addObject:[MRGSBankProduct productWithId:sku andType:@"cons"]];
        }
        
        [[MRGSBank_ios sharedInstance] loadProductsFromAppleServer:objects];
    }

    void MRGSBilling::buyItem(const std::string &sku) {
        NSString *identifier = [NSString stringWithUTF8String:sku.c_str()];
        [[MRGSBank_ios sharedInstance] addPayment:identifier];
    }
    
    void MRGSBilling::restoreTransaction() {
        [[MRGSBank_ios sharedInstance] restorePurchased];
    }
    
    void MRGSBilling::closePayment(const std::string &sku, const std::string &transactionId) {
        NSString *transaction = [NSString stringWithUTF8String:transactionId.c_str()];
        NSString *product = [NSString stringWithUTF8String:sku.c_str()];
    
        [[MRGSBank_ios sharedInstance] closePaymentWithTransactionId:transaction andProductIdentifier:product];
    }

    void MRGSBilling::addSamsungProductPrice(const std::string& groupId, const std::string& itemId, double priceInUSD) {
    }
	
	void MRGSBilling::navigateToUrl(const std::string &url) {
		NSString *product = [NSString stringWithUTF8String:url.c_str()];
		[[MRGSBank_ios sharedInstance] navigateToUrl:product];
	}
	
/** Local Notifications */
	
//    MRGSNotification::MRGSNotification(int notificationId, const std::string& message, long long timestamp) {
//        this->notificationId = notificationId;
//        this->message = message;
//        this->timestamp = timestamp;
//    }
//    
//    void MRGSNotificationService::addLocalNotification(const mrgs::MRGSNotification &notification) {
//        NSString *title = [NSString stringWithUTF8String:notification.message.c_str()];
//        NSString *ref = [NSString stringWithFormat:@"%d", notification.notificationId];
//        NSDate *date = [NSDate dateWithTimeIntervalSince1970:notification.timestamp];
//
//        MRGSLocalNotification *localNotification = [[MRGSLocalNotification alloc] initWithTitle:title identifier:ref date:date];
//        [[MRGSNotificationManager sharedInstance] scheduleLocalNotification:localNotification];
//        [localNotification release];
//    }
//    
//    void MRGSNotificationService::removeLocalNotification(const int notificationId) {
//        NSString *identifier = [NSString stringWithFormat:@"%d", notificationId];
//        [[MRGSNotificationManager sharedInstance] cancelLocalNotificationWithIdentifier:identifier];
//    }
//    
//    void MRGSNotificationService::getLocalNotification(int notificationId, mrgs::MRGSNotification &notification) {
//        NSString *identifier = [NSString stringWithFormat:@"%d", notificationId];
//        MRGSLocalNotification *localNotifiaction = [[MRGSNotificationManager sharedInstance] findLocalNotificationWithIdentifier:identifier];
//        
//        notification.timestamp = [localNotifiaction.date timeIntervalSince1970];
//        notification.message = string([localNotifiaction.title UTF8String]);
//        notification.notificationId = notificationId;
//    }
//    
//    void MRGSNotificationService::getAllLocalNotifications(std::vector<MRGSNotification> &list){
//        NSArray *notifications = [[MRGSNotificationManager sharedInstance] allLocalNotifications];
//        
//        for (MRGSLocalNotification *n in notifications) {
//            MRGSNotification notification;
//            notification.timestamp = [n.date timeIntervalSince1970];
//            notification.message = string([n.title UTF8String]);
//            notification.notificationId = [n.identifier intValue];
//            
//            list.push_back(notification);
//        }
//    }
//    
    void MRGService::showSupport(UIView *parentToAdd)
    {
        UIView *rootView = parentToAdd;//[UIApplication sharedApplication].keyWindow.rootViewController.view;
        MRGSMyComSupport *support = [MRGSMyComSupport sharedInstance];
        [support showSupportViewOnSuperview:rootView];
    }
    
    void MRGService::showMyTargetShowcase()
    {
        MRGSMyTarget *myTarget = [MRGSMyTarget sharedInstance];
        [myTarget loadShowcaseData];
    }
    
    void MRGService::showMyTargetFullscreen()
    {
        NSLog(@"MRGService::showMyTargetFullscreen start server request");
        MRGSMyTarget *myTarget = [MRGSMyTarget sharedInstance];
        [myTarget loadFullscreenBannerData];
    }
}

#endif

#endif
