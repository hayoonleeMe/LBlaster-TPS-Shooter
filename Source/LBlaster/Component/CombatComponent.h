// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUD/LBlasterHUD.h"
#include "LBTypes/CombatState.h"
#include "LBTypes/WeaponTypes.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000.f;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LBLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE bool IsAiming() const { return bIsAiming; }
	FORCEINLINE bool IsFiring() const { return bIsFiring; }
	FORCEINLINE bool IsReloading() const { return CombatState == ECombatState::ECS_Reloading; }
	FORCEINLINE class AWeapon* GetEquippingWeapon() const { return EquippingWeapon; }
	void EquipWeapon(AWeapon* InWeapon);	
	void SetAiming(bool bInAiming);
	void SetFiring(bool bInFiring);
	UAnimMontage* SelectHitReactMontage(const FVector& HitNormal);
	UAnimMontage* SelectDeathMontage(const FVector& HitNormal);
	UAnimMontage* SelectReloadMontage();
	void DropWeapon();
	void Reload();
	void ReloadFinished();

private:
	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<class ALBlasterCharacter> OwnerCharacter;

	bool IsValidOwnerCharacter();

	UPROPERTY()
	TObjectPtr<class ALBlasterPlayerController> OwnerController;

	bool IsValidOwnerController();

	UPROPERTY()
	TObjectPtr<class ALBlasterHUD> HUD;

	bool IsValidHUD();
	
	/*
	 *	Weapon
	 */
	UPROPERTY(ReplicatedUsing = OnRep_EquippingWeapon)
	TObjectPtr<AWeapon> EquippingWeapon;

	UFUNCTION()
	void OnRep_EquippingWeapon();

	static FString GetWeaponTypeString(EWeaponType InWeaponType);

	/*
	 *	Aiming
	 */
	UPROPERTY(Replicated)
	uint8 bIsAiming : 1;

	UPROPERTY(EditAnywhere, Category="LBlaster|Aiming")
	float ADSMultiplier;

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bInAiming);
	
	/*
	 *	Firing
	 */
	UPROPERTY(Replicated)
	uint8 bIsFiring : 1;

	bool CanFire() const;	
	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerSetFiring(bool bInFiring);
	
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshair(FHitResult& TraceHitResult);

	UPROPERTY(EditAnywhere, Category = "LBlaster|Firing")
	TMap<EWeaponType, UAnimMontage*> FireMontages;

	/*
	 *	Crosshair
	 */
	void SetHUDCrosshair(float DeltaTime);

	FHUDPackage HUDPackage;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	/*
	 *	HitReact
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|HitReact")
	TArray<UAnimMontage*> FrontHitReact;

	UPROPERTY(EditAnywhere, Category="LBlaster|HitReact")
	TArray<UAnimMontage*> BackHitReact;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|HitReact")
	TArray<UAnimMontage*> LeftHitReact;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|HitReact")
	TArray<UAnimMontage*> RightHitReact;

	/*
	 *	Auto Fire
	 */
	bool bCanFire = true;
	FTimerHandle FireTimer;
	
	void StartFireTimer();
	void FireTimerFinished();

	/*
	 *	Death
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Death")
	TArray<UAnimMontage*> FrontDeath;

	UPROPERTY(EditAnywhere, Category="LBlaster|Death")
	TArray<UAnimMontage*> BackDeath;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Death")
	TArray<UAnimMontage*> LeftDeath;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Death")
	TArray<UAnimMontage*> RightDeath;

	/*
	 *	Ammo
	 */
	UPROPERTY(ReplicatedUsing=OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	UPROPERTY(EditAnywhere, Category="LBlaster|Ammo")
	TMap<EWeaponType, int32> CarriedAmmoMap;

	/*
	 *	Reload
	 */
	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Reload")
	TMap<EWeaponType, UAnimMontage*> ReloadMontages;

	void UpdateAmmoValues();
	int32 AmountToReload(); 

	/*
	 *	Combat State
	 */
	UPROPERTY(ReplicatedUsing=OnRep_CombatState)
	ECombatState CombatState;

	UFUNCTION()
	void OnRep_CombatState();
};
