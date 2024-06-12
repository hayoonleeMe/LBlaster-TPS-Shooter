// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "WeaponPickup.generated.h"

DECLARE_DELEGATE(FOnWeaponPickupEquipped);

UCLASS()
class LBLASTER_API AWeaponPickup : public APickup
{
	GENERATED_BODY()

public:
	AWeaponPickup();
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void SetShouldRotate(bool bInShouldRotate) { bShouldRotate = bInShouldRotate; }
	FORCEINLINE FVector GetSpawnPosOffset() const { return SpawnPosOffset; }
	FORCEINLINE void SetParentLocation(const FVector& InLoc) { ParentLocation = InLoc; }

	FOnWeaponPickupEquipped OnWeaponPickupEquipped;

protected:
	virtual void OnWeaponEquipped(bool bInSelected);

private:
	bool bShouldRotate = false;

	UPROPERTY(EditAnywhere, Category="LBlaster|Pickup")
	FVector SpawnPosOffset = FVector::ZeroVector;

	FVector ParentLocation;
};
