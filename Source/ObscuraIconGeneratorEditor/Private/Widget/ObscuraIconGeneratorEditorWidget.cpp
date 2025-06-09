// Nightfall Protocol 2024 Mortal Machines Studio. All Rights Reserved

#include "Widget/ObscuraIconGeneratorEditorWidget.h"

#include "EditorAssetLibrary.h"
#include "Actor/ObscuraItemSceneCaptureActor.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Editor/ScriptableEditorWidgets/Public/Components/DetailsView.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Interfaces/IPluginManager.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "ObscuraIconGenerator"

void UObscuraIconGeneratorEditorWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	

	if (IconGeneratorDetailsView.Get())	IconGeneratorDetailsView.Get()->SetObject(this);

	
}

void UObscuraIconGeneratorEditorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CreateMaterial();
	CreateViewport();
	CreateScreenCaptureActor();
	SetActor(ActorClass);
}

FReply UObscuraIconGeneratorEditorWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (RenderTargetImage) {
		if (IsMouseAboveWidget(RenderTargetImage, this)) {
			bCanTransformActor = true;
			SetCursor(EMouseCursor::Type::GrabHandClosed);
			return FReply::Handled();
		}
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UObscuraIconGeneratorEditorWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bCanTransformActor = false;
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UObscuraIconGeneratorEditorWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bCanTransformActor) {
		if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)) {
			UpdateActorLocation(InMouseEvent.GetCursorDelta());
		} else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton)) {
			UpdateActorRotation(InMouseEvent.GetCursorDelta());
		}
	}

	if (RenderTargetImage->IsHovered()) {
		if (GetCursor() != EMouseCursor::Type::GrabHand || GetCursor() != EMouseCursor::Type::GrabHandClosed)
		SetCursor(EMouseCursor::Type::GrabHand);
	} else {
		SetCursor(EMouseCursor::Type::Default);
	}
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UObscuraIconGeneratorEditorWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (RenderTargetImage) {
		if (IsMouseAboveWidget(RenderTargetImage, this)) {
			UpdateActorZoom(InMouseEvent.GetWheelDelta());
			return FReply::Handled();
		}
	}	
	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}


void UObscuraIconGeneratorEditorWidget::OnActorSet_Implementation(AActor* NewActor)
{
	if (NewActor) {
		/*
		 * There is a default light in the viewport that messes with the SceneCaptureActor light setup
		 * Setting the Intensity to 0 prevents it from influencing the Scene Capture
		 */
		if (Viewport) {
			Viewport->SetLightIntensity(0.0f);
		}
		
		UpdateRenderTarget();		
	}
}

void UObscuraIconGeneratorEditorWidget::CreateViewport()
{
	if (Viewport == nullptr) Viewport = NewObject<UViewport>(this, UViewport::StaticClass());
	if (ViewportOverlay && Viewport) ViewportOverlay->AddChild(Viewport);
}

void UObscuraIconGeneratorEditorWidget::CreateEditorNotification(UTexture2D* CreatedTexture) const
{
	// If for some reason this is not valid we return
	if (CreatedTexture == nullptr) return;
	
	auto OpenTextureInNewTab = [CreatedTexture]() {
		if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor ? GEditor->GetEditorSubsystem<UAssetEditorSubsystem>() : nullptr) {
			AssetEditorSubsystem->OpenEditorForAsset(CreatedTexture);
		}
	};
	
	const FString Message = FString::Printf(TEXT("%s %s"),
		*LOCTEXT("Obscura_Notification_Title", "Texture Generated with name:").ToString(),
		*GetTextureName()
	);
	
	FNotificationInfo NotifyInfo(FText::FromString(Message)); 
	NotifyInfo.bUseLargeFont = false;
	NotifyInfo.FadeOutDuration = 10.0f; // Notification will fade out in 10 seconds.
	NotifyInfo.SubText = LOCTEXT("Obscura_Notification_OpenAsset", "Obscura Icon Generator");
	NotifyInfo.Hyperlink = FSimpleDelegate::CreateLambda(OpenTextureInNewTab);
	NotifyInfo.HyperlinkText = LOCTEXT("Obscura_Notification_OpenAsset", "Open Texture");

	// Make Notification appear
	FSlateNotificationManager::Get().AddNotification(NotifyInfo);
}

