// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/WeaponSpawnPoint.h"

#include "LBlaster.h"
#include "Pickup.h"
#include "WeaponPickup.h"

void AWeaponSpawnPoint::SpawnPickup()
{
	if (PickupClasses.Num() > 0)
	{
		const int32 Index = FMath::RandRange(0, PickupClasses.Num() - 1);
		SpawnedPickup = GetWorld()->SpawnActorDeferred<APickup>(PickupClasses[Index], GetActorTransform());
		if (AWeaponPickup* SpawnedWeaponPickup = Cast<AWeaponPickup>(SpawnedPickup))
		{
			SpawnedWeaponPickup->OnWeaponPickupEquipped.BindUObject(this, &ThisClass::StartSpawnWeaponPickupTimer);
			SpawnedWeaponPickup->SetShouldRotate(true);
			SpawnedWeaponPickup->SetParentLocation(GetActorLocation());
			SpawnedWeaponPickup->SetActorLocation(GetActorLocation() + SpawnedWeaponPickup->GetSpawnPosOffset());
		}
		SpawnedPickup->FinishSpawning(GetActorTransform());

		MulticastDeactivatePadLoadingParticle();
	}
}

void AWeaponSpawnPoint::StartSpawnWeaponPickupTimer()
{
	MulticastActivatePadLoadingParticle();
	GetWorld()->GetTimerManager().SetTimer(SpawnPickupTimer, this, &ThisClass::SpawnPickup, SpawnCooldownTime);
}
