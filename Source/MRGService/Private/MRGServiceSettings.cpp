// Copyright 2016 Mail.Ru Group. All Rights Reserved.

#include "MRGServicePrivatePCH.h"
#include "MRGServiceSettings.h"

#include "UObject/Package.h"
#include "Misc/ConfigCacheIni.h"

UMRGServiceSettings::UMRGServiceSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	iOsMrgsAppId = 0;
	iOsMyTargetShowcaseSlotId = 0;
	iOsMyTargetFullscreenSlotId = 0;
	iOsMyTargetInterstitialSlotId = 62736;
}

void UMRGServiceSettings::ReadFromConfig()
{
	const FString ConfigSection = FString("/Script/MRGService.MRGServiceSettings");

	// Android Basic Settings
	GConfig->GetString(*ConfigSection, TEXT("AndroidSupportSecretKey"), AndroidSupportSecretKey, GEngineIni);

	// iOS Basic Settings
	GConfig->GetInt(*ConfigSection, TEXT("iOsMrgsAppId"), iOsMrgsAppId, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsMrgsClientSecret"), iOsMrgsClientSecret, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsAppsFlyerDevKey"), iOsAppsFlyerDevKey, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsGATrackingId"), iOsGATrackingId, GEngineIni);

	GConfig->GetInt(*ConfigSection, TEXT("iOsMyTargetShowcaseSlotId"), iOsMyTargetShowcaseSlotId, GEngineIni);
	GConfig->GetInt(*ConfigSection, TEXT("iOsMyTargetFullscreenSlotId"), iOsMyTargetFullscreenSlotId, GEngineIni);
	GConfig->GetInt(*ConfigSection, TEXT("iOsMyTargetInterstitialSlotId"), iOsMyTargetInterstitialSlotId, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsMyTrackerAppId"), iOsMyTrackerAppId, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsSupportSecretKey"), iOsSupportSecretKey, GEngineIni);

	// iOS Specific settings
	GConfig->GetString(*ConfigSection, TEXT("iOsFlurryApiKey"), iOsFlurryApiKey, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsAppleAppId"), iOsAppleAppId, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsKeyChainAccount"), iOsKeyChainAccount, GEngineIni);
}
