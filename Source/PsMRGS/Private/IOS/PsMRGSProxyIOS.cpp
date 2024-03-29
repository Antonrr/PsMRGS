// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "PsMRGSProxyIOS.h"

#include "PsMRGSCommon.h"
#include "PsMRGSSettings.h"

#include "Async/Async.h"

#if PLATFORM_IOS

@implementation PsMRGSDelegate

/** MRGSGDPRDelegate */

- (void)userHasAcceptedGDPR:(MRGSGDPR*)gdpr withAdvertising:(BOOL)flag
{
	UE_LOG(LogMRGS, Log, TEXT("%s"), *PS_FUNC_LINE);
	if (self.Proxy)
	{
		self.Proxy->OnGDPRAccepted(flag);
	}
}

- (void)errorShowingAgreement:(MRGSGDPR*)gdpr
{
	UE_LOG(LogMRGS, Log, TEXT("%s"), *PS_FUNC_LINE);
	if (self.Proxy)
	{
		self.Proxy->OnGDPRError();
	}
}

/** MRGSServerDataDelegate */

- (void)loadServerDataDidFinished:(NSDictionary*)serverData
{
	UE_LOG(LogMRGS, Log, TEXT("%s"), *PS_FUNC_LINE);
}

- (void)initializationFinish
{
	UE_LOG(LogMRGS, Log, TEXT("%s: MRGS init complete"), *PS_FUNC_LINE);
	if (self.Proxy)
	{
		self.Proxy->OnInitComplete();
	}
}

