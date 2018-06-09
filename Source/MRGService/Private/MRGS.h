// Copyright 2016 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "MRGServicePrivatePCH.h"
#include <vector>
#include <map>
#include <string>
#include <functional>

#if WITH_MRGS

#if PLATFORM_IOS
    namespace mrgs {
#endif
#if PLATFORM_ANDROID
    #include <jni.h>
    #include "Runtime/Launch/Public/Android/AndroidJNI.h"
    #include "AndroidApplication.h"
#endif
		
	void MRGSLOG(const char* format, ...);
		
/** Common */
    
    class MRGServiceDelegate {
    public:
        virtual ~MRGServiceDelegate() {}
        virtual void loadServerDataDidFinished(std::string json);
        virtual void loadPromoBannersDidFinished(std::string json);
    };
    
    class MRGService {
    public:
        #if PLATFORM_ANDROID
        static void initUserWithId(const std::string& uid);
        static jmethodID AndroidThunkJava_MRGService_initUserWithId;
        
        static jmethodID AndroidThunkJava_MRGService_admanLoadData;
        static jmethodID AndroidThunkJava_MRGService_admanOpen;
        static jmethodID AndroidThunkJava_MRGService_admanClose;
        static jmethodID AndroidThunkJava_MRGService_admanRelease;
        static jmethodID AndroidThunkJava_MRGService_admanSetTitle;
        
        static jmethodID AndroidThunkJava_MRGService_showSupport;
        
        static jmethodID AndroidThunkJava_MRGService_flurryEvent;
        static jmethodID AndroidThunkJava_MRGService_sendGAEvent;
        static jmethodID AndroidThunkJava_MRGService_sendGAScreen;
        static jmethodID AndroidThunkJava_MRGService_sendAFEvent;
        
        static jmethodID AndroidThunkJava_MRGService_getProductsInfo;
        static jmethodID AndroidThunkJava_MRGService_buyItem;
		static jmethodID AndroidThunkJava_MRGService_addMetric;
        
        static void showSupportWithSecret(const std::string& secretKey);
		static void addMetric(int metric);
        #endif
        static void flurryEvent(const std::string& event);
        static void flurryEvent(const std::string& event, const std::map<std::string, std::string> params);
        static void flurryEventStart(const std::string& event, const std::map<std::string, std::string> params);
        static void flurryEventStop(const std::string& event, const std::map<std::string, std::string> params);
        
        static void sendGAEvent(const std::string& category, const std::string& action, const std::string& label, int value);
        static void sendGAScreen(const std::string& screenName);
        static void sendGASocEvent(const std::string& network, const std::string& action, const std::string& target);
        static void sendGATimings(const std::string& category, long long interval, const std::string& name, const std::string& label);
        
        static void sendAFEvent(const std::string& eventName, const std::string& value);
        
        static void sendHandleException(const std::string& description);
        static void sendHandleException(const std::string& description, const std::string& reason);
        
        static void markUserAsCheater(int want, int have);
        static void markUserAsCheater(int want, int have, const std::string& comment);
        
        #if PLATFORM_IOS
        static void showSupport(UIView *parentToAdd);
        static void showMyTargetShowcase();
        static void showMyTargetFullscreen();
        static void sendAFEventIos(const std::string& eventName, const std::string& value);
        #endif
        
        static MRGServiceDelegate* delegate;
        
        static std::function< void() > initCompleteCallback;
        static std::function< void() > supportDidClosedCallback;
        static std::function< void() > supportDidRecieveError;
        static std::function< void() > fullscreenDidClosed;
        
    };
    
    
/** Billing */
    
    struct MRGSPurchaseItem {
        std::string sku;
        std::string price;
        std::string title;
        std::string type;
        std::string description;
    };
        
    class MRGSBillingDelegate {
    public:
        virtual ~MRGSBillingDelegate() {}
        virtual void loadProductsDidFinished(const std::vector<MRGSPurchaseItem>& items);
        virtual void purchaseComplete(const std::string& sku, const std::string& transactionId, const std::string& answer);
        virtual void purchaseFail(const std::string& sku, const std::string& answer);
        
        static std::function< void() > loadProductsComplete;
        static std::function< void(const std::string& sku, const std::string& transactionId) > purchaseCompleteCallback;
        static std::function< void(const std::string& sku, const std::string& answer) > purchaseFailCallback;
    };

    class MRGSBilling {
    public:
        static void getProductsInfo(const std::vector<std::string>& skuList);
        static void buyItem(const std::string& sku);
        static void restoreTransaction();
        static void closePayment(const std::string& sku, const std::string& transactionId);
        static void addSamsungProductPrice(const std::string& groupId, const std::string& itemId, double priceInUSD);
		static void navigateToUrl(const std::string& url);
        /** Делегат для обратных вызовов */
        static MRGSBillingDelegate* delegate;
        static std::vector<MRGSPurchaseItem> PurchaseItems;
    };
        
    #if PLATFORM_ANDROID
    enum MRGSAdmanEntityType {
        MRGSAdmanShowcase = 1,
        MRGSAdmanFullscreenBanner = 2,
    };
    
    class MRGSAdmanDelegate {
    public:
        virtual ~MRGSAdmanDelegate() {}
        virtual void admanLoadDataComplete(MRGSAdmanEntityType entityType, bool hasNotificaitons);
        virtual void admanLoadDataFail(MRGSAdmanEntityType entityType, const std::string& reason);
        virtual void admanDidClose(MRGSAdmanEntityType entityType);
    };
    
    class MRGSAdman {
    public:
        static void showMyTargetShowcase();
        static void showMyTargetFullscreen();
        static void setShowcaseTitle(const std::string& title);
        // iOS only
        static void loadData(const MRGSAdmanEntityType& entityType);
        static void open(const MRGSAdmanEntityType& entityType);
        static void close(const MRGSAdmanEntityType& entityType);
        static void release(const MRGSAdmanEntityType& entityType);
        
        static MRGSAdmanDelegate* delegate;
    };

    #endif
    
    
/** Local Notifications */
    
    struct MRGSNotification {
        MRGSNotification() {}
        MRGSNotification(int notificationId, const std::string& message, long long timestamp);
        
        int notificationId;
        std::string message;
        long long timestamp;
    };
    
    class MRGSNotificationServiceDelegate {
    public:
        virtual ~MRGSNotificationServiceDelegate() {}
        virtual void clickOnNotification(int notificationId, std::string title, std::string message, bool isLocal);
        virtual void receivedNotification(int notificationId, std::string title, std::string message, bool isLocal);
    };
    
    class MRGSNotificationService {
    public:
        static void addLocalNotification(const MRGSNotification& notification);
        static void removeLocalNotification(const int notificationId);
        static void getLocalNotification(int notificationId, MRGSNotification& notification);
        static void getAllLocalNotifications(std::vector<MRGSNotification>& list);
        
//        static MRGSNotificationServiceDelegate* delegate;
    };
#if PLATFORM_IOS
        }
#endif

#endif

