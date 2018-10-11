// Copyright 2018 Pushkin Studio. All Rights Reserved.

#pragma once

#include "PsMRGSSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class UPsMRGSSettings : public UObject
{
	GENERATED_UCLASS_BODY()
	
	/** Reads settings from config. Used to read settings before UObject system is initialized */
	void ReadFromConfig();
	
	/** On/Off mrgs initialization*/
	UPROPERTY(Config, EditAnywhere)
	bool bEnableMRGS;

	/** On/Off mrgs initialization*/
	UPROPERTY(Config, EditAnywhere)
	bool bInitMRGSONStart;
	
	/** On/Off mrgs debug mode */
	UPROPERTY(Config, EditAnywhere)
	bool bDebugMode;
	
	/** iOS Mrgs appid */
	UPROPERTY(Config, EditAnywhere)
	int32 iOSMrgsAppId;
	
	/** iOS Mrgs client secret */
	UPROPERTY(Config, EditAnywhere)
	FString iOSMrgsClientSecret;
	
	/** iOS Mrgs flurry api key */
	UPROPERTY(Config, EditAnywhere)
	FString iOSFlurryApiKey;
	
	/** iOS Google analytics tracking id */
	UPROPERTY(Config, EditAnywhere)
	FString iOSGATrackingId;
	
	/** iOS Mrgs appsfluer dev key */
	UPROPERTY(Config, EditAnywhere)
	FString iOSAppsFlyerDevKey;
	
	/** iOS app apple id */
	UPROPERTY(Config, EditAnywhere)
	FString iOSAppleAppId;
	
	/** iOS Mrgs mytaraget showcase slot id */
	UPROPERTY(Config, EditAnywhere)
	int32 iOSMyTargetShowcaseSlotId;
	
	/** iOS Mrgs mytaraget fullscreen slot id */
	UPROPERTY(Config, EditAnywhere)
	int32 iOSMyTargetFullscreenSlotId;
	
	/** iOS Mrgs MyTarget Interstitial Slot Id */
	UPROPERTY(Config, EditAnywhere)
	int32 iOSMyTargetInterstitialSlotId;
	
	/** iOS Mrgs mytracker app id */
	UPROPERTY(Config, EditAnywhere)
	FString iOSMyTrackerAppId;
	
	/** iOS support secret key */
	UPROPERTY(Config, EditAnywhere)
	FString iOSSupportSecretKey;
	
	/** Android support secret key */
	UPROPERTY(Config, EditAnywhere)
	FString AndroidSupportSecretKey;
};
