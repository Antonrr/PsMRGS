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

	public string PluginAndroidSettingsXmlDefault
	{
		get
		{
			return System.IO.Path.GetFullPath(
			System.IO.Path.Combine(ModuleDirectory, "../../")) + "ThirdParty/Android/Assets/MRGService.xml.default";
		}
	}

	public string PluginAndroidSettingsXml
	{
		get
		{
			return System.IO.Path.GetFullPath(
			System.IO.Path.Combine(ModuleDirectory, "../../")) + "ThirdParty/Android/Assets/MRGService.xml";
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

			/** Write settings from DefaultEngineIni of MRGS to MRGService.xml in Plugin dir */
			
			string AndroidGATrackingId = null;
			string AndroidAppsFlyerDevKey = null;
			string AndroidFlurryApiKey = null;
			string AndroidMyTargetShowcaseSlotId = null;
			string AndroidMyTargetFullscreenSlotId = null;
			string AndroidMyTrackerAppId = null;

			/** Opens DefaultEngine.ini to read values for variables */
	        using (StreamReader sr = File.OpenText(ConfigPath))
	        {
	            string s = "";
	            while ((s = sr.ReadLine()) != null)
	            {
	            	string[] tokens;
	            	tokens = s.Split('=');
	        		if (tokens.Length == 2)
	        		{
	        			if (tokens[0] == "AndroidGATrackingId")
	            		{
	            			AndroidGATrackingId = tokens[1];
	            		}
	            		else if (tokens[0] == "AndroidAppsFlyerDevKey")
	            		{
							AndroidAppsFlyerDevKey = tokens[1];
	            		}
	            		else if (tokens[0] == "AndroidFlurryApiKey")
	            		{
							AndroidFlurryApiKey = tokens[1];
	            		}
	            		else if (tokens[0] == "AndroidMyTargetShowcaseSlotId")
	            		{
							AndroidMyTargetShowcaseSlotId = tokens[1];
	            		}
	            		else if (tokens[0] == "AndroidMyTargetFullscreenSlotId")
	            		{
							AndroidMyTargetFullscreenSlotId = tokens[1];
	            		}
	            		else if (tokens[0] == "AndroidMyTrackerAppId")
	            		{
	            			AndroidMyTrackerAppId = tokens[1];
	            		}
	            	}
	            }
	        }

			/** Saves temp contents of file separated to lines */
	        List<string> lines = new List<string>();
	        using (StreamReader sr = File.OpenText(PluginAndroidSettingsXmlDefault))
	        {
	            string s = "";
	            while ((s = sr.ReadLine()) != null)
	            {
	            	lines.Add(s);
	            }
	        }

	        /** Clear MRGService.xml to empty state */
			System.IO.File.WriteAllText(PluginAndroidSettingsXml, string.Empty);

	        for (int i = 0; i < lines.Count; i++)
	        {
	        	string s = lines[i];
	        	if (s.Contains("trackingId"))
	        	{
					lines[i] = "trackingId=" + '"' + AndroidGATrackingId + '"';
	        	}
	        	if (s.Contains("app_key"))
	        	{
					lines[i] = "app_key=" + '"' + AndroidAppsFlyerDevKey + '"';
	        	}
	        	if (s.Contains("applicationKey"))
	        	{
					lines[i] = "applicationKey=" + '"' + AndroidFlurryApiKey + '"';
	        	}
	        	if (s.Contains("slotId"))
	        	{
					lines[i] = "slotId=" + '"' + AndroidMyTargetShowcaseSlotId + '"';
	        	}
	        	if (s.Contains("fullscreenSlotId"))
	        	{
					lines[i] = "fullscreenSlotId=" + '"' + AndroidMyTargetFullscreenSlotId + '"';
	        	}
	        	if (s.Contains("appId"))
	        	{
					lines[i] = "appId=" + '"' + AndroidMyTrackerAppId + '"';
	        	}
	        }

	        /** Saves edited MRGService.xml back with parsed values from config*/
			TextWriter tw = new StreamWriter(PluginAndroidSettingsXml);
	        foreach (String s in lines)
	        {
	   			tw.WriteLine(s);
	        }
			tw.Close();
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
				new Framework(
				"MRGService",
				"../../ThirdParty/IOS/MRGService.embeddedframework.zip",
				"MRGServiceResources.bundle"
				)
			);

			PublicAdditionalFrameworks.Add(
				new Framework(
				"AppsFlyerLib",
				"../../ThirdParty/IOS/AppsFlyerLib.embeddedframework.zip"
				)
			);

			PublicAdditionalFrameworks.Add(
				new Framework(
				"MyTargetSDK",
				"../../ThirdParty/IOS/MyTargetSDK.embeddedframework.zip"
				)
			);

			PublicAdditionalFrameworks.Add(
				new Framework(
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
