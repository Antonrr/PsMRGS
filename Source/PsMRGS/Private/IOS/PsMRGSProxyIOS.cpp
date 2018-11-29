// Copyright 2018 Pushkin Studio. All Rights Reserved.

#include "PsMRGSProxyIOS.h"
#include "Async.h"
#include "PsMRGSSettings.h"
#include "PsMRGSCommon.h"

UPsMRGSProxyIOS::UPsMRGSProxyIOS(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bInitComplete = false;
	bUserLoggedin = false;
	MaxUsersSlots = 10;
}

#if PLATFORM_IOS

@implementation PsMRGSDelegate

/** MRGSServerDataDelegate */

- (void)loadServerDataDidFinished:(NSDictionary *)serverData
{
	UE_LOG(LogMRGS, Error, TEXT("%s"), *PS_FUNC_LINE);
}

- (void)initializationFinish
{
	UE_LOG(LogMRGS, Error, TEXT("%s: MRGS init complete"), *PS_FUNC_LINE);
	if(self.Proxy)
	{
		self.Proxy->OnInitComplete();
	}
}

- (void)loadPromoBannersDidFinished:(NSDictionary *)promoBanners
{
	NSError *error;
	NSData *jsonData = [NSJSONSerialization dataWithJSONObject:promoBanners
													   options:NSJSONWritingPrettyPrinted // Pass 0 if you don't care about the readability of the generated string
														 error:&error];
	NSString *jsonString = nil;
	if (!error) {
		jsonString = [[[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding] autorelease];
	}
}

/** MRGSMyTarget delegate */

- (void)mrgsMyTarget:(MRGSMyTarget *)mrgsMyTarget didReceiveShowcaseDataAndFoundNotifications:(BOOL)hasNotifications withAppWallAd:(NSArray*)ad
{
	UE_LOG(LogMRGS, Log, TEXT("%s: MyTarget has notifications: %d"), *PS_FUNC_LINE, int32(hasNotifications));
	
	dispatch_async(dispatch_get_main_queue(), ^{
		mrgsMyTarget.showcaseTitle = @"More games";
		mrgsMyTarget.showcaseCloseButtonTitle = @"Close";
		[mrgsMyTarget openShowcaseWithViewController:[UIApplication sharedApplication].keyWindow.rootViewController
										 onComplete:^{
											 [mrgsMyTarget releaseShowcase];
											 UE_LOG(LogMRGS, Log, TEXT("%s: Showcase closed"), *PS_FUNC_LINE);
										 } onError:^(NSError *error) {
											 UE_LOG(LogMRGS, Log, TEXT("%s: Showcase error"), *PS_FUNC_LINE);
										 }];
	});
}

- (void)mrgsMyTarget:(MRGSMyTarget *)mrgsAdman didNotReceiveShowcaseDataWithReason:(NSString *)reason
{
	UE_LOG(LogMRGS, Log, TEXT("%s: didnt receive showcase data with reason: %s"), *PS_FUNC_LINE, *FString(reason));
	if(self.Proxy)
	{
		self.Proxy->OnShowcaseDataRecieveError(FString(reason));
	}
}

- (void)mrgsMyTargetShowcaseHasNoAds:(MRGSMyTarget *)mrgsMyTarget
{
	UE_LOG(LogMRGS, Log, TEXT("%s: mrgs MyTarget showcase has no ads"), *PS_FUNC_LINE);
	if(self.Proxy)
	{
		self.Proxy->OnShowCaseDataHasNoAds();
	}
}

- (void)mrgsMyTargetDidReceiveFullscreenBannerData:(MRGSMyTarget *)mrgsMyTarget
{
	UE_LOG(LogMRGS, Log, TEXT("%s: mrgs MyTarget open fullscreen"), *PS_FUNC_LINE);
	dispatch_async(dispatch_get_main_queue(), ^{
		[mrgsMyTarget openFullscreenBannerInViewController:[UIApplication sharedApplication].keyWindow.rootViewController];
	});
}

- (void)mrgsMyTarget:(MRGSMyTarget *)mrgsMyTarget didNotReceiveFullscreenBannerDataWithReason:(NSString *)reason
{
	UE_LOG(LogMRGS, Log, TEXT("%s: mrgs MyTarget didnt receive fullscreen banner data with reason: %s"), *PS_FUNC_LINE, *FString(reason));
	if(self.Proxy)
	{
		self.Proxy->OnFullscreenDataRecieveError(FString(reason));
	}
}

- (void)mrgsMyTargetFullscreenBannerClosed:(MRGSMyTarget *)mrgsMyTarget
{
	UE_LOG(LogMRGS, Log, TEXT("%s: mrgs MyTarget fullscreen banner closed"), *PS_FUNC_LINE);
	[mrgsMyTarget releaseFullscreenBanner];
	if(self.Proxy)
	{
		self.Proxy->OnFullscreenClosed();
	}
}

- (void)mrgsMyTargetDidReceiveInterstitialSliderAdData:(MRGSMyTarget *)mrgsMyTarget
{
	UE_LOG(LogMRGS, Log, TEXT("%s"), *PS_FUNC_LINE);
	[mrgsMyTarget openInterstitialSliderAdWithViewController:[UIApplication sharedApplication].keyWindow.rootViewController];
}

- (void)mrgsMyTarget:(MRGSMyTarget *)mrgsMyTarget didNotReceiveInterstitialSliderAdDataWithReason:(NSString *)reason
{
	UE_LOG(LogMRGS, Log, TEXT("%s: mrgs MyTarget didnt receive interstitial slider data with reason: %s"), *PS_FUNC_LINE, *FString(reason));
	if(self.Proxy)
	{
		self.Proxy->OnInterstitialDataRecieveError(FString(reason));
	}
}

- (void)mrgsMyTargetInterstitialSliderAdClosed:(MRGSMyTarget *)mrgsMyTarget
{
	UE_LOG(LogMRGS, Log, TEXT("%s: mrgs MyTarget interstitial slider closed"), *PS_FUNC_LINE);
	[mrgsMyTarget releaseInterstitialSliderAd];
	if(self.Proxy)
	{
		self.Proxy->OnInterstitialSliderClosed();
	}
}

- (void)showAlertViewWithTitle:(NSString *)title andMessage:(NSString *)message
{
	dispatch_async(dispatch_get_main_queue(), ^{
		UIAlertController* alert = [UIAlertController
								   alertControllerWithTitle:title
								   message:message
								   preferredStyle:UIAlertControllerStyleAlert];

		UIAlertAction* yesButton = [UIAlertAction
								   actionWithTitle:@"Ok"
								   style:UIAlertActionStyleDefault
								   handler:^(UIAlertAction * action) {
									   
								   }];

		[alert addAction:yesButton];

		[[IOSAppDelegate GetDelegate].IOSController presentViewController:alert animated:YES completion:nil];
	});
}

/** MRGSMyComSupport delegate */

- (void)myComSupportViewDidClosed
{
	UE_LOG(LogMRGS, Log, TEXT("%s"), *PS_FUNC_LINE);
	if (self.Proxy)
	{
		self.Proxy->OnSupportClosed();
	}
}

- (void)myComSupportDidReceiveError:(NSError *)error
{
	UE_LOG(LogMRGS, Warning, TEXT("%s"), *PS_FUNC_LINE);
	if (self.Proxy)
	{
		self.Proxy->OnSupportReceivedError(FString(error.localizedDescription));
	}
}

- (void)myComSupportDidReceiveNotificationsForTickets
{
	UE_LOG(LogMRGS, Log, TEXT("%s"), *PS_FUNC_LINE);
}

- (void)myComSupportCheckTicketsFailWithError:(NSError *)error
{
	UE_LOG(LogMRGS, Log, TEXT("%s"), *PS_FUNC_LINE);
	if (self.Proxy)
	{
		self.Proxy->OnSupportTicketsFailWithError(FString(error.localizedDescription));
	}
}

- (void)loadingPaymentsResponce:(SKProductsResponse *)response
{
	if ([response.invalidProductIdentifiers count] > 0)
	{
		UE_LOG(LogMRGS, Warning, TEXT("%s invalidProductIdentifiers: %s"), *PS_FUNC_LINE, *FString([response.invalidProductIdentifiers description]));
	}
	
	TArray<FPsMRGSPurchaseInfo> Items;
	for (SKProduct* Product in response.products)
	{
		if (Product == nil)
		{
			UE_LOG(LogMRGS, Warning, TEXT("%s product invalid"), *PS_FUNC_LINE);
			continue;
		}
		
		if (Product.productIdentifier == nil)
		{
			UE_LOG(LogMRGS, Warning, TEXT("%s productIdentifier invalid"), *PS_FUNC_LINE);
			continue;
		}
		
		if ([self getCurrencyCode:Product] == nil)
		{
			UE_LOG(LogMRGS, Warning, TEXT("%s getCurrencyCode invalid for %s"), *PS_FUNC_LINE, *FString([Product.productIdentifier UTF8String]));
			continue;
		}
		
		if (Product.localizedTitle == nil)
		{
			UE_LOG(LogMRGS, Warning, TEXT("%s localizedTitle invalid for %s"), *PS_FUNC_LINE, *FString([Product.productIdentifier UTF8String]));
			continue;
		}
		
		if (Product.localizedDescription == nil)
		{
			UE_LOG(LogMRGS, Warning, TEXT("%s localizedDescription invalid for %s"), *PS_FUNC_LINE, *FString([Product.productIdentifier UTF8String]));
			continue;
		}
		
		FPsMRGSPurchaseInfo Item;
		Item.Sku = FString(Product.productIdentifier);
		
		NSNumberFormatter* NumberFormatter = [[NSNumberFormatter alloc] init];
		[NumberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
		[NumberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
		[NumberFormatter setLocale:Product.priceLocale];
		NSString* FormattedString = [NumberFormatter stringFromNumber:Product.price];
		
		Item.Price = FString(FormattedString);
		Item.FormattedPrice = FString([NSString stringWithFormat:@"%.02f %@", [Product.price doubleValue], [self getCurrencyCode:Product]]);
		Item.Title = FString(Product.localizedTitle);
		Item.Type = FString(TEXT("inapp"));
		Item.Description = FString(Product.localizedDescription);
		Items.Add(Item);
	}
	
	if (Items.Num() == 0)
	{
		UE_LOG(LogMRGS, Warning, TEXT("%s items are empty"), *PS_FUNC_LINE);
	}
	if (self.Proxy)
	{
		self.Proxy->OnStoreProductsLoaded(Items);
	}
}

- (void)restorePurchase
{
	[[MRGSBank sharedInstance] restorePurchase];
}

- (void)paymentSuccessful:(SKPaymentTransaction *)transaction answer:(NSString *)answer
{
	FString PaymentId = FString(transaction.payment.productIdentifier);
	FString TransId = FString(transaction.transactionIdentifier);
	UE_LOG(LogMRGS, Log, TEXT("%s paymentId: %s transId: %s"), *PS_FUNC_LINE, *PaymentId, *TransId);
	
	if (self.Proxy)
	{
		self.Proxy->OnPurchaseComplete(PaymentId, TransId, FString(TEXT("")));
	}
}

- (void)paymentSuccessful:(SKPaymentTransaction *)transaction answer:(NSString *)answer withDeveloperPayload:(NSString *)payload
{
	FString PaymentId = FString(transaction.payment.productIdentifier);
	FString TransId = FString(transaction.transactionIdentifier);
	FString PayloadAnswer = FString(payload);
	UE_LOG(LogMRGS, Log, TEXT("%s paymentId: %s transId: %s payload: %a"), *PS_FUNC_LINE, *PaymentId, *TransId, *PayloadAnswer);
	
	if (self.Proxy)
	{
		self.Proxy->OnPurchaseComplete(PaymentId, TransId, PayloadAnswer);
	}
}

- (void)paymentFailed:(SKPaymentTransaction *)transaction error:(NSError *)error
{
	FString PaymentId = FString(transaction.payment.productIdentifier);
	FString Answer = FString([error description]);
	UE_LOG(LogMRGS, Warning, TEXT("%s paymentId: %s Answer: %s"), *PS_FUNC_LINE, *PaymentId, *Answer);
	
	if (self.Proxy)
	{
		if (error.code != SKErrorPaymentCancelled)
		{
			self.Proxy->OnPurchaseFailed(PaymentId, Answer);
		}
		else
		{
			self.Proxy->OnPurchaseCanceled(PaymentId, Answer);
		}
	}
}

- (NSString *)getCurrencyCode:(SKProduct *)product
{
	NSLocale* PriceLocale = [product priceLocale];
	NSNumberFormatter* CurrencyFormatter = [[[NSNumberFormatter alloc] init] autorelease];
	[CurrencyFormatter setLocale:PriceLocale];
	return [CurrencyFormatter internationalCurrencySymbol];
}

- (void)openStoreUrl:(NSString* )urlToGo
{
	if ([SKStoreProductViewController class] != nil)
	{
		SKStoreProductViewController* Skpvc = [[SKStoreProductViewController alloc] init];
		Skpvc.delegate = self;
		NSDictionary* Dict = [NSDictionary dictionaryWithObject: urlToGo forKey: SKStoreProductParameterITunesItemIdentifier];
		[Skpvc loadProductWithParameters: Dict completionBlock: nil];
		
		[[IOSAppDelegate GetDelegate].IOSController presentViewController:Skpvc animated:YES completion:nil];
	}
}

- (void)productViewControllerDidFinish:(SKStoreProductViewController *)viewController
{
	dispatch_async(dispatch_get_main_queue(), ^{
		if (viewController)
		{
		   [[IOSAppDelegate GetDelegate].IOSController dismissViewControllerAnimated:YES completion:nil];
		}
	});
}

@end

void UPsMRGSProxyIOS::InitModule()
{
	if (IsReady())
	{
		return;
	}
	
	const UPsMRGSSettings* MRGSSettings = GetDefault<UPsMRGSSettings>();
	if (MRGSSettings == nullptr || MRGSSettings->IsValidLowLevel() == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSSettings not initialized or removed"), *PS_FUNC_LINE);
		return;
	}
	
	dispatch_async(dispatch_get_main_queue(), ^{
		
		Delegate = [[PsMRGSDelegate alloc] init];
		Delegate.Proxy = this;
		
		int AppId = MRGSSettings->iOSMrgsAppId;
		bool bDebug = MRGSSettings->bDebugMode;
		NSString* Secret = MRGSSettings->iOSMrgsClientSecret.GetNSString();
		MRGServiceParams* MrgsParams = [[MRGServiceParams alloc] initWithAppId:AppId andSecret:Secret];
		MrgsParams.debug = bDebug;
		MrgsParams.startOnTestDevice = bDebug;
		MrgsParams.shouldResetBadge = bDebug;
		MrgsParams.crashReportEnabled = false;
		MrgsParams.allowPushNotificationHooks = false;
		
		NSArray* Paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, bDebug);
		MrgsParams.storePath = [[Paths objectAtIndex : 0] stringByAppendingPathComponent:@"/mrgsStore"];
		
		// Flurry
		NSString* FlurryKey = MRGSSettings->iOSFlurryApiKey.GetNSString();
		MRGSFlurryParams* FlurryParams = [[MRGSFlurryParams alloc] initWithAPIKey:FlurryKey];
		FlurryParams.crashReportEnabled = false;
		FlurryParams.debug = bDebug;
		
		// Google Analytics
		NSString* GaTrackingId = MRGSSettings->iOSGATrackingId.GetNSString();
		MRGSGoogleAnalyticsParams* GoogleAnalyticsParams = [[MRGSGoogleAnalyticsParams alloc] initWithTrackingId:GaTrackingId];
		GoogleAnalyticsParams.exceptionHandlerEnabled = bDebug;
		GoogleAnalyticsParams.logLevel = 4;
		
		// AppsFlyer
		NSString* AppleAppId = MRGSSettings->iOSAppleAppId.GetNSString();
		NSString* AppsFlyerDevKey = MRGSSettings->iOSAppsFlyerDevKey.GetNSString();
		MRGSAppsFlyerParams* AppsFlyerParams = [[MRGSAppsFlyerParams alloc] initWithDevKey:AppsFlyerDevKey andAppleAppId:AppleAppId];
		AppsFlyerParams.debug = bDebug;
		
		// MyTarget
		int MyTargetShowcaseSlotId = MRGSSettings->iOSMyTargetShowcaseSlotId;
		int MyTargetFullscreenSlotId = MRGSSettings->iOSMyTargetFullscreenSlotId;
		int MyTargetInterstitialSlotId = MRGSSettings->iOSMyTargetInterstitialSlotId;
		MRGSMyTargetParams* MyTargetParams = [[MRGSMyTargetParams alloc] initWithShowcaseSlotId:MyTargetShowcaseSlotId
																		 fullscreenBannerSlotId:MyTargetFullscreenSlotId
																			 interstitialSlotId:MyTargetInterstitialSlotId];
		MyTargetParams.debug = bDebug;
		
		// MyTracker
		NSString* MyTrackerAppId = MRGSSettings->iOSMyTrackerAppId.GetNSString();
		MRGSMyTrackerParams* MyTrackerParams = [[MRGSMyTrackerParams alloc] initWithAppId:MyTrackerAppId];
		MyTrackerParams.forwardMetrics = YES;
		MyTrackerParams.enableLogging = bDebug;
		
		NSArray* ExternalParams = @[FlurryParams,
									GoogleAnalyticsParams,
									AppsFlyerParams,
									MyTargetParams,
									MyTrackerParams];
		
		[MRGServiceInit startWithServiceParams:MrgsParams
							 externalSDKParams:ExternalParams
									  delegate:Delegate];
	});
}

const bool UPsMRGSProxyIOS::IsReady() const
{
	return bInitComplete;
}

const bool UPsMRGSProxyIOS::UserLoggedIn() const
{
	return bUserLoggedin;
}

void UPsMRGSProxyIOS::OnInitComplete()
{
	if (IsReady())
	{
		return;
	}
	
	bInitComplete = true;
	
	const UPsMRGSSettings* MRGSSettings = GetDefault<UPsMRGSSettings>();
	if (MRGSSettings == nullptr || MRGSSettings->IsValidLowLevel() == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSSettings not initialized or removed"), *PS_FUNC_LINE);
		return;
	}
	
	MRGSMyComSupport* Support = [MRGSMyComSupport sharedInstance];
	Support.secret = MRGSSettings->iOSSupportSecretKey.GetNSString();
	Support.delegate = Delegate;
	
	MRGSMyTarget* MyTarget = [MRGSMyTarget sharedInstance];
	MyTarget.delegate = Delegate;
	
	[MRGSBank sharedInstance].delegate = Delegate;
	
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
		  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_INIT_COMPLETE);
		}
	});
}

