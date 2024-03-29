// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsMRGSProxy.generated.h"

UENUM(BlueprintType)
enum class EPsMRGSEventsTypes : uint8
{
	MRGS_INIT_COMPLETE = 0,
	MRGS_PRODUCTS_LOADED,
	MRGS_PURCHASE_COMPLETE,
	MRGS_PURCHASE_PENDING,
	MRGS_PURCHASE_FAILED,
	MRGS_PURCHASE_CANCELED,
	MRGS_SUPPORT_OPENED,
	MRGS_SUPPORT_CLOSED,
	MRGS_SUPPORT_ERROR,
	MRGS_SUPPORT_TICKETS_NEW,
	MRGS_SUPPORT_TICKETS_ERROR,
	MRGS_FULLSCREEN_CLOSED,          // deprecated
	MRGS_INTERSTITIAL_SLIDER_CLOSED, // deprecated
	MRGS_INTERSTITIAL_DATA_ERROR,    // deprecated
	MRGS_FULLSCREEN_DATA_ERROR,      // deprecated
	MRGS_SHOWCASE_DATA_ERROR,        // deprecated
	MRGS_SHOWCASE_DATA_EMPTY,        // deprecated
	MRGS_USERINIT_COMPLETE,
	MRGS_USERINIT_ERROR,
	MRGS_GDPR_ACCEPTED_WITH_ADS,
	MRGS_GDPR_ACCEPTED_WITHOUT_ADS,
	MRGS_GDPR_ERROR,
	MRGS_NOT_IMPLEMENTED,
	MRGS_ATT_NOT_DETERMINED,
	MRGS_ATT_RESTRICTED,
	MRGS_ATT_DENIED,
	MRGS_ATT_AUTHORIZED,
	MRGS_ATT_ERROR,
	MRGS_NOTIFICATION_CLICKED,
	MRGS_ADVERTISING_LOADED,
	MRGS_ADVERTISING_LOAD_ERROR,
	MRGS_ADVERTISING_SHOW,
	MRGS_ADVERTISING_SHOW_ERROR,
	MRGS_ADVERTISING_FINISHED,
	MRGS_ADVERTISING_FINISHED_SKIPPED,
	MRGS_SHOWCASE_NEW_CONTENT,
	MRGS_SHOWCASE_CLOSED,
	MRGS_RECEIVED_UDID,
};

/** CCPA setting */
UENUM(BlueprintType)
enum class EPsMRGSCCPASetting : uint8
{
	Share = 0,
	DontShare = 1
};

/** App Tracking status */
UENUM(BlueprintType)
enum class EPsMRGSATTStatus : uint8
{
	NotDetermined = 0,
	Restricted = 1,
	Denied = 2,
	Authorized = 3
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPsMRGSDelegate, EPsMRGSEventsTypes, MyEventName);
typedef TMulticastDelegate<void(EPsMRGSEventsTypes MyEventName, FString ProductId, FString TransactionId, FString Payload)> FPsMRGSIAPDelegate;

USTRUCT(BlueprintType)
struct FPsMRGSPurchaseInfo
{
	GENERATED_BODY()

	/** Product bundle id */
	UPROPERTY(BlueprintReadOnly)
	FString Sku;

	/** Product price from store */
	UPROPERTY(BlueprintReadOnly)
	FString Price;

	/** Product micros from store (Android) */
	UPROPERTY(BlueprintReadOnly)
	FString PriceMicros;

	/** Product currency from store */
	UPROPERTY(BlueprintReadOnly)
	FString Currency;

	/** Price formatted */
	UPROPERTY(BlueprintReadOnly)
	FString FormattedPrice;

	/** Product title from store */
	UPROPERTY(BlueprintReadOnly)
	FString Title;

	/** Product type from store */
	UPROPERTY(BlueprintReadOnly)
	FString Type;

	/** Product description from store */
	UPROPERTY(BlueprintReadOnly)
	FString Description;

	/** Whether this is a debug (fake) product generated on client for testing purposes */
	UPROPERTY(BlueprintReadOnly)
	bool bDebug;
};

