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
	void SetHUDAmmo();
	void AddAmmo(int32 InAmmo);

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE TSubclassOf<UAnimInstance> GetWeaponAnimLayer() const { return WeaponAnimLayer; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetFireDelay() const { return FireDelay; }
	FORCEINLINE bool IsAutomatic() const { return bAutomatic; }
	FORCEINLINE bool IsAmmoEmpty() const { return Ammo <= 0.f; }
	FORCEINLINE int32 GetRoomInMag() const { return MagCapacity - Ammo; }
	FORCEINLINE USoundBase* GetEquipSound() const { return EquipSound; }
	FORCEINLINE bool NeedReload() const { return Ammo < MagCapacity; }

	virtual void Fire(const FVector& HitTarget);
	void Dropped();

protected:
	virtual void BeginPlay() override;
	virtual void OnRep_Owner() override;

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

	UPROPERTY(EditAnywhere, Category="LBlaster|Pickup Widget")
	FVector LocOffset;
	
	/*
	 *	Weapon State
	 */
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category="LBlaster|Weapon State")
	EWeaponState WeaponState;

    UFUNCTION()
    void OnRep_WeaponState();
    void OnChangedWeaponState();

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
	
	/*
	 *	Auto Fire
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Auto Fire")
	uint8 bAutomatic : 1;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Auto Fire")
	float FireDelay;

	/*
	 *	Ammo
	 */
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Ammo, Category="LBlaster|Ammo")
	int32 Ammo;

	UFUNCTION()
	void OnRep_Ammo();

	void SpendRound();

	UPROPERTY(EditAnywhere, Category="LBlaster|Ammo")
	int32 MagCapacity;

	/*
	 *	Sound
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Sound")
	TObjectPtr<USoundBase> EquipSound;

	/*
	 *	Damage
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Damage")
	float Damage;

public:
	/*
	 *	Crosshair
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Crosshair")
	TObjectPtr<UTexture2D> CenterCrosshair;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Crosshair")
	TObjectPtr<UTexture2D> TopCrosshair;

	UPROPERTY(EditAnywhere, Category="LBlaster|Crosshair")
	TObjectPtr<UTexture2D> BottomCrosshair;

	UPROPERTY(EditAnywhere, Category="LBlaster|Crosshair")
	TObjectPtr<UTexture2D> LeftCrosshair;

	UPROPERTY(EditAnywhere, Category="LBlaster|Crosshair")
	TObjectPtr<UTexture2D> RightCrosshair;
};