void UPsMRGSProxyIOS::OnFullscreenClosed()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
		  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_FULLSCREEN_CLOSED);
		}
	});
}

void UPsMRGSProxyIOS::OnInterstitialSliderClosed()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
		  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_INTERSTITIAL_SLIDER_CLOSED);
		}
	});
}

void UPsMRGSProxyIOS::OnInterstitialDataRecieveError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
		  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_INTERSTITIAL_DATA_ERROR);
		}
	});
}

void UPsMRGSProxyIOS::OnFullscreenDataRecieveError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
		  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_FULLSCREEN_DATA_ERROR);
		}
	});
}

void UPsMRGSProxyIOS::OnShowcaseDataRecieveError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
		  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SHOWCASE_DATA_ERROR);
		}
	});
}

void UPsMRGSProxyIOS::OnShowCaseDataHasNoAds()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
		  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SHOWCASE_DATA_EMPTY);
		}
	});
}

void UPsMRGSProxyIOS::OnSupportReceivedError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
		  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SUPPORT_ERROR);
		}
	});
}

void UPsMRGSProxyIOS::OnSupportTicketsFailWithError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
		  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SUPPORT_TICKETS_ERROR);
		}
	});
}

void UPsMRGSProxyIOS::InitUser(const FString& UserId)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	NSString* RealUserId = UserId.GetNSString();
	NSArray* Users = [[MRGSUsers sharedInstance] getAllUsers];
	int Count = [Users count];
	if (Users && Count > 0)
	{
		for(int i = 0; i < Count; i++)
		{
			NSDictionary* User = Users[i];
			if(!User)
			{
				return;
			}
			
			NSString* TempUserId = [User objectForKey:@"userId"];
			if (!TempUserId)
			{
				return;
			}
			
			if ([TempUserId isEqualToString:RealUserId])
			{
				[[MRGSUsers sharedInstance] authorizationUserWithId:TempUserId];
				UE_LOG(LogMRGS, Log, TEXT("%s: MRGService user %s founded in previous sessions and authorized"), *PS_FUNC_LINE, *FString(TempUserId));
				OnUserAuthSuccess();
				return;
			}
			
			if (i == MaxUsersSlots - 1)
			{
				[[MRGSUsers sharedInstance] removeUserWIthId:TempUserId];
				UE_LOG(LogMRGS, Log, TEXT("%s: MRGService user %s removed"), *PS_FUNC_LINE, *FString(TempUserId));
			}
		}
	}
	
	NSError* Err = nil;
	[[MRGSUsers sharedInstance] registerNewUserWithId:RealUserId andError:&Err];
	if (Err)
	{
		UE_LOG(LogMRGS, Log, TEXT("%s: MRGService user registration error: %s"), *PS_FUNC_LINE, *FString(Err.localizedDescription));
		OnUserAuthError();
		return;
	}
	
	[[MRGSUsers sharedInstance] authorizationUserWithId:RealUserId];
	UE_LOG(LogMRGS, Log, TEXT("%s: MRGService  %s registred and authorized"), *PS_FUNC_LINE, *FString(RealUserId));
	OnUserAuthSuccess();
}

