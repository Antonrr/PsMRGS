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
	
	virtual void InitModule() override;
	
	virtual void InitUser(const FString& UserId) override;
	
	virtual void LoadStoreProducts(const TArray<FString>& ProductsList) override;
	
	virtual const TArray<FPsMRGSPurchaseInfo>& GetProducts() const override;
	
	virtual void BuyProduct(const FString& ProductId, const FString& Payload) override;
	
	virtual void SendGAScreen(const FString& ScreenName) override;
	
	virtual void SendGAEvent(const FString& Category, const FString& Action, const FString& Label) override;
	
	virtual void SendFlurryEvent(const FString& Action) override;
	
	virtual void SendAFEvent(const FString& EventName, const FString& Value) override;
	
	virtual void AddMetric(int32 MetricId) override;
	
	virtual void ShowMyTargetShowcase() override;
	
	virtual void ShowMyTargetFullscreen() override;
	
	virtual void ShowMyTargetInterstitialSlider() override;
	
	virtual void ShowSupport() override;
	
	virtual const bool IsReady() const override;
	
	virtual const bool UserLoggedIn() const;
	
public:
	
	virtual void OnInitComplete() override;
	
	virtual void OnFullscreenClosed() override;
	
	virtual void OnInterstitialSliderClosed() override;
	
	virtual void OnInterstitialDataRecieveError(const FString& Error) override;
	
	virtual void OnFullscreenDataRecieveError(const FString& Error) override;
	
	virtual void OnShowcaseDataRecieveError(const FString& Error) override;
	
	virtual void OnSupportReceivedError(const FString& Error) override;
	
	virtual void OnSupportTicketsFailWithError(const FString& Error) override;
	
	virtual void OnShowCaseDataHasNoAds() override;
	
	virtual void OnSupportClosed() override;
	
	virtual void OnStoreProductsLoaded(TArray<FPsMRGSPurchaseInfo>& InLoadedProducts) override;
	
	virtual void OnPurchaseComplete(const FString& PaymentId, const FString& TransactionId, const FString& Payload) override;
	
	virtual void OnPurchaseFailed(const FString& ProductId, const FString& Answer) override;

	virtual void OnUserAuthSuccess();

	virtual void OnUserAuthError();
	
	virtual void OnUserAuthSuccess();
	
	virtual void OnUserAuthError();
	
protected:
	
	/** Main mrgs delegate */
	PsMRGSDelegate* Delegate;
	
	/** Max mrgs users count */
	int32 MaxUsersSlots;
	
#endif
};
