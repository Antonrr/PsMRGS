// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

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