- (void)loadPromoBannersDidFinished:(NSDictionary*)promoBanners
{
	NSError* error;
	NSData* jsonData = [NSJSONSerialization dataWithJSONObject:promoBanners
													   options:NSJSONWritingPrettyPrinted
														 error:&error];
	NSString* jsonString = nil;
	if (!error)
	{
		jsonString = [[[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding] autorelease];
	}
}

- (void)showAlertViewWithTitle:(NSString*)title andMessage:(NSString*)message
{
	dispatch_async(dispatch_get_main_queue(), ^{
	  UIAlertController* alert = [UIAlertController
		  alertControllerWithTitle:title
						   message:message
					preferredStyle:UIAlertControllerStyleAlert];

	  UIAlertAction* yesButton = [UIAlertAction
		  actionWithTitle:@"Ok"
					style:UIAlertActionStyleDefault
				  handler:^(UIAlertAction* action){

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

- (void)myComSupportDidReceiveError:(NSError*)error
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
	if (self.Proxy)
	{
		self.Proxy->OnSupportTicketsReceived();
	}
}

- (void)myComSupportCheckTicketsFailWithError:(NSError*)error
{
	UE_LOG(LogMRGS, Log, TEXT("%s"), *PS_FUNC_LINE);
	if (self.Proxy)
	{
		self.Proxy->OnSupportTicketsFailWithError(FString(error.localizedDescription));
	}
}

/** MRGSBankDelegateEx */

- (void)didReceiveProductsResponce:(MRGSBankProductsResponce* _Nonnull)response
{
	if ([response.invalidProductsIds count] > 0)
	{
		UE_LOG(LogMRGS, Warning, TEXT("%s invalidProductsIds: %s"), *PS_FUNC_LINE, *FString([response.invalidProductsIds description]));
	}

	TArray<FPsMRGSPurchaseInfo> Items;
	for (MRGSBankProduct* Product in response.validProducts)
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

		SKProduct* nativeProduct = Product.skProduct;
		NSNumberFormatter* NumberFormatter = [[NSNumberFormatter alloc] init];
		[NumberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
		[NumberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
		[NumberFormatter setLocale:nativeProduct.priceLocale];
		NSString* FormattedString = [NumberFormatter stringFromNumber:nativeProduct.price];

		Item.Price = FString(FormattedString);
		Item.FormattedPrice = FString([NSString stringWithFormat:@"%.02f %@", [nativeProduct.price doubleValue], [self getCurrencyCode:Product]]);
		Item.Currency = FString(nativeProduct.priceLocale.currencyCode);
		Item.Title = FString(Product.localizedTitle);
		Item.Type = FString(TEXT("inapp"));
		Item.Description = FString(Product.localizedDescription);
		Item.bDebug = false;
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

- (void)didReceiveProductsError:(MRGSBankProductsResponce* _Nonnull)response
{
	if ([response.invalidProductsIds count] > 0)
	{
		UE_LOG(LogMRGS, Warning, TEXT("%s invalidProductsIds: %s"), *PS_FUNC_LINE, *FString([response.invalidProductsIds description]));
	}

	if (response.requestError != nil)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: didReceiveProductsError: %s"), *PS_FUNC_LINE, *FString([response.requestError localizedDescription]));
	}

	if (self.Proxy)
	{
		self.Proxy->OnStoreProductsLoaded({});
	}
}

- (void)didReceiveSucessfullPurchase:(MRGSBankPurchaseResult* _Nonnull)purchase
{
	SKPaymentTransaction* nativeTransaction = purchase.transaction.transaction;
	FString PaymentId = FString(nativeTransaction.payment.productIdentifier);
	FString TransId = FString(nativeTransaction.transactionIdentifier);
	FString Payload;

	if (purchase.developerPayload != nil)
	{
		Payload = FString(purchase.developerPayload);
	}

	UE_LOG(LogMRGS, Log, TEXT("%s paymentId: %s transId: \"%s\", payload: \"%s\""), *PS_FUNC_LINE, *PaymentId, *TransId, *Payload);

	if (self.Proxy)
	{
		self.Proxy->OnPurchaseComplete(PaymentId, TransId, Payload);
	}
}

- (void)didReceiveFailedPurchase:(MRGSBankPurchaseResult* _Nonnull)purchase
{
	SKPaymentTransaction* nativeTransaction = purchase.transaction.transaction;
	FString PaymentId = FString(nativeTransaction.payment.productIdentifier);
	FString Error;
	if (purchase.error != nil)
	{
		Error = FString([purchase.error description]);
	}

	UE_LOG(LogMRGS, Warning, TEXT("%s paymentId: %s Error: %s"), *PS_FUNC_LINE, *PaymentId, *Error);

	if (self.Proxy)
	{
		if (purchase.error.code != SKErrorPaymentCancelled)
		{
			self.Proxy->OnPurchaseFailed(PaymentId, Error);
		}
		else
		{
			self.Proxy->OnPurchaseCanceled(PaymentId, Error);
		}
	}
}

- (void)didReceivePendingPurchase:(MRGSBankPurchaseResult* _Nonnull)purchase
{
}

- (void)didReceiveCancelledPurchase:(MRGSBankPurchaseResult* _Nonnull)purchase
{
	SKPaymentTransaction* nativeTransaction = purchase.transaction.transaction;
	FString PaymentId = FString(nativeTransaction.payment.productIdentifier);
	FString Error;
	if (purchase.error != nil)
	{
		Error = FString([purchase.error description]);
	}

	UE_LOG(LogMRGS, Warning, TEXT("%s paymentId: %s Error: %s"), *PS_FUNC_LINE, *PaymentId, *Error);

	if (self.Proxy)
	{
		self.Proxy->OnPurchaseCanceled(PaymentId, Error);
	}
}

- (void)didCompleteTransactionsRestore
{
}

/** end of MRGSBankDelegateEx */

- (NSString*)getCurrencyCode:(MRGSBankProduct*)product
{
	NSLocale* PriceLocale = [product.skProduct priceLocale];
	NSNumberFormatter* CurrencyFormatter = [[[NSNumberFormatter alloc] init] autorelease];
	[CurrencyFormatter setLocale:PriceLocale];
	return [CurrencyFormatter internationalCurrencySymbol];
}

- (void)restorePurchase
{
	[[MRGSBank sharedInstance] restorePurchase];
}

- (void)registerLifeCycleListener
{
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(didFinishLaunching:)
												 name:UIApplicationDidFinishLaunchingNotification
											   object:nil];
}

- (void)didFinishLaunching:(NSNotification*)notification
{
	[MRGServiceInit application:[UIApplication sharedApplication] didFinishLaunchingWithOptions:[IOSAppDelegate GetDelegate].launchOptions];
}

- (void)productViewControllerDidFinish:(SKStoreProductViewController*)viewController
{
	dispatch_async(dispatch_get_main_queue(), ^{
	  if (viewController)
	  {
		  [[IOSAppDelegate GetDelegate].IOSController dismissViewControllerAnimated:YES completion:nil];
	  }
	});
}

- (void)initAdvertising
{
	self.videoAdvertising = [[MRGSAdvertisingManager sharedInstance] createAdvertising:YES];
	[self.videoAdvertising setLoadDelegate:self];
	[self.videoAdvertising setShowDelegate:self];
	[self.videoAdvertising loadContent];
}

- (void)loadAdvertisingContent
{
	if (self.videoAdvertising)
	{
		[self.videoAdvertising loadContent];
	}
}

- (bool)isAdvertisingLoaded
{
	if (self.videoAdvertising)
	{
		return [self.videoAdvertising canShowContent];
	}
	else
	{
		return false;
	}
}

- (bool)showAdvertising
{
	if (self.videoAdvertising)
	{
		[self.videoAdvertising showContentFromUIViewController:UIApplication.sharedApplication.delegate.window.rootViewController];
		return true;
	}
	else
	{
		return false;
	}
}

/** MRGSAdvertisingLoadDelegate */
- (void)onAdvertisingLoaded:(MRGSAdvertising*)advertising
{
	if (self.Proxy)
	{
		self.Proxy->OnAdvertisingLoaded();
	}
}

- (void)onAdvertisingLoadingError:(MRGSAdvertising*)advertising
{
	if (self.Proxy)
	{
		self.Proxy->OnAdvertisingLoadingError();
	}
}

/** MRGSAdvertisingShowDelegate */
- (void)onAdvertisingFinished:(MRGSAdvertising*)advertising wasSkipped:(BOOL)skipped
{
	if (self.Proxy)
	{
		self.Proxy->OnAdvertisingFinished(skipped);
	}
}

/** MRGSShowcaseDelegate */
- (void)didReceiveNewShowcaseContent:(NSNumber*)numberOfNewEvents
{
	if (self.Proxy)
	{
		self.Proxy->OnNewShowcaseContent();
	}
}

@end

//////////////////////////////////////////////////////////////////////////
// UPsMRGSProxyIOS

UPsMRGSProxyIOS::UPsMRGSProxyIOS(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bInitComplete = false;
	bUserLoggedin = false;
	MaxUsersSlots = 10;

	Delegate = [[PsMRGSDelegate alloc] init];
	Delegate.Proxy = this;

	[Delegate registerLifeCycleListener];
	[Delegate didFinishLaunching:nil];
}

//////////////////////////////////////////////////////////////////////////
// GDPR

void UPsMRGSProxyIOS::ShowDefaultGDPRAgreement(bool bOnlyEU, bool bWithAdvertising, FString LocalizationLanguage)
{
	dispatch_async(dispatch_get_main_queue(), ^{
	  const UPsMRGSSettings* MRGSSettings = GetDefault<UPsMRGSSettings>();
	  if (MRGSSettings == nullptr || MRGSSettings->IsValidLowLevel() == false)
	  {
		  UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSSettings not initialized or removed"), *PS_FUNC_LINE);
		  return;
	  }

	  MRGSGDPR* GDPRInstance = [MRGSGDPR sharedInstance];
	  GDPRInstance.delegate = Delegate;
	  GDPRInstance.onlyEU = bOnlyEU;
	  GDPRInstance.withAdvertising = bWithAdvertising;
	  GDPRInstance.localizationLanguage = LocalizationLanguage.IsEmpty() ? @"en" : LocalizationLanguage.GetNSString();
	  [GDPRInstance enableAutomaticCOPPAFlowForAppId:MRGSSettings->iOSMrgsAppId andSecret:MRGSSettings->iOSMrgsClientSecret.GetNSString()];
	  [GDPRInstance showDefaultAgreementAtViewController:UIApplication.sharedApplication.delegate.window.rootViewController
												forAppId:MRGSSettings->iOSMrgsAppId];
	});
}

void UPsMRGSProxyIOS::ShowGDPRAgreement(bool bOnlyEU, bool bWithAdvertising, FString LocalizationLanguage)
{
	dispatch_async(dispatch_get_main_queue(), ^{
	  const UPsMRGSSettings* MRGSSettings = GetDefault<UPsMRGSSettings>();
	  if (MRGSSettings == nullptr || MRGSSettings->IsValidLowLevel() == false)
	  {
		  UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSSettings not initialized or removed"), *PS_FUNC_LINE);
		  return;
	  }

	  MRGSGDPR* GDPRInstance = [MRGSGDPR sharedInstance];
	  GDPRInstance.delegate = Delegate;
	  GDPRInstance.onlyEU = bOnlyEU;
	  GDPRInstance.withAdvertising = bWithAdvertising;
	  GDPRInstance.localizationLanguage = LocalizationLanguage.IsEmpty() ? @"en" : LocalizationLanguage.GetNSString();
	  [GDPRInstance enableAutomaticCOPPAFlowForAppId:MRGSSettings->iOSMrgsAppId andSecret:MRGSSettings->iOSMrgsClientSecret.GetNSString()];
	  [GDPRInstance showAgreementAtViewController:UIApplication.sharedApplication.delegate.window.rootViewController
										 forAppId:MRGSSettings->iOSMrgsAppId
										 fromFile:[[NSBundle mainBundle] URLForResource:@"gdpr"
																		  withExtension:@"html"
																		   subdirectory:@"gdpr"]];
	});
}

int32 UPsMRGSProxyIOS::GetGDPRAcceptedVersion()
{
	return [[MRGSGDPR sharedInstance] getAgreedVersion];
}

//////////////////////////////////////////////////////////////////////////
// CCPA

bool UPsMRGSProxyIOS::ShouldShowCCPA()
{
	return [[MRGSGDPR sharedInstance] shouldShowCCPAButton];
}

EPsMRGSCCPASetting UPsMRGSProxyIOS::GetCCPASettingValue()
{
	MRGSCCPAUserPreference Preference = [[MRGSGDPR sharedInstance] getCurrentCCPAUserPrefrence];
	if (Preference == kMRGSCCPAUserPreferenceShare)
	{
		return EPsMRGSCCPASetting::Share;
	}
	else if (Preference == kMRGSCCPAUserPreferenceNotSharing)
	{
		return EPsMRGSCCPASetting::DontShare;
	}
	else
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: unhandled MRGSCCPAUserPreference value"), *PS_FUNC_LINE);
		return EPsMRGSCCPASetting::Share;
	}
}

void UPsMRGSProxyIOS::SetCCPASettingValue(EPsMRGSCCPASetting Value)
{
	if (Value == EPsMRGSCCPASetting::Share)
	{
		[[MRGSGDPR sharedInstance] setUserChangedCCPAPrefrences:kMRGSCCPAUserPreferenceShare];
	}
	else if (Value == EPsMRGSCCPASetting::DontShare)
	{
		[[MRGSGDPR sharedInstance] setUserChangedCCPAPrefrences:kMRGSCCPAUserPreferenceNotSharing];
	}
	else
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: unhandled EPsMRGSCCPASetting value"), *PS_FUNC_LINE);
		[[MRGSGDPR sharedInstance] setUserChangedCCPAPrefrences:kMRGSCCPAUserPreferenceNotSharing];
	}
}

//////////////////////////////////////////////////////////////////////////
// App tracking

bool UPsMRGSProxyIOS::ShouldShowTrackingAuthorizationDialog()
{
	return [[MRGSDevice currentDevice] shouldShowTrackingAuthorizationDialog];
}

void UPsMRGSProxyIOS::RequestTrackingAuthorization()
{
	[[MRGSDevice currentDevice] requestTrackingAuthorizationWithCompletionHandler:^(MRGSIDFATrackingAuthorizationStatus Status) {
	  AsyncTask(ENamedThreads::GameThread, [this, Status]() {
		  if (MRGSDelegate.IsBound())
		  {
			  switch (Status)
			  {
			  case MRGSIDFATrackingAuthorizationStatus::MRGSIDFATrackingAuthorizationStatusNotDetermined:
				  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_ATT_NOT_DETERMINED);
				  break;

			  case MRGSIDFATrackingAuthorizationStatus::MRGSIDFATrackingAuthorizationStatusRestricted:
				  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_ATT_RESTRICTED);
				  break;

			  case MRGSIDFATrackingAuthorizationStatus::MRGSIDFATrackingAuthorizationStatusDenied:
				  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_ATT_DENIED);
				  break;

			  case MRGSIDFATrackingAuthorizationStatus::MRGSIDFATrackingAuthorizationStatusAuthorized:
				  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_ATT_AUTHORIZED);
				  break;

			  default:
				  UE_LOG(LogMRGS, Error, TEXT("%s: unknown ATT status"), *PS_FUNC_LINE);
				  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_ATT_ERROR);
				  break;
			  }
		  }
	  });
	}];
}

