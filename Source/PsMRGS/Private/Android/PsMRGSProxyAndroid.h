// Copyright 2018 Pushkin Studio. All Rights Reserved.
#pragma once

#if PLATFORM_ANDROID
#include "AndroidApplication.h"
#include "Runtime/Launch/Public/Android/AndroidJNI.h"
#include <jni.h>
#include <string>
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
	//////////////////////////////////////////////////////////////////////////
	// GDPR

public:
	/** Show MRGS built-in GDPR agreement */
	virtual void ShowDefaultGDPRAgreement(bool bOnlyEU, bool bWithAdvertising) override;

	/** Show specified GDPR agreement */
	virtual void ShowGDPRAgreement(int32 AgreementVersion, bool bOnlyEU, bool bWithAdvertising) override;

	/** Get accepted version of the agreement */
	virtual int32 GetGDPRAcceptedVersion() override;

	/** Set current version of the agreement */
	virtual void SetGDPRAgreementVersion(int32 Version) override;

	/** Get current version of the agreement */
	virtual int32 GetGDPRAgreementVersion() override;

	//////////////////////////////////////////////////////////////////////////
	// MRGS

	virtual void InitModule() override;

	virtual void InitUser(const FString& UserId) override;

	virtual void LoadStoreProducts(const TArray<FString>& ProductsList) override;

	virtual const TArray<FPsMRGSPurchaseInfo>& GetProducts() const override;

	virtual void BuyProduct(const FString& ProductId, const FString& Payload) override;

	virtual void SendGAScreen(const FString& ScreenName) override;

	virtual void SendGAEvent(const FString& Category, const FString& Action, const FString& Label) override;

	virtual void SendFlurryEvent(const FString& Action) override;

	virtual void SendAFEvent(const FString& EventName, const FString& Value) override;

	virtual void AddMetricWithId(int32 MetricId) override;

	virtual void AddMetricWithCode(const FString& MetricCode, int32 Value, int32 Level, int32 ObjectId) override;

	virtual void ShowMyTargetShowcase() override;

	virtual void ShowMyTargetFullscreen() override;

	virtual void ShowMyTargetInterstitialSlider() override;

	virtual void ShowSupport() override;

	virtual bool IsReady() const override;

	virtual bool UserLoggedIn() const;

public:
	/** GDPR agreement accepted */
	virtual void OnGDPRAccepted(bool bWithAdvertising) override;

	/** GDPR error */
	virtual void OnGDPRError() override;

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

	virtual void OnStoreProductsLoaded(TArray<FPsMRGSPurchaseInfo> InLoadedProducts) override;

	virtual void OnPurchaseComplete(const FString& PaymentId, const FString& TransactionId, const FString& Payload) override;

	virtual void OnPurchaseFailed(const FString& ProductId, const FString& Answer) override;

	virtual void OnPurchaseCanceled(const FString& ProductId, const FString& Answer) override;

	virtual void OnUserAuthSuccess();

	virtual void OnUserAuthError();

private:
	/** Process adman by type */
	void ProcessAdmanCase(FString AdType);

public:
	virtual FString GetDevicePlatform() const override;

	virtual FString GetOpenUDID() const override;

#endif
};
