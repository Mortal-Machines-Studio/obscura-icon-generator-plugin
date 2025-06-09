// Nightfall Protocol 2024 Mortal Machines Studio. All Rights Reserved


#include "Actor/ObscuraItemSceneCaptureActor.h"

#include "Components/LocalLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Widget/ObscuraIconGeneratorEditorWidget.h"


// Sets default values
AObscuraItemSceneCaptureActor::AObscuraItemSceneCaptureActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/*
	 * Main Scene Capture
	 */
	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	RootComponent = CaptureComponent;
	
	/*
	 * Opacity Screen Capture
	 * Need have another screen capture going for opacity because Post Process doesn't apply to modes that have opacity
	 */
	OpacityCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("OpacitySceneCaptureComponent"));
	OpacityCaptureComponent->SetupAttachment(GetRootComponent());

	/*
	 * Creating the scene component that will be moved to accomodate for the zoom
	 */
	MeshZoomSceneComponent = CreateDefaultSubobject<USceneComponent>("MeshZoomSceneComponent");
	MeshZoomSceneComponent->SetupAttachment(GetRootComponent());

	/*
	 * Creating the scene component that actually "holds" the created actor. This will be translated and rotated accordingly
	 */
	MeshParentSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MeshParentSceneComponent"));
	MeshParentSceneComponent->SetupAttachment(MeshZoomSceneComponent);

}

// Called when the game starts or when spawned
void AObscuraItemSceneCaptureActor::BeginPlay()
{
	Super::BeginPlay();
	CreateAndAssignRenderTarget();
}

void AObscuraItemSceneCaptureActor::CreateAndAssignRenderTarget()
{
	// If the render target already exist we don't want to make a new one
	if (RenderTarget && OpacityRenderTarget) return;

	// Creating Display render target
	UTextureRenderTarget2D* NewRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this,
		RenderTargetResolution.X, RenderTargetResolution.Y, RenderTargetFormat, RenderTargetClearColor);

	// Creating Opacity Mask render target
	UTextureRenderTarget2D* NewOpacityRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this,
		RenderTargetResolution.X, RenderTargetResolution.Y, RenderTargetFormat, RenderTargetClearColor);

	// Assigning variables & setting render targets on Scene Capture Components
	if (NewRenderTarget && NewOpacityRenderTarget) {
		RenderTarget = NewRenderTarget;
		OpacityRenderTarget = NewOpacityRenderTarget;
		
		if (CaptureComponent) {
			CaptureComponent->TextureTarget = RenderTarget;
			OpacityCaptureComponent->TextureTarget = OpacityRenderTarget;
		}
	}
}

UTextureRenderTarget2D* AObscuraItemSceneCaptureActor::CreateRenderTargetForExport_Implementation()
{
	return UKismetRenderingLibrary::CreateRenderTarget2D(this,
		RenderTargetResolution.X, RenderTargetResolution.Y, RenderTargetFormat, RenderTargetClearColor);
}

void AObscuraItemSceneCaptureActor::CaptureScenes() const
{
	if (CaptureComponent && OpacityCaptureComponent) {
		if (!CaptureComponent->bCaptureEveryFrame) CaptureComponent->CaptureScene();
		if (!OpacityCaptureComponent->bCaptureEveryFrame) OpacityCaptureComponent->CaptureScene();
	}
}

void AObscuraItemSceneCaptureActor::UpdateCurrentActorOffset_Implementation()
{
	if (CurrentActor) {
		CurrentActor->AddActorLocalOffset(GetMeshCenterPointOffset() * -1.0f);
	}
}

void AObscuraItemSceneCaptureActor::UpdateShowOnlyList_Implementation()
{
	if (CurrentActor && CaptureComponent) {
		// Adding CurrentActor to the ShowOnly list
		CaptureComponent->ShowOnlyActors.Add(CurrentActor); 

		// Getting all the attached actors
		TArray<AActor*> AttachedActors;
		CurrentActor->GetAttachedActors(AttachedActors);

		// If there are any we add them to the show only list of the CaptureComponent
		for (AActor* AttachedActor : AttachedActors) {
			if (AttachedActor) {
				CaptureComponent->ShowOnlyActors.Add(AttachedActor);
			}
		}
	}
}

FVector AObscuraItemSceneCaptureActor::GetMeshCenterPointOffset(const bool bOnlyMeasureCollidingComponents) const
{
	if (!CurrentActor) return FVector();
	
	FVector OutCenter;
	FVector OutBoxExtent;

	// Getting all the attached actors
	TArray<AActor*> AllActors;
	CurrentActor->GetAttachedActors(AllActors);
	AllActors.Add(CurrentActor); // Adding the current actor to the array as well

	// Getting the center and bounds of all actors attached to the CurrentActor including the CurrentActor
	UGameplayStatics::GetActorArrayBounds(AllActors, bOnlyMeasureCollidingComponents, OutCenter, OutBoxExtent);

	// Need to InverseTransformLocation because the OutCenter is in worldspace
	return  UKismetMathLibrary::InverseTransformLocation(CurrentActor->GetTransform(), OutCenter);
}