void UObscuraIconGeneratorEditorWidget::CreateMaterial()
{
	if (DisplayRenderTargetMaterialInstance) {
		DisplayRenderTargetMaterialInstanceDynamic = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, DisplayRenderTargetMaterialInstance);
		RenderTargetImage->SetBrushFromMaterial(DisplayRenderTargetMaterialInstanceDynamic);
	}
	if (ExportRenderTargetMaterialInstance) {
		ExportRenderTargetMaterialInstanceDynamic = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, ExportRenderTargetMaterialInstance);
	}
}

void UObscuraIconGeneratorEditorWidget::UpdateRenderTarget() const
{
	if (SceneCaptureActor && DisplayRenderTargetMaterialInstanceDynamic && ExportRenderTargetMaterialInstanceDynamic) {
		// Display
		DisplayRenderTargetMaterialInstanceDynamic->SetTextureParameterValue(MaterialTextureParameterName, SceneCaptureActor->GetRenderTarget());
		DisplayRenderTargetMaterialInstanceDynamic->SetTextureParameterValue(MaterialOpacityTextureParameterName, SceneCaptureActor->GetOpacityRenderTarget());

		// Export
		ExportRenderTargetMaterialInstanceDynamic->SetTextureParameterValue(MaterialTextureParameterName, SceneCaptureActor->GetRenderTarget());
		ExportRenderTargetMaterialInstanceDynamic->SetTextureParameterValue(MaterialOpacityTextureParameterName, SceneCaptureActor->GetOpacityRenderTarget());
	}
}

void UObscuraIconGeneratorEditorWidget::CreateScreenCaptureActor()
{
	if (SceneCaptureActorClass && Viewport) {
		if (AObscuraItemSceneCaptureActor* ItemSceneCaptureActor = Cast<AObscuraItemSceneCaptureActor>(Viewport->Spawn(SceneCaptureActorClass))) {
			SceneCaptureActor = ItemSceneCaptureActor;
			SceneCaptureActor->Init(Viewport);
			SceneCaptureActor->UpdateZoom(ObscuraActorTransform.ActorZoom); // Setting default zoom value
			SetupSceneLightSettings();
			UpdatePostProcess();
			SceneCaptureActor->OnActorSetEvent.AddUniqueDynamic(this, &ThisClass::UObscuraIconGeneratorEditorWidget::OnActorSet);
			UpdateRenderTarget();
			UpdateGeneratedTextureNameDisplay();
		}
	}
}

void UObscuraIconGeneratorEditorWidget::SetObscuraActorTransform(const FObscuraActorTransform& NewTransform)
{
	ObscuraActorTransform = NewTransform;
	if (SceneCaptureActor) {
		SceneCaptureActor->UpdateLocation(ObscuraActorTransform.ActorLocation);
		SceneCaptureActor->UpdateRotation(ObscuraActorTransform.ActorRotation);
		SceneCaptureActor->UpdateZoom(ObscuraActorTransform.ActorZoom);
	}
}

void UObscuraIconGeneratorEditorWidget::UpdateSceneCaptureShowOnlyList_Implementation()
{
	if (SceneCaptureActor) SceneCaptureActor->UpdateShowOnlyList();
}

void UObscuraIconGeneratorEditorWidget::UpdateActorOffset_Implementation()
{
	if (SceneCaptureActor) SceneCaptureActor->UpdateCurrentActorOffset();
}

void UObscuraIconGeneratorEditorWidget::UpdatePostProcess_Implementation()
{
	if (SceneCaptureActor) SceneCaptureActor->UpdatePostProcess(PostProcessSettings);
}

