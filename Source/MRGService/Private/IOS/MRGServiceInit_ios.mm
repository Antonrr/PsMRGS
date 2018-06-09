// Copyright 2016 Mail.Ru Group. All Rights Reserved.

#include "MRGServicePrivatePCH.h"

#if WITH_MRGS

#if PLATFORM_IOS

using namespace std;
using namespace mrgs;

@implementation MRGServiceInit_ios

+ (instancetype)sharedInstance {
    static MRGServiceInit_ios * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[MRGServiceInit_ios alloc] init];
    });
    return sharedInstance;
}

/** MRGSServerDataDelegate */

- (void)loadServerDataDidFinished:(NSDictionary *)serverData {
//    NSError *error;
//    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:serverData
//                                                       options:NSJSONWritingPrettyPrinted // Pass 0 if you don't care about the readability of the generated string
//                                                         error:&error];
//    NSString *jsonString = nil;
//    if (!error) {
//        jsonString = [[[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding] autorelease];
//    }
//    if(jsonString != nil)
//        MRGService::delegate->loadServerDataDidFinished([jsonString UTF8String]);
//    else {
//        NSLog(@"Какая-то ошибочка в мргсе loadServerDataDidFinished nil");
//    }
}

- (void)initializationFinish {
    NSLog(@"MRGSServerDataDelegate::MRGS init complete");
    if(MRGService::initCompleteCallback != nullptr)
    {
        MRGService::initCompleteCallback();
    }
}

- (void)loadPromoBannersDidFinished:(NSDictionary *)promoBanners {
	NSError *error;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:promoBanners
                                                       options:NSJSONWritingPrettyPrinted // Pass 0 if you don't care about the readability of the generated string
                                                         error:&error];
    NSString *jsonString = nil;
    if (!error) {
        jsonString = [[[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding] autorelease];
    }
    
    MRGService::delegate->loadPromoBannersDidFinished([jsonString UTF8String]);
}

/** MRGSMyComSupport delegate */

- (void)myComSupportViewDidClosed {
    NSLog(@"%@", NSStringFromSelector(_cmd));
    MRGService::supportDidClosedCallback();
}

- (void)myComSupportDidReceiveError:(NSError *)error {
    NSLog(@"%@ %@", NSStringFromSelector(_cmd), error);
    MRGService::supportDidRecieveError();
}

- (void)myComSupportDidReceiveNotificationsForTickets {
    NSLog(@"%@", NSStringFromSelector(_cmd));
}

- (void)myComSupportCheckTicketsFailWithError:(NSError *)error {
    NSLog(@"%@ %@", NSStringFromSelector(_cmd), error);
}

/** MyTarget delegate */

- (void)mrgsMyTarget:(MRGSMyTarget *)mrgsMyTarget didReceiveShowcaseDataAndFoundNotifications:(BOOL)hasNotifications withAppWallAd:(NSArray*)ad {
    NSLog(@"MRGSMyTargetDelegate::didReceiveShowcaseDataAndFoundNotifications");
    NSLog(@"%@ %@", NSStringFromSelector(_cmd), hasNotifications ? @"YES" : @"NO");
    mrgsMyTarget.showcaseTitle = @"Еще игры";
    mrgsMyTarget.showcaseCloseButtonTitle = @"Закрыть";
    [mrgsMyTarget openShowcaseWithViewController:[UIApplication sharedApplication].keyWindow.rootViewController
                                      onComplete:^{
                                          NSLog(@"showcase close");
                                          [mrgsMyTarget releaseShowcase];
                                      } onError:^(NSError *error) {
                                          NSLog(@"showcase error: %@", error);
                                      }];
}

- (void)mrgsMyTarget:(MRGSMyTarget *)mrgsAdman didNotReceiveShowcaseDataWithReason:(NSString *)reason {
    NSLog(@"MRGSMyTargetDelegate::didNotReceiveShowcaseDataWithReason");
    NSLog(@"%@ %@", NSStringFromSelector(_cmd), reason);
//    [self showAlertViewWithTitle:@"Open MyTarget showcase"
//                      andMessage:reason];
}

- (void)mrgsMyTargetShowcaseHasNoAds:(MRGSMyTarget *)mrgsMyTarget {
    NSLog(@"MRGSMyTargetDelegate::mrgsMyTargetShowcaseHasNoAds");
    NSLog(@"%@", NSStringFromSelector(_cmd));
}

- (void)mrgsMyTargetDidReceiveFullscreenBannerData:(MRGSMyTarget *)mrgsMyTarget {
    NSLog(@"MRGSMyTargetDelegate::mrgsMyTargetDidReceiveFullscreenBannerData");
    [mrgsMyTarget openFullscreenBannerInViewController:[UIApplication sharedApplication].keyWindow.rootViewController];
}

- (void)mrgsMyTarget:(MRGSMyTarget *)mrgsMyTarget didNotReceiveFullscreenBannerDataWithReason:(NSString *)reason {
    NSLog(@"MRGSMyTargetDelegate::didNotReceiveFullscreenBannerDataWithReason");
    NSLog(@"%@ %@", NSStringFromSelector(_cmd), reason);
//    [self showAlertViewWithTitle:@"Open MyTarget banner"
//                      andMessage:reason];
}
- (void)mrgsMyTargetFullscreenBannerClosed:(MRGSMyTarget *)mrgsMyTarget {
    NSLog(@"MRGSMyTargetDelegate::mrgsMyTargetFullscreenBannerClosed");
    NSLog(@"%@", NSStringFromSelector(_cmd));
    [mrgsMyTarget releaseFullscreenBanner];
    MRGService::fullscreenDidClosed();
}

- (void)showAlertViewWithTitle:(NSString *)title andMessage:(NSString *)message {
    NSLog(@"MRGSMyTargetDelegate::showAlertViewWithTitle");
    [[[UIAlertView alloc] initWithTitle:title
                                message:message
                               delegate:nil
                      cancelButtonTitle:@"Ok"
                      otherButtonTitles:nil] show];
}

- (void)mrgsMyTargetDidReceiveInterstitialSliderAdData:(MRGSMyTarget *)mrgsMyTarget {
	NSLog(@"MRGSMyTargetDelegate::mrgsMyTargetDidReceiveInterstitialSliderAdData");
	[mrgsMyTarget openInterstitialSliderAdWithViewController:[UIApplication sharedApplication].keyWindow.rootViewController];
}


@end
#endif

#endif // WITH_MRGS
