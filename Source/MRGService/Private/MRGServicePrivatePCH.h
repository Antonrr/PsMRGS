// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Runtime/Launch/Resources/Version.h"

#if ENGINE_MINOR_VERSION >= 15
#include "CoreMinimal.h"
#else
#include "CoreUObject.h"
#include "Engine.h"
#include "Core.h"
#endif

// You should place include statements to your module's private header files here.  You only need to
// add includes for headers that are used in most of your module's source files though.
//
#if PLATFORM_IOS
#if WITH_MRGS
    #import <MRGService/MRGService.h>
#endif
    #include <iostream>
    #include "MRGS.h"
    #include "IOS/MRGSBank_ios.h"
    #include "IOS/MRGServiceInit_ios.h"
	#import	 <Security/Security.h>
	#import  <GameKit/GKLocalPlayer.h>
	#import	 <StoreKit/StoreKit.h>
	#include "IOSAppDelegate.h"
#endif

#if PLATFORM_ANDROID
    #include <iostream>
    #include "MRGS.h"
    #include "Runtime/Launch/Public/Android/AndroidJNI.h"
#endif

#include "ModuleManager.h"
#include "MRGService.h"
#include "MRGServiceClasses.h"