void UObscuraIconGeneratorEditorWidget::UpdateActorLocation_Implementation(const FVector2D& MouseDelta)
{
	if (SceneCaptureActor) {
		const FVector2D AdjustedMouseDelta = MouseDelta * MouseDeltaLocationMultiplier;
		const FVector AdditionalLocation = FVector(0.0f, AdjustedMouseDelta.X, AdjustedMouseDelta.Y * -1.0f);
		
		ObscuraActorTransform.ActorLocation += AdditionalLocation;
		
		SceneCaptureActor->UpdateLocation(ObscuraActorTransform.ActorLocation);
	}
}

void UObscuraIconGeneratorEditorWidget::UpdateActorRotation_Implementation(const FVector2D& MouseDelta)
{
	if (SceneCaptureActor) {
		const FVector2D AdjustedMouseDelta = MouseDelta * MouseDeltaRotationMultiplier;
		const FRotator AdditionalRotation = FRotator(AdjustedMouseDelta.Y, AdjustedMouseDelta.X * -1.0f, 0.0f);
		
		// Combining Rotators so it can be passed to the SceneCapture actor
		ObscuraActorTransform.ActorRotation = UKismetMathLibrary::ComposeRotators(ObscuraActorTransform.ActorRotation, AdditionalRotation);
		
		SceneCaptureActor->UpdateRotation(ObscuraActorTransform.ActorRotation);
	}
}

void UObscuraIconGeneratorEditorWidget::UpdateActorZoom_Implementation(const float MouseWheelDelta)
{
	if (SceneCaptureActor) {
		ObscuraActorTransform.ActorZoom = FMath::Clamp((MouseWheelDelta * -1.0f) + ObscuraActorTransform.ActorZoom, 0.0f, 200.0f);
		SceneCaptureActor->UpdateZoom(ObscuraActorTransform.ActorZoom);
	}
}

void UObscuraIconGeneratorEditorWidget::SetActor_Implementation(const TSubclassOf<AActor> InActorClass)
{
	if (SceneCaptureActor) {
		SceneCaptureActor->SetActor(InActorClass);
	}
}

void UObscuraIconGeneratorEditorWidget::SetupSceneLightSettings_Implementation()
{
	if (SceneCaptureActor) {
		SceneLightSettings.Empty();

		// Iterating through Light Components
		for (const ULocalLightComponent* SceneLight : SceneCaptureActor->GetSceneLights()) {
			if (SceneLight) {
				// Adding them to the array of Scene Light Settings
				SceneLightSettings.Add(FSceneLightData(
					SceneLight->GetFName(), SceneLight->Intensity, SceneLight->IntensityUnits, SceneLight->LightColor,
					SceneLight->AttenuationRadius, SceneLight->bUseTemperature > 0, SceneLight->Temperature
				));
			}
			
		}
	}
}

void UObscuraIconGeneratorEditorWidget::UpdateSceneLights_Implementation()
{
	if (SceneCaptureActor) {
		for (const FSceneLightData& LightSetting : SceneLightSettings) {
			SceneCaptureActor->UpdateSceneLight(LightSetting);
		}
	}
}

void UObscuraIconGeneratorEditorWidget::ResetLocation_Implementation()
{
	ObscuraActorTransform.ActorLocation = FVector();
}

void UObscuraIconGeneratorEditorWidget::ResetRotation_Implementation()
{
}

void UObscuraIconGeneratorEditorWidget::ResetZoom_Implementation()
{
}

UObject* UObscuraIconGeneratorEditorWidget::GetTargetAsset_Implementation() const
{
	if (ActorClass) {
		return ActorClass;
	}
	return nullptr;
}

FString UObscuraIconGeneratorEditorWidget::GetTargetAssetFilePath_Implementation() const
{
	if (const UObject* TargetAsset = GetTargetAsset()) {
		const FString TargetAssetPath = UKismetSystemLibrary::GetPathName(TargetAsset);

		// Splitting the string into individual parts to remove the last one that's the asset's name
		TArray<FString> ParsedPath;
		TargetAssetPath.ParseIntoArray(ParsedPath, TEXT("/"));
		ParsedPath.RemoveAt(ParsedPath.Num() - 1);

		// Adding a / at the start and end so it's /Folder/Subfolder/ instead of Folder/Subfolder
		return TEXT("/") + FString::Join(ParsedPath, TEXT("/")) + TEXT("/");
	}
	return GetPluginContentFolderPath();
}

