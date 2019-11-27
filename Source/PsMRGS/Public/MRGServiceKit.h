//
//  MRGServiceKit.h
//  MRGServiceFramework
//
//  This is a global header. It imports all MRGS kits which are linked to the project
//  Copyright © 2019 Mail.Ru Games. All rights reserved.
//

#import <MRGService/MRGService.h>

#if !defined(__has_include)
#error "MRGServiceKit.h won't import anything if your compiler doesn't support __has_include. Please \
    import the headers individually."
#else

#if __has_include(<MRGSAdvertising/MRGSAdvertising.h>)
#import <MRGSAdvertising/MRGSAdvertising.h>
#endif

#if __has_include(<MRGSBank/MRGSBank.h>)
#import <MRGSBank/MRGSBank.h>
#endif

#if __has_include(<MRGSGDPR/MRGSGDPR.h>)
#import <MRGSGDPR/MRGSGDPR.h>
#endif

#if __has_include(<MRGSNotifications/MRGSNotifications.h>)
#import <MRGSNotifications/MRGSNotifications.h>
#endif

#if __has_include(<MRGSSupport/MRGSSupport.h>)
#import <MRGSSupport/MRGSSupport.h>
#endif

#if __has_include(<MRGSAnalytics/MRGSAnalytics.h>)
#import <MRGSAnalytics/MRGSAnalytics.h>
#endif

#if __has_include(<MRGSGameCenter/MRGSGameCenter.h>)
#import <MRGSGameCenter/MRGSGameCenter.h>
#endif

#if __has_include(<MRGSMyTarget/MRGSMyTarget.h>)
#import <MRGSMyTarget/MRGSMyTarget.h>
#endif

#if __has_include(<MRGSFirebase/MRGSFirebase.h>)
#import <MRGSFirebase/MRGSFirebase.h>
#endif

#endif
