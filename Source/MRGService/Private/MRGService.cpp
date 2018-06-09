// Copyright 2016 Mail.Ru Group. All Rights Reserved.

#include "MRGServicePrivatePCH.h"
#include "Async.h"
#include "EngineMinimal.h"
#include "MRGService.h"
#include "MRGServiceSettings.h"

#if WITH_MRGS
#if PLATFORM_IOS
    #include "IOSAppDelegate.h"
	#include "MRGS.h"
	#import <sys/utsname.h>
#endif	// PLATFORM_IOS
#if PLATFORM_ANDROID
	#include "Android/AndroidJNI.h"
	#include "Android/AndroidApplication.h"
	#include "Android/AndroidWindow.h"
	#include "Android/AndroidMisc.h"
    #include "MRGS.h"
    #include "Runtime/Launch/Public/Android/AndroidJNI.h"
#endif	// PLATFORM_ANDROID
#endif	// WITH_MRGS

#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FMRGServiceModule"

#ifdef UE_BUILD_DEBUG
    #define DEBUG true
#else
    #define DEBUG false
#endif
#define MAX_USERS_SLOTS 10
#define ALLOW_PUSH_NOTIFICATIONS false
#define CRASH_REPORTS_ENABLED false

#if WITH_MRGS
#if PLATFORM_IOS
    using namespace mrgs;
#endif
    std::function<void()> MRGService::initCompleteCallback = NULL;
    std::function<void()> MRGSBillingDelegate::loadProductsComplete = NULL;
    std::function<void()> MRGService::supportDidClosedCallback = NULL;
    std::function<void()> MRGService::supportDidRecieveError = NULL;
    std::function<void()> MRGService::fullscreenDidClosed = NULL;
    std::function<void(const std::string& sku, const std::string& transactionId)> MRGSBillingDelegate::purchaseCompleteCallback = NULL;
    std::function<void(const std::string& sku, const std::string& answer)> MRGSBillingDelegate::purchaseFailCallback = NULL;

#endif

DEFINE_LOG_CATEGORY(LogAwmMrgs)

//FPurchaseItem::FPurchaseItem
//{ }

void FMRGServiceModule::StartupModule()
{
	MrgsSettings = NewObject<UMRGServiceSettings>(GetTransientPackage(), "MrgServiceSettings", RF_Standalone);
	MrgsSettings->AddToRoot();

	// No UOBJECT system at the moment of StartupModule call, need to read settings from config manually
	MrgsSettings->ReadFromConfig();

	// Register settings for editor
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "MRGS",
			LOCTEXT("RuntimeSettingsName", "MRG Service"),
			LOCTEXT("RuntimeSettingsDescription", "Configure API keys for MRGService"),
			GetMutableDefault<UMRGServiceSettings>()
		);
	}

#if WITH_MRGS
#if PLATFORM_IOS
	UE_LOG(LogAwmMrgs, Warning, TEXT("START UP MRGS IOS"));
	FMRGServiceModule::InitModule();
#endif
#endif
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FMRGServiceModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "MRGService iOS");
		SettingsModule->UnregisterSettings("Project", "Plugins", "MRGService Android");
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		MrgsSettings->RemoveFromRoot();
	}
	else
	{
		MrgsSettings = nullptr;
	}
}

//#if PLATFORM_IOS
//static void ListenOpenURL(UIApplication* application, NSURL* url, NSString* sourceApplication, id annotation)
//{
//    [MRGServiceInit application:application openUrl:url sourceApplication:sourceApplication annotation:annotation];
//}
//#endif

bool FMRGServiceModule::IsReady() const
{
	return bIsReady;
}

