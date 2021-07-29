// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Runtime/Launch/Resources/Version.h"

#if ENGINE_MINOR_VERSION >= 15
#include "CoreMinimal.h"
#else
#include "CoreUObject.h"
#include "Engine.h"
#endif

DECLARE_LOG_CATEGORY_EXTERN(LogMRGS, Log, All);

#define PS_FUNC (FString(__FUNCTION__))              // Current Class Name + Function Name where this is called
#define PS_LINE (FString::FromInt(__LINE__))         // Current Line Number in the code where this is called
#define PS_FUNC_LINE (PS_FUNC + "(" + PS_LINE + ")") // Current Class and Line Number where this is called!
