// Nightfall Protocol 2024 Mortal Machines Studio. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/Viewport.h"
#include "Components/Widget.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Actor.h"
#include "ObscuraItemSceneCaptureActor.generated.h"

class USpringArmComponent;
struct FSceneLightData;
class ULocalLightComponent;
enum ETextureRenderTargetFormat : int;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorSetSignature, AActor*, Actor);

UCLASS(Abstract, Blueprintable, DisplayName="Obscura Item Scene Capture Actor")
class OBSCURAICONGENERATOREDITOR_API AObscuraItemSceneCaptureActor : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AObscuraItemSceneCaptureActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/*
	 * Components
	 */
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneCaptureComponent2D> CaptureComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneCaptureComponent2D> OpacityCaptureComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> MeshZoomSceneComponent = nullptr;

	/*
	 * This is the component that will be the parent for the attached actor.
	 * Can be offset so the attached actor will always stay centered regardless of pivot point.
	 */
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> MeshParentSceneComponent = nullptr;


	/*
	 * Render Target
	 */
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="RenderTarget")
	TObjectPtr<UTextureRenderTarget2D> RenderTarget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="RenderTarget")
	TObjectPtr<UTextureRenderTarget2D> OpacityRenderTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RenderTarget|Params")
	FIntPoint RenderTargetResolution = FIntPoint(1024, 1024);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RenderTarget|Params")
	TEnumAsByte<ETextureRenderTargetFormat> RenderTargetFormat = RTF_RGBA8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RenderTarget|Params")
	FLinearColor RenderTargetClearColor = FLinearColor::Black;

	void CreateAndAssignRenderTarget();

	void CaptureScenes() const;
	

	/*
	 * Helpers
	 */
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Actor",
		meta=(ForceAsFunction, ToolTip="Used to destroy the current displayed actor"))
	void DestroyCurrentActor();

	/*
	 * Variables
	 */

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Actor")
	TObjectPtr<AActor> CurrentActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Viewport")
	TObjectPtr<UViewport> ParentViewport = nullptr;

public:

	void Init(UViewport* InParentViewport);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Setters", meta=(ForceAsFunction))
	void SetActor(TSubclassOf<AActor> NewActorClass);

	/*
	 * Getters
	 */
	
	UFUNCTION(BlueprintPure, Category="Components")
	USceneCaptureComponent2D* GetCaptureComponent();

	UFUNCTION(BlueprintPure, Category="Components")
	USceneCaptureComponent2D* GetOpacityCaptureComponent();

	UFUNCTION(BlueprintPure, Category="Components")
	UTextureRenderTarget2D* GetRenderTarget();

	UFUNCTION(BlueprintPure, Category="Components")
	UTextureRenderTarget2D* GetOpacityRenderTarget();

	UFUNCTION(BlueprintPure, Category="Getters")
	TArray<ULocalLightComponent*> GetSceneLights() const;

	/*
	 * Events
	 */
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Actor|Event")
	void OnActorSet();

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Actor|Event")
	FOnActorSetSignature OnActorSetEvent;

	/*
	 * Update & Reset
	 */

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Actor", meta=(ForceAsFunction))
	void UpdateLocation(const FVector& NewLocation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Actor", meta=(ForceAsFunction))
	void UpdateRotation(const FRotator& NewRotation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Actor", meta=(ForceAsFunction))
	void UpdateZoom(const float NewZoom);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Actor", meta=(ForceAsFunction))
	void ResetRotation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Actor", meta=(ForceAsFunction))
	void ResetZoom();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Actor", meta=(ForceAsFunction))
	void UpdateShowOnlyList();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Actor", meta=(ForceAsFunction))
	void UpdateCurrentActorOffset(const bool bOnlyMeasureCollidingComponents = false);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Getters", meta=(ForceAsFunction))
	void UpdateSceneLight(UPARAM(ref) const FSceneLightData& SceneLightData);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Getters", meta=(ForceAsFunction))
	void UpdatePostProcess(const FPostProcessSettings& PostProcessSettings);

	/*
	 * Render Target
	 */

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="RenderTarget", meta=(ForceAsFunction))
	UTextureRenderTarget2D* CreateRenderTargetForExport();

	/*
	 * Helpers
	 */
	
	UFUNCTION(BlueprintPure, Category="Actor|Transform")
	FVector GetMeshCenterPointOffset(const bool bOnlyMeasureCollidingComponents = false) const;

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category="Getters", meta=(ForceAsFunction, ToolTip="Fetches the current actor's Static Mesh if it exists."))
	UStaticMesh* GetCurrentActorStaticMesh() const;

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category="Getters", meta=(ForceAsFunction, ToolTip="Fetches the current actor's Static Mesh if it exists."))
	USkeletalMesh* GetCurrentActorSkeletalMesh() const;
};
