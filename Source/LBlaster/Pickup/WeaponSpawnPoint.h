// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup/PickupSpawnPoint.h"
#include "WeaponSpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API AWeaponSpawnPoint : public APickupSpawnPoint
{
	GENERATED_BODY()

protected:
	virtual void OnRep_SpawnedPickup() override;
	void ForceEquipNewOverlappingWeapon() const;

private:
	/*
	 *	Spawn Pickup
	 */
	virtual void SpawnPickup() override;
	void StartSpawnWeaponPickupTimer(AActor* WeaponPickup);
};
