// Copyright 2018 Pushkin Studio. All Rights Reserved.

#include "PsMRGSProxy.h"
#include "PsMRGSCommon.h"

UPsMRGSProxy::UPsMRGSProxy(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

//////////////////////////////////////////////////////////////////////////
// GDPR

void UPsMRGSProxy::ShowDefaultGDPRAgreement(bool bOnlyEU, bool bWithAdvertising)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::ShowGDPRAgreement(int32 AgreementVersion, bool bOnlyEU, bool bWithAdvertising)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

int32 UPsMRGSProxy::GetGDPRAcceptedVersion()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	return -1;
}

void UPsMRGSProxy::SetGDPRAgreementVersion(int32 Version)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

int32 UPsMRGSProxy::GetGDPRAgreementVersion()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Setup

void UPsMRGSProxy::InitModule()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::InitUser(const FString& UserId)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
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

void UPsMRGSProxy::SendGAScreen(const FString& InScreenName)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::SendGAEvent(const FString& InCategory, const FString& InAction, const FString& InLabel)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::SendFlurryEvent(const FString& InAction)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

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

//////////////////////////////////////////////////////////////////////////
// Ads

void UPsMRGSProxy::ShowMyTargetShowcase()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::ShowMyTargetFullscreen()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::ShowMyTargetInterstitialSlider()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

//////////////////////////////////////////////////////////////////////////
// Store

void UPsMRGSProxy::LoadStoreProducts(const TArray<FString>& ProductsList)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
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

//////////////////////////////////////////////////////////////////////////
// Support

void UPsMRGSProxy::ShowSupport()
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

FString UPsMRGSProxy::GetOpenUDID() const
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
	return FString();
}

//////////////////////////////////////////////////////////////////////////
// Callbacks

void UPsMRGSProxy::OnGDPRAccepted(bool bWithAdvertising)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnGDPRError()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
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

void UPsMRGSProxy::OnSupportReceivedError(const FString& Error)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnSupportTicketsFailWithError(const FString& Error)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnSupportClosed()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnStoreProductsLoaded(TArray<FPsMRGSPurchaseInfo> InLoadedProducts)
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnPurchaseComplete(const FString& PaymentId, const FString& TransactionId, const FString& Payload)
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
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnUserAuthError()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}
