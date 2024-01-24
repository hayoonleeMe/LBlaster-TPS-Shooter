// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class LBLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void ShowPickupWidget(bool bInShow) const;
	void SetWeaponState(EWeaponState InWeaponState);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	virtual void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
	
private:
	/*
	 *	Mesh & Overlap Sphere
	 */
	UPROPERTY(VisibleAnywhere, Category="Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category="Weapon")
	TObjectPtr<class USphereComponent> AreaSphere;

	/*
	 *	Pickup Widget
	 */
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<class UWidgetComponent> PickupWidgetComponent;
	
	/*
	 *	Weapon State
	 */
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category="Weapon")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();
	void OnChangedWeaponState();
};
