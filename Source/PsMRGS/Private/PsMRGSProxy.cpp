// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "PsMRGSProxy.h"

#include "PsMRGSCommon.h"

#include "Async/Async.h"

UPsMRGSProxy::UPsMRGSProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DebugCCPASetting = EPsMRGSCCPASetting::DontShare;
	bInitComplete = false;
	bUserLoggedin = false;
	bShowcaseNewContent = false;
}

//////////////////////////////////////////////////////////////////////////
// GDPR

void UPsMRGSProxy::ShowDefaultGDPRAgreement(bool bOnlyEU, bool bWithAdvertising, FString LocalizationLanguage)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_NOT_IMPLEMENTED);
}

void UPsMRGSProxy::ShowGDPRAgreement(bool bOnlyEU, bool bWithAdvertising, FString LocalizationLanguage)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_NOT_IMPLEMENTED);
}

int32 UPsMRGSProxy::GetGDPRAcceptedVersion()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	return -1;
}

//////////////////////////////////////////////////////////////////////////
// CCPA

bool UPsMRGSProxy::ShouldShowCCPA()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	return true;
}

EPsMRGSCCPASetting UPsMRGSProxy::GetCCPASettingValue()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	return DebugCCPASetting;
}

void UPsMRGSProxy::SetCCPASettingValue(EPsMRGSCCPASetting Value)
{
	DebugCCPASetting = Value;
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

//////////////////////////////////////////////////////////////////////////
// App tracking

bool UPsMRGSProxy::ShouldShowTrackingAuthorizationDialog()
{
	return false;
}

void UPsMRGSProxy::RequestTrackingAuthorization()
{
	MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_NOT_IMPLEMENTED);
}

EPsMRGSATTStatus UPsMRGSProxy::GetTrackingAuthorizationStatus()
{
	return EPsMRGSATTStatus::NotDetermined;
}

//////////////////////////////////////////////////////////////////////////
// Setup

void UPsMRGSProxy::CheckIntegration()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_NOT_IMPLEMENTED);
}

void UPsMRGSProxy::InitModule()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_NOT_IMPLEMENTED);
}

void UPsMRGSProxy::InitUser(const FString& UserId)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_NOT_IMPLEMENTED);
}

bool UPsMRGSProxy::IsReady() const
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	return false;
}

bool UPsMRGSProxy::UserLoggedIn() const
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	return false;
}

//////////////////////////////////////////////////////////////////////////
// Events

void UPsMRGSProxy::SendAFEvent(const FString& InEventName, const FString& InValue)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::AddMetricWithId(int32 MetricId)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::AddMetricWithCode(const FString& MetricCode, int32 Value, int32 Level, int32 ObjectId)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::SetAutoRestoreTransactions(bool bRestore)
{
}

//////////////////////////////////////////////////////////////////////////
// Store

void UPsMRGSProxy::LoadStoreProducts(const TArray<FString>& ProductsList)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PRODUCTS_LOADED);
}

void UPsMRGSProxy::BuyProduct(const FString& ProductId, const FString& Payload)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

const TArray<FPsMRGSPurchaseInfo>& UPsMRGSProxy::GetProducts() const
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	return LoadedProducts;
}

void UPsMRGSProxy::DebugSetProducts(const TArray<FPsMRGSPurchaseInfo>& InProducts)
{
	LoadedProducts = InProducts;
}

//////////////////////////////////////////////////////////////////////////
// Support

void UPsMRGSProxy::ShowSupport()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::CheckSupportTickets()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

//////////////////////////////////////////////////////////////////////////
// Tools

FString UPsMRGSProxy::GetDevicePlatform() const
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	return FString();
}

FString UPsMRGSProxy::GetCountryCode() const
{
	return TEXT("editor");
}

void UPsMRGSProxy::RequestOpenUDID()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	OnReceivedOpenUDID(FString{});
}

FString UPsMRGSProxy::GetOpenUDID() const
{
	return OpenUDID;
}

void UPsMRGSProxy::OpenApplicationPageInSystemSettings()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

//////////////////////////////////////////////////////////////////////////
// Notifications

void UPsMRGSProxy::EnableNotifications()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::DisableNotifications()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

//////////////////////////////////////////////////////////////////////////
// Advertising

void UPsMRGSProxy::LoadAdvertising()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

bool UPsMRGSProxy::IsAdvertisingLoaded()
{
	return false;
}

