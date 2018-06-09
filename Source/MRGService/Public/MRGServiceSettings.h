// Copyright 2016 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "MRGServiceSettings.generated.h"


//////////////////////////////////////////////////////////////////////////
// Base class for MRGS settings. Should be overridden per platform

UCLASS(config = Engine, defaultconfig)
class UMRGServiceSettings : public UObject
{
	GENERATED_UCLASS_BODY()
	
public:

	/** Reads settings from config. Used to read settings before UObject system is initialized */
	void ReadFromConfig();


	//////////////////////////////////////////////////////////////////////////
	// Basic Settings - Android (only support key, rest of settings defined in MRGService.xml)

	/** Support Secret Key */
	UPROPERTY(Config, EditAnywhere, Category = "Android", meta = (DisplayName = "Support Secret Key"))
	FString AndroidSupportSecretKey;


	//////////////////////////////////////////////////////////////////////////
	// Basic Settings - iOS

	/** MRGS App Id */
	UPROPERTY(Config, EditAnywhere, Category = "iOS", meta = (DisplayName = "MRGS App Id"))
	int32 iOsMrgsAppId;

	/** MRGS Client Secret */
	UPROPERTY(Config, EditAnywhere, Category = "iOS", meta = (DisplayName = "MRGS Client Secret"))
	FString iOsMrgsClientSecret;

	/** AppsFlyer Dev Key */
	UPROPERTY(Config, EditAnywhere, Category = "iOS", meta = (DisplayName = "AppsFlyer Dev Key"))
	FString iOsAppsFlyerDevKey;

	/** GA Tracking Id */
	UPROPERTY(Config, EditAnywhere, Category = "iOS", meta = (DisplayName = "GA Tracking Id"))
	FString iOsGATrackingId;

	/** MyTarget Showcase Slot Id */
	UPROPERTY(Config, EditAnywhere, Category = "iOS", meta = (DisplayName = "MyTarget Showcase Slot Id"))
	int32 iOsMyTargetShowcaseSlotId;

	/** MyTarget Fullscreen Slot Id */
	UPROPERTY(Config, EditAnywhere, Category = "iOS", meta = (DisplayName = "MyTarget Fullscreen Slot Id"))
	int32 iOsMyTargetFullscreenSlotId;

	/** MyTarget Interstitial Slot Id */
	UPROPERTY(Config, EditAnywhere, Category = "iOS", meta = (DisplayName = "MyTarget Interstitial Slot Id"))
	int32 iOsMyTargetInterstitialSlotId;

	/** MyTracker App Id */
	UPROPERTY(Config, EditAnywhere, Category = "iOS", meta = (DisplayName = "MyTracker App Id"))
	FString iOsMyTrackerAppId;

	/** Support Secret Key */
	UPROPERTY(Config, EditAnywhere, Category = "iOS", meta = (DisplayName = "Support Secret Key"))
	FString iOsSupportSecretKey;


	//////////////////////////////////////////////////////////////////////////
	// iOS-Specific Settings

	/** Flurry API Key */
	UPROPERTY(Config, EditAnywhere, Category = "iOS", meta = (DisplayName = "Flurry Api Key"))
	FString iOsFlurryApiKey;

	/** Apple App Id */
	UPROPERTY(Config, EditAnywhere, Category = "iOS", meta = (DisplayName = "Apple AppId"))
	FString iOsAppleAppId;

	/** KeyChain Account */
	UPROPERTY(Config, EditAnywhere, Category = "iOS", meta = (DisplayName = "KeyChain Account"))
	FString iOsKeyChainAccount;
};
