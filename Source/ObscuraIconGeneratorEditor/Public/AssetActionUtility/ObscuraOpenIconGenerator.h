// Nightfall Protocol 2024 Mortal Machines Studio. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "ObscuraOpenIconGenerator.generated.h"

class UObscuraIconGeneratorEditorWidget;
/**
 * 
 */
UCLASS(Blueprintable, DisplayName="Open Icon Generator Asset Action")
class OBSCURAICONGENERATOREDITOR_API UObscuraOpenIconGenerator : public UAssetActionUtility {
	GENERATED_BODY()

	UObscuraOpenIconGenerator();

protected:

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widget")
	TSubclassOf<UObscuraIconGeneratorEditorWidget> IconGeneratorWidgetClass;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widget",
		meta=(ToolTip="This has to be a child class of UObscuraIconGeneratorEditorWidget, otherwise the utility won't run"))
	UEditorUtilityWidgetBlueprint* IconGeneratorWidget = nullptr;
	
public:

	UFUNCTION(BlueprintNativeEvent, CallInEditor, meta=(ForceAsFunction))
	void OpenIconGenerator();

	
};