EPsMRGSATTStatus UPsMRGSProxyIOS::GetTrackingAuthorizationStatus()
{
	MRGSIDFATrackingAuthorizationStatus Status = [MRGSDevice currentDevice].currentTrackingAuthorizationStatus;
	switch (Status)
	{
	case MRGSIDFATrackingAuthorizationStatus::MRGSIDFATrackingAuthorizationStatusNotDetermined:
		return EPsMRGSATTStatus::NotDetermined;

	case MRGSIDFATrackingAuthorizationStatus::MRGSIDFATrackingAuthorizationStatusRestricted:
		return EPsMRGSATTStatus::Restricted;

	case MRGSIDFATrackingAuthorizationStatus::MRGSIDFATrackingAuthorizationStatusDenied:
		return EPsMRGSATTStatus::Denied;

	case MRGSIDFATrackingAuthorizationStatus::MRGSIDFATrackingAuthorizationStatusAuthorized:
		return EPsMRGSATTStatus::Authorized;

	default:
		UE_LOG(LogMRGS, Error, TEXT("%s: unknown ATT status"), *PS_FUNC_LINE);
		return EPsMRGSATTStatus::NotDetermined;
	}
}

//////////////////////////////////////////////////////////////////////////
// Setup

void UPsMRGSProxyIOS::CheckIntegration()
{
	dispatch_async(dispatch_get_main_queue(), ^{
	  [[MRGServiceInit sharedInstance] checkIntegration];
	});
}