void FMRGServiceModule::InitModule()
{
#if WITH_MRGS
#if PLATFORM_IOS
	if(bIsReady) return;
	dispatch_async(dispatch_get_main_queue(), ^{
		int appId = MrgsSettings->iOsMrgsAppId;
		NSString *secret = MrgsSettings->iOsMrgsClientSecret.GetNSString();
		MRGServiceParams *mrgsParams = [[MRGServiceParams alloc] initWithAppId:appId andSecret:secret];
		mrgsParams.debug = DEBUG;
		mrgsParams.startOnTestDevice = DEBUG;
		mrgsParams.shouldResetBadge = DEBUG;
		mrgsParams.crashReportEnabled = false;
		mrgsParams.allowPushNotificationHooks = ALLOW_PUSH_NOTIFICATIONS;
		
		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, DEBUG);
		mrgsParams.storePath = [[paths objectAtIndex : 0] stringByAppendingPathComponent:@"/mrgsStore"];


#if WITH_MRGS_EVENTS
		// Flurry(ready)(only params, dont use in code)
		NSString *flurryKey = MrgsSettings->iOsFlurryApiKey.GetNSString();
		MRGSFlurryParams *flurryParams = [[MRGSFlurryParams alloc] initWithAPIKey:flurryKey];
		flurryParams.crashReportEnabled = false;
		flurryParams.debug = DEBUG;
		
		// Google Analytics(ready)
		NSString *gaTrackingId = MrgsSettings->iOsGATrackingId.GetNSString();
		MRGSGoogleAnalyticsParams *googleAnalyticsParams = [[MRGSGoogleAnalyticsParams alloc] initWithTrackingId:gaTrackingId];
		googleAnalyticsParams.exceptionHandlerEnabled = DEBUG;
		googleAnalyticsParams.logLevel = 4;
		
		// AppsFlyer(ready)(only params, dont use in code)
		NSString *appleAppId = MrgsSettings->iOsAppleAppId.GetNSString();
		NSString *appsFlyerDevKey = MrgsSettings->iOsAppsFlyerDevKey.GetNSString();
		MRGSAppsFlyerParams *appsFlyerParams = [[MRGSAppsFlyerParams alloc] initWithDevKey:appsFlyerDevKey andAppleAppId:appleAppId];
		appsFlyerParams.debug = DEBUG;
		
		
		// MyTarget(ready)(показ банеров корпоративных)
		int myTargetShowcaseSlotId = MrgsSettings->iOsMyTargetShowcaseSlotId;
		int myTargetFullscreenSlotId = MrgsSettings->iOsMyTargetFullscreenSlotId;
		int myTargetInterstitialSlotId = MrgsSettings->iOsMyTargetInterstitialSlotId;
		MRGSMyTargetParams *myTargetParams = [[MRGSMyTargetParams alloc] initWithShowcaseSlotId:myTargetShowcaseSlotId
																		 fullscreenBannerSlotId:myTargetFullscreenSlotId
																			 interstitialSlotId:myTargetInterstitialSlotId];
		myTargetParams.debug = DEBUG;
		
		// MyTracker(ready)
		NSString *myTrackerAppId = MrgsSettings->iOsMyTrackerAppId.GetNSString();
		MRGSMyTrackerParams *myTrackerParams = [[MRGSMyTrackerParams alloc] initWithAppId:myTrackerAppId];
		myTrackerParams.forwardMetrics = YES;
		myTrackerParams.enableLogging = DEBUG;
		
		NSArray *externalParams = @[flurryParams,
									/*matParams,*/
									/*googleConversionParams,*/
									googleAnalyticsParams,
									appsFlyerParams,
									myTargetParams,
									myTrackerParams];
#else
		NSArray *externalParams = @[];
#endif
		
		
		
		//    MRGSNotificationService::delegate = new MRGSNotificationServiceDelegate();
		MRGService::delegate = new MRGServiceDelegate();
		
		MRGService::initCompleteCallback =  [&]() {
			FMRGServiceModule::MRGSInitCompleteCallback();
		};
		
		MRGService::supportDidClosedCallback =  [&]() {
			FMRGServiceModule::SupportDidClosed();
		};
		
		MRGService::supportDidRecieveError =  [&]() {
			FMRGServiceModule::SupportDidReceiveError();
		};
		
		MRGService::fullscreenDidClosed =  [&]() {
			FMRGServiceModule::FullscreenDidClosed();
		};
		
		
		
		[MRGServiceInit startWithServiceParams:mrgsParams
							 externalSDKParams:externalParams
									  delegate:[MRGServiceInit_ios sharedInstance]];
	});
	
	
#endif
#endif
}

void FMRGServiceModule::InitModuleWithUid(const FString& UserId)
{
#if WITH_MRGS
#if PLATFORM_ANDROID
	bIsReady = true;
	MRGService::delegate = new MRGServiceDelegate();
	
	MRGSAdman::delegate = new MRGSAdmanDelegate();
	
	MRGSAdman::setShowcaseTitle("More games");
	
	MRGSBilling::delegate = new MRGSBillingDelegate();
	
	MRGService::initCompleteCallback =  [&]() {
		FMRGServiceModule::MRGSInitCompleteCallback();
	};
	
	MRGService::supportDidClosedCallback =  [&]() {
		FMRGServiceModule::SupportDidClosed();
	};
	
	MRGService::supportDidRecieveError =  [&]() {
		FMRGServiceModule::SupportDidReceiveError();
	};
	
	MRGService::fullscreenDidClosed =  [&]() {
		FMRGServiceModule::FullscreenDidClosed();
	};
	
#if WITH_MRGS_PURCHASES
	MRGSBillingDelegate::loadProductsComplete = [&]() {
		FMRGServiceModule::ProductsLoaded();
	};
	
	MRGSBillingDelegate::purchaseCompleteCallback =  [&](const std::string& sku, const std::string& transactionId) {
		FMRGServiceModule::ProductsPurchaseComplete(sku, transactionId);
	};
	
	MRGSBillingDelegate::purchaseFailCallback =  [&](const std::string& sku, const std::string& answer) {
		FMRGServiceModule::ProductsPurchaseFailed(sku, answer);
	};
	
#endif
	
	std::string tempUserId = TCHAR_TO_UTF8(*UserId);
	
	MRGService::initUserWithId(tempUserId);
	
#endif
#endif
}

