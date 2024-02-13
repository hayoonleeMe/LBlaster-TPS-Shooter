// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LBTypes/WeaponTypes.h"
#include "Weapon.generated.h"

UCLASS()
class LBLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void ShowPickupWidget(bool bInShow) const;
	void SetWeaponState(EWeaponState InWeaponState);

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE TSubclassOf<UAnimInstance> GetWeaponAnimLayer() const { return WeaponAnimLayer; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	virtual void Fire(const FVector& HitTarget);

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

	/*
	 *	Weapon Type
	 */
	UPROPERTY(EditDefaultsOnly, Category="LBlaster|Weapon Type")
	EWeaponType WeaponType;

	/*
	 * Linked Anim Layer
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Linked Anim Layer")
	TSubclassOf<UAnimInstance> WeaponAnimLayer;
	
	/*
	 *	Mesh & Overlap Sphere
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USphereComponent> AreaSphere;

	/*
	 *	Pickup Widget
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UWidgetComponent> PickupWidgetComponent;
	
	/*
	 *	Weapon State
	 */
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category="LBlaster|Weapon State")
	EWeaponState WeaponState;

	/*
	 *	Animation
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Animation")
	TObjectPtr<UAnimationAsset> FireAnimation;

	/*
	 *	Casing
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Casing")
	TSubclassOf<class ACasing> CasingClass;

	UFUNCTION()
	void OnRep_WeaponState();
	void OnChangedWeaponState();

public:
	/*
	 *	Crosshair
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Crosshair")
	TObjectPtr<UTexture2D> TopCrosshair;

	UPROPERTY(EditAnywhere, Category="LBlaster|Crosshair")
	TObjectPtr<UTexture2D> BottomCrosshair;

	UPROPERTY(EditAnywhere, Category="LBlaster|Crosshair")
	TObjectPtr<UTexture2D> LeftCrosshair;

	UPROPERTY(EditAnywhere, Category="LBlaster|Crosshair")
	TObjectPtr<UTexture2D> RightCrosshair;

	UPROPERTY(EditAnywhere, Category="LBlaster|Crosshair")
	TObjectPtr<UTexture2D> CenterCrosshair;
};