void UPsMRGSProxyIOS::LoadStoreProducts(const TArray<FString>& ProductsList)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	NSMutableArray* Objects = [[NSMutableArray alloc] init];
	for (auto& Product : ProductsList)
	{
		[Objects addObject:[MRGSBankProduct productWithId:Product.GetNSString() andType:@"cons"]];
	}
	
	NSArray* ResultArray = [NSArray arrayWithArray:Objects];
	[[MRGSBank sharedInstance] loadProductsWithTypesFromAppleServer:ResultArray];
}

void UPsMRGSProxyIOS::OnStoreProductsLoaded(TArray<FPsMRGSPurchaseInfo> InLoadedProducts)
{
	LoadedProducts = InLoadedProducts;
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if(MRGSDelegate.IsBound())
		{
		  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PRODUCTS_LOADED);
		}
	});
}

const TArray<FPsMRGSPurchaseInfo>& UPsMRGSProxyIOS::GetProducts() const
{
	return LoadedProducts;
}

void UPsMRGSProxyIOS::BuyProduct(const FString& ProductId, const FString& Payload)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	[[MRGSBank sharedInstance] addPayment:ProductId.GetNSString() withDeveloperPayload:Payload.GetNSString()];
}

void UPsMRGSProxyIOS::OnPurchaseComplete(const FString& PaymentId, const FString& TransactionId, const FString& Payload)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if(MRGSDelegate.IsBound())
		{
		  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_COMPLETE);
		}
	});
}

