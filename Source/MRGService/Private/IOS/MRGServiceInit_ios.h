// Copyright 2016 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "MRGServicePrivatePCH.h"

#if WITH_MRGS

#if PLATFORM_IOS
//#include "MRGS.h"
//#import <MRGService/MRGService.h>

@interface MRGServiceInit_ios : NSObject <MRGSServerDataDelegate, MRGSMyComSupportDelegate, MRGSMyTargetDelegate>

+ (instancetype)sharedInstance;

@end
#endif

#endif // WITH_MRGS