USTRUCT(BlueprintType)
struct FPsMRGSTrackerEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString EventName;
	UPROPERTY(BlueprintReadWrite)
	int32 Level;

	UPROPERTY(BlueprintReadWrite)
	int32 customInt1;
	UPROPERTY(BlueprintReadWrite)
	int32 customInt2;
	UPROPERTY(BlueprintReadWrite)
	int32 customInt3;
	UPROPERTY(BlueprintReadWrite)
	int32 customInt4;
	UPROPERTY(BlueprintReadWrite)
	int32 customInt5;
	UPROPERTY(BlueprintReadWrite)
	int32 customInt6;
	UPROPERTY(BlueprintReadWrite)
	int32 customInt7;
	UPROPERTY(BlueprintReadWrite)
	int32 customInt8;
	UPROPERTY(BlueprintReadWrite)
	int32 customInt9;
	UPROPERTY(BlueprintReadWrite)
	int32 customInt10;

	UPROPERTY(BlueprintReadWrite)
	float customFloat1;
	UPROPERTY(BlueprintReadWrite)
	float customFloat2;
	UPROPERTY(BlueprintReadWrite)
	float customFloat3;
	UPROPERTY(BlueprintReadWrite)
	float customFloat4;
	UPROPERTY(BlueprintReadWrite)
	float customFloat5;
	UPROPERTY(BlueprintReadWrite)
	float customFloat6;
	UPROPERTY(BlueprintReadWrite)
	float customFloat7;
	UPROPERTY(BlueprintReadWrite)
	float customFloat8;
	UPROPERTY(BlueprintReadWrite)
	float customFloat9;
	UPROPERTY(BlueprintReadWrite)
	float customFloat10;

	UPROPERTY(BlueprintReadWrite)
	FString customString1;
	UPROPERTY(BlueprintReadWrite)
	FString customString2;
	UPROPERTY(BlueprintReadWrite)
	FString customString3;
	UPROPERTY(BlueprintReadWrite)
	FString customString4;
	UPROPERTY(BlueprintReadWrite)
	FString customString5;
	UPROPERTY(BlueprintReadWrite)
	FString customString6;
	UPROPERTY(BlueprintReadWrite)
	FString customString7;
	UPROPERTY(BlueprintReadWrite)
	FString customString8;
	UPROPERTY(BlueprintReadWrite)
	FString customString9;
	UPROPERTY(BlueprintReadWrite)
	FString customString10;
	UPROPERTY(BlueprintReadWrite)
	FString customString11;
	UPROPERTY(BlueprintReadWrite)
	FString customString12;
	UPROPERTY(BlueprintReadWrite)
	FString customString13;
	UPROPERTY(BlueprintReadWrite)
	FString customString14;
	UPROPERTY(BlueprintReadWrite)
	FString customString15;
	UPROPERTY(BlueprintReadWrite)
	FString customString16;
	UPROPERTY(BlueprintReadWrite)
	FString customString17;
	UPROPERTY(BlueprintReadWrite)
	FString customString18;
	UPROPERTY(BlueprintReadWrite)
	FString customString19;
	UPROPERTY(BlueprintReadWrite)
	FString customString20;

	FPsMRGSTrackerEvent() = default;
	FPsMRGSTrackerEvent(const FString& InEventName)
		: EventName(InEventName)
		, Level(0)
		, customInt1(0)
		, customInt2(0)
		, customInt3(0)
		, customInt4(0)
		, customInt5(0)
		, customInt6(0)
		, customInt7(0)
		, customInt8(0)
		, customInt9(0)
		, customInt10(0)
		, customFloat1(0.f)
		, customFloat2(0.f)
		, customFloat3(0.f)
		, customFloat4(0.f)
		, customFloat5(0.f)
		, customFloat6(0.f)
		, customFloat7(0.f)
		, customFloat8(0.f)
		, customFloat9(0.f)
		, customFloat10(0.f)
	{
	}
};

UCLASS()
class PSMRGS_API UPsMRGSProxy : public UObject
{
	GENERATED_UCLASS_BODY()

	//////////////////////////////////////////////////////////////////////////
	// GDPR

public:
	/** Show MRGS built-in GDPR agreement */
	UFUNCTION(BlueprintCallable, Category = "MRGS|GDPR")
	virtual void ShowDefaultGDPRAgreement(bool bOnlyEU, bool bWithAdvertising, FString LocalizationLanguage);

