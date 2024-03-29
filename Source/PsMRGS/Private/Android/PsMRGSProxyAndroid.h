// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsMRGSLibrary.h"
#include "PsMRGSProxy.h"

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Runtime/Launch/Public/Android/AndroidJNI.h"
#include <jni.h>
#include <string>
#endif

#include "PsMRGSProxyAndroid.generated.h"

#if PLATFORM_ANDROID

class MRGSJniHelper
{
public:
	static FString JavaStringToFstring(jstring str);
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
	virtual void ShowDefaultGDPRAgreement(bool bOnlyEU, bool bWithAdvertising, FString LocalizationLanguage) override;

	/** Show specified GDPR agreement */
	virtual void ShowGDPRAgreement(bool bOnlyEU, bool bWithAdvertising, FString LocalizationLanguage) override;

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
	// MRGS

	virtual void CheckIntegration() override;

	virtual void InitModule() override;

	virtual void InitUser(const FString& UserId) override;

	virtual void SetAutoRestoreTransactions(bool bRestore);

	virtual void LoadStoreProducts(const TArray<FString>& ProductsList) override;

	virtual const TArray<FPsMRGSPurchaseInfo>& GetProducts() const override;

	virtual void BuyProduct(const FString& ProductId, const FString& Payload) override;

	virtual void SendAFEvent(const FString& EventName, const FString& Value) override;

	virtual void AddMetricWithId(int32 MetricId) override;

	virtual void AddMetricWithCode(const FString& MetricCode, int32 Value, int32 Level, int32 ObjectId) override;

	virtual void AddTrackerEvent(const FPsMRGSTrackerEvent& Event) override;

	virtual void ShowSupport() override;

	virtual void CheckSupportTickets() override;

	virtual bool IsReady() const override;

	virtual bool UserLoggedIn() const;

	virtual void RequestOpenUDID() override;

	//////////////////////////////////////////////////////////////////////////
	// Notifications

	/** Call EnableMRGSNotifications */
	virtual void EnableNotifications() override;

	/** Call DisableMRGSNotifications */
	virtual void DisableNotifications() override;

	//////////////////////////////////////////////////////////////////////////
	// Advertising

public:
	/** Load next ad */
	virtual void LoadAdvertising() override;

	/** Get whether advertising is loaded */
	virtual bool IsAdvertisingLoaded() override;

	/** Show advertising */
	virtual void ShowAdvertising() override;

	//////////////////////////////////////////////////////////////////////////
	// Showcase

public:
	/** Open Showcase */
	virtual void OpenShowcase() override;

public:
	virtual void OnInitComplete() override;

	virtual void OnFullscreenClosed() override;

	virtual void OnInterstitialSliderClosed() override;

	virtual void OnInterstitialDataRecieveError(const FString& Error) override;

	virtual void OnFullscreenDataRecieveError(const FString& Error) override;

	virtual void OnShowcaseDataRecieveError(const FString& Error) override;

	virtual void OnShowCaseDataHasNoAds() override;

	virtual void OnClickOnNotification(int32 NotificationId, const FString& Title, const FString& Message, const FString& DeveloperPayload, bool bIsLocal) override;

	virtual FString GetNotificationDeveloperPayload() const override;

	virtual void OnPurchaseComplete(const FString& PaymentId, const FString& TransactionId, const FString& Payload) override;

	virtual void OnPurchasePending(const FString& ProductId) override;

	virtual void OnPurchaseFailed(const FString& ProductId, const FString& Answer) override;

	virtual void OnPurchaseCanceled(const FString& ProductId, const FString& Answer) override;

	virtual FString GetDevicePlatform() const override;

	virtual FString GetCountryCode() const override;

protected:
	/** Notification developer payload */
	FString NotificationDeveloperPayload;
#endif
};
