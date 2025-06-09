// Nightfall Protocol 2024 Mortal Machines Studio. All Rights Reserved


#include "AssetActionUtility/ObscuraOpenIconGenerator.h"

#include "AssetSelection.h"
#include "EditorUtilitySubsystem.h"
#include "Widget/ObscuraIconGeneratorEditorWidget.h"

UObscuraOpenIconGenerator::UObscuraOpenIconGenerator()
{
	bIsActionForBlueprints = true;

}


void UObscuraOpenIconGenerator::OpenIconGenerator_Implementation()
{
	// If widget is not set we return
	if (!IconGeneratorWidget) return;

	// Making sure IconGenerator Widget is a subclass of UObscuraIconGeneratorEditorWidget
	if (IconGeneratorWidget->GeneratedClass && IconGeneratorWidget->GeneratedClass->IsChildOf(UObscuraIconGeneratorEditorWidget::StaticClass())) {
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
						CreatedIconGeneratorWidget->SetActor(SelectedAsset.GetClass());
					}
				}
			} 
		}
	} else {
		UE_LOG(LogTemp, Error, TEXT("IconGeneratorWidget is not a child of UObscuraIconGeneratorEditorWidget in: %s"),
			*GetNameSafe(this));	
	}
	
}


