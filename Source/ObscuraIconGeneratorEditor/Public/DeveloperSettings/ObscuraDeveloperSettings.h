// Nightfall Protocol 2024 Mortal Machines Studio. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Actor/ObscuraItemActor.h"
#include "Engine/DeveloperSettings.h"
#include "Widget/ObscuraIconGeneratorEditorWidget.h"
#include "ObscuraDeveloperSettings.generated.h"

class UEditorUtilityWidgetBlueprint;
class UObscuraIconGeneratorEditorWidget;
/**
 * 
 */
UCLASS(Config=Plugins, Category="Plugins", defaultconfig, DisplayName="Obscura Developer Settings")
class OBSCURAICONGENERATOREDITOR_API UObscuraDeveloperSettings : public UDeveloperSettings {
	GENERATED_BODY()

	UObscuraDeveloperSettings();
	
protected:

	UPROPERTY(EditAnywhere, Config, Category="Obscura",
		meta=(ToolTip="The classes for which the 'Open Icon Generator' should appear in the context menu"))
	TArray<TSoftClassPtr<UObject>> SupportedClasses = {AObscuraItemActor::StaticClass() };

	UPROPERTY(EditAnywhere, Config, Category="Obscura", meta=(ToolTip="This class will be used to instantiate the Icon Generator"))
	TSoftObjectPtr<UEditorUtilityWidgetBlueprint> IconGeneratorWidgetClass;

	/**
	 * Is the action for Opening the Icon generator is supposed to work specifically on Blueprints (true) or on all assets (false).
	 * If true, SupportedClasses is treated as a filter against the Parent Class of selected Blueprint assets.
	 * Set this to false if you are working with DataAssets
	 */
	UPROPERTY(EditAnywhere, Config, Category="Obscura")
	bool bIsActionForBlueprints = true;

public:

	UFUNCTION(BlueprintPure, Category="Obscura")
	TArray<TSoftClassPtr<UObject>> GetSupportedClasses() const;

	UFUNCTION(BlueprintPure, Category="Obscura")
	TSoftObjectPtr<UEditorUtilityWidgetBlueprint> GetIconGeneratorWidgetClass() const;

	UFUNCTION(BlueprintPure, Category="Obscura")
	bool IsActionForBlueprints() const;
};
