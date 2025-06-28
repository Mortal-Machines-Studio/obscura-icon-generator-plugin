// Copyright Mortal Machines Studio All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FObscuraIconGeneratorEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RegisterMenus();

	static void OpenIconGenerator(const FAssetData& SelectedAsset);
};