void AObscuraItemSceneCaptureActor::DestroyCurrentActor_Implementation()
{
	if (CurrentActor) {
		TArray<AActor*> AttachedActors;
		CurrentActor->GetAttachedActors(AttachedActors);
		for (AActor* AttachedActor : AttachedActors) {
			AttachedActor->Destroy();
		}
		
		CurrentActor->Destroy();
		CurrentActor = nullptr;

		CaptureScenes();
	}
}

void AObscuraItemSceneCaptureActor::Init(UViewport* InParentViewport)
{
	if (InParentViewport) {
		ParentViewport = InParentViewport;
		CreateAndAssignRenderTarget();
	}
	
}

void AObscuraItemSceneCaptureActor::SetActor_Implementation(TSubclassOf<AActor> NewActorClass)
{
	if (ParentViewport == nullptr) return;

	// Destroy previous displayed Actor
	DestroyCurrentActor();

	// If we don't set a new actor
	if (NewActorClass == nullptr) return;
	
	if (AActor* NewActor = ParentViewport->Spawn(NewActorClass)) {		
		CurrentActor = NewActor;
		if (CurrentActor && MeshParentSceneComponent && CaptureComponent) {
			// Attaching the Actor to the Scene Component
			CurrentActor->AttachToComponent(MeshParentSceneComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

			UpdateCurrentActorOffset();
			UpdateShowOnlyList();
			
			OnActorSetEvent.Broadcast(CurrentActor);
			CaptureScenes();
		}
		
	}
}


USceneCaptureComponent2D* AObscuraItemSceneCaptureActor::GetCaptureComponent()
{
	return CaptureComponent;
}

USceneCaptureComponent2D* AObscuraItemSceneCaptureActor::GetOpacityCaptureComponent()
{
	return OpacityCaptureComponent;
}

UTextureRenderTarget2D* AObscuraItemSceneCaptureActor::GetRenderTarget()
{
	return RenderTarget;
}

UTextureRenderTarget2D* AObscuraItemSceneCaptureActor::GetOpacityRenderTarget()
{
	return OpacityRenderTarget;
}

TArray<ULocalLightComponent*> AObscuraItemSceneCaptureActor::GetSceneLights() const
{
	TArray<ULocalLightComponent*> FoundLights;
	GetComponents<ULocalLightComponent>(FoundLights);
	
	return FoundLights;
}

void AObscuraItemSceneCaptureActor::UpdatePostProcess_Implementation(const FPostProcessSettings& PostProcessSettings)
{
	if (CaptureComponent) {
		CaptureComponent->PostProcessSettings = PostProcessSettings;
		CaptureScenes();
	}
}

void AObscuraItemSceneCaptureActor::UpdateSceneLight_Implementation(const FSceneLightData& SceneLightData)
{
	TArray<ULocalLightComponent*> FoundLights;
	GetComponents<ULocalLightComponent>(FoundLights);

	for (ULocalLightComponent* FoundLight : FoundLights) {
		if (FoundLight && SceneLightData.ComponentName == FoundLight->GetFName()) {
			// Setting Attributes on the Light Component
			FoundLight->SetIntensity(SceneLightData.LightIntensity);			// Intensity
			FoundLight->SetIntensityUnits(SceneLightData.IntensityUnits);		// Intensity Units
			FoundLight->SetLightColor(SceneLightData.LightColor);				// Light Color
			FoundLight->SetAttenuationRadius(SceneLightData.AttenuationRadius);	// Attenuation Radius
			FoundLight->SetUseTemperature(SceneLightData.bUseTemperature);		// Use Temperature
			FoundLight->SetTemperature(SceneLightData.Temperature);				// Temperature
		}
	}
	CaptureScenes();
}


void AObscuraItemSceneCaptureActor::OnActorSet_Implementation()
{
}

void AObscuraItemSceneCaptureActor::UpdateLocation_Implementation(const FVector& NewLocation)
{
	if (MeshParentSceneComponent) {
		MeshParentSceneComponent->SetRelativeLocation(NewLocation);
		CaptureScenes();
	}
	
}

void AObscuraItemSceneCaptureActor::UpdateRotation_Implementation(const FRotator& NewRotation)
{
	if (MeshParentSceneComponent) {
		MeshParentSceneComponent->SetRelativeRotation(NewRotation);
		CaptureScenes();
	}
}

void AObscuraItemSceneCaptureActor::UpdateZoom_Implementation(const float NewZoom)
{
	
	if (MeshZoomSceneComponent) {
		MeshZoomSceneComponent->SetRelativeLocation(FVector(NewZoom, 0.0f, 0.0f));
		CaptureScenes();
	}
}

void AObscuraItemSceneCaptureActor::ResetRotation_Implementation()
{
}

void AObscuraItemSceneCaptureActor::ResetZoom_Implementation()
{
}

