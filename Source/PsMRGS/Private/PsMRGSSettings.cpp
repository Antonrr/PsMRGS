// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "PsMRGSSettings.h"

#include "Misc/ConfigCacheIni.h"

UPsMRGSSettings::UPsMRGSSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	iOSMrgsAppId = 0;
}

void UPsMRGSSettings::ReadFromConfig()
{
	const FString ConfigSection = FString("/Script/PsMRGS.PsMRGSSettings");

	// Android Basic Settings
	GConfig->GetString(*ConfigSection, TEXT("AndroidMrgsSupportSecretKey"), AndroidMrgsSupportSecretKey, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("AndroidMrgsAppId"), AndroidMrgsAppId, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("AndroidMrgsSecret"), AndroidMrgsSecret, GEngineIni);

	// iOS Basic Settings
	GConfig->GetInt(*ConfigSection, TEXT("iOsMrgsAppId"), iOSMrgsAppId, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsMrgsClientSecret"), iOSMrgsClientSecret, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsAppsFlyerDevKey"), iOSAppsFlyerDevKey, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsGATrackingId"), iOSGATrackingId, GEngineIni);
	GConfig->GetBool(*ConfigSection, TEXT("bEnableMRGS"), bEnableMRGS, GEngineIni);
	GConfig->GetBool(*ConfigSection, TEXT("bDebugInDevelopment"), bDebugInDevelopment, GEngineIni);
	GConfig->GetBool(*ConfigSection, TEXT("bShowAppTrackingRequestAtStartup"), bShowAppTrackingRequestAtStartup, GEngineIni);

	GConfig->GetString(*ConfigSection, TEXT("iOsMyTrackerAppId"), iOSMyTrackerAppId, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsSupportSecretKey"), iOSSupportSecretKey, GEngineIni);
	GConfig->GetString(*ConfigSection, TEXT("iOsAppleAppId"), iOSAppleAppId, GEngineIni);
}
