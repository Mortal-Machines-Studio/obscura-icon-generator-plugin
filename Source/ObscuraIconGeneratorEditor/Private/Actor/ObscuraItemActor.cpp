// Nightfall Protocol 2024 Mortal Machines Studio. All Rights Reserved


#include "Actor/ObscuraItemActor.h"


// Sets default values
AObscuraItemActor::AObscuraItemActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Item Static Mesh");
	SetRootComponent(ItemStaticMesh); 
}

// Called when the game starts or when spawned
void AObscuraItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

