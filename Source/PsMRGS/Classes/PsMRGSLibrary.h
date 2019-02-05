// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsMRGSLibrary.generated.h"

UCLASS()
class PSMRGS_API UPsMRGSLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Get MRGS proxy object */
	UFUNCTION(BlueprintCallable, Category = MRGS, BlueprintPure)
	static UPsMRGSProxy* GetMRGSProxy();
};
