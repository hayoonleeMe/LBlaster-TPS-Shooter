// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "WeaponPickup.generated.h"

DECLARE_DELEGATE_OneParam(FOnWeaponPickupEquipped, AActor* /* WeaponPickup */);

UCLASS()
class LBLASTER_API AWeaponPickup : public APickup
{
	GENERATED_BODY()

public:
	AWeaponPickup();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FORCEINLINE void SetShouldRotate(bool bInShouldRotate) { bShouldRotate = bInShouldRotate; }
	FORCEINLINE FVector GetSpawnPosOffset() const { return SpawnPosOffset; }
	FORCEINLINE void SetParentLocation(const FVector& InLoc) { ParentLocation = InLoc; }

	FOnWeaponPickupEquipped OnWeaponPickupEquipped;

protected:
	virtual void OnWeaponEquipped(bool bInSelected);

private:
	UPROPERTY(Replicated)
	uint8 bShouldRotate : 1;

	UPROPERTY(Replicated)
	FVector_NetQuantize ParentLocation;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Pickup")
	FVector SpawnPosOffset = FVector::ZeroVector;

};