void FMRGServiceModule::MRGSInitCompleteCallback()
{
#if WITH_MRGS
#if PLATFORM_IOS
	if(bIsReady) return;
	bIsReady = true;
	
	 // Off on com.modern.tanks
	 MRGSMyComSupport *support = [MRGSMyComSupport sharedInstance];
	 support.secret = MrgsSettings->iOsSupportSecretKey.GetNSString();
	 support.delegate = [MRGServiceInit_ios sharedInstance];
	
	MRGSMyTarget *myTarget = [MRGSMyTarget sharedInstance];
	myTarget.delegate = [MRGServiceInit_ios sharedInstance];
	
	MRGSBilling::delegate = new MRGSBillingDelegate();
	
	
#if WITH_MRGS_PURCHASES
	MRGSBillingDelegate::loadProductsComplete =  [&]() {
		FMRGServiceModule::ProductsLoaded();
	};
	
	MRGSBillingDelegate::purchaseCompleteCallback =  [&](const std::string& sku, const std::string& transactionId) {
		FMRGServiceModule::ProductsPurchaseComplete(sku, transactionId);
	};
	
	MRGSBillingDelegate::purchaseFailCallback =  [&](const std::string& sku, const std::string& answer) {
		FMRGServiceModule::ProductsPurchaseFailed(sku, answer);
	};
#endif
	//    TArray<FString> productList;
	//    productList.Add(TEXT("com.my.awm.test.buy2"));
	//    FMRGServiceModule::LoadBankProducts(productList);
	
	//    FMRGServiceModule::SendGAScreen(FString(TEXT("AWM test screen")));
	//    FMRGServiceModule::SendGAEvent(FString(TEXT("Test events")), FString(TEXT("MRGS started succesfully")), FString(TEXT("test label")), 1);
	//    FMRGServiceModule::ShowMyTargetShowcase();
	//    FMRGServiceModule::ShowMyTargetFullscreen();
	//    FMRGServiceModule::InitUser(FString(TEXT("Trofimenko")));
	//    FMRGServiceModule::ShowSupport();
#endif
#endif
	
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		if(MRGSEvent.IsBound())
		{
			MRGSEvent.Broadcast(uint8(EMrgsEventsTypes::TYPE_INIT_COMPLETE));
		}
	});
	
}

void FMRGServiceModule::InitUser(const FString& UserId)
{
#if WITH_MRGS && WITH_MRGS_REGISTRATION
#if PLATFORM_IOS
	if(FMRGServiceModule::IsReady())
	{
		NSString *RealUserId = UserId.GetNSString();
		
		NSArray *Users = [[MRGSUsers singleton] getAllUsers];
		int count = [Users count];
		if(Users && count > 0)
		{
			for(int i = 0; i < count; i++)
			{
				NSDictionary *TempUser = Users[i];
				if(!TempUser) return;
				NSString *TempUserId = [TempUser objectForKey:@"userId"];
				if(!TempUserId) return;
				if([TempUserId isEqualToString:RealUserId])
				{
					[[MRGSUsers singleton] authorizationUserWithId:TempUserId];
					NSLog(@"MRGService user %@ founded in previous sessions and authorized", TempUserId);
					AsyncTask(ENamedThreads::GameThread, [this]()
					{
						if(MRGSEvent.IsBound())
						{
							MRGSEvent.Broadcast(uint8(EMrgsEventsTypes::TYPE_USERINIT_COMPLETE));
						}
					});
                    return;
                }
				
				if(i == MAX_USERS_SLOTS - 1)
				{
					[[MRGSUsers singleton] removeUserWIthId:TempUserId];
					NSLog(@"MRGService user %@ removed", TempUserId);
				}
			}
		}
		NSError *Err = nil;
		[[MRGSUsers singleton] registerNewUserWithId:RealUserId
											 andSlot:count >= MAX_USERS_SLOTS ? MAX_USERS_SLOTS: count + 1 andError:&Err];
		[[MRGSUsers singleton] authorizationUserWithId:RealUserId];
		NSLog(@"MRGService  %@ registred and authorized", RealUserId);
		AsyncTask(ENamedThreads::GameThread, [this]()
		{
			if(MRGSEvent.IsBound())
			{
				MRGSEvent.Broadcast(uint8(EMrgsEventsTypes::TYPE_USERINIT_COMPLETE));
			}
		});
    }
#endif
#endif
}
void FMRGServiceModule::SendGAScreen(const FString& ScreenName)
{
	if(FMRGServiceModule::IsReady())
	{
#if WITH_MRGS && WITH_MRGS_EVENTS
		std::string const tempScreeName = TCHAR_TO_UTF8(*ScreenName);
#if PLATFORM_IOS || PLATFORM_ANDROID
		MRGService::sendGAScreen(tempScreeName);
#endif
#endif
	}
}

void FMRGServiceModule::SendGAEvent(const FString& Category, const FString& Action, const FString& Label, int32 Value)
{
	if(FMRGServiceModule::IsReady())
	{
#if WITH_MRGS && WITH_MRGS_EVENTS
		std::string const tempCat = TCHAR_TO_UTF8(*Category);
		std::string const tempAction = TCHAR_TO_UTF8(*Action);
		std::string const tempLabel = TCHAR_TO_UTF8(*Label);
#if PLATFORM_IOS || PLATFORM_ANDROID
		MRGService::sendGAEvent(tempCat, tempAction, tempLabel, 1L);
#endif
#endif
	}
}

