// Copyright Epic Games, Inc. All Rights Reserved.

#include "RpgDialogSystem.h"
#include "HAL/IConsoleManager.h"

static TAutoConsoleVariable<int32> CVarAllowSkipCinematics(
	TEXT("RpgDialog.AlwaysAllowSkipCinematics"),
	0,
	TEXT("Developer override to skip all cinematics"),
	ECVF_Cheat
);

#define LOCTEXT_NAMESPACE "FRpgDialogSystemModule"

void FRpgDialogSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FRpgDialogSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRpgDialogSystemModule, RpgDialogSystem)