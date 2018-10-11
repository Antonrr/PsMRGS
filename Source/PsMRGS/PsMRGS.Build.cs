// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

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

		if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			PrivateIncludePaths.Add("PsMRGS/Private/IOS/");
		}

		else if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PrivateIncludePaths.Add("PsMRGS/Private/Android/");
		}

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


		if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			PublicFrameworks.AddRange(
			new string[]
			{
				"AdSupport",
				"iAd",
				"CoreTelephony",
				"SystemConfiguration",
				"UIKit",
				"Foundation",
				"CoreGraphics",
				"MobileCoreServices",
				"StoreKit",
				"CFNetwork",
				"CoreData",
				"Security",
				"CoreLocation",
				"WatchConnectivity",
				"MediaPlayer",
				"CoreFoundation",
				"UserNotifications",
				"GameKit",
				"SafariServices"
			}
			);

			PublicAdditionalLibraries.AddRange(
			new string[]
			{
				"sqlite3"
			}
			);

			PublicAdditionalFrameworks.Add(
				new UEBuildFramework(
				"MRGService",
				"../../ThirdParty/IOS/MRGService.embeddedframework.zip"
				)
			);

			PublicAdditionalFrameworks.Add(
				new UEBuildFramework(
				"AppsFlyerLib",
				"../../ThirdParty/IOS/AppsFlyerLib.embeddedframework.zip"
				)
			);

			PublicAdditionalFrameworks.Add(
				new UEBuildFramework(
				"MyTargetSDK",
				"../../ThirdParty/IOS/MyTargetSDK.embeddedframework.zip"
				)
			);

			PublicAdditionalFrameworks.Add(
				new UEBuildFramework(
				"MyTrackerSDK",
				"../../ThirdParty/IOS/MyTrackerSDK.embeddedframework.zip"
				)
			);

			string thirdPartyPath = ProjectRoot + "/ThirdParty/IOS/";
			string tempPathPrefix = thirdPartyPath + "Flurry/";

			//adding Flurry
			PublicIncludePaths.Add(tempPathPrefix);
			PublicAdditionalLibraries.Add(tempPathPrefix + "/libFlurry_9.0.0.a");

			//adding GA
			tempPathPrefix = thirdPartyPath + "GoogleAnalytics/";
			PublicIncludePaths.Add(tempPathPrefix);
			PublicAdditionalLibraries.Add(tempPathPrefix + "libAdIdAccess.a");
			PublicAdditionalLibraries.Add(tempPathPrefix + "libGoogleAnalyticsServices.a");
		}
	}
}