void FMRGServiceModule::SendFlurryEvent(const FString& Action)
{
	if(FMRGServiceModule::IsReady())
	{
#if WITH_MRGS && WITH_MRGS_EVENTS
#if PLATFORM_IOS
		std::string const tempAction = TCHAR_TO_UTF8(*Action);
		std::map <std::string, std::string> tempMap;
		tempMap.insert(std::pair<std::string, std::string>(tempAction, tempAction));
		MRGService::flurryEvent(tempAction, tempMap);
#endif
		
#if PLATFORM_ANDROID
		std::string const tempAction = TCHAR_TO_UTF8(*Action);
		MRGService::flurryEvent(tempAction);
#endif
#endif
	}
}

void FMRGServiceModule::SendAFEvent(const FString& EventName, const FString& Value)
{
	if(FMRGServiceModule::IsReady())
	{
#if WITH_MRGS && WITH_MRGS_EVENTS
#if PLATFORM_ANDROID
		std::string const tempevEntName = TCHAR_TO_UTF8(*EventName);
		std::string const tempValue = TCHAR_TO_UTF8(*Value);
		MRGService::sendAFEvent(tempevEntName, tempValue);
#endif
		
#if PLATFORM_IOS
		std::string const tempevEntName = TCHAR_TO_UTF8(*EventName);
		std::string const tempValue = TCHAR_TO_UTF8(*Value);
		MRGService::sendAFEventIos(tempevEntName, tempValue);
#endif
#endif
	}
}

void FMRGServiceModule::ShowSupport()
{
	UE_LOG(LogAwmMrgs, Warning, TEXT("START SUPPORT"));
	if(FMRGServiceModule::IsReady())
	{
#if WITH_MRGS
#if PLATFORM_IOS
		
		dispatch_async(dispatch_get_main_queue(), ^{

			//            MRGSMyComSupport *support = [MRGSMyComSupport sharedInstance];
			//            support.secret = @SUPPORT_SECRET;
			//            support.delegate = [MRGServiceInit_ios sharedInstance];
			
			UIView *ResultSubView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];

			//        UIWebView *webView = [[UIWebView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
			//        webView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
			
			MRGService::showSupport([UIApplication sharedApplication].keyWindow.rootViewController.view);

			if ([[[IOSAppDelegate GetDelegate].RootView subviews] count] > 0)
			{
				ResultSubView = [[[IOSAppDelegate GetDelegate].RootView subviews] objectAtIndex:0];
				if (ResultSubView != nil)
				{
					NSLog(@"%@", NSStringFromCGRect(ResultSubView.bounds));
					//MRGService::showSupport(ResultSubView);
				}
			}
			for (UIView * SubView in[[IOSAppDelegate GetDelegate].RootView subviews])
			{
				NSLog(@"subviews %d", (int)[[[IOSAppDelegate GetDelegate].RootView subviews] count]);
			}
			//
		});
#endif
		
#if PLATFORM_ANDROID
		MRGService::showSupportWithSecret(std::string(TCHAR_TO_UTF8(*MrgsSettings->AndroidSupportSecretKey)));
#endif
#endif
	}
}

void FMRGServiceModule::ShowMyTargetShowcase()
{
	if(FMRGServiceModule::IsReady())
	{
#if WITH_MRGS && WITH_MRGS_EVENTS
#if PLATFORM_IOS
		MRGService::showMyTargetShowcase();
#endif
		
#if PLATFORM_ANDROID
		MRGSAdman::showMyTargetShowcase();
#endif
#endif
	}
}

void FMRGServiceModule::ShowMyTargetFullscreen()
{
	if(FMRGServiceModule::IsReady())
	{
#if WITH_MRGS && WITH_MRGS_EVENTS
#if PLATFORM_IOS
		MRGService::showMyTargetFullscreen();
#endif
		
#if PLATFORM_ANDROID
		MRGSAdman::showMyTargetFullscreen();
#endif
#endif
	}
}

void FMRGServiceModule::LoadBankProducts(const TArray<FString>& ProductsList)
{
#if WITH_MRGS
	if(FMRGServiceModule::IsReady())
	{
#if PLATFORM_IOS && WITH_MRGS_PURCHASES
		std::vector<std::string> TempProductsList;
		for(int i = 0; i < ProductsList.Num(); i++)
		{
			std::string TempValue = TCHAR_TO_UTF8(*ProductsList[i]);
			TempProductsList.push_back(TempValue);
			NSLog(@"FMRGServiceModule::product to load: %@", [NSString stringWithCString:TempValue.c_str()
																				encoding:[NSString defaultCStringEncoding]]);
		}
		MRGSBilling::getProductsInfo(TempProductsList);
#endif
		
#if PLATFORM_ANDROID && WITH_MRGS_PURCHASES
		std::vector<std::string> TempProductsList;
		for(int i = 0; i < ProductsList.Num(); i++)
		{
			std::string TempValue = TCHAR_TO_UTF8(*ProductsList[i]);
			TempProductsList.push_back(TempValue);
		}
		MRGSBilling::getProductsInfo(TempProductsList);
#endif
	}
#endif
}