void UPsMRGSProxyIOS::InitModule()
{
	if (IsReady())
	{
		AsyncTask(ENamedThreads::GameThread, [this]() {
			if (MRGSDelegate.IsBound())
			{
				MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_INIT_COMPLETE);
			}
		});
		return;
	}

	const UPsMRGSSettings* MRGSSettings = GetDefault<UPsMRGSSettings>();
	if (MRGSSettings == nullptr || MRGSSettings->IsValidLowLevel() == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSSettings not initialized or removed"), *PS_FUNC_LINE);
		return;
	}

	dispatch_async(dispatch_get_main_queue(), ^{
	  int32 AppId = MRGSSettings->iOSMrgsAppId;
#if !UE_BUILD_SHIPPING
	  bool bDebug = MRGSSettings->bDebugInDevelopment;
#else
		  bool bDebug = false;
#endif
	  NSString* Secret = MRGSSettings->iOSMrgsClientSecret.GetNSString();
	  MRGServiceParams* MrgsParams = [[MRGServiceParams alloc] initWithAppId:AppId andSecret:Secret];
	  MrgsParams.debug = bDebug;
	  MrgsParams.startOnTestDevice = bDebug;
	  MrgsParams.shouldResetBadge = false;
	  MrgsParams.crashReportEnabled = false;
	  MrgsParams.allowPushNotificationHooks = true;
	  MrgsParams.disablePaymentsCheck = false;
	  MrgsParams.automaticPaymentTracking = false;
	  MrgsParams.MRGSNotificationCenterSupported = true;
	  MrgsParams.defferedMRGSNotificationCenterStart = true;
	  MrgsParams.automaticallyShowIDFARequestAtStartup = MRGSSettings->bShowAppTrackingRequestAtStartup;

	  NSArray* Paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, bDebug);
	  MrgsParams.storePath = [[Paths objectAtIndex:0] stringByAppendingPathComponent:@"/mrgsStore"];

	  NSMutableArray* ExternalParams = [[NSMutableArray alloc] init];

	  // Google Analytics
	  NSString* GaTrackingId = MRGSSettings->iOSGATrackingId.GetNSString();
	  if ([GaTrackingId length] > 0)
	  {
		  MRGSGoogleAnalyticsParams* GoogleAnalyticsParams = [[MRGSGoogleAnalyticsParams alloc] initWithTrackingId:GaTrackingId];
		  GoogleAnalyticsParams.exceptionHandlerEnabled = bDebug;
		  GoogleAnalyticsParams.logLevel = 4;
		  [ExternalParams addObject:GoogleAnalyticsParams];
	  }

	  // AppsFlyer
	  NSString* AppleAppId = MRGSSettings->iOSAppleAppId.GetNSString();
	  NSString* AppsFlyerDevKey = MRGSSettings->iOSAppsFlyerDevKey.GetNSString();
	  if ([AppleAppId length] > 0 && [AppsFlyerDevKey length] > 0)
	  {
		  MRGSAppsFlyerParams* AppsFlyerParams = [[MRGSAppsFlyerParams alloc] initWithDevKey:AppsFlyerDevKey andAppleAppId:AppleAppId];
		  AppsFlyerParams.debug = bDebug;
		  AppsFlyerParams.waitForATTUserAuthorization = 20.f;
		  [ExternalParams addObject:AppsFlyerParams];
	  }

	  // MyTracker
	  NSString* MyTrackerAppId = MRGSSettings->iOSMyTrackerAppId.GetNSString();
	  MRGSMyTrackerParams* MyTrackerParams = [[MRGSMyTrackerParams alloc] initWithAppId:MyTrackerAppId];
	  MyTrackerParams.forwardMetrics = YES;
	  MyTrackerParams.enableLogging = bDebug;
	  [ExternalParams addObject:MyTrackerParams];

	  // MRGSMyComSupport
	  MRGSMyComSupportParams* SupportParams = [[MRGSMyComSupportParams alloc] init];
	  SupportParams.projectId = [NSString stringWithFormat:@"%d", AppId];
	  SupportParams.secret = MRGSSettings->iOSSupportSecretKey.GetNSString();
	  [ExternalParams addObject:SupportParams];

	  [MRGServiceInit startWithServiceParams:MrgsParams
						   externalSDKParams:ExternalParams
									delegate:Delegate];
	});
}

