// Copyright 2018 Pushkin Studio. All Rights Reserved.
#pragma once

#if PLATFORM_ANDROID
#include <string>
#include <jni.h>
#include "Runtime/Launch/Public/Android/AndroidJNI.h"
#include "AndroidApplication.h"
#endif

#include "PsMRGSLibrary.h"
#include "PsMRGSProxy.h"
#include "PsMRGSProxyAndroid.generated.h"

#if PLATFORM_ANDROID

class MRGSJniHelper
{
public:
	
	static FString JavaStringToFstring(jstring str);
};

enum MRGSAdmanEntityType : uint8
{
	MRGSAdmanShowcase = 1,
	MRGSAdmanFullscreenBanner = 2,
	MRGSAdmanInterstitialSlider = 3,
};

#endif

UCLASS()
class UPsMRGSProxyAndroid : public UPsMRGSProxy
{
	GENERATED_UCLASS_BODY()
	
#if PLATFORM_ANDROID
	
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
	
#endif
};
