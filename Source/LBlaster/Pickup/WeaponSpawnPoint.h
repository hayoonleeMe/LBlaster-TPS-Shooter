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

	/*
	 *	Spawn Pickup
	 */
	virtual void SpawnPickup() override;

	void StartSpawnWeaponPickupTimer(AActor* WeaponPickup);
};