void FMRGServiceModule::ProductsLoaded()
{
#if WITH_MRGS
#if PLATFORM_IOS
	NSLog(@"FMRGServiceModule::bank products loaded");
#endif
#if PLATFORM_ANDROID
	UE_LOG(LogAwmMrgs, Warning, TEXT("FMRGServiceModule::ProductsLoaded"));
#endif
	
#if (PLATFORM_IOS || PLATFORM_ANDROID) && WITH_MRGS_PURCHASES
	for(auto& Elem : MRGSBilling::PurchaseItems)
	{
		FAwmPurchaseInfo Purchase;
		
		Purchase.Sku = FString(UTF8_TO_TCHAR(Elem.sku.c_str()));
		Purchase.Price = FString(UTF8_TO_TCHAR(Elem.price.c_str()));
		Purchase.Title = FString(UTF8_TO_TCHAR(Elem.title.c_str()));
		Purchase.Type = FString(UTF8_TO_TCHAR(Elem.type.c_str()));
		Purchase.Description = FString(UTF8_TO_TCHAR(Elem.description.c_str()));
		Products.Add(Purchase);
	}
#endif
	
#endif
	
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		if(MRGSEvent.IsBound())
		{
			MRGSEvent.Broadcast(uint8(EMrgsEventsTypes::TYPE_PRODUCTS_LOADED));
		}
	});
}

void FMRGServiceModule::ByProduct(const FString& ProductId)
{
	if(FMRGServiceModule::IsReady())
	{
#if WITH_MRGS
		std::string TempValue = TCHAR_TO_UTF8(*ProductId);
#if (PLATFORM_IOS || PLATFORM_ANDROID) && WITH_MRGS_PURCHASES
		MRGSBilling::buyItem(TempValue);
#endif
		//
		//    TArray<PurchaseItem*> tempItems = FMRGServiceModule::GetPurchaseItems();
		//    NSLog(@"itemsnum %d", tempItems.Num());
		//    for(int i = 0; i < tempItems.Num(); i++)
		//    {
		//        PurchaseItem* tempItem = tempItems[i];
		//        std::string tempItemSku = TCHAR_TO_UTF8(*tempItem->sku);
		//        std::string tempItemPrice = TCHAR_TO_UTF8(*tempItem->price);
		//        NSLog(@"FMRGServiceModule::item: %@ and price: %@", [NSString stringWithCString:tempItemSku.c_str()
		//                                                                               encoding:[NSString defaultCStringEncoding]], [NSString stringWithCString:tempItemPrice.c_str()
		//                                                                                                                                               encoding:[NSString defaultCStringEncoding]]);
		//    }
#endif
	}
}
TArray<FAwmPurchaseInfo>& FMRGServiceModule::GetProducts()
{
	return Products;
}

//TArray<FPurchaseItem*> FMRGServiceModule::GetPurchaseItems() const
//{
//    TArray<FPurchaseItem*> Result;
//    if(FMRGServiceModule::IsReady())
//    {
//    #if PLATFORM_IOS
//        std::vector<MRGSPurchaseItem> TempItems = MRGSBilling::PurchaseItems;
//        for(int i = 0; i < TempItems.size(); i++)
//        {
//            MRGSPurchaseItem Item = TempItems[i];
//            PurchaseItem *TempItem = new PurchaseItem();
//            TempItem->sku = FString(Item.sku.c_str());
//            TempItem->price = FString(Item.price.c_str());
//            TempItem->title = FString(Item.title.c_str());
//            TempItem->type = FString(Item.type.c_str());
//            TempItem->description = FString(Item.description.c_str());
//            Result.Add(TempItem);
//        }
//    #endif
//    }
//    return Result;
//}

void FMRGServiceModule::ProductsPurchaseComplete(const std::string& sku, const std::string& transactionId)
{
#if WITH_MRGS
#if PLATFORM_IOS
	NSLog(@"FMRGServiceModule::bank purchase complete %@", [NSString stringWithCString:sku.c_str()
																			  encoding:[NSString defaultCStringEncoding]]);
#endif
#endif
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		if(MRGSEvent.IsBound())
		{
			MRGSEvent.Broadcast(uint8(EMrgsEventsTypes::TYPE_PURCHASE_COMPLETE));
		}
	});
}

void FMRGServiceModule::ProductsPurchaseFailed(const std::string& sku, const std::string& answer)
{
#if WITH_MRGS
#if PLATFORM_IOS
	NSLog(@"FMRGServiceModule::bank purchase failed %@ with reason:%@", [NSString stringWithCString:sku.c_str() encoding:[NSString defaultCStringEncoding]], [NSString stringWithCString:answer.c_str() encoding:[NSString defaultCStringEncoding]]);
#endif
#endif
	
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		if(MRGSEvent.IsBound())
		{
			MRGSEvent.Broadcast(uint8(EMrgsEventsTypes::TYPE_PURCHASE_FAILED));
		}
	});
}

void FMRGServiceModule::SupportDidClosed()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		if(MRGSEvent.IsBound())
		{
			MRGSEvent.Broadcast(uint8(EMrgsEventsTypes::TYPE_SUPPORT_CLOSED));
		}
	});