bool UPsMRGSProxyIOS::IsReady() const
{
	return bInitComplete;
}

bool UPsMRGSProxyIOS::UserLoggedIn() const
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

	[MRGSMyComSupport sharedInstance].delegate = Delegate;
	[MRGSBank sharedInstance].delegateExtended = Delegate;
	[Delegate initAdvertising];
	[MRGSShowcase sharedInstance].delegate = Delegate;

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

void UPsMRGSProxyIOS::InitUser(const FString& UserId)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}

	[MRGSUsers setUserId:UserId.GetNSString()];
	UE_LOG(LogMRGS, Log, TEXT("%s: MRGService user id %s authorized"), *PS_FUNC_LINE, *UserId);
	OnUserAuthSuccess();
}

void UPsMRGSProxyIOS::LoadStoreProducts(const TArray<FString>& ProductsList)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}

	MRGSBankProductsRequest* request = [[MRGSBankProductsRequest alloc] init];
	for (auto& Product : ProductsList)
	{
		[request addProductIdentifier:Product.GetNSString() withType:kMRGSBankProductTypeConsumable];
	}

	[[MRGSBank sharedInstance] requestProductsInfo:request];
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

	[[MRGSBank sharedInstance] purchaseProduct:ProductId.GetNSString() withDeveloperPayload:Payload.GetNSString()];
}

