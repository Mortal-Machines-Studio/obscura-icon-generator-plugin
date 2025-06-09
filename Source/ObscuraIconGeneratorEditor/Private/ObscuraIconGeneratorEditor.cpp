// Copyright Mortal Machines Studio All Rights Reserved.

#include "ObscuraIconGeneratorEditor.h"
#include "ObscuraIconGeneratorEditor/Public/ObscuraIconGeneratorEditor.h"

#define LOCTEXT_NAMESPACE "FObscuraIconGeneratorEditorModule"

void FObscuraIconGeneratorEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FObscuraIconGeneratorEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FObscuraIconGeneratorEditorModule, ObscuraIconGeneratorEditor)