#if PLATFORM_IOS

	/*dispatch_async(dispatch_get_main_queue(), ^{

		[[UIApplication sharedApplication] sendAction:@selector(resignFirstResponder) to:nil from : nil forEvent : nil];
		
	});*/

#endif
}

void FMRGServiceModule::SupportDidReceiveError()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		if(MRGSEvent.IsBound())
		{
			MRGSEvent.Broadcast(uint8(EMrgsEventsTypes::TYPE_SUPPORT_ERROR));
		}
	});
}

void FMRGServiceModule::FullscreenDidClosed()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		if(MRGSEvent.IsBound())
		{
			MRGSEvent.Broadcast(uint8(EMrgsEventsTypes::TYPE_FULLSCREEN_CLOSED));
		}
	});
}

FString FMRGServiceModule::GetDeviceModel() const
{
	FString Result = TEXT("");
#if WITH_MRGS && WITH_GET_DEVICE
#if PLATFORM_IOS
	
	struct utsname systemInfo;
	uname(&systemInfo);
	
	Result = FString([NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding]);
	
	/**
	 // get the device hardware type string length
	 size_t DeviceIDLen;
	 sysctlbyname("hw.machine", NULL, &DeviceIDLen, NULL, 0);
	 
	 // get the device hardware type
	 char* DeviceID = (char*)malloc(DeviceIDLen);
	 sysctlbyname("hw.machine", DeviceID, &DeviceIDLen, NULL, 0);
	 
	 // convert to NSStringt
	 NSString *DeviceIDString= [NSString stringWithCString:DeviceID encoding:NSUTF8StringEncoding];
	 Result = FString(DeviceIDString);
	 free(DeviceID);
	 */
#endif // PLATFORM_IOS

#if PLATFORM_ANDROID
	
	Result = FAndroidMisc::GetDeviceModel();
	
#endif // PLATFORM_ANDROID
	
#endif // WITH_MRGS && WITH_GET_DEVICE
	return Result;
}

FString FMRGServiceModule::GetDevicePlatform() const
{
	FString Result = TEXT("");
#if WITH_MRGS && WITH_GET_DEVICE
#if PLATFORM_IOS
	
	MRGSDevice* Device = [[MRGSDevice alloc] init];
	Result = FString([NSString stringWithFormat:@"%@ %@", Device.systemName, [MRGSDevice systemVersion]]);
	
#endif
#endif
	return Result;
}

void FMRGServiceModule::SaveTokenToKeychain(const FString& TokenToSave)
{
#if WITH_AWM_ADDON
#if PLATFORM_IOS
	NSMutableDictionary* KeychainItem = [NSMutableDictionary dictionary];
	
	NSString* IosToken = TokenToSave.GetNSString();
	NSString* Account = MrgsSettings->iOsKeyChainAccount.GetNSString();
	NSString* Service = @"Login";
	
	KeychainItem[(__bridge id)kSecClass] = (__bridge id)kSecClassGenericPassword; // Specify what kind of keychain item this is.
	KeychainItem[(__bridge id)kSecAttrAccessible] = (__bridge id)kSecAttrAccessibleAlways;
	KeychainItem[(__bridge id)kSecAttrAccount] = Account;
	KeychainItem[(__bridge id)kSecAttrService] = Service;
	KeychainItem[(__bridge id)kSecAttrSynchronizable] = (__bridge id)kCFBooleanTrue;
	
	//Check if this keychain item already exists.
	
	if(SecItemCopyMatching((__bridge CFDictionaryRef)KeychainItem, NULL) == noErr)
	{
		UpdateAuthToken(TokenToSave);
	}
	else
	{
		KeychainItem[(__bridge id)kSecValueData] = [IosToken dataUsingEncoding:NSUTF8StringEncoding]; //Our token
		
		OSStatus sts = SecItemAdd((__bridge CFDictionaryRef)KeychainItem, NULL);
		NSLog(@"Error Code: %d", (int)sts);
	}
	
#endif
#endif
}

FString FMRGServiceModule::GetAuthToken() const
{
	FString ResultToken = TEXT("");
	
#if WITH_AWM_ADDON
#if PLATFORM_IOS
	
	NSMutableDictionary* KeychainItem = [NSMutableDictionary dictionary];
	NSString* Account = MrgsSettings->iOsKeyChainAccount.GetNSString();
	NSString* Service = @"Login";
	
	KeychainItem[(__bridge id)kSecClass] = (__bridge id)kSecClassGenericPassword;
	KeychainItem[(__bridge id)kSecAttrAccessible] = (__bridge id)kSecAttrAccessibleAlways;
	KeychainItem[(__bridge id)kSecAttrAccount] = Account;
	KeychainItem[(__bridge id)kSecAttrService] = Service;
	KeychainItem[(__bridge id)kSecAttrSynchronizable] = (__bridge id)kCFBooleanTrue;
	
	KeychainItem[(__bridge id)kSecReturnData] = (__bridge id)kCFBooleanTrue;
	KeychainItem[(__bridge id)kSecReturnAttributes] = (__bridge id)kCFBooleanTrue;
	
	CFDictionaryRef Result = nil;
	
	OSStatus Sts = SecItemCopyMatching((__bridge CFDictionaryRef)KeychainItem, (CFTypeRef *)&Result);
	
	NSLog(@"Error Code: %d", (int)Sts);
	
	if(Sts == noErr)
	{
		NSDictionary* ResultDict = (NSDictionary *)Result;
		NSData* TokenData = ResultDict[(__bridge id)kSecValueData];
		NSString* Token = [[NSString alloc] initWithData:TokenData encoding:NSUTF8StringEncoding];
		
		ResultToken = FString(Token);
	}
	
#endif
#endif
	
	return ResultToken;
}

