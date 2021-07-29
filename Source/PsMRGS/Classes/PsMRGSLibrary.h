// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsMRGSLibrary.generated.h"

class UPsMRGSProxy;

UCLASS()
class PSMRGS_API UPsMRGSLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/** Get MRGS proxy object */
	UFUNCTION(BlueprintCallable, Category = MRGS, BlueprintPure)
	static UPsMRGSProxy* GetMRGSProxy();
};