void UPsMRGSProxyIOS::OnPurchaseComplete(const FString& PaymentId, const FString& TransactionId, const FString& Payload)
{
	AsyncTask(ENamedThreads::GameThread, [this, PaymentId, TransactionId, Payload]() {
		MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_COMPLETE);
		MRGSIAPDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_COMPLETE, PaymentId, TransactionId, Payload);
	});
}

void UPsMRGSProxyIOS::OnPurchaseFailed(const FString& ProductId, const FString& Answer)
{
	AsyncTask(ENamedThreads::GameThread, [this, ProductId, Answer]() {
		MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_FAILED);
		MRGSIAPDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_FAILED, ProductId, "", Answer);
	});
}

void UPsMRGSProxyIOS::OnPurchaseCanceled(const FString& ProductId, const FString& Answer)
{
	AsyncTask(ENamedThreads::GameThread, [this, ProductId, Answer]() {
		MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_CANCELED);
		MRGSIAPDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_CANCELED, ProductId, "", Answer);
	});
}

void UPsMRGSProxyIOS::SendAFEvent(const FString& InEventName, const FString& InValue)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}

	[[[MRGSAnalytics sharedInstance] appsFlyer] trackEvent:InEventName.GetNSString() withValues:nil];
}

void UPsMRGSProxyIOS::AddMetricWithId(int32 MetricId)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}

	[MRGSMetrics addMetricWithId:MetricId];
}