void FMRGServiceModule::UpdateAuthToken(const FString& AuthToken)
{
#if WITH_AWM_ADDON
#if PLATFORM_IOS
	NSMutableDictionary* KeychainItem = [NSMutableDictionary dictionary];
	NSString* Account = MrgsSettings->iOsKeyChainAccount.GetNSString();
	NSString* Service = @"Login";
	
	NSString* IosToken = AuthToken.GetNSString();
	
	KeychainItem[(__bridge id)kSecClass] = (__bridge id)kSecClassGenericPassword;
	KeychainItem[(__bridge id)kSecAttrAccessible] = (__bridge id)kSecAttrAccessibleAlways;
	KeychainItem[(__bridge id)kSecAttrAccount] = Account;
	KeychainItem[(__bridge id)kSecAttrService] = Service;
	KeychainItem[(__bridge id)kSecAttrSynchronizable] = (__bridge id)kCFBooleanTrue;
	
	// Check if this keychain item already exists.
	if(SecItemCopyMatching((__bridge CFDictionaryRef)KeychainItem, NULL) == noErr)
	{
		// The item was found we can update the keychain item.
		
		NSMutableDictionary *AttributesToUpdate = [NSMutableDictionary dictionary];
		AttributesToUpdate[(__bridge id)kSecValueData] = [IosToken dataUsingEncoding:NSUTF8StringEncoding];
		
		OSStatus Sts = SecItemUpdate((__bridge CFDictionaryRef)KeychainItem, (__bridge CFDictionaryRef)AttributesToUpdate);
		NSLog(@"Error Code: %d", (int)Sts);
	}
	
#endif
#endif
}

void FMRGServiceModule::SaveTokenToICloud(const FString& AuthToken)
{
#if WITH_AWM_ADDON
#if PLATFORM_IOS
	
	if (AuthToken.Len() > 0)
	{
		NSUbiquitousKeyValueStore* Store = [NSUbiquitousKeyValueStore defaultStore];
		if (Store)
		{
			std::string TempToken = TCHAR_TO_UTF8(*AuthToken);
			NSString* IosToken = [NSString stringWithCString:TempToken.c_str() encoding:[NSString defaultCStringEncoding]];
			[Store setString:IosToken forKey:@"AUTH_TOKEN"];
			[Store synchronize];
		}
	}
	
#endif
#endif
}

FString FMRGServiceModule::GetTokenFromICloud() const
{
	FString ResultToken = TEXT("");
	
#if WITH_AWM_ADDON
#if PLATFORM_IOS
	NSUbiquitousKeyValueStore* Store = [NSUbiquitousKeyValueStore defaultStore];
	if (Store)
	{
		NSString* CloudToken = [[Store stringForKey:@"AUTH_TOKEN"] mutableCopy];
		if (CloudToken)
		{
			ResultToken = FString(CloudToken);
		}
	}
#endif
#endif
	
	return ResultToken;
}

FAwmGameCenterAuthInfo FMRGServiceModule::GetGameCenterAuthInfo() 
{
	return GameCenterAuthInfo;
}

FAwmGooglePlayAuthInfo FMRGServiceModule::GetGooglePlayAuthInfo()
{
	return GooglePlayAuthInfo;
}

void FMRGServiceModule::AddMetric(int32 MetricId)
{
#if WITH_MRGS && WITH_MRGS_EVENTS
#if PLATFORM_IOS
	
	[MRGSMetrics addMetricWithId:MetricId];
	
#endif
#if PLATFORM_ANDROID
	MRGService::addMetric(MetricId);
#endif
#endif
}

void FMRGServiceModule::UpdateGooglePlayAuthInfo(const FString& AuthToken)
{
#if PLATFORM_ANDROID
	if (AuthToken.Len())
	{
		GooglePlayAuthInfo.AuthToken = AuthToken;
		
		if (MRGSEvent.IsBound())
		{
			UE_LOG(LogAwmMrgs, Display, TEXT("GooglePlayAuthInfo: %s"), *GooglePlayAuthInfo.AuthToken);
			MRGSEvent.Broadcast(uint8(EMrgsEventsTypes::TYPE_LOGIN_COMPLETE));
		}
	}
	else
	{
		if (MRGSEvent.IsBound())
		{
			UE_LOG(LogAwmMrgs, Warning, TEXT("GetGooglePlayAuthInfo error"));
			MRGSEvent.Broadcast(uint8(EMrgsEventsTypes::TYPE_LOGIN_ERROR));
		}
	}
#endif // PLATFORM_ANDROID
}

