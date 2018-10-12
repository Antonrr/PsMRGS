// Copyright 2018 Pushkin Studio. All Rights Reserved.

#pragma once

#include "PsMRGSProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPsMRGSDelegate, uint8, MyEventName);

UENUM(BlueprintType)
enum class EPsMRGSEventsTypes:uint8
{
	MRGS_INIT_COMPLETE = 0,
	MRGS_PRODUCTS_LOADED,
	MRGS_PURCHASE_COMPLETE,
	MRGS_PURCHASE_FAILED,
	MRGS_SUPPORT_CLOSED,
	MRGS_SUPPORT_ERROR,
	MRGS_SUPPORT_TICKETS_ERROR,
	MRGS_FULLSCREEN_CLOSED,
	MRGS_INTERSTITIAL_SLIDER_CLOSED,
	MRGS_INTERSTITIAL_DATA_ERROR,
	MRGS_FULLSCREEN_DATA_ERROR,
	MRGS_SHOWCASE_DATA_ERROR,
	MRGS_SHOWCASE_DATA_EMPTY,
	MRGS_USERINIT_COMPLETE,
	MRGS_USERINIT_ERROR,
	MRGS_LOGIN_COMPLETE,
	MRGS_LOGIN_ERROR
};

USTRUCT(BlueprintType)
struct FPsMRGSPurchaseInfo
{
	GENERATED_USTRUCT_BODY()
	
	/** Product bundle id */
	UPROPERTY(BlueprintReadOnly)
	FString Sku;
	
	/** Product price from store */
	UPROPERTY(BlueprintReadOnly)
	FString Price;
	
	/** Price formatted */
	UPROPERTY(BlueprintReadOnly)
	FString FormattedPrice;
	
	/** Procudt title from store */
	UPROPERTY(BlueprintReadOnly)
	FString Title;
	
	/** Product type from store */
	UPROPERTY(BlueprintReadOnly)
	FString Type;
	
	/** Product description from store */
	UPROPERTY(BlueprintReadOnly)
	FString Description;
};

UCLASS()
class PSMRGS_API UPsMRGSProxy : public UObject
{
	GENERATED_UCLASS_BODY()

	/** Start mrgs initialization */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Setup")
	virtual void InitModule();

	/** Login or register user in mrgs */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Setup")
	virtual void InitUser(const FString& UserId);
	
	/** Send google analytics screen */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Events")
	virtual void SendGAScreen(const FString& InScreenName);
	
	/** Send google analytics event */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Events")
	virtual void SendGAEvent(const FString& InCategory, const FString& InAction, const FString& InLabel);
	
	/** Send flurry event */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Events")
	virtual void SendFlurryEvent(const FString& InAction);
	
	/** Send applsflyer event */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Events")
	virtual void SendAFEvent(const FString& InEventName, const FString& InValue);
	
	/** Log metric on mrgs */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Events")
	virtual void AddMetric(int32 MetricId);
	
	/** Show mytarget apps */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Adds")
	virtual void ShowMyTargetShowcase();
	
	/** Show mytarget banner */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Adds")
	virtual void ShowMyTargetFullscreen();
	
	/** Show mytarget interstitial slider */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Adds")
	virtual void ShowMyTargetInterstitialSlider();
	
	/** Load products from store */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Store")
	virtual void LoadStoreProducts(const TArray<FString>& ProductsList);
	
	/** Buy product in store */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Store")
	virtual void BuyProduct(const FString& ProductId, const FString& Payload);
	
	/** Get loaded products */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Store")
	virtual const TArray<FPsMRGSPurchaseInfo>& GetProducts() const;
	
	/** Show support screen */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Support")
	virtual void ShowSupport();
	
public:
	
	/** Mrgs initialization complete  */
	virtual void OnInitComplete();
	
	/** Fullscreen closed */
	virtual void OnFullscreenClosed();
	
	/** Interstitial slider closed */
	virtual void OnInterstitialSliderClosed();
	
	/** Interstitial data receive error */
	virtual void OnInterstitialDataRecieveError(const FString& Error);
	
	/** Fullscreen data receive error */
	virtual void OnFullscreenDataRecieveError(const FString& Error);
	
	/** Showcase data receive error */
	virtual void OnShowcaseDataRecieveError(const FString& Error);
	
	/** Showcase data is empty */
	virtual void OnShowCaseDataHasNoAds();
	
	/** Support received error */
	virtual void OnSupportReceivedError(const FString& Error);
	
	/** Support tickets update fail with error */
	virtual void OnSupportTicketsFailWithError(const FString& Error);
	
	/** Support closed */
	virtual void OnSupportClosed();
	
	/** Productrs successfully loaded from store */
	virtual void OnStoreProductsLoaded(TArray<FPsMRGSPurchaseInfo>& InLoadedProducts);
	
	/** Purchase successfully complete on store */
	virtual void OnPurchaseComplete(const FString& PaymentId, const FString& TransactionId, const FString& Payload);
	
	/** Error while processing purchase on store */
	virtual void OnPurchaseFailed(const FString& ProductId, const FString& Answer);
	
	/** Dispatch success user auth */
	virtual void OnUserAuthSuccess();
	
	/** Dispatch failed user auth */
	virtual void OnUserAuthError();

	
protected:
	
	/** Loaded products from store */
	TArray<FPsMRGSPurchaseInfo> LoadedProducts;
	
public:
	
	UPROPERTY(BlueprintAssignable, Category = "MRGS|Events")
	FPsMRGSDelegate MRGSDelegate;
	
};
