// Copyright 2016 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "MRGServicePrivatePCH.h"

#if WITH_MRGS

#if PLATFORM_IOS
#include "IOSAppDelegate.h"
//#include "MRGS.h"
//#import <MRGService/MRGService.h>

@interface MRGSBank_ios : NSObject <MRGSBankDelegate, SKStoreProductViewControllerDelegate>

+ (instancetype)sharedInstance;

- (void)loadProductsFromAppleServer:(NSMutableArray *)productIdentifiers;
- (void)addPayment:(NSString *)paymentIdentifier;
- (void)restorePurchased;
- (void)closePaymentWithTransactionId:(NSString *) transactionId andProductIdentifier:(NSString *) productIdentifier;

- (void)navigateToUrl:(NSString* )UrlToGo;

@end
#endif

#endif // WITH_MRGS