void FMRGServiceModule::UpdateGameCenterAuthInfo()
{
#if WITH_AWM_ADDON && PLATFORM_IOS
	
	struct LocalAuthInfo {
		FString PlayerId;
		FString PublicKeyUrl;
		FString Signature;
		FString Salt;
		FString Timestamp;
		FString Nick;
	};

	GameCenterAuthInfo.Reset();
	GKLocalPlayer* Player = [GKLocalPlayer localPlayer];

	if(Player && Player.isAuthenticated == YES)
	{
		
		[Player generateIdentityVerificationSignatureWithCompletionHandler:^(NSURL *publicKeyUrl, NSData *signature, NSData *salt, uint64_t timestamp, NSError *error) {
			
			if(error != nil)
			{
				UE_LOG(LogAwmMrgs, Warning, TEXT("GetGameCenterAuthInfo error"));
				AsyncTask(ENamedThreads::GameThread, [this]()
				{
					if(MRGSEvent.IsBound())
					{
						MRGSEvent.Broadcast(uint8(EMrgsEventsTypes::TYPE_LOGIN_ERROR));
					}
				});
				return; //some sort of error, can't authenticate right now
			}
			
			auto TempAuthInfo = TSharedPtr<LocalAuthInfo, ESPMode::ThreadSafe>(new LocalAuthInfo());

			TempAuthInfo->PlayerId = FString(Player.playerID);
			TempAuthInfo->PublicKeyUrl = FString([publicKeyUrl absoluteString]);
			TempAuthInfo->Signature = FString([signature base64EncodedStringWithOptions:0]);
			TempAuthInfo->Salt = FString([salt base64EncodedStringWithOptions:0]);
			TempAuthInfo->Timestamp = FString([NSString stringWithFormat:@"%lld", timestamp]);
			TempAuthInfo->Nick = FString([Player alias]);
			 
			AsyncTask(ENamedThreads::GameThread, [TempAuthInfo, this]()
			{
				GameCenterAuthInfo.PlayerId = TempAuthInfo->PlayerId;
				GameCenterAuthInfo.PublicKeyUrl = TempAuthInfo->PublicKeyUrl;
				GameCenterAuthInfo.Signature = TempAuthInfo->Signature;
				GameCenterAuthInfo.Salt = TempAuthInfo->Salt;
				GameCenterAuthInfo.Timestamp = TempAuthInfo->Timestamp;
				GameCenterAuthInfo.Nick = TempAuthInfo->Nick;
				
				UE_LOG(LogAwmMrgs, Display, TEXT("GC TOKEN IS: %s %s %s %s %s %s"), *GameCenterAuthInfo.PlayerId, *GameCenterAuthInfo.PublicKeyUrl, *GameCenterAuthInfo.Signature, *GameCenterAuthInfo.Salt, *GameCenterAuthInfo.Timestamp, *GameCenterAuthInfo.Nick);
				
				if(MRGSEvent.IsBound())
				{
					MRGSEvent.Broadcast(uint8(EMrgsEventsTypes::TYPE_LOGIN_COMPLETE));
				}
			});
		}];
	}
	else
	{
		AsyncTask(ENamedThreads::GameThread, [this]()
		{
			if(MRGSEvent.IsBound())
			{
				UE_LOG(LogAwmMrgs, Warning, TEXT("GetGameCenterAuthInfo error"));
				MRGSEvent.Broadcast(uint8(EMrgsEventsTypes::TYPE_LOGIN_ERROR));
			}
		});
	}
#endif
}

FString FMRGServiceModule::GetOpenUDID() const
{
	FString Result;
#if WITH_AWM_ADDON && PLATFORM_IOS
//	MRGSDevice* Device = [[MRGSDevice alloc] init];
//	NSString* DeviceOpenUDID = [Device openUDID];
	NSString* DeviceOpenUDID = [MRGSDevice openUDID];
	if (DeviceOpenUDID)
	{
		Result = FString(DeviceOpenUDID);
		UE_LOG(LogAwmMrgs, Log, TEXT("GetOpenUDID DeviceOpenUDID: %s"), *Result);
	}
	else
	{
		MRGSDevice* Device = [[MRGSDevice alloc] init];
		DeviceOpenUDID = [Device openUDID];
		if (DeviceOpenUDID)
		{
			Result = FString(DeviceOpenUDID);
			UE_LOG(LogAwmMrgs, Log, TEXT("GetOpenUDID DeviceOpenUDID step 2: %s"), *Result);
		}
		else
		{
			UE_LOG(LogAwmMrgs, Warning, TEXT("GetOpenUDID total error"));
		}
	}
#endif
	return Result;
}

void FMRGServiceModule::NavigateToUrl(FString Url)
{
#if PLATFORM_IOS
	LastUrl = Url;
	dispatch_async(dispatch_get_main_queue(), ^{
		std::string const tempUrl = TCHAR_TO_UTF8(*LastUrl);
		MRGSBilling::navigateToUrl(tempUrl);
	});
#endif
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMRGServiceModule, MRGService)
