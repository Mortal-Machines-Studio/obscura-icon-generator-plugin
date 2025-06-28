// Copyright Mortal Machines Studio All Rights Reserved.

#include "ObscuraIconGeneratorEditor.h"
#include "ToolMenu.h"
#include "ContentBrowserMenuContexts.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "DeveloperSettings/ObscuraDeveloperSettings.h"
#include "Kismet/KismetMathLibrary.h"

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
	// Getting the Right Click menu of the Assets in the Content Browser
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");

	FToolMenuSection& Section = Menu->AddSection(
		"ObscuraIconGenerator",
		FText::FromString("Obscura"),
		FToolMenuInsert("AssetContextExploreMenuOptions", EToolMenuInsertType::Before)
	);
	
	Menu->AddDynamicSection("ObscuraIconGenerator_Dynamic", FNewToolMenuDelegate::CreateLambda([](UToolMenu* InMenu){
		// Getting the Developer Settings here, because this way the user won't need to restart the editor for the SupportedClasses change to be valid
		if (const UObscuraDeveloperSettings* ObscuraDeveloperSettings = GetDefault<UObscuraDeveloperSettings>()) {
			TArray<TSoftClassPtr<UObject>> SupportedClasses = ObscuraDeveloperSettings->GetSupportedClasses();
			
			if (const UContentBrowserAssetContextMenuContext* Context = InMenu->FindContext<UContentBrowserAssetContextMenuContext>()) {
				// Getting the class of the selected asset
				if (UClass* SelectedAssetClass = Context->CommonClass) {
					if (!Context->SelectedAssets.IsValidIndex(0)) return;

					const FAssetData& SelectedAsset = Context->SelectedAssets[0];

					// If it's a Blueprint we need to extract the actual class
					if (Context->CommonClass == UBlueprint::StaticClass()) {
						if (UClass* FoundBlueprintClass = GetClassFromBlueprintAsset(SelectedAsset)) {
							SelectedAssetClass = FoundBlueprintClass;
						}
					}
					
					// Looping Through the SupportedClasses 
					for (const TSoftClassPtr<UObject>& SupportedClass : SupportedClasses) {
						// If the select asset's class is a child of the SupportedClass
						if (UKismetMathLibrary::ClassIsChildOf(SelectedAssetClass,SupportedClass.Get())) {

							// Menu Entry Action
							const FToolUIActionChoice OpenIconGeneratorAction(FExecuteAction::CreateLambda([SelectedAsset]() {
								// Opening the Icon Generator Widget
								OpenIconGenerator(SelectedAsset);
							}));

							// Adding the Menu Entry
							InMenu->AddMenuEntry("ObscuraIconGenerator", FToolMenuEntry::InitMenuEntry(
								FName("Obscura_OpenIconGenerator"),FText::FromString("Open Icon Generator"),
								FText::FromString("Opens the Icon Generator for this asset."),
								FSlateIcon(FAppStyle::Get().GetStyleSetName(), "Icons.PlusCircle"), OpenIconGeneratorAction)
							);
							// We break out of the loop
							break;
							
						}						
					}
				}
			}
		}

	}),
	FToolMenuInsert("AssetContextExploreMenuOptions", EToolMenuInsertType::Before));
}

void FObscuraIconGeneratorEditorModule::OpenIconGenerator(const FAssetData& SelectedAsset)
{
	if (const UObscuraDeveloperSettings* ObscuraDeveloperSettings = GetDefault<UObscuraDeveloperSettings>()) {
		UEditorUtilityWidgetBlueprint* IconGeneratorWidget = ObscuraDeveloperSettings->GetIconGeneratorWidgetClass();
		UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();

		// If either of these is invalid/nullptr we return
		if (!IconGeneratorWidget || !EditorUtilitySubsystem) return;
		
		// Making sure IconGenerator Widget is a subclass of UObscuraIconGeneratorEditorWidget
		if (IconGeneratorWidget && IconGeneratorWidget->GeneratedClass->IsChildOf(UObscuraIconGeneratorEditorWidget::StaticClass())) {
			// Creating IconGenerator widget 
			UObscuraIconGeneratorEditorWidget* CreatedIconGeneratorWidget = Cast<UObscuraIconGeneratorEditorWidget>(
				EditorUtilitySubsystem->SpawnAndRegisterTab(IconGeneratorWidget));
			
			if (!CreatedIconGeneratorWidget) return; // If not valid return

			// Checking if it's a blueprint
			if (const UBlueprint* BlueprintAsset = Cast<UBlueprint>(SelectedAsset.GetAsset())) {
				// Checking if it's an actor blueprint by getting the GeneratedClass's Default Object
				if (const AActor* DefaultActor = Cast<AActor>(BlueprintAsset->GeneratedClass.GetDefaultObject())) {
					CreatedIconGeneratorWidget->SetActor(DefaultActor->GetClass());
				}						
			} else {
				// Fallback for any other type of assets. The developer should take care of implementation
				CreatedIconGeneratorWidget->OnOpenAsset(SelectedAsset);
			}
			
		}
	}
}

UClass* FObscuraIconGeneratorEditorModule::GetClassFromBlueprintAsset(const FAssetData& Asset)
{
	if (const UBlueprint* BlueprintAsset = Cast<UBlueprint>(Asset.GetAsset())) {
		if (const AActor* DefaultActor = Cast<AActor>( BlueprintAsset->GeneratedClass.GetDefaultObject())) {
			return DefaultActor->GetClass();
		}
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FObscuraIconGeneratorEditorModule, ObscuraIconGeneratorEditor)