// Nightfall Protocol 2024 Mortal Machines Studio. All Rights Reserved


#include "DeveloperSettings/ObscuraDeveloperSettings.h"

UObscuraDeveloperSettings::UObscuraDeveloperSettings()
{
	CategoryName = "Plugins";
}

TArray<TSoftClassPtr<UObject>> UObscuraDeveloperSettings::GetSupportedClasses() const
{
	return SupportedClasses;
}

TSoftObjectPtr<UEditorUtilityWidgetBlueprint> UObscuraDeveloperSettings::GetIconGeneratorWidgetClass() const
{
	return IconGeneratorWidgetClass;
}

bool UObscuraDeveloperSettings::IsActionForBlueprints() const
{
	return bIsActionForBlueprints;
}
