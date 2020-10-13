// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

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
	MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_NOT_IMPLEMENTED);
}

void UPsMRGSProxy::ShowGDPRAgreement(int32 AgreementVersion, bool bOnlyEU, bool bWithAdvertising)
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
#if !UE_BUILD_SHIPPING
	UE_LOG(LogMRGS, Warning, TEXT("%s: setting DebugSetProducts"), *PS_FUNC_LINE);
	LoadedProducts = InProducts;
#else
	UE_LOG(LogMRGS, Error, TEXT("%s: DebugSetProducts called on Shipping build; ignoring"), *PS_FUNC_LINE);
#endif
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
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}

void UPsMRGSProxy::OnUserAuthError()
{
	UE_LOG(LogMRGS, Warning, TEXT("%s: Null proxy used"), *PS_FUNC_LINE);
}
