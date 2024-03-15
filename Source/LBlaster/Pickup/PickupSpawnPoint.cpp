// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/PickupSpawnPoint.h"

#include "Pickup.h"

APickupSpawnPoint::APickupSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	/* Spawn Pickup */
	SpawnTime = 5.f;

	/* Pad Mesh */
	PadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pad Mesh"));
	PadMesh->SetupAttachment(RootComponent);
	PadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PadMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/LBlaster/Items/Pickups/Mesh/SM_launchpad_Round.SM_launchpad_Round'"));
	if (PadMeshRef.Object)
	{
		PadMesh->SetStaticMesh(PadMeshRef.Object);
	}
}

void APickupSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	// 서버에서만 생성
	if (HasAuthority())
	{
		SpawnPickup();
	}
}

void APickupSpawnPoint::SpawnPickup()
{
	if (PickupClasses.Num() > 0)
	{
		const int32 Index = FMath::RandRange(0, PickupClasses.Num() - 1);
		SpawnedPickup = GetWorld()->SpawnActorDeferred<APickup>(PickupClasses[Index], GetActorTransform());
		SpawnedPickup->OnDestroyed.AddDynamic(this, &ThisClass::StartSpawnPickupTimer);
		SpawnedPickup->FinishSpawning(GetActorTransform());
	}
}

void APickupSpawnPoint::StartSpawnPickupTimer(AActor* DestroyedActor)
{
	GetWorld()->GetTimerManager().SetTimer(SpawnPickupTimer, this, &ThisClass::SpawnPickup, SpawnTime);
}



