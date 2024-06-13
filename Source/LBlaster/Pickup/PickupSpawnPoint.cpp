// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/PickupSpawnPoint.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Pickup.h"
#include "Net/UnrealNetwork.h"

APickupSpawnPoint::APickupSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	/* Spawn Pickup */
	SpawnCooldownTime = 5.f;

	/* Pad Mesh */
	PadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pad Mesh"));
	SetRootComponent(PadMesh);
	PadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PadMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/LBlaster/Items/Pickups/Mesh/SM_launchpad_Round.SM_launchpad_Round'"));
	if (PadMeshRef.Object)
	{
		PadMesh->SetStaticMesh(PadMeshRef.Object);
	}

	/* Pad Particle */
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> PadParticleRef(TEXT("/Script/Niagara.NiagaraSystem'/Game/LBlaster/Items/Pickups/FX/Particles/Item/NS_GunPad.NS_GunPad'"));
	if (PadParticleRef.Object)
	{
		PadParticle = PadParticleRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> PadPickupParticleRef(TEXT("/Script/Niagara.NiagaraSystem'/Game/LBlaster/Items/Pickups/FX/Particles/Item/NS_GunPad_Pickup.NS_GunPad_Pickup'"));
	if (PadPickupParticleRef.Object)
	{
		PadPickupParticle = PadPickupParticleRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> PadLoadingParticleRef(TEXT("/Script/Niagara.NiagaraSystem'/Game/LBlaster/Items/Pickups/FX/Particles/Item/NS_GunPad_Loading.NS_GunPad_Loading'"));
	if (PadLoadingParticleRef.Object)
	{
		PadLoadingParticle = PadLoadingParticleRef.Object;
	}

	PadColor = FLinearColor(1.f, 0.8f, 0.3f, 0.1f);
}

void APickupSpawnPoint::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Update Cooldown Timer Particle
	if (PadLoadingParticleComponent && PadLoadingParticleComponent->IsActive())
	{
		const float CooldownRate = 1 - GetWorld()->GetTimerManager().GetTimerRemaining(SpawnPickupTimer) / SpawnCooldownTime;
		PadLoadingParticleComponent->SetFloatParameter(FName(TEXT("RibbonProgression")), CooldownRate);
	}
}

void APickupSpawnPoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickupSpawnPoint, SpawnedPickup);
}

void APickupSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	if (!RootComponent)
	{
		return;
	}

	if (PadParticle)
    {
    	PadParticleComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(PadParticle, RootComponent, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
		if (PadParticleComponent)
		{
			PadParticleComponent->SetColorParameter(FName(TEXT("GunPad_Color")), PadColor);
		}
    }
	if (PadPickupParticle)
	{
		PadPickupParticleComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(PadPickupParticle, RootComponent, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
		if (PadPickupParticleComponent)
		{
			PadPickupParticleComponent->SetColorParameter(FName(TEXT("GunPad_Color")), PadColor);
		}
	}
	if (PadLoadingParticle)
	{
		PadLoadingParticleComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(PadLoadingParticle, RootComponent, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false, false);
		if (PadLoadingParticleComponent)
		{
			PadLoadingParticleComponent->SetColorParameter(FName(TEXT("GunPad_Color")), PadColor);
		}
	}
	
	// 서버에서만 생성
	if (HasAuthority())
	{
		SpawnPickup();
	}
}

void APickupSpawnPoint::OnRep_SpawnedPickup()
{
}

void APickupSpawnPoint::SpawnPickup()
{
	if (PickupClasses.Num() > 0)
	{
		const int32 Index = FMath::RandRange(0, PickupClasses.Num() - 1);
		SpawnedPickup = GetWorld()->SpawnActorDeferred<APickup>(PickupClasses[Index], GetActorTransform());
		SpawnedPickup->OnDestroyed.AddDynamic(this, &ThisClass::StartSpawnPickupTimer);
		SpawnedPickup->FinishSpawning(GetActorTransform());

		MulticastDeactivatePadLoadingParticle();
	}
}

void APickupSpawnPoint::StartSpawnPickupTimer(AActor* DestroyedActor)
{
	MulticastActivatePadLoadingParticle();
	GetWorld()->GetTimerManager().SetTimer(SpawnPickupTimer, this, &ThisClass::SpawnPickup, SpawnCooldownTime);
}

void APickupSpawnPoint::MulticastActivatePadLoadingParticle_Implementation()
{
	if (PadLoadingParticleComponent)
	{
		PadLoadingParticleComponent->Activate();
	}

	// 클라이언트에서 Tick 로직 수행을 위해 Timer 재생
	if (!HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnPickupTimer, SpawnCooldownTime, false);
	}
}

void APickupSpawnPoint::MulticastDeactivatePadLoadingParticle_Implementation()
{
	if (PadLoadingParticleComponent)
	{
		PadLoadingParticleComponent->Deactivate();
	}
}
