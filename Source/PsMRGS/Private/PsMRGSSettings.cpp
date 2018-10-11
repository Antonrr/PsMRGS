// Copyright 2018 Pushkin Studio. All Rights Reserved.

#include "PsMRGSSettings.h"

UPsMRGSSettings::UPsMRGSSettings(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	iOSMrgsAppId = 0;
	iOSMyTargetShowcaseSlotId = 0;
	iOSMyTargetFullscreenSlotId = 0;
	iOSMyTargetInterstitialSlotId = 62736;
}

void UPsMRGSSettings::ReadFromConfig()
{
	const FString ConfigSection = FString("/Script/PsMRGS.PsMRGSSettings");
	
	// Android Basic Settings
	GConfig->GetString(*ConfigSection, TEXT("AndroidSupportSecretKey"), AndroidSupportSecretKey, GEngineIni);
	
	// iOS Basic Settings
	GConfig->GetInt(*ConfigSection, TEXT("iOsMrgsAppId"), iOSMrgsAppId, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsMrgsClientSecret"), iOSMrgsClientSecret, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsAppsFlyerDevKey"), iOSAppsFlyerDevKey, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsGATrackingId"), iOSGATrackingId, GEngineIni);
	GConfig->GetBool(*ConfigSection, TEXT("bInitMRGSONStart"), bInitMRGSONStart, GEngineIni);
	GConfig->GetBool(*ConfigSection, TEXT("bEnableMRGS"), bEnableMRGS, GEngineIni);
	GConfig->GetBool(*ConfigSection, TEXT("bDebugMode"), bDebugMode, GEngineIni);
	
	
	GConfig->GetInt(*ConfigSection, TEXT("iOsMyTargetShowcaseSlotId"), iOSMyTargetShowcaseSlotId, GEngineIni);
	GConfig->GetInt(*ConfigSection, TEXT("iOsMyTargetFullscreenSlotId"), iOSMyTargetFullscreenSlotId, GEngineIni);
	GConfig->GetInt(*ConfigSection, TEXT("iOsMyTargetInterstitialSlotId"), iOSMyTargetInterstitialSlotId, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsMyTrackerAppId"), iOSMyTrackerAppId, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsSupportSecretKey"), iOSSupportSecretKey, GEngineIni);
	
	// iOS Specific settings
	GConfig->GetString(*ConfigSection, TEXT("iOsFlurryApiKey"), iOSFlurryApiKey, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsAppleAppId"), iOSAppleAppId, GEngineIni);
}