void UPsMRGSProxyIOS::AddMetricWithCode(const FString& MetricCode, int32 Value, int32 Level, int32 ObjectId)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}

	[MRGSMetrics addMetricWithCode:MetricCode.GetNSString() andValue:Value andLevel:Level andObjectId:ObjectId];
}

void UPsMRGSProxyIOS::AddTrackerEvent(const FPsMRGSTrackerEvent& Event)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}

	MRGSTrackerCustomEvent* CustomEvent = [MRGSTrackerEvent customEventWithName:Event.EventName.GetNSString()];
	CustomEvent.level = Event.Level;

	CustomEvent.customInt1 = Event.customInt1;
	CustomEvent.customInt2 = Event.customInt2;
	CustomEvent.customInt3 = Event.customInt3;
	CustomEvent.customInt4 = Event.customInt4;
	CustomEvent.customInt5 = Event.customInt5;
	CustomEvent.customInt6 = Event.customInt6;
	CustomEvent.customInt7 = Event.customInt7;
	CustomEvent.customInt8 = Event.customInt8;
	CustomEvent.customInt9 = Event.customInt9;
	CustomEvent.customInt10 = Event.customInt10;

	CustomEvent.customFloat1 = Event.customFloat1;
	CustomEvent.customFloat2 = Event.customFloat2;
	CustomEvent.customFloat3 = Event.customFloat3;
	CustomEvent.customFloat4 = Event.customFloat4;
	CustomEvent.customFloat5 = Event.customFloat5;
	CustomEvent.customFloat6 = Event.customFloat6;
	CustomEvent.customFloat7 = Event.customFloat7;
	CustomEvent.customFloat8 = Event.customFloat8;
	CustomEvent.customFloat9 = Event.customFloat9;
	CustomEvent.customFloat10 = Event.customFloat10;

	CustomEvent.customString1 = Event.customString1.GetNSString();
	CustomEvent.customString2 = Event.customString2.GetNSString();
	CustomEvent.customString3 = Event.customString3.GetNSString();
	CustomEvent.customString4 = Event.customString4.GetNSString();
	CustomEvent.customString5 = Event.customString5.GetNSString();
	CustomEvent.customString6 = Event.customString6.GetNSString();
	CustomEvent.customString7 = Event.customString7.GetNSString();
	CustomEvent.customString8 = Event.customString8.GetNSString();
	CustomEvent.customString9 = Event.customString9.GetNSString();
	CustomEvent.customString10 = Event.customString10.GetNSString();
	CustomEvent.customString11 = Event.customString11.GetNSString();
	CustomEvent.customString12 = Event.customString12.GetNSString();
	CustomEvent.customString13 = Event.customString13.GetNSString();
	CustomEvent.customString14 = Event.customString14.GetNSString();
	CustomEvent.customString15 = Event.customString15.GetNSString();
	CustomEvent.customString16 = Event.customString16.GetNSString();
	CustomEvent.customString17 = Event.customString17.GetNSString();
	CustomEvent.customString18 = Event.customString18.GetNSString();
	CustomEvent.customString19 = Event.customString19.GetNSString();
	CustomEvent.customString20 = Event.customString20.GetNSString();

	[MRGSTracker trackEvent:CustomEvent];
}

