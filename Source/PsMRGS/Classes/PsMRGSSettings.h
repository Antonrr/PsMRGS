// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsMRGSSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class UPsMRGSSettings : public UObject
{
	GENERATED_UCLASS_BODY()

	/** Reads settings from config. Used to read settings before UObject system is initialized */
	void ReadFromConfig();

	/** On/Off mrgs */
	UPROPERTY(Config, EditAnywhere)
	bool bEnableMRGS;

	/** On/Off mrgs debug mode */
	UPROPERTY(Config, EditAnywhere)
	bool bDebugInDevelopment;

	/** Whether to show app stracking request on startup */
	UPROPERTY(Config, EditAnywhere)
	bool bShowAppTrackingRequestAtStartup;

	/** iOS Mrgs appid */
	UPROPERTY(Config, EditAnywhere)
	int32 iOSMrgsAppId;

	/** iOS Mrgs client secret */
	UPROPERTY(Config, EditAnywhere)
	FString iOSMrgsClientSecret;

	/** iOS Google analytics tracking id */
	UPROPERTY(Config, EditAnywhere)
	FString iOSGATrackingId;

	/** iOS Mrgs appsfluer dev key */
	UPROPERTY(Config, EditAnywhere)
	FString iOSAppsFlyerDevKey;

	/** iOS app apple id */
	UPROPERTY(Config, EditAnywhere)
	FString iOSAppleAppId;

	/** iOS Mrgs mytracker app id */
	UPROPERTY(Config, EditAnywhere)
	FString iOSMyTrackerAppId;

	/** iOS support secret key */
	UPROPERTY(Config, EditAnywhere)
	FString iOSSupportSecretKey;

	/** Android support secret key */
	UPROPERTY(Config, EditAnywhere)
	FString AndroidMrgsSupportSecretKey;

	/** Android mrgs app id */
	UPROPERTY(Config, EditAnywhere)
	FString AndroidMrgsAppId;

	/** Android mrgs secret key */
	UPROPERTY(Config, EditAnywhere)
	FString AndroidMrgsSecret;
};
