// Copyright 2016 Mail.Ru Group. All Rights Reserved.

#include "MRGServicePrivatePCH.h"

#if WITH_MRGS

#if PLATFORM_IOS

using namespace std;
using namespace mrgs;

@implementation MRGSBank_ios

+ (instancetype)sharedInstance {
    static MRGSBank_ios* sharedInstance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[MRGSBank_ios alloc] init];
        [MRGSBank sharedInstance].delegate = sharedInstance;
    });
    return sharedInstance;
}

- (void)loadProductsFromAppleServer:(NSMutableArray *)productIdentifiers {
	NSArray *array = [NSArray arrayWithArray:productIdentifiers];
	[[MRGSBank sharedInstance] loadProductsWithTypesFromAppleServer:array];
}

- (void)addPayment:(NSString *)paymentIdentifier {
    [[MRGSBank sharedInstance] addPayment:paymentIdentifier];
}

- (void)restorePurchased {
    [[MRGSBank sharedInstance] restorePurchase];
}

- (void) closePaymentWithTransactionId:(NSString *) transactionId andProductIdentifier:(NSString *) productIdentifier {
    [[MRGSBank sharedInstance] closePaymentWithTransactionId:transactionId andProductIdentifier:productIdentifier];
}

/** MRGSBankDelegate */

- (void)loadingPaymentsResponce:(SKProductsResponse *)response {
    if ([response.invalidProductIdentifiers count] > 0) {
        mrgs::MRGSLOG("MRGSBank_Ios::response.invalidProductIdentifiers = %s", [[response.invalidProductIdentifiers description] UTF8String]);
	}

    vector<MRGSPurchaseItem> items;
    for(SKProduct *product in response.products) {
		if (product == nil)
		{
			mrgs::MRGSLOG("MRGSBank_Ios::response.product invalid");
			continue;
		}
		
		if (product.productIdentifier == nil)
		{
			mrgs::MRGSLOG("MRGSBank_Ios::response.product.productIdentifier invalid");
			continue;
		}
		
		if ([self getCurrencyCode:product] == nil)
		{
			mrgs::MRGSLOG("MRGSBank_Ios::response getCurrencyCode invalid for %s", [product.productIdentifier UTF8String]);
			continue;
		}
		
		if (product.localizedTitle == nil)
		{
			mrgs::MRGSLOG("MRGSBank_Ios::response.product.localizedTitle invalid for %s", [product.productIdentifier UTF8String]);
			continue;
		}
		
		if (product.localizedDescription == nil)
		{
			mrgs::MRGSLOG("MRGSBank_Ios::response.product.localizedDescription invalid for %s", [product.productIdentifier UTF8String]);
			continue;
		}
		
        MRGSPurchaseItem item;
        item.sku = [product.productIdentifier UTF8String];
        item.price = [[NSString stringWithFormat:@"%.02f %@", [product.price doubleValue], [self getCurrencyCode:product]] UTF8String];
        item.title = [product.localizedTitle UTF8String];
        item.type = "inapp";
        item.description = [product.localizedDescription UTF8String];
        items.push_back(item);
    }
	
	if (items.empty())
	{
		mrgs::MRGSLOG("MRGSBank_Ios::response.items empty");
	}
	
    mrgs::MRGSBilling::PurchaseItems = items;

    mrgs::MRGSBilling::delegate->loadProductsDidFinished(items);
}

- (void)paymentSuccessful:(SKPaymentTransaction *)transaction answer:(NSString *)answer {
    NSString *paymentId = transaction.payment.productIdentifier;
    MRGSBilling::delegate->purchaseComplete([paymentId UTF8String],
                                            [transaction.transactionIdentifier UTF8String],
                                            [answer UTF8String]);
}

- (void)paymentFailed:(SKPaymentTransaction *)transaction error:(NSError *)error {
    NSString *paymentId = transaction.payment.productIdentifier;
    MRGSBilling::delegate->purchaseFail([paymentId UTF8String],
                                        [[error description] UTF8String]);
}

- (NSString *)getCurrencyCode:(SKProduct *)product {
	NSLocale *priceLocale = [product priceLocale];
	NSNumberFormatter *currencyFormatter = [[[NSNumberFormatter alloc] init] autorelease];
	[currencyFormatter setLocale:priceLocale];
	return [currencyFormatter internationalCurrencySymbol];
}

- (void)productViewControllerDidFinish:(SKStoreProductViewController *)viewController
{
	if (viewController) {
//		UIViewController* MainViewController = [IOSAppDelegate GetDelegate].IOSController;
//		if (MainViewController) {
			[[IOSAppDelegate GetDelegate].IOSController dismissViewControllerAnimated:YES completion:nil];
//		}
	}
}

- (void)navigateToUrl:(NSString* )UrlToGo {
	if ([SKStoreProductViewController class] != nil) {
		SKStoreProductViewController* skpvc = [[SKStoreProductViewController alloc] init];
		skpvc.delegate = self;
		NSDictionary* dict = [NSDictionary dictionaryWithObject: UrlToGo forKey: SKStoreProductParameterITunesItemIdentifier];
		[skpvc loadProductWithParameters: dict completionBlock: nil];

//		UIViewController* MainViewController = [IOSAppDelegate GetDelegate].IOSController;
		[[IOSAppDelegate GetDelegate].IOSController presentViewController:skpvc animated:YES completion:nil];
	}
	else {
		NSString* const iOS7AppStoreURLFormat = @"itms-apps://itunes.apple.com/app/id%@";
		NSString* const iOSAppStoreURLFormat = @"itms-apps://itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?type=Purple+Software&id=%@";
		NSString* url = [[NSString alloc] initWithFormat: ([[UIDevice currentDevice].systemVersion floatValue] >= 7.0f) ? iOS7AppStoreURLFormat : iOSAppStoreURLFormat, UrlToGo];
		[[UIApplication sharedApplication] openURL: [NSURL URLWithString: url]];
	}
}

@end
#endif

#endif // WITH_MRGS
