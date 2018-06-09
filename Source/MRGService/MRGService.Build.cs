// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;

public class MRGService : ModuleRules
{
    static bool bCompileMRGS = false;
    static bool bCompileAwmAddon = false;
    static bool bUsePurchases = false;
    static bool bUseEvents = false;
    static bool bUseGetDevice = false;
	static bool bUseRegistration = false;

    public string ProjectRoot
    {
        get
        {
            return System.IO.Path.GetFullPath(
            System.IO.Path.Combine(ModuleDirectory, "../../")
        );
        }
    }

    public MRGService(ReadOnlyTargetRules Target) : base(Target)
    {
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
			bCompileMRGS = true;
			bCompileAwmAddon = true;
			bUsePurchases = true;
			bUseEvents = true;
			bUseGetDevice = true;
			bUseRegistration = true;
        }
        else if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            bCompileMRGS = true;
            bCompileAwmAddon = true;
            bUsePurchases = true;
            bUseEvents = true;
            bUseGetDevice = true;
            bUseRegistration = true;
        }

        PublicDefinitions.Add("WITH_MRGS=" + (bCompileMRGS ? "1" : "0"));
        PublicDefinitions.Add("WITH_AWM_ADDON=" + (bCompileAwmAddon ? "1" : "0"));
        PublicDefinitions.Add("WITH_MRGS_EVENTS=" + (bUseEvents ? "1" : "0"));
        PublicDefinitions.Add("WITH_MRGS_PURCHASES=" + (bUsePurchases ? "1" : "0"));
        PublicDefinitions.Add("WITH_GET_DEVICE=" + (bUseGetDevice ? "1" : "0"));
		PublicDefinitions.Add("WITH_MRGS_REGISTRATION=" + (bUseRegistration ? "1" : "0"));

		string BuildDistributionVar = System.Environment.GetEnvironmentVariable("bamboo_BUILD_DISTRIBUTION");
		if (String.IsNullOrEmpty(BuildDistributionVar) == false)
		{
			PublicDefinitions.Add("MRGS_DEFINE_ITEM_SET=" + BuildDistributionVar);
		}
		else
		{
			PublicDefinitions.Add("MRGS_DEFINE_ITEM_SET=" + 0);
		}

        PublicIncludePaths.AddRange(
            new string[] {
                "MRGService/Public"//,
                //"Source/Runtime/Launch/Public"
                // ... add public include paths required here ...
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "MRGService/Private"//,
               // "Source/Runtime/Launch/Private"
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
                //"Launch"
                // ... add other public dependencies that you statically link with here ...
            }
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
//              "CoreUObject",
//              "Engine",
//              "Slate",
//              "SlateCore",
//              "Core",
                "Engine",
                "CoreUObject",
                "RHI",
                "InputCore",
                "Slate",
                "SlateCore",
                "Serialization",
                "CEF3Utils"
                // ... add private dependencies that you statically link with here ...
            }
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
            // ... add any modules that your module loads dynamically here ...
            }
            );

        if (bCompileMRGS == true)
        {
            if (Target.Platform == UnrealTargetPlatform.Android)
            {
                PrivateIncludePathModuleNames.AddRange(
                    new string[] {
                    "Settings",
                    "Launch",
                    }
                );
                string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
                AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "MRGService_APL.xml"));
            }
            else if (Target.Platform == UnrealTargetPlatform.IOS)
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

                if (Target.Platform == UnrealTargetPlatform.IOS)
                {
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
                    //                  PublicAdditionalFrameworks.Add(
                    //                      new UEBuildFramework(
                    //                          "Chartboost",
                    //                          "../../ThirdParty/Chartboost.embeddedframework.zip"
                    //                      )
                    //                  );

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

                    /*string thirdPartyPath = Path.GetDirectoryName( RulesCompiler.GetModuleFilename(this.GetType().Name)) + "../../../ThirdParty/IOS/";*/
                    string thirdPartyPath = ProjectRoot + "/ThirdParty/IOS/";

                    string tempPathPrefix = thirdPartyPath + "Flurry/";
                    //adding Flurry
                    PublicIncludePaths.Add(tempPathPrefix);
                    PublicAdditionalLibraries.Add(tempPathPrefix + "/libFlurry_8.2.1.a");
                    //PublicAdditionalLibraries.Add(tempPathPrefix + "/libFlurryWatch_7.6.0.a");

                    //adding GA
                    tempPathPrefix = thirdPartyPath + "GoogleAnalytics/";
                    PublicIncludePaths.Add(tempPathPrefix);
                    PublicAdditionalLibraries.Add(tempPathPrefix + "libAdIdAccess.a");
                    PublicAdditionalLibraries.Add(tempPathPrefix + "libGoogleAnalyticsServices.a");

                    //adding GoogleConversationTracking
                    //tempPathPrefix = thirdPartyPath + "GoogleConversionTracking/";
                    //PublicIncludePaths.Add(tempPathPrefix);
                    //PublicAdditionalLibraries.Add(tempPathPrefix + "libGoogleConversionTracking.a");

                    //adding MobileAppTracking
                    tempPathPrefix = thirdPartyPath + "MobileAppTracking/";
                    PublicIncludePaths.Add(tempPathPrefix);
                    PublicAdditionalLibraries.Add(tempPathPrefix + "libMobileAppTracker.a");
                }

            }
        }
    }
}
