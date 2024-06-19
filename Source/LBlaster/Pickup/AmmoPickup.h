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

public:
	AAmmoPickup();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	/*
	 *	Mesh & Overlap Sphere
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USphereComponent> OverlapSphere;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	/*
	 *	Sound
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Sound")
	TObjectPtr<USoundBase> PickupSound;

	/*
	 *	Ammo
	 */
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Ammo")
	int32 AmmoAmount;

	UPROPERTY(EditAnywhere, Category="LBlaster|Ammo")
	EWeaponType WeaponType;
};