FString UObscuraIconGeneratorEditorWidget::GetPluginContentFolderPath_Implementation() const
{
	FString ContentDir = IPluginManager::Get().FindPlugin("ObscuraIconGenerator")->GetBaseDir();
	
	return ContentDir;
}

UTexture2D* UObscuraIconGeneratorEditorWidget::GenerateTexture_Implementation()
{
	if (SceneCaptureActor && ExportRenderTargetMaterialInstanceDynamic) {
		// Creating temporary RenderTarget for export 
		if (UTextureRenderTarget2D* ExportRenderTarget = SceneCaptureActor->CreateRenderTargetForExport()) {
			// Rendering the content of the ExportRenderTargetMaterial to the ExportRenderTarget
			UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, ExportRenderTarget, ExportRenderTargetMaterialInstanceDynamic);

			// Creating the path where the texture will be saved eg.: /ObscuraIconGenerator/Textures/T_Icon
			const FString TextureTargetPath = bCreateTextureInTargetAssetFolder
												? GetTargetAssetFilePath() + GetTextureName()		// Finding asset's folder and placing the texture there 
												: GetTargetDirectoryPath().Path + GetTextureName();	// Putting the created textures in the desired folder
						
			if(UTexture2D* GeneratedTexture = UKismetRenderingLibrary::RenderTargetCreateStaticTexture2DEditorOnly(
					ExportRenderTarget, TextureTargetPath, GeneratedTextureCompressionSettings
				)
			) {
				GeneratedTexture->LODGroup = GeneratedTextureGroup;
				GeneratedTexture->UpdateResource(); // Need this so the engine doesn't crash for modifying the TextureGroup
				
				UEditorAssetLibrary::SaveLoadedAsset(GeneratedTexture);

				CreateEditorNotification(GeneratedTexture);
				OnTextureGenerated(GeneratedTexture);
				
            	return GeneratedTexture;
			}
			
		}
	}

	return nullptr;
}

FDirectoryPath UObscuraIconGeneratorEditorWidget::GetTargetDirectoryPath() const
{
	// If the last character doesn't equal / we add it so it's consistent
	if (const FString LastCharacter = &TargetDirectoryPath.Path.GetCharArray().Last(); LastCharacter != "/") {
		return FDirectoryPath(TargetDirectoryPath.Path + "/");
	}

	return TargetDirectoryPath;
}

void UObscuraIconGeneratorEditorWidget::UpdateGeneratedTextureNameDisplay()
{
	GeneratedTextureName = GetTextureName();
}

bool UObscuraIconGeneratorEditorWidget::IsMouseAboveWidget(const UWidget* TargetWidget, UObject* WorldContextObject)
{
	/*
	 * There might be a better way to do this, if you know such way please don't hesitate to reach out
	 */
	if(TargetWidget) {
		const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(WorldContextObject);
		const FGeometry CachedGeometry = TargetWidget->GetCachedGeometry();
		const FVector2D LocalTopLeft = USlateBlueprintLibrary::GetLocalTopLeft(CachedGeometry);

		FVector2D PixelPosition;
		FVector2D ViewportPosition;
		USlateBlueprintLibrary::LocalToViewport(WorldContextObject, CachedGeometry, LocalTopLeft, PixelPosition, ViewportPosition);
		

		const FVector2D TopLeftPosition = ViewportPosition;
		const FVector2D ElementSize = CachedGeometry.GetLocalSize();
		const FVector2D LocalMousePos = MousePosition - CachedGeometry.GetAbsolutePosition();
		

		/*
		 * Checking if mouse position is within bounds of the widget
		 */
		return (LocalMousePos.X >= TopLeftPosition.X) && (LocalMousePos.X <= TopLeftPosition.X + ElementSize.X)
				&& (LocalMousePos.Y >= TopLeftPosition.Y) && (LocalMousePos.Y <= TopLeftPosition.Y + ElementSize.Y);
	}

	return false;

}

