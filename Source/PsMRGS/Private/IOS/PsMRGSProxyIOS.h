// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

// clang-format off

#include "PsMRGSProxy.h"

#if PLATFORM_IOS
#import <MRGService/MRGServiceKit.h>

#include "IOSAppDelegate.h"
#import	<StoreKit/StoreKit.h>
#endif

#include "PsMRGSProxyIOS.generated.h"

#if PLATFORM_IOS
@interface PsMRGSDelegate : NSObject <MRGSGDPRDelegate, MRGSServerDataDelegate, MRGSMyComSupportDelegate, MRGSBankDelegateEx, SKStoreProductViewControllerDelegate>

- (NSString*)getCurrencyCode:(MRGSBankProduct*)product;
- (void)restorePurchase;
- (void)registerLifeCycleListener;
- (void)didFinishLaunching:(NSNotification*)notification;

@property (nonatomic) UPsMRGSProxy* Proxy;

@end
#endif // PLATFORM_IOS

UCLASS()
class PSMRGS_API UPsMRGSProxyIOS : public UPsMRGSProxy
{
	GENERATED_UCLASS_BODY()
	
#if PLATFORM_IOS
	
	//////////////////////////////////////////////////////////////////////////
	// GDPR
	
public:
	/** Show MRGS built-in GDPR agreement */
	virtual void ShowDefaultGDPRAgreement(bool bOnlyEU, bool bWithAdvertising) override;
	
	/** Show specified GDPR agreement */
	virtual void ShowGDPRAgreement(bool bOnlyEU, bool bWithAdvertising) override;
	
	/** Get accepted version of the agreement */
	virtual int32 GetGDPRAcceptedVersion() override;

	//////////////////////////////////////////////////////////////////////////
	// CCPA

public:
	/** Show MRGS built-in GDPR agreement */
	virtual bool ShouldShowCCPA() override;

	/** Get CPPA setting value */
	virtual EPsMRGSCCPASetting GetCCPASettingValue() override;

	/** Set CPPA setting value */
	virtual void SetCCPASettingValue(EPsMRGSCCPASetting Value) override;

	//////////////////////////////////////////////////////////////////////////
	// App tracking

public:
	/** Get whether should show tracking auth dialog */
	virtual bool ShouldShowTrackingAuthorizationDialog() override;

	/** Request app tracking authorization */
	virtual void RequestTrackingAuthorization() override;

	/** Get app tracking authorization status */
	virtual EPsMRGSATTStatus GetTrackingAuthorizationStatus() override;
	
	//////////////////////////////////////////////////////////////////////////
	// Setup

	/** Check library integration */
	virtual void CheckIntegration() override;
	
	/** Start mrgs initialization */
	virtual void InitModule() override;
	
	/** Login or register user in mrgs */
	virtual void InitUser(const FString& UserId) override;
	
	/** MRGS initialize complete */
	virtual bool IsReady() const override;
	
	/** MRGS user auth complete */
	virtual bool UserLoggedIn() const override;
	
	//////////////////////////////////////////////////////////////////////////
	// Events
	
	/** Send applsflyer event */
	virtual void SendAFEvent(const FString& InEventName, const FString& InValue) override;
	
	/** Log metric on mrgs with numeric id */
	virtual void AddMetricWithId(int32 MetricId) override;
	
	/** Log metric on mrgs with string code */
	virtual void AddMetricWithCode(const FString& MetricCode, int32 Value, int32 Level, int32 ObjectId) override;
	
	//////////////////////////////////////////////////////////////////////////
	// Store
	
	/** Load products from store */
	virtual void LoadStoreProducts(const TArray<FString>& ProductsList) override;
	
	/** Buy product in store */
	virtual void BuyProduct(const FString& ProductId, const FString& Payload) override;
	
	/** Get loaded products */
	virtual const TArray<FPsMRGSPurchaseInfo>& GetProducts() const override;
	
	//////////////////////////////////////////////////////////////////////////
	// Support
	
	/** Show support screen */
	virtual void ShowSupport() override;

	/** Check if there are new support tickets */
	virtual void CheckSupportTickets() override;
	
	//////////////////////////////////////////////////////////////////////////
	// Tools
	
	/** Get platform */
	virtual FString GetDevicePlatform() const override;
	
	/** Get OpenUDID */
	virtual FString GetOpenUDID() const override;

	/** Open application page in system settings */
	virtual void OpenApplicationPageInSystemSettings() override;

	//////////////////////////////////////////////////////////////////////////
	// Notifications

	/** Call EnableMRGSNotifications */
	virtual void EnableNotifications() override;

	/** Call DisableMRGSNotifications */
	virtual void DisableNotifications() override;
	
	//////////////////////////////////////////////////////////////////////////
	// Callbacks
	
public:
	/** GDPR agreement accepted */
	virtual void OnGDPRAccepted(bool bWithAdvertising) override;
	
	/** GDPR error */
	virtual void OnGDPRError() override;
	
	/** Mrgs initialization complete  */
	virtual void OnInitComplete() override;
	
	/** Fullscreen closed */
	virtual void OnFullscreenClosed() override;
	
	/** Interstitial slider closed */
	virtual void OnInterstitialSliderClosed() override;
	
	/** Interstitial data receive error */
	virtual void OnInterstitialDataRecieveError(const FString& Error) override;
	
	/** Fullscreen data receive error */
	virtual void OnFullscreenDataRecieveError(const FString& Error) override;
	
	/** Showcase data receive error */
	virtual void OnShowcaseDataRecieveError(const FString& Error) override;
	
	/** Showcase data is empty */
	virtual void OnShowCaseDataHasNoAds() override;
	
	/** Support received error */
	virtual void OnSupportReceivedError(const FString& Error) override;
	
	/** Support closed */
	virtual void OnSupportClosed() override;
	
	/** Productrs successfully loaded from store */
	virtual void OnStoreProductsLoaded(TArray<FPsMRGSPurchaseInfo> InLoadedProducts) override;
	
	/** Purchase successfully complete on store */
	virtual void OnPurchaseComplete(const FString& PaymentId, const FString& TransactionId, const FString& Payload) override;
	
	/** Error while processing purchase on store */
	virtual void OnPurchaseFailed(const FString& ProductId, const FString& Answer) override;
	
	/** Canceled while processing purchase on store */
	virtual void OnPurchaseCanceled(const FString& ProductId, const FString& Answer) override;
	
	/** Dispatch success user auth */
	virtual void OnUserAuthSuccess() override;
	
	/** Dispatch failed user auth */
	virtual void OnUserAuthError() override;
	
protected:
	/** Main mrgs delegate */
	PsMRGSDelegate* Delegate;
	
	/** Max mrgs users count */
	int32 MaxUsersSlots;
#endif // PLATFORM_IOS
};

// clang-format on