void UPsMRGSProxyIOS::OnPurchaseFailed(const FString& ProductId, const FString& Answer)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if(MRGSDelegate.IsBound())
		{
		  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_FAILED);
		}
	});
}

void UPsMRGSProxyIOS::OnPurchaseCanceled(const FString& ProductId, const FString& Answer)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if(MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_CANCELED);
		}
	});
}

void UPsMRGSProxyIOS::SendGAScreen(const FString& InScreenName)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	[MRGSGoogleAnalytics setNewScreenName:InScreenName.GetNSString()];
}

void UPsMRGSProxyIOS::SendGAEvent(const FString& InCategory, const FString& InAction, const FString& InLabel)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	[MRGSGoogleAnalytics createEventWithCategory:InCategory.GetNSString() action:InAction.GetNSString() label:InLabel.GetNSString() value:[NSNumber numberWithInt:1]];
}

void UPsMRGSProxyIOS::SendFlurryEvent(const FString& InAction)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	[MRGSFlurry logEvent:InAction.GetNSString() withParameters:nil];
}

void UPsMRGSProxyIOS::SendAFEvent(const FString& InEventName, const FString& InValue)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	[MRGSAppsFlyer trackEvent:InEventName.GetNSString() withValues:nil];
}

void UPsMRGSProxyIOS::AddMetric(int32 MetricId)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	[MRGSMetrics addMetricWithId:MetricId];
}

