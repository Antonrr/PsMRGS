// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "PsMRGSLibrary.h"

#include "PsMRGS.h"

class UPsMRGSProxy;

UPsMRGSLibrary::UPsMRGSLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UPsMRGSProxy* UPsMRGSLibrary::GetMRGSProxy()
{
	return IPsMRGS::Get().MRGSProxy;
}