	/** Show specified GDPR agreement */
	UFUNCTION(BlueprintCallable, Category = "MRGS|GDPR")
	virtual void ShowGDPRAgreement(bool bOnlyEU, bool bWithAdvertising, FString LocalizationLanguage);

	/** Get accepted version of the agreement */
	UFUNCTION(BlueprintCallable, Category = "MRGS|GDPR")
	virtual int32 GetGDPRAcceptedVersion();

	//////////////////////////////////////////////////////////////////////////
	// CCPA

public:
	/** Get whether CCPA is applicable */
	UFUNCTION(BlueprintCallable, Category = "MRGS|CCPA")
	virtual bool ShouldShowCCPA();

	/** Get CPPA setting value */
	UFUNCTION(BlueprintCallable, Category = "MRGS|CCPA")
	virtual EPsMRGSCCPASetting GetCCPASettingValue();

	/** Set CPPA setting value */
	UFUNCTION(BlueprintCallable, Category = "MRGS|CCPA")
	virtual void SetCCPASettingValue(EPsMRGSCCPASetting Value);

	//////////////////////////////////////////////////////////////////////////
	// App tracking

public:
	/** Get whether should show tracking auth dialog */
	UFUNCTION(BlueprintCallable, Category = "MRGS|App Tracking")
	virtual bool ShouldShowTrackingAuthorizationDialog();

	/** Request app tracking authorization */
	UFUNCTION(BlueprintCallable, Category = "MRGS|App Tracking")
	virtual void RequestTrackingAuthorization();

	/** Get app tracking authorization status */
	UFUNCTION(BlueprintCallable, Category = "MRGS|App Tracking")
	virtual EPsMRGSATTStatus GetTrackingAuthorizationStatus();

	//////////////////////////////////////////////////////////////////////////
	// Setup

	/** Check library integration */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Setup")
	virtual void CheckIntegration();

	/** Start mrgs initialization */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Setup")
	virtual void InitModule();

	/** Login or register user in mrgs */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Setup")
	virtual void InitUser(const FString& UserId);

	/** MRGS initialize complete */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Setup")
	virtual bool IsReady() const;

	/** MRGS user auth complete */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Setup")
	virtual bool UserLoggedIn() const;

	//////////////////////////////////////////////////////////////////////////
	// Events

	/** Send applsflyer event */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Events")
	virtual void SendAFEvent(const FString& InEventName, const FString& InValue);

	/** Log metric on mrgs with numeric id */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Events")
	virtual void AddMetricWithId(int32 MetricId);

	/** Log metric on mrgs with string code */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Events")
	virtual void AddMetricWithCode(const FString& MetricCode, int32 Value, int32 Level, int32 ObjectId);

	/** Send tracker event */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Events")
	virtual void AddTrackerEvent(const FPsMRGSTrackerEvent& Event);

	//////////////////////////////////////////////////////////////////////////
	// Store

	/** Whether to restore transactions after loading products. Must be called after InitModule but before LoadStoreProducts */
	virtual void SetAutoRestoreTransactions(bool bRestore);

	/** Load products from store */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Store")
	virtual void LoadStoreProducts(const TArray<FString>& ProductsList);

	/** Buy product in store */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Store")
	virtual void BuyProduct(const FString& ProductId, const FString& Payload);

	/** Get loaded products */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Store")
	virtual const TArray<FPsMRGSPurchaseInfo>& GetProducts() const;

	/** Override loaded products list (debug purposes only */
	void DebugSetProducts(const TArray<FPsMRGSPurchaseInfo>& InProducts);

	//////////////////////////////////////////////////////////////////////////
	// Support

	/** Show support screen */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Support")
	virtual void ShowSupport();

	/** Check if there are new support tickets */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Support")
	virtual void CheckSupportTickets();

	//////////////////////////////////////////////////////////////////////////
	// Tools

	/** Get platform */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Tools")
	virtual FString GetDevicePlatform() const;

	/** Get country code */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Tools")
	virtual FString GetCountryCode() const;

	UFUNCTION(BlueprintCallable, Category = "MRGS|Tools")
	virtual void RequestOpenUDID();

	/** Get OpenUDID */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Tools")
	virtual FString GetOpenUDID() const;