void UObscuraIconGeneratorEditorWidget::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	const FName PropertyName = PropertyChangedEvent.GetMemberPropertyName();
	const EPropertyChangeType::Type PropertyChangeType = PropertyChangedEvent.ChangeType;

	/*
	 * On ActorClass Change in Editor
	 * Set Actor in SceneCaptureActor with new class
	 */
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UObscuraIconGeneratorEditorWidget, ActorClass)) {
		SetActor(ActorClass);
		UpdateGeneratedTextureNameDisplay();
	}

	/*
	 * On SceneLightSettings Change in Editor
	 * Update Scene Lights
	 */
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UObscuraIconGeneratorEditorWidget, SceneLightSettings) ) {

		if (PropertyChangeType == EPropertyChangeType::Interactive || PropertyChangeType == EPropertyChangeType::ValueSet) {
			UpdateSceneLights();
		}		
	}

	/*
	 * On ObscuraActorTransform Change in Editor
	 * Update Location & Rotation
	 */
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UObscuraIconGeneratorEditorWidget, ObscuraActorTransform)) {
		if (SceneCaptureActor) {
			SceneCaptureActor->UpdateLocation(ObscuraActorTransform.ActorLocation);
			SceneCaptureActor->UpdateRotation(ObscuraActorTransform.ActorRotation);
		}
	}
	
	/*
	 * On PostProcessSettings Change in Editor
	 * Update Zoom
	 */
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UObscuraIconGeneratorEditorWidget, PostProcessSettings)) {
		UpdatePostProcess();
	}
	
	/*
	 * On Prefix/Suffix Change in Editor
	 * Update GeneratedTextureNameDisplay
	 */
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UObscuraIconGeneratorEditorWidget, TextureNamePrefix) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UObscuraIconGeneratorEditorWidget, TextureNameSuffix)
	) {
		UpdateGeneratedTextureNameDisplay();
	}
}

void UObscuraIconGeneratorEditorWidget::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	const FName PropertyName = PropertyChangedEvent.GetMemberPropertyName();
	// Getting the Struct Property name. By default PostEditChangeChainProperty will return the smallest part of any given property.
	// For example FVector, it will return the individual axis that's being changed
	// Getting the Tail(last node) and going to the previous node we can get the FVector property itself, instead of the X/Y/Z property
	const FName StructPropertyName = PropertyChangedEvent.PropertyChain.GetTail()->GetPrevNode()->GetValue()->GetFName();
	
	/*
	 * On ActorLocation Change in Editor
	 * Update Actor Location
	 */
	if (StructPropertyName == GET_MEMBER_NAME_CHECKED(FObscuraActorTransform, ActorLocation)) {
		if (SceneCaptureActor) {
			SceneCaptureActor->UpdateLocation(ObscuraActorTransform.ActorLocation);
		}
	}

	/*
	 * On ActorRotation Change in Editor
	 * Update Actor Rotation
	 */
	if (StructPropertyName == GET_MEMBER_NAME_CHECKED(FObscuraActorTransform, ActorRotation)) {
		if (SceneCaptureActor) {
			SceneCaptureActor->UpdateRotation(ObscuraActorTransform.ActorRotation);
		}
	}
	
	
	/*
	 * On ActorZoom Change in Editor
	 * Update Zoom
	 */
	if (PropertyName == GET_MEMBER_NAME_CHECKED(FObscuraActorTransform, ActorZoom)) {
		if (SceneCaptureActor) {
			SceneCaptureActor->UpdateZoom(ObscuraActorTransform.ActorZoom);
		}
	}
}

void UObscuraIconGeneratorEditorWidget::OnTextureGenerated_Implementation(UTexture2D* GeneratedTexture)
{
}

FString UObscuraIconGeneratorEditorWidget::GetTextureName_Implementation() const
{
	FString CurrentActorName = "None";
	if (ActorClass) CurrentActorName = GetNameSafe(ActorClass);
	return TextureNamePrefix + CurrentActorName + TextureNameSuffix;
}

#undef LOCTEXT_NAMESPACE