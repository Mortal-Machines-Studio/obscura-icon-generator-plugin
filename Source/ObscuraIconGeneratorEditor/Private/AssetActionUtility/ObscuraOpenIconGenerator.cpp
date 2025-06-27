// Nightfall Protocol 2024 Mortal Machines Studio. All Rights Reserved


#include "AssetActionUtility/ObscuraOpenIconGenerator.h"

#include "AssetSelection.h"
#include "EditorUtilitySubsystem.h"
#include "DeveloperSettings/ObscuraDeveloperSettings.h"
#include "Widget/ObscuraIconGeneratorEditorWidget.h"

UObscuraOpenIconGenerator::UObscuraOpenIconGenerator()
{
	if (const UObscuraDeveloperSettings* ObscuraDeveloperSettings = GetDefault<UObscuraDeveloperSettings>()) {
		SupportedClasses = ObscuraDeveloperSettings->GetSupportedClasses();
		bIsActionForBlueprints = ObscuraDeveloperSettings->IsActionForBlueprints();
	}
}

void UObscuraOpenIconGenerator::OpenIconGenerator_Implementation()
{
	if (const UObscuraDeveloperSettings* ObscuraDeveloperSettings = GetDefault<UObscuraDeveloperSettings>()) {
		IconGeneratorWidget = ObscuraDeveloperSettings->GetIconGeneratorWidgetClass();
		
		if (!IconGeneratorWidget) return;
		
		// Making sure IconGenerator Widget is a subclass of UObscuraIconGeneratorEditorWidget
		if (IconGeneratorWidget && IconGeneratorWidget->GeneratedClass->IsChildOf(UObscuraIconGeneratorEditorWidget::StaticClass())) {
			if (UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>()) {
				TArray<FAssetData> SelectedAssets;
				AssetSelectionUtils::GetSelectedAssets(SelectedAssets);

				for (const FAssetData& SelectedAsset : SelectedAssets) {
					if(UObscuraIconGeneratorEditorWidget* CreatedIconGeneratorWidget = Cast<UObscuraIconGeneratorEditorWidget>(
						EditorUtilitySubsystem->SpawnAndRegisterTab(IconGeneratorWidget))
					) {
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
		} else {
			UE_LOG(LogTemp, Error, TEXT("IconGeneratorWidget is not a child of UObscuraIconGeneratorEditorWidget in: %s"),
				*GetNameSafe(this));	
		}
	}
}


