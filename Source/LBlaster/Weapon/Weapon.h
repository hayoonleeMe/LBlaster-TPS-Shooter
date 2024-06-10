// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LBTypes/WeaponTypes.h"
#include "Weapon.generated.h"

USTRUCT()
struct FAmmoChange
{
	GENERATED_BODY()

	UPROPERTY()
	int32 AmmoToAdd;

	UPROPERTY()
	float Time;
};

USTRUCT()
struct FAmmoState
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Ammo;
	
	UPROPERTY()
	FAmmoChange LastAmmoChange;
};

USTRUCT()
struct FWeaponStateChange
{
	GENERATED_BODY()

	UPROPERTY()
	EWeaponState WeaponStateToChange;

	UPROPERTY()
	float Time;
};

USTRUCT()
struct FWeaponStateChangedState
{
	GENERATED_BODY()

	UPROPERTY()
	EWeaponState WeaponState;
	
	UPROPERTY()
	FWeaponStateChange LastWeaponStateChange;
};

UCLASS()
class LBLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetOwner(AActor* NewOwner) override;
	
	void ShowPickupWidget(bool bInShow) const;
	void ChangeWeaponState(EWeaponState InWeaponStateToChange);
	void SetHUDAmmo();
	void AddAmmo(int32 InAmmoToAdd);
	FORCEINLINE void SetSelected(bool bInSelected) { bSelected = bInSelected; }
	void SetWeaponVisibility(bool bInVisible);

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE TSubclassOf<UAnimInstance> GetWeaponAnimLayer() const { return WeaponAnimLayer; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetFireDelay() const { return FireDelay; }
	FORCEINLINE bool IsAutomatic() const { return bAutomatic; }
	FORCEINLINE bool IsAmmoEmpty() const { return Ammo <= 0.f; }
	FORCEINLINE int32 GetRoomInMag() const { return MagCapacity - Ammo; }
	FORCEINLINE USoundBase* GetEquipSound() const { return EquipSound; }
	FORCEINLINE bool NeedReload() const { return Ammo < MagCapacity; }
	FORCEINLINE float GetADSFOV() const { return ADSFOV; }
	FORCEINLINE USoundBase* GetDryFireSound() const { return DryFireSound; }
	FORCEINLINE const FTransform& GetAttachTransform() const { return AttachTransform; }
	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetHeadshotMultiplier() const { return HeadshotMultiplier; }
	FORCEINLINE float GetVerticalRecoilValue() const { return VerticalRecoilValue; }

	virtual void Fire(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const FVector& HitTarget);
	virtual void ShotgunFire(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const TArray<FVector_NetQuantize>& HitTargets) {}
	virtual bool DoesUseScatter() const { return bUseScatter; } 
	FVector TraceEndWithScatter(const FVector_NetQuantize& TraceStart, const FVector& HitTarget) const;
	virtual TArray<FVector_NetQuantize> ShotgunTraceEndWithScatter(const FVector_NetQuantize& TraceStart, const FVector& HitTarget) const { return TArray<FVector_NetQuantize>(); }
	bool GetMuzzleFlashLocation(FVector_NetQuantize& OutMuzzleFlashLocation, FRotator& OutMuzzleFlashRotation) const;

	void Dropped();
	void Holstered();
	void EnableCustomDepth(bool bEnable) const;
	float GetDamageFallOffMultiplier(float InDistance) const;

	virtual void CallServerScoreRequest(class ALBlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime, float InDamage, float InHeadshotMultiplier, float InProjectileGravityScale);

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
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<class ALBlasterCharacter> OwnerCharacter;

	bool IsValidOwnerCharacter();

	UPROPERTY()
	TObjectPtr<class ALBlasterPlayerController> OwnerController;

	bool IsValidOwnerController();

	bool bSelected = false;
	
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
	UPROPERTY(VisibleAnywhere, Category="LBlaster|Weapon State")
	EWeaponState WeaponState;

    UFUNCTION()
    void OnRep_WeaponState();
    void OnChangedWeaponState();

	void ProcessChangeWeaponState(EWeaponState InWeaponStateToChange);
	
	/*
	 *	Client-Side Prediction for Weapon State
	 */
	UPROPERTY(ReplicatedUsing=OnRep_ServerWeaponStateChangedState)
	FWeaponStateChangedState ServerWeaponStateChangedState;

	UFUNCTION()
	void OnRep_ServerWeaponStateChangedState();

	FWeaponStateChange CreateWeaponStateChange(EWeaponState InWeaponStateToChange);

	UFUNCTION(Server, Reliable)
	void ServerSendWeaponStateChange(const FWeaponStateChange& InWeaponStateChange);

	UPROPERTY()
	TArray<FWeaponStateChange> UnacknowledgedWeaponStateChanges;

	void ClearAcknowledgedWeaponStateChanges(const FWeaponStateChange& LastWeaponStateChange);

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
	 *	Recoil
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Recoil")
	float VerticalRecoilValue;

	/*
	 *	Scatter (Minute of Angle)
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Weapon Scatter")
	float DistanceToSphere;

	// 이 SphereRadius로 정의되는 Sphere의 랜덤한 지점으로 총알이 발사된다. 이 값이 작을수록 맞추기 쉽다. bUseScatter 옵션을 사용해야 적용된다.
	UPROPERTY(EditAnywhere, Category="LBlaster|Weapon Scatter")
	float MOA;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Weapon Scatter")
	uint8 bUseScatter : 1;

	float GetSphereRadius() const;

	/*
	 *	Ammo
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Ammo")
	int32 Ammo;

	void SpendRound();

	UPROPERTY(EditAnywhere, Category="LBlaster|Ammo")
	int32 MagCapacity;

	void ProcessAddAmmo(int32 InAmmoToAdd);

	/*
	 *	Client-Side Prediction for Ammo
	 */
	UPROPERTY(ReplicatedUsing=OnRep_ServerAmmoState)
	FAmmoState ServerAmmoState;

	UFUNCTION()
	void OnRep_ServerAmmoState();

	FAmmoChange CreateAmmoChange(int32 InAmmoToAdd);

	UFUNCTION(Server, Reliable)
	void ServerSendAmmoChange(const FAmmoChange& InAmmoChange);

	UPROPERTY()
	TArray<FAmmoChange> UnacknowledgedAmmoChanges;

	void ClearAcknowledgedAmmoChanges(const FAmmoChange& LastAmmoChange);

	/*
	 *	Sound
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Sound")
	TObjectPtr<USoundBase> EquipSound;

	UPROPERTY(EditAnywhere, Category="LBlaster|Sound")
	TObjectPtr<USoundBase> DryFireSound;

	/*
	 *	Damage
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Damage")
	float Damage;

	UPROPERTY(EditAnywhere, Category="LBlaster|Damage")
	float HeadshotMultiplier;

	/*
	 *	ADS FOV
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|ADS FOV")
	float ADSFOV;

	/*
	 *	Attach Transform
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Attach Transform")
	FTransform AttachTransform;
	
	/*
	 *	Damage FallOff
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Damage FallOff")
	TObjectPtr<UCurveFloat> DamageFallOffCurve;
};