	/** Open application page in system settings */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Tools")
	virtual void OpenApplicationPageInSystemSettings();

	//////////////////////////////////////////////////////////////////////////
	// Notifications

	/** Call EnableMRGSNotifications */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Notifications")
	virtual void EnableNotifications();

	/** Call DisableMRGSNotifications */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Notifications")
	virtual void DisableNotifications();

	//////////////////////////////////////////////////////////////////////////
	// Advertising

	/** Load next ad */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Advertising")
	virtual void LoadAdvertising();

	/** Get whether advertising is loaded */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Advertising")
	virtual bool IsAdvertisingLoaded();

	/** Show advertising */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Advertising")
	virtual void ShowAdvertising();

	//////////////////////////////////////////////////////////////////////////
	// Showcase

	/** Open Showcase */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Showcase")
	virtual void OpenShowcase();

	/** Get whether showcase has new content */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Showcase")
	bool GetShowcaseNewContent() const;

	/** Reset showcase has new content flag */
	UFUNCTION(BlueprintCallable, Category = "MRGS|Showcase")
	void ResetShowcaseNewContent();

	//////////////////////////////////////////////////////////////////////////
	// Callbacks

public:
	/** GDPR agreement accepted */
	virtual void OnGDPRAccepted(bool bWithAdvertising);

	/** GDPR error */
	virtual void OnGDPRError();

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

	/** Notification clicked handler */
	virtual void OnClickOnNotification(int32 NotificationId, const FString& Title, const FString& Message, const FString& DeveloperPayload, bool bIsLocal);

	/** Get notification developer payload */
	UFUNCTION(BlueprintCallable)
	virtual FString GetNotificationDeveloperPayload() const;

	/** Support opened */
	virtual void OnSupportOpened();

	/** Support received error */
	virtual void OnSupportReceivedError(const FString& Error);

	/** Support has updates on tickets */
	virtual void OnSupportTicketsReceived();

	/** Support tickets update fail with error */
	virtual void OnSupportTicketsFailWithError(const FString& Error);

	/** Support closed */
	virtual void OnSupportClosed();

	/** Productrs successfully loaded from store */
	virtual void OnStoreProductsLoaded(TArray<FPsMRGSPurchaseInfo> InLoadedProducts);

	/** Purchase successfully complete on store */
	virtual void OnPurchaseComplete(const FString& PaymentId, const FString& TransactionId, const FString& Payload);

	/** Purchase is pending */
	virtual void OnPurchasePending(const FString& ProductId);

	/** Error while processing purchase on store */
	virtual void OnPurchaseFailed(const FString& ProductId, const FString& Answer);

	/** Canceled while processing purchase on store */
	virtual void OnPurchaseCanceled(const FString& ProductId, const FString& Answer);

	/** Dispatch success user auth */
	virtual void OnUserAuthSuccess();

	/** Dispatch failed user auth */
	virtual void OnUserAuthError();

	/** Advertising loaded */
	virtual void OnAdvertisingLoaded();

	/** Advertising loading error */
	virtual void OnAdvertisingLoadingError();

	/** Advertising show finished */
	virtual void OnAdvertisingFinished(bool bSkipped);

	/** New showcase content is present */
	virtual void OnNewShowcaseContent();

	/** Showcase show finished */
	virtual void OnShowcaseShowFinished();

	/** Received OpenUDID value */
	virtual void OnReceivedOpenUDID(const FString& UDID);

protected:
	/** Loaded products from store */
	TArray<FPsMRGSPurchaseInfo> LoadedProducts;

	/** MRGS initialize complete */
	bool bInitComplete;

	/** MRGS auth complete */
	bool bUserLoggedin;

	/** OpenUDID value */
	FString OpenUDID;

	/** Whether showcase has new content flag */
	bool bShowcaseNewContent;

public:
	UPROPERTY(BlueprintAssignable, Category = "MRGS|Events")
	FPsMRGSDelegate MRGSDelegate;

	/** Delegate that would be trigget on purchase related events */
	FPsMRGSIAPDelegate MRGSIAPDelegate;

private:
	/** Debug CPPA setting value*/
	EPsMRGSCCPASetting DebugCCPASetting;
};
