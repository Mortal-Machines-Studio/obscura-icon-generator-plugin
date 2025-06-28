// Copyright Mortal Machines Studio All Rights Reserved.

#include "ObscuraIconGeneratorEditor.h"
#include "ToolMenu.h"
#include "ContentBrowserMenuContexts.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "AssetRegistry/AssetRegistryModule.h"
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
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");

	FToolMenuSection& Section = Menu->AddSection(
		"ObscuraIconGenerator",
		FText::FromString("Obscura"),
		FToolMenuInsert("AssetContextExploreMenuOptions", EToolMenuInsertType::Before)
	);

	/*if (const UContentBrowserAssetContextMenuContext* Context = Menu->FindContext<UContentBrowserAssetContextMenuContext>()) {
			if (const UClass* SelectedAssetClass = Context->CommonClass) {
				if (SupportedClasses.Find(SelectedAssetClass) != INDEX_NONE) {

					UE_LOG(LogTemp, Warning, TEXT("Supported class: %s"), *GetNameSafe(SelectedAssetClass));
					
					const FToolUIActionChoice OpenIconGeneratorAction(FExecuteAction::CreateLambda([Context]() {
													
						UE_LOG(LogTemp, Warning, TEXT("Open Icon Generator"));
						
					}));
					
					Menu->AddMenuEntry("ObscuraIconGenerator", FToolMenuEntry::InitMenuEntry(
							FName("Obscura_OpenIconGenerator"),FText::FromString("Open Icon Generator"),
							FText::FromString("Opens the Icon Generator for this asset."),
							FSlateIcon(FAppStyle::Get().GetStyleSetName(), "Icons.PlusCircle"), OpenIconGeneratorAction));
				}
			}
		}*/
		
	Menu->AddDynamicSection("ObscuraIconGenerator_Dynamic", FNewToolMenuDelegate::CreateLambda([](UToolMenu* InMenu){
		// Getting the Developer Settings here, because this way the user won't need to restart the editor for the SupportedClasses change to be valid
		if (const UObscuraDeveloperSettings* ObscuraDeveloperSettings = GetDefault<UObscuraDeveloperSettings>()) {
			TArray<TSoftClassPtr<UObject>> SupportedClasses = ObscuraDeveloperSettings->GetSupportedClasses();
			
			if (const UContentBrowserAssetContextMenuContext* Context = InMenu->FindContext<UContentBrowserAssetContextMenuContext>()) {
				// Getting the class of the selected asset
				if (UClass* SelectedAssetClass = Context->CommonClass) {
					
					// Looping Through the SupportedClasses 
					for (const TSoftClassPtr<UObject>& SupportedClass : SupportedClasses) {
						// If the select asset's class is a child of the SupportedClass
						if (UKismetMathLibrary::ClassIsChildOf(SelectedAssetClass,SupportedClass.Get())) {

							// Menu Entry Action
							const FToolUIActionChoice OpenIconGeneratorAction(FExecuteAction::CreateLambda([Context]() {
								// Check if the First selected asset is valid;
								if (Context->SelectedAssets.IsValidIndex(0)) {
									// If it is open the Icon Generator
									OpenIconGenerator(Context->SelectedAssets[0]);
								}
								
								//UE_LOG(LogTemp, Warning, TEXT("Debug: %s"), Context->GetSelectedItems());
								
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
					/*TSoftClassPtr<UObject> FoundClass = SupportedClasses.FindByPredicate(
						[SelectedAssetClass](const TSoftClassPtr<UObject>& SupportedClass) {
								return SelectedAssetClass->StaticClass()->IsInA(SupportedClass.Get());
							}
					);*/
					
				}
			}
		}
		
		
		
		// Maybe try to get asset type from context?
	}),
	FToolMenuInsert("AssetContextExploreMenuOptions", EToolMenuInsertType::Before));
	
	/*FToolMenuSection& Section = Menu->AddSection(
		"ObscuraIconGenerator",
		FText::FromString("Obscura"),
		FToolMenuInsert("AssetContextExploreMenuOptions", EToolMenuInsertType::Before)
	);*/

	

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

void FObscuraIconGeneratorEditorModule::OpenIconGenerator(const FAssetData& SelectedAsset)
{
	if (const UObscuraDeveloperSettings* ObscuraDeveloperSettings = GetDefault<UObscuraDeveloperSettings>()) {
		UEditorUtilityWidgetBlueprint* IconGeneratorWidget = ObscuraDeveloperSettings->GetIconGeneratorWidgetClass();
		UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
		
		if (!IconGeneratorWidget || !EditorUtilitySubsystem) return;
		
		// Making sure IconGenerator Widget is a subclass of UObscuraIconGeneratorEditorWidget
		if (IconGeneratorWidget && IconGeneratorWidget->GeneratedClass->IsChildOf(UObscuraIconGeneratorEditorWidget::StaticClass())) {
			// Creating IconGenerator widget 
			UObscuraIconGeneratorEditorWidget* CreatedIconGeneratorWidget = Cast<UObscuraIconGeneratorEditorWidget>(
				EditorUtilitySubsystem->SpawnAndRegisterTab(IconGeneratorWidget));
			
			if (!CreatedIconGeneratorWidget) return; // If for some reason it's not valid, return.

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

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FObscuraIconGeneratorEditorModule, ObscuraIconGeneratorEditor)