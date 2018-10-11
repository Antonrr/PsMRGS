// Copyright 2018 Pushkin Studio. All Rights Reserved.

#include "PsMRGSLibrary.h"
#include "PsMRGS.h"

UPsMRGSLibrary::UPsMRGSLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

UPsMRGSProxy* UPsMRGSLibrary::GetMRGSProxy()
{
	return IPsMRGS::Get().MRGSProxy;
}