void UPsMRGSProxyIOS::ShowMyTargetShowcase()
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	MRGSMyTarget* MyTarget = [MRGSMyTarget sharedInstance];
	[MyTarget loadShowcaseData];
}

void UPsMRGSProxyIOS::ShowMyTargetFullscreen()
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	MRGSMyTarget* MyTarget = [MRGSMyTarget sharedInstance];
	[MyTarget loadFullscreenBannerData];
}

void UPsMRGSProxyIOS::ShowMyTargetInterstitialSlider()
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	MRGSMyTarget* MyTarget = [MRGSMyTarget sharedInstance];
	[MyTarget loadInterstitialSliderAd];
}

void UPsMRGSProxyIOS::ShowSupport()
{
	UE_LOG(LogMRGS, Log, TEXT("%s"), *PS_FUNC_LINE);
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	dispatch_async(dispatch_get_main_queue(), ^{
		UIView* ViewContainer = [UIApplication sharedApplication].keyWindow.rootViewController.view;
		MRGSMyComSupport *support = [MRGSMyComSupport sharedInstance];
		[support showSupportViewOnSuperview:ViewContainer];
	});
}

void UPsMRGSProxyIOS::OnSupportClosed()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if(MRGSDelegate.IsBound())
		{
		  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SUPPORT_CLOSED);
		}
	});
}


void UPsMRGSProxyIOS::OnUserAuthSuccess()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
	if(MRGSDelegate.IsBound())
	{
	  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_USERINIT_COMPLETE);
	}
	});
}

void UPsMRGSProxyIOS::OnUserAuthError()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if(MRGSDelegate.IsBound())
		{
		  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_USERINIT_ERROR);
		}
	});
}

#endif //IOS
