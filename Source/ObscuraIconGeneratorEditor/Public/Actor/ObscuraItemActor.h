// Nightfall Protocol 2024 Mortal Machines Studio. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObscuraItemActor.generated.h"

UCLASS(DisplayName="Obscura Item Actor")
class OBSCURAICONGENERATOREDITOR_API AObscuraItemActor : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AObscuraItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Component")
	TObjectPtr<UStaticMeshComponent> ItemStaticMesh = nullptr;

public:
};
