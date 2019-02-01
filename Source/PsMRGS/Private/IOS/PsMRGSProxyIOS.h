// Copyright 2018 Pushkin Studio. All Rights Reserved.

#pragma once

#if PLATFORM_IOS
#include "IOSAppDelegate.h"
#import	<StoreKit/StoreKit.h>
#import <MRGService/MRGService.h>
#endif
#include "PsMRGSProxy.h"
#include "PsMRGSProxyIOS.generated.h"

#if PLATFORM_IOS
@interface PsMRGSDelegate : NSObject <MRGSServerDataDelegate, MRGSMyComSupportDelegate, MRGSMyTargetDelegate, MRGSBankDelegate, SKStoreProductViewControllerDelegate>

- (NSString *)getCurrencyCode:(SKProduct *)product;
- (void)restorePurchase;
- (void)openStoreUrl:(NSString* )urlToGo;


@property (nonatomic) UPsMRGSProxy* Proxy;

@end
#endif

UCLASS()
class UPsMRGSProxyIOS : public UPsMRGSProxy
{
	GENERATED_UCLASS_BODY()
	
#if PLATFORM_IOS
public:
	/** Start mrgs initialization */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Setup")
	virtual void InitModule() override;
	
	/** Login or register user in mrgs */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Setup")
	virtual void InitUser(const FString& UserId) override;
	
	/** Send google analytics screen */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Events")
	virtual void SendGAScreen(const FString& ScreenName) override;
	
	/** Send google analytics event */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Events")
	virtual void SendGAEvent(const FString& Category, const FString& Action, const FString& Label) override;
	
	/** Send flurry event */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Events")
	virtual void SendFlurryEvent(const FString& Action) override;
	
	/** Send applsflyer event */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Events")
	virtual void SendAFEvent(const FString& EventName, const FString& Value) override;

	/** Log metric on mrgs with numeric id */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Events")
	virtual void AddMetricWithId(int32 MetricId) override;
	
	/** Log metric on mrgs with string code */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Events")
	virtual void AddMetricWithCode(const FString& MetricCode, int32 Value, int32 Level, int32 ObjectId) override;
	
	/** Show mytarget apps */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Adds")
	virtual void ShowMyTargetShowcase() override;
	
	/** Show mytarget banner */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Adds")
	virtual void ShowMyTargetFullscreen() override;
	
	/** Show mytarget interstitial slider */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Adds")
	virtual void ShowMyTargetInterstitialSlider() override;
	
	/** Load products from store */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Store")
	virtual void LoadStoreProducts(const TArray<FString>& ProductsList) override;
	
	/** Buy product in store */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Store")
	virtual void BuyProduct(const FString& ProductId, const FString& Payload) override;
	
	/** Get loaded products */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Store")
	virtual const TArray<FPsMRGSPurchaseInfo>& GetProducts() const override;
	
	/** Show support screen */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Support")
	virtual void ShowSupport() override;
	
	/** MRGS initialize complete */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Setup")
	virtual const bool IsReady() const override;
	
	/** MRGS user auth complete */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Setup")
	virtual const bool UserLoggedIn() const;
	
public:
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
	
	/** Support tickets update fail with error */
	virtual void OnSupportTicketsFailWithError(const FString& Error) override;
	
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
	virtual void OnUserAuthSuccess();
	
	/** Dispatch failed user auth */
	virtual void OnUserAuthError();
	
protected:
	/** Main mrgs delegate */
	PsMRGSDelegate* Delegate;
	
	/** Max mrgs users count */
	int32 MaxUsersSlots;
	
public:
	/** Get platform */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Tools")
	virtual FString GetDevicePlatform() const override;
	
	/** Get OpenUDID */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Tools")
	virtual FString GetOpenUDID() const override;
#endif
};
