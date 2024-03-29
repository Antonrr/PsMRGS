// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;
using System.Collections.Generic;

public class PsMRGS : ModuleRules
{
	public string ProjectRoot
	{
		get
		{
			return System.IO.Path.GetFullPath(
			System.IO.Path.Combine(ModuleDirectory, "../../")
		);
		}
	}

	public string ConfigPath
	{
		get
		{
			return System.IO.Path.GetFullPath(
			System.IO.Path.Combine(ModuleDirectory, "../../../../Config/DefaultEngine.ini")
		);
		}
	}

	public PsMRGS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[]
			{
				"PsMRGS/Private/"
				// ... add other private include paths required here ...
			}
			);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);


		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "PsMRGS_UPL_Android.xml"));
			PrivateIncludePaths.Add("PsMRGS/Private/Android/");
		}
		else if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
			AdditionalPropertiesForReceipt.Add("IOSPlugin", Path.Combine(PluginPath, "PsMRGS_UPL_IOS.xml"));

			PrivateIncludePaths.Add("PsMRGS/Private/IOS/");

			PublicFrameworks.AddRange(
			new string[]
			{
				"AdSupport",
				"iAd",
				"CoreTelephony",
				"SystemConfiguration",
				"Foundation",
				"StoreKit",
				"CFNetwork",
				"CoreData",
				"CoreLocation",
				"CoreFoundation",
				"UserNotifications",
				"AppTrackingTransparency"
			}
			);

			PublicSystemLibraries.AddRange(
			new string[]
			{
				// Google analytics uses this library
				"sqlite3"
			}
			);

			PublicAdditionalFrameworks.Add(
				new Framework(
				"AppsFlyerLib",
				"../../ThirdParty/IOS/AppsFlyerLib.embeddedframework.zip"
				)
			);

			PublicAdditionalFrameworks.Add(
				new Framework(
				"MRGSAdvertising",
				"../../ThirdParty/IOS/MRGSAdvertising.embeddedframework.zip"
				)
			);

			PublicAdditionalFrameworks.Add(
				new Framework(
				"MRGSShowcase",
				"../../ThirdParty/IOS/MRGSShowcase.embeddedframework.zip"
				)
			);

			PublicAdditionalFrameworks.Add(
				new Framework(
				"MRGSAnalytics",
				"../../ThirdParty/IOS/MRGSAnalytics.embeddedframework.zip"
				)
			);

			PublicAdditionalFrameworks.Add(
				new Framework(
				"MRGSBank",
				"../../ThirdParty/IOS/MRGSBank.embeddedframework.zip"
				)
			);

			PublicAdditionalFrameworks.Add(
				new Framework(
				"MRGService",
				"../../ThirdParty/IOS/MRGService.embeddedframework.zip"
				)
			);

			PublicAdditionalFrameworks.Add(
				new Framework(
				"MRGSGDPR",
				"../../ThirdParty/IOS/MRGSGDPR.embeddedframework.zip",
				"MRGSGDPRResources.bundle"
				)
			);

			PublicAdditionalFrameworks.Add(
				new Framework(
				"MRGSSupport",
				"../../ThirdParty/IOS/MRGSSupport.embeddedframework.zip"
				)
			);

			PublicAdditionalFrameworks.Add(
				new Framework(
				"MyTrackerSDK",
				"../../ThirdParty/IOS/MyTrackerSDK.embeddedframework.zip"
				)
			);

			PublicAdditionalFrameworks.Add(
				new Framework(
				"MRGSNotifications",
				"../../ThirdParty/IOS/MRGSNotifications.embeddedframework.zip"
				)
			);

            // Hack for remote compilation on windows
            string thirdPartyPath = (ProjectRoot.Contains("/UE4/Builds") ? "/opt/build-dir/ps-mrgs" : ProjectRoot) + "/ThirdParty/IOS/";
            string tempPathPrefix = thirdPartyPath + "Flurry/";

			//adding Flurry
			PublicIncludePaths.Add(tempPathPrefix);
			PublicAdditionalLibraries.Add(tempPathPrefix + "/libFlurry_10.1.0.a");

			//adding GA
			tempPathPrefix = thirdPartyPath + "GoogleAnalytics/";
			PublicIncludePaths.Add(tempPathPrefix);
			PublicAdditionalLibraries.Add(tempPathPrefix + "libAdIdAccess.a");
			PublicAdditionalLibraries.Add(tempPathPrefix + "libGoogleAnalyticsServices.a");
		}
	}
}
