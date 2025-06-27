// Copyright Mortal Machines Studio All Rights Reserved.

#include "ObscuraIconGeneratorEditor.h"
#include "ToolMenu.h"
#include "ContentBrowserMenuContexts.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "DeveloperSettings/ObscuraDeveloperSettings.h"

#define LOCTEXT_NAMESPACE "FObscuraIconGeneratorEditorModule"

void FObscuraIconGeneratorEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	UToolMenus::RegisterStartupCallback(
	FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FObscuraIconGeneratorEditorModule::RegisterMenus));
}

void FObscuraIconGeneratorEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FObscuraIconGeneratorEditorModule::RegisterMenus()
{
	if (const UObscuraDeveloperSettings* ObscuraDeveloperSettings = GetDefault<UObscuraDeveloperSettings>()) {
		TArray<TSoftClassPtr<UObject>> SupportedClasses = ObscuraDeveloperSettings->GetSupportedClasses();

		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");
		//TODO: Figure out how to make this conditional 
		
		//Menu->Context
		Menu->AddDynamicSection("ObscuraIconGenerator", FNewToolMenuDelegate::CreateLambda([](UToolMenu* InMenu){
			// Maybe try to get asset type from context?
		}),
		FToolMenuInsert("AssetContextExploreMenuOptions", EToolMenuInsertType::Before));
		
		FToolMenuSection& Section = Menu->AddSection(
			"ObscuraIconGenerator",
			FText::FromString("Obscura"),
			FToolMenuInsert("AssetContextExploreMenuOptions", EToolMenuInsertType::Before)
		);

		Section.AddMenuEntry(
			"OpenIconGenerator",
			LOCTEXT("OpenIconGenerator", "Open Icon Generator"),
			LOCTEXT("OpenIconGeneratorTooltip", "Opens the Icon Generator for this Asset"),
			FSlateIcon(FAppStyle::Get().GetStyleSetName(), "Icons.PlusCircle"),
			FUIAction()
		);
	
		UContentBrowserAssetContextMenuContext* Context = Section.FindContext<UContentBrowserAssetContextMenuContext>();
		
	
		/*FToolMenuEntry& Entry = Section.AddDynamicEntry("AssetManagerEditorViewCommands",
			FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection){
				UContentBrowserAssetContextMenuContext* Context = InSection.FindContext<UContentBrowserAssetContextMenuContext>();
				if (Context) {
					FToolUIActionChoice LockAction(FExecuteAction::CreateLambda([Context]() {
						FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	                
						for(const FAssetData& SelectedAsset : Context->SelectedAssets)
						{
						
						}
				}));

				InSection.AddEntry(FToolMenuEntry::InitMenuEntry(FName("Lock"), FText::FromString("Lock Export"),
					FText::FromString("Lock this asset. Can't export if locked."), FSlateIcon(NAME_None, "MyIconName"), LockAction));
			}
		}));*/
	}
	
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FObscuraIconGeneratorEditorModule, ObscuraIconGeneratorEditor)