void UPsMRGSProxyIOS::ShowSupport()
{
	UE_LOG(LogMRGS, Log, TEXT("%s"), *PS_FUNC_LINE);
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyIOS not initialized"), *PS_FUNC_LINE);
		return;
	}

	const UPsMRGSSettings* MRGSSettings = GetDefault<UPsMRGSSettings>();
	if (MRGSSettings == nullptr || MRGSSettings->IsValidLowLevel() == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSSettings not initialized or removed"), *PS_FUNC_LINE);
		return;
	}

	if (MRGSSettings->iOSSupportSecretKey.IsEmpty())
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: iOSSupportSecretKey is empty"), *PS_FUNC_LINE);
		return;
	}

	dispatch_async(dispatch_get_main_queue(), ^{
	  UIView* ViewContainer = UIApplication.sharedApplication.delegate.window.rootViewController.view;
	  MRGSMyComSupport* support = [MRGSMyComSupport sharedInstance];
	  [support showSupportViewOnSuperview:ViewContainer];
	});

	OnSupportOpened();
}

void UPsMRGSProxyIOS::CheckSupportTickets()
{
	UE_LOG(LogMRGS, Log, TEXT("%s"), *PS_FUNC_LINE);
	[[MRGSMyComSupport sharedInstance] checkTickets];
}

FString UPsMRGSProxyIOS::GetDevicePlatform() const
{
	MRGSDevice* Device = [[MRGSDevice alloc] init];
	FString Result = FString([NSString stringWithFormat:@"%@ %@", Device.systemName, [MRGSDevice systemVersion]]);
	return Result;
}

FString UPsMRGSProxyIOS::GetCountryCode() const
{
	return FString([MRGSDevice country]);
}

void UPsMRGSProxyIOS::RequestOpenUDID()
{
	OnReceivedOpenUDID(FString([MRGSDevice openUDID]));
}

void UPsMRGSProxyIOS::OpenApplicationPageInSystemSettings()
{
	[MRGSDevice openSystemSettingsOfApplication];
}

void UPsMRGSProxyIOS::EnableNotifications()
{
	[[MRGSNotificationCenter currentCenter] enableMRGSNotifications];
}

void UPsMRGSProxyIOS::DisableNotifications()
{
	[[MRGSNotificationCenter currentCenter] disableMRGSNotifications];
}

//////////////////////////////////////////////////////////////////////////
// Advertising

void UPsMRGSProxyIOS::LoadAdvertising()
{
	dispatch_async(dispatch_get_main_queue(), ^{
	  [Delegate loadAdvertisingContent];
	});
}

bool UPsMRGSProxyIOS::IsAdvertisingLoaded()
{
	return [Delegate isAdvertisingLoaded];
}

void UPsMRGSProxyIOS::ShowAdvertising()
{
	dispatch_async(dispatch_get_main_queue(), ^{
	  const bool bSuccess = [Delegate showAdvertising];
	  AsyncTask(ENamedThreads::GameThread, [this, bSuccess]() {
		  if (MRGSDelegate.IsBound())
		  {
			  MRGSDelegate.Broadcast(bSuccess ? EPsMRGSEventsTypes::MRGS_ADVERTISING_SHOW : EPsMRGSEventsTypes::MRGS_ADVERTISING_SHOW_ERROR);
		  }
	  });
	});
}

void UPsMRGSProxyIOS::OpenShowcase()
{
	dispatch_async(dispatch_get_main_queue(), ^{
	  [[MRGSShowcase sharedInstance] presentShowcaseFromViewController:UIApplication.sharedApplication.delegate.window.rootViewController
												 withCompletionHandler:^{
												   AsyncTask(ENamedThreads::GameThread, [this]() {
													   OnShowcaseShowFinished();
												   });
												 }];
	});
}

#endif //IOS
