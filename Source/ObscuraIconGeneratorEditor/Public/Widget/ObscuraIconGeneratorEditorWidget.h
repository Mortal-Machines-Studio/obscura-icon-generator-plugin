// Nightfall Protocol 2024 Mortal Machines Studio. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "Components/LocalLightComponent.h"
#include "Components/Overlay.h"
#include "Components/Viewport.h"
#include "Editor/Blutility/Classes/EditorUtilityWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ObscuraIconGeneratorEditorWidget.generated.h"


class UDetailsView;
class AObscuraItemSceneCaptureActor;

USTRUCT(BlueprintType, Category="Obscura")
struct FObscuraActorTransform {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transform")
	FVector ActorLocation = FVector();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transform")
	FRotator ActorRotation = FRotator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transform", meta=(ToolTip="The Distance from the Camera"))
	float ActorZoom = 0.0f;

	FObscuraActorTransform(){}
	
};

USTRUCT(BlueprintType, Category="Obscura")
struct FSceneLightData {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Scene Light")
	FName ComponentName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scene Light", meta=(ClampMin=0.0f))
	float LightIntensity = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scene Light")
	ELightUnits IntensityUnits = ELightUnits::Unitless;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scene Light")
	FLinearColor LightColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, interp, Category="Scene Light", meta=(UIMin = "8.0", UIMax = "16384.0"))
	float AttenuationRadius = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scene Light")
	bool bUseTemperature = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, interp, Category="Scene Light", meta = (UIMin = "1700.0", UIMax = "12000.0", DisplayAfter ="bUseTemperature"))
	float Temperature = 5500.0f;

	

	FSceneLightData() {}

	FSceneLightData(const FName& ComponentName, const float Intensity, const ELightUnits& IntensityUnits,
		const FLinearColor& LightColor, const float AttenuationRadius, const bool bUseTemperature, const float Temperature)
	{
		this->ComponentName = ComponentName;
		this->LightIntensity = Intensity;
		this->IntensityUnits = IntensityUnits;
		this->LightColor = LightColor;
		this->AttenuationRadius = AttenuationRadius;
		this->bUseTemperature = bUseTemperature;
		this->Temperature = Temperature;
	}
};


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, DisplayName="Obscura Icon Generator Widget")
class OBSCURAICONGENERATOREDITOR_API UObscuraIconGeneratorEditorWidget : public UEditorUtilityWidget {
	GENERATED_BODY()

protected:

	/*
	 * Native Widget Functions
	 */

	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/*
	 * Settings
	 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Obscura|Settings",
		meta=(ToolTip="Controls how sensitive the Mouse Location Drag is"))
	float MouseDeltaLocationMultiplier = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Obscura|Settings",
		meta=(ToolTip="Controls how sensitive the Mouse Rotation is"))
	float MouseDeltaRotationMultiplier = 0.25f;


	/*
	 * Events
	 */

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Obscura|Events", meta=(ForceAsFunction))
	void OnActorSet(AActor* NewActor);
	
	/*
	 * Transform
	 */
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Obscura|Scene Capture|Transforms")
	bool bCanTransformActor = false;

	/*
	 * Widgets
	 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Viewport")
	TObjectPtr<UViewport> Viewport = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widget", meta=(BindWidget))
	TObjectPtr<UOverlay> ViewportOverlay = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widget", meta=(BindWidget))
	TObjectPtr<UImage> RenderTargetImage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widget", meta=(BindWidget))
	TObjectPtr<UDetailsView> IconGeneratorDetailsView = nullptr;

	void CreateViewport();

	void CreateEditorNotification(UTexture2D* CreatedTexture) const;

	/*
	 * Material & Render Target
	 */

	void CreateMaterial();

	UFUNCTION(BlueprintCallable, Category="Obscura|Scene Capture")
	void UpdateRenderTarget() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Obscura|Scene Capture|Material")
	TObjectPtr<UMaterialInstance> DisplayRenderTargetMaterialInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Obscura|Scene Capture|Material")
	TObjectPtr<UMaterialInstance> ExportRenderTargetMaterialInstance = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Obscura|Scene Capture|Material")
	TObjectPtr<UMaterialInstanceDynamic> DisplayRenderTargetMaterialInstanceDynamic = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Obscura|Scene Capture|Material")
	TObjectPtr<UMaterialInstanceDynamic> ExportRenderTargetMaterialInstanceDynamic = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Obscura|Scene Capture|Material")
	FName MaterialTextureParameterName = "Target_Texture";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Obscura|Scene Capture|Material")
	FName MaterialOpacityTextureParameterName = "Opacity_Texture";

	/*
	 * Scene Capture Actor
	 */

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Obscura|Scene Capture")
	TObjectPtr<AObscuraItemSceneCaptureActor> SceneCaptureActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Obscura|Scene Capture")
	TSubclassOf<AObscuraItemSceneCaptureActor> SceneCaptureActorClass = nullptr;

