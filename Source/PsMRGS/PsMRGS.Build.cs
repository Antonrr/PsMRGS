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
			System.IO.Path.Combine(ModuleDirectory, "../../")) + "ThirdParty/Android/assets/MRGService.xml.default";
		}
	}

	public string PluginAndroidSettingsXml
	{
		get
		{
			return System.IO.Path.GetFullPath(
			System.IO.Path.Combine(ModuleDirectory, "../../")) + "ThirdParty/Android/assets/MRGService.xml";
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
			
			string AndroidGATrackingId = "";
			string AndroidAppsFlyerDevKey = "";
			string AndroidMyTrackerAppId = "";
			string DebugValue = "false";

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
	            		else if (tokens[0] == "AndroidMyTrackerAppId")
	            		{
	            			AndroidMyTrackerAppId = tokens[1];
	            		}
	            		else if (tokens[0] == "bDebugInDevelopment")
	            		{
	            			DebugValue = tokens[1];
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
	        // @todo refactor this some day
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

	        	if (s.Contains("appId"))
	        	{
					lines[i] = "appId=" + '"' + AndroidMyTrackerAppId + '"';
	        	}

	        	if (s.Contains("<debug>false</debug>"))
	        	{
	        		if (Target.Configuration != UnrealTargetConfiguration.Shipping)
	        		{
	        			lines[i] = "<debug>" + DebugValue.ToLower() + "</debug>";
	        		}
	        	}

	        	if (s.Contains("<testDevice>false</testDevice>"))
	        	{
	        		if (Target.Configuration != UnrealTargetConfiguration.Shipping)
	        		{
	        			lines[i] = "<testDevice>" + DebugValue.ToLower() + "</testDevice>";
	        		}
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
