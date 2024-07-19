// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/WeaponSpawnPoint.h"

#include "LBlaster.h"
#include "Pickup.h"
#include "WeaponPickup.h"
#include "Interface/LBCharacterPickupInterface.h"
#include "GameFramework/Character.h"

void AWeaponSpawnPoint::OnRep_SpawnedPickup()
{
	Super::OnRep_SpawnedPickup();
	
	ForceEquipNewOverlappingWeapon();
}

void AWeaponSpawnPoint::ForceEquipNewOverlappingWeapon() const
{
	if (SpawnedPickup && GetWorld())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (ILBCharacterPickupInterface* Interface = Cast<ILBCharacterPickupInterface>(PlayerController->GetCharacter()))
			{
				Interface->EquipNewOverlappingWeapon();
			}
		}
	}
}

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
		// 생성된 이후에 일정 시간 뒤에 습득할 수 있게
		SpawnedPickup->SetActorEnableCollision(false);
		SpawnedPickup->FinishSpawning(GetActorTransform());

		// Pickup을 생성하자마자 바로 획득하는 것을 방지하기 위해 딜레이
		FTimerHandle DelayTimerHandle;
		GetWorldTimerManager().SetTimer(DelayTimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			// 게임이 시작할 때 바로 착용되는 Default Weapon은 수행하지 않도록
			if (SpawnedPickup)
			{
				SpawnedPickup->SetActorEnableCollision(true);
			}
		}), MinimumPickupDelay, false);

		MulticastDeactivatePadLoadingParticle();
		ForceEquipNewOverlappingWeapon();
	}
}

void AWeaponSpawnPoint::StartSpawnWeaponPickupTimer(AActor* WeaponPickup)
{
	MulticastActivatePadLoadingParticle();
	GetWorld()->GetTimerManager().SetTimer(SpawnPickupTimer, this, &ThisClass::SpawnPickup, SpawnCooldownTime);
}
