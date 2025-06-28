// Nightfall Protocol 2024 Mortal Machines Studio. All Rights Reserved


#include "DeveloperSettings/ObscuraDeveloperSettings.h"

#include "EditorUtilityWidgetBlueprint.h"

UObscuraDeveloperSettings::UObscuraDeveloperSettings()
{
	CategoryName = "Plugins";
}

TArray<TSoftClassPtr<UObject>> UObscuraDeveloperSettings::GetSupportedClasses() const
{
	return SupportedClasses;
}

UEditorUtilityWidgetBlueprint* UObscuraDeveloperSettings::GetIconGeneratorWidgetClass() const
{
	return IconGeneratorWidgetClass.LoadSynchronous();
}
