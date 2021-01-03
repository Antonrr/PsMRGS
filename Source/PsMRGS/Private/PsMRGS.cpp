// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "PsMRGS.h"

#include "PsMRGSCommon.h"
#include "PsMRGSProxy.h"
#include "PsMRGSSettings.h"

#if PLATFORM_IOS
#include "PsMRGSProxyIOS.h"
#endif

#if PLATFORM_ANDROID
#include "PsMRGSProxyAndroid.h"
#endif

#include "Developer/Settings/Public/ISettingsModule.h"
#include "UObject/Package.h"

#define LOCTEXT_NAMESPACE "PsMRGS"

class FPsMRGS : public IPsMRGS
{
	virtual void StartupModule() override
	{
		KitSettings = NewObject<UPsMRGSSettings>(GetTransientPackage(), "PsMRGSSettings", RF_Standalone);
		KitSettings->AddToRoot();
		KitSettings->ReadFromConfig();

		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->RegisterSettings("Project", "Plugins", "PsMRGS",
				LOCTEXT("RuntimeSettingsName", "PsMRGS plugin"),
				LOCTEXT("RuntimeSettingsDescription", "Configure API keys for MRGS"),
				KitSettings);
		}

		// Proxy class depends on platform
		UClass* KitPlatformClass = UPsMRGSProxy::StaticClass();
		if (KitSettings->bEnableMRGS)
		{
#if PLATFORM_IOS
			KitPlatformClass = UPsMRGSProxyIOS::StaticClass();
#endif

#if PLATFORM_ANDROID
			KitPlatformClass = UPsMRGSProxyAndroid::StaticClass();
#endif
		}

		MRGSProxy = NewObject<UPsMRGSProxy>(GetTransientPackage(), KitPlatformClass);
		MRGSProxy->SetFlags(RF_Standalone);
		MRGSProxy->AddToRoot();
	}

	virtual void ShutdownModule() override
	{
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->UnregisterSettings("Project", "Plugins", "PsMRGS");
		}

		if (!GExitPurge)
		{
			// If we're in exit purge, this object has already been destroyed
			MRGSProxy->RemoveFromRoot();
			KitSettings->RemoveFromRoot();
		}
		else
		{
			MRGSProxy = nullptr;
			KitSettings = nullptr;
		}
	}

private:
	/** Holds the kit settings */
	UPsMRGSSettings* KitSettings;
};

IMPLEMENT_MODULE(FPsMRGS, PsMRGS)

DEFINE_LOG_CATEGORY(LogMRGS);

#undef LOCTEXT_NAMESPACE