void UPsMRGSProxy::ShowAdvertising()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

//////////////////////////////////////////////////////////////////////////
// Showcase

void UPsMRGSProxy::OpenShowcase()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

bool UPsMRGSProxy::GetShowcaseNewContent() const
{
	return bShowcaseNewContent;
}

void UPsMRGSProxy::ResetShowcaseNewContent()
{
	bShowcaseNewContent = false;
}

//////////////////////////////////////////////////////////////////////////
// Callbacks

void UPsMRGSProxy::OnGDPRAccepted(bool bWithAdvertising)
{
	AsyncTask(ENamedThreads::GameThread, [this, bWithAdvertising]() {
		if (MRGSDelegate.IsBound())
		{
			if (bWithAdvertising)
			{
				MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_GDPR_ACCEPTED_WITH_ADS);
			}
			else
			{
				MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_GDPR_ACCEPTED_WITHOUT_ADS);
			}
		}
	});
}

void UPsMRGSProxy::OnGDPRError()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_GDPR_ERROR);
		}
	});
}

void UPsMRGSProxy::OnInitComplete()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnFullscreenClosed()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnInterstitialSliderClosed()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnInterstitialDataRecieveError(const FString& Error)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnFullscreenDataRecieveError(const FString& Error)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnShowcaseDataRecieveError(const FString& Error)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnShowCaseDataHasNoAds()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnClickOnNotification(int32 NotificationId, const FString& Title, const FString& Message, const FString& DeveloperPayload, bool bIsLocal)
{
	// no op
}

FString UPsMRGSProxy::GetNotificationDeveloperPayload() const
{
	return FString{};
}

void UPsMRGSProxy::OnSupportOpened()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SUPPORT_OPENED);
		}
	});
}

void UPsMRGSProxy::OnSupportReceivedError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SUPPORT_ERROR);
		}
	});
}

void UPsMRGSProxy::OnSupportTicketsReceived()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SUPPORT_TICKETS_NEW);
		}
	});
}

void UPsMRGSProxy::OnSupportTicketsFailWithError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SUPPORT_TICKETS_ERROR);
		}
	});
}

void UPsMRGSProxy::OnSupportClosed()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SUPPORT_CLOSED);
		}
	});
}

void UPsMRGSProxy::OnStoreProductsLoaded(TArray<FPsMRGSPurchaseInfo> InLoadedProducts)
{
	AsyncTask(ENamedThreads::GameThread, [this, InLoadedProducts]() {
		LoadedProducts = InLoadedProducts;
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PRODUCTS_LOADED);
		}
	});
}

void UPsMRGSProxy::OnPurchaseComplete(const FString& PaymentId, const FString& TransactionId, const FString& Payload)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnPurchasePending(const FString& ProductId)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnPurchaseFailed(const FString& ProductId, const FString& Answer)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnPurchaseCanceled(const FString& ProductId, const FString& Answer)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnUserAuthSuccess()
{
	bUserLoggedin = true;

	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_USERINIT_COMPLETE);
		}
	});
}

void UPsMRGSProxy::OnUserAuthError()
{
	bUserLoggedin = false;

	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_USERINIT_ERROR);
		}
	});
}

void UPsMRGSProxy::OnAdvertisingLoaded()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_ADVERTISING_LOADED);
		}
	});
}

void UPsMRGSProxy::OnAdvertisingLoadingError()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_ADVERTISING_LOAD_ERROR);
		}
	});
}

void UPsMRGSProxy::OnAdvertisingFinished(bool bSkipped)
{
	AsyncTask(ENamedThreads::GameThread, [this, bSkipped]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(bSkipped ? EPsMRGSEventsTypes::MRGS_ADVERTISING_FINISHED_SKIPPED : EPsMRGSEventsTypes::MRGS_ADVERTISING_FINISHED);
		}
	});
}

void UPsMRGSProxy::OnNewShowcaseContent()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SHOWCASE_NEW_CONTENT);
		}
	});

	bShowcaseNewContent = true;
}

void UPsMRGSProxy::OnShowcaseShowFinished()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SHOWCASE_CLOSED);
		}
	});
}

void UPsMRGSProxy::OnReceivedOpenUDID(const FString& UDID)
{
	AsyncTask(ENamedThreads::GameThread, [this, UDID]() {
		OpenUDID = UDID;
		MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_RECEIVED_UDID);
	});
}