	void CreateScreenCaptureActor();
	
	/*
	 * Actor
	 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Obscura|PreviewActor")
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Obscura|PreviewActor", BlueprintSetter=SetObscuraActorTransform)
	FObscuraActorTransform ObscuraActorTransform;

	UFUNCTION(BlueprintSetter, Category="Setter")
	void SetObscuraActorTransform(UPARAM(ref) const FObscuraActorTransform& NewTransform);

	/*
	 * Update Functions
	 */
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Obscura|Scene Capture", meta=(ForceAsFunction))
	void UpdateActorLocation(const FVector2D& MouseDelta);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Obscura|Scene Capture", meta=(ForceAsFunction))
	void UpdateActorRotation(const FVector2D& MouseDelta);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Obscura|Scene Capture", meta=(ForceAsFunction))
	void UpdateActorZoom(const float MouseWheelDelta);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Obscura|Scene Capture", meta=(ForceAsFunction))
	void UpdatePostProcess();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Obscura|Scene Capture", meta=(ForceAsFunction))
	void UpdateActorOffset();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Obscura|Scene Capture", meta=(ForceAsFunction))
	void UpdateSceneCaptureShowOnlyList();

	/*
	 * Reset Functions
	 */
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Obscura|Scene Capture", meta=(ForceAsFunction))
	void ResetLocation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Obscura|Scene Capture", meta=(ForceAsFunction))
	void ResetRotation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Obscura|Scene Capture", meta=(ForceAsFunction))
	void ResetZoom();

	/*
	 * Texture Generation
	 */
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Obscura|Texture Generation",
		meta=(ToolTip="Creates the texture where the currently selected Asset is"))
	bool bCreateTextureInTargetAssetFolder = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Obscura|Texture Generation", BlueprintGetter=GetTargetDirectoryPath,
		meta=(EditCondition="!bCreateTextureInTargetAssetFolder", EditConditionHides, RelativePath, ContentDir,
				ToolTip="This is where Textures will be saved. Defaults to the project's content folder"
		))
	FDirectoryPath TargetDirectoryPath = FDirectoryPath(FPaths::ProjectContentDir());
	

	UFUNCTION(BlueprintPure, BlueprintGetter, Category="Obscura|Texture Generation", meta=(RelativePath))
	FDirectoryPath GetTargetDirectoryPath() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Obscura|Texture Generation",
		meta=(ToolTip="Optional Prefix, leave empty if you don't want to use it or override GetTextureName()"))
	FString TextureNamePrefix = TEXT("T_");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Obscura|Texture Generation",
		meta=(ToolTip="Optional Suffix, leave empty if you don't want to use it or override GetTextureName()"))
	FString TextureNameSuffix = TEXT("_Icon");

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Obscura|Texture Generation")
	FString GeneratedTextureName = "None";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Obscura|Texture Generation")
	TEnumAsByte<TextureCompressionSettings> GeneratedTextureCompressionSettings = TC_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Obscura|Texture Generation")
	TEnumAsByte<TextureGroup> GeneratedTextureGroup = TEXTUREGROUP_UI;
	

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Obscura|Texture Generation", meta=(ForceAsFunction))
	UObject* GetTargetAsset() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Obscura|Texture Generation", meta=(ForceAsFunction))
	FString GetTargetAssetFilePath() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Obscura|Texture Generation",
		meta=(ForceAsFunction,
			ToolTip="This a fullback that defaults to the Plugins Folder in cases where there is no valid Target Path available"
		)
	)
	FString GetPluginContentFolderPath() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category="Obscura|Texture Generation",
		meta=(ForceAsFunction, ToolTip="Used for generating a name for the texture. Should be overwritten"))
	FString GetTextureName() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Obscura|Texture Generation", meta=(ForceAsFunction))
	UTexture2D* GenerateTexture();

	void UpdateGeneratedTextureNameDisplay();

	/*
	 * Lights
	 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Obscura|Scene Lights", meta=(TitleProperty="ComponentName"))
	TArray<FSceneLightData> SceneLightSettings;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Obscura|Scene Lights", meta=(ForceAsFunction))
	void SetupSceneLightSettings();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Obscura|Scene Lights", meta=(ForceAsFunction))
	void UpdateSceneLights();

	/*
	 * Post Process Settings
	 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Obscura|Post Process")
	FPostProcessSettings PostProcessSettings;

public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Obscura|Actor", meta=(ForceAsFunction))
	void SetActor(TSubclassOf<AActor> InActorClass);
	
	UFUNCTION(BlueprintPure, Category="Helpers", meta=(WorldContext="WorldContextObject"))
	static bool IsMouseAboveWidget(const UWidget* TargetWidget, UObject* WorldContextObject);

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Obscura|Events", meta=(ForceAsFunction))
	void OnTextureGenerated(UTexture2D* GeneratedTexture);
};


