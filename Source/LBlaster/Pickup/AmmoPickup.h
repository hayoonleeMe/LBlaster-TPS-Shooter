// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBTypes/WeaponTypes.h"
#include "Pickup/Pickup.h"
#include "AmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API AAmmoPickup : public APickup
{
	GENERATED_BODY()

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category="LBlaster|Ammo")
	int32 AmmoAmount;

	UPROPERTY(EditAnywhere, Category="LBlaster|Ammo")
	EWeaponType WeaponType;
};
