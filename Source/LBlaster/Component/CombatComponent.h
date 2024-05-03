// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUD/LBlasterHUD.h"
#include "LBTypes/CombatState.h"
#include "LBTypes/WeaponTypes.h"
#include "LBTypes/EquipSlot.h"
#include "CombatComponent.generated.h"

USTRUCT()
struct FCarriedAmmoChange
{
	GENERATED_BODY()

	UPROPERTY()
	EWeaponType WeaponType;
	
	UPROPERTY()
	int32 CarriedAmmoToAdd;

	UPROPERTY()
	float Time;
};

USTRUCT()
struct FCarriedAmmoState
{
	GENERATED_BODY()

	UPROPERTY()
	EWeaponType WeaponType;

	UPROPERTY()
	int32 CarriedAmmo;

	UPROPERTY()
	FCarriedAmmoChange LastCarriedAmmoChange;
};

USTRUCT()
struct FCombatStateChange
{
	GENERATED_BODY()
	
	UPROPERTY()
	ECombatState CombatStateToChange;

	UPROPERTY()
	float Time;
};

USTRUCT()
struct FCombatStateChangedState
{
	GENERATED_BODY()

	UPROPERTY()
	ECombatState CombatState;

	UPROPERTY()
	FCombatStateChange LastCombatStateChange;
};

UENUM()
enum class EEquipMode : uint8
{
	EEM_ChooseWeaponSlot,
	EEM_OverlappingWeapon,
	EEM_UnarmedState
};

USTRUCT()
struct FWeaponEquip
{
	GENERATED_BODY()

	UPROPERTY()
	EEquipSlot SlotToEquip;
	
	UPROPERTY()
	TObjectPtr<class AWeapon> WeaponToEquip;

	UPROPERTY()
	EEquipMode EquipMode;

	UPROPERTY()
	float Time;
};

USTRUCT()
struct FWeaponEquipState
{
	GENERATED_BODY()

	UPROPERTY()
	EEquipSlot EquippingSlot;
	
	UPROPERTY()
	TObjectPtr<AWeapon> EquippingWeapon;

	UPROPERTY()
	EEquipMode EquipMode;

	UPROPERTY()
	FWeaponEquip LastWeaponEquip;
};

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
	FORCEINLINE class AWeapon* GetEquippingWeapon() { return GetEquippingWeapon(EquipSlotType); }
	AWeapon* GetEquippingWeapon(EEquipSlot InEquipSlotType);
	FTransform GetWeaponLeftHandTransform();
	FORCEINLINE int32 GetGrenadeAmount() const { return GrenadeAmount; }
	FORCEINLINE EEquipSlot GetEquipSlotType() const { return EquipSlotType; }

	UFUNCTION(Server, Reliable)
	void ServerEquipDefaultWeapon();

	void ChooseWeaponSlot(EEquipSlot InEquipSlotType);
	void EquipOverlappingWeapon();
	
	void SetAiming(bool bInAiming);
	void SetFiring(bool bInFiring);
	UAnimMontage* SelectHitReactMontage(const FVector& HitNormal);
	UAnimMontage* SelectDeathMontage(const FVector& HitNormal);
	UAnimMontage* SelectReloadMontage();
	void DropWeapon();
	void ElimWeapon();
	void Reload();
	void ReloadFinished();
	void ShowSniperScopeWidget(bool bShowScope);
	void TossGrenade();
	void TossGrenadeFinished();
	void LaunchGrenade();
	void UpdateHUDGrenadeAmount();
	void PickupAmmo(EWeaponType InWeaponType, int32 InAmmoAmount);
	void ShowWeapon();
	void StartTossGrenade();
	void EquipFinished();

protected:
	virtual void BeginPlay() override;

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
	TObjectPtr<ALBlasterHUD> HUD;

	bool IsValidHUD();
	
	/*
	 *	Weapon
	 */
	void EquipWeapon(EEquipSlot InEquipSlotType, EEquipMode InEquipMode, AWeapon* InWeaponToEquip = nullptr);
	void ProcessEquipWeapon(EEquipSlot InEquipSlotType, EEquipMode InEquipMode, AWeapon* InWeaponToEquip, bool bPlayEquipMontage = true);
	void HolsterWeapon(EEquipSlot InEquipSlotType);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSwitchToUnarmedState(bool bSkipUnEquipMontage);

	static FString GetWeaponTypeString(EWeaponType InWeaponType = EWeaponType::EWT_Unarmed);
	void AttachWeapon();

	void HandleEquip(EWeaponType InWeaponType);
	UAnimMontage* GetEquipMontage(EWeaponType InWeaponType);

	UPROPERTY(EditAnywhere, Category="LBlaster|Weapon")
	TMap<EWeaponType, UAnimMontage*> EquipMontages;

	UPROPERTY()
	EEquipSlot EquipSlotType;

	UPROPERTY()
	TArray<TObjectPtr<AWeapon>> EquipSlots;

	UPROPERTY(EditAnywhere, Category="LBlaster|Weapon")
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY(ReplicatedUsing=OnRep_DefaultWeapon)
	TObjectPtr<AWeapon> DefaultWeapon;

	UFUNCTION()
	void OnRep_DefaultWeapon();

	bool bEquipDefaultWeapon = false;

	FTimerHandle EquipDelayTimer;

	float EquipDelay = 0.3f;

	bool bCanEquipWeapon = true;

	/*
	 *	Client-Side Prediction for Equip
	 */
	UPROPERTY(ReplicatedUsing=OnRep_ServerWeaponEquipState)
	FWeaponEquipState ServerWeaponEquipState;

	UFUNCTION()
	void OnRep_ServerWeaponEquipState();

	FWeaponEquip CreateWeaponEquip(EEquipSlot InEquipSlotType, EEquipMode InEquipMode, AWeapon* InWeaponToEquip);

	UFUNCTION(Server, Reliable)
	void ServerSendWeaponEquip(const FWeaponEquip& InWeaponEquip);

	UPROPERTY()
	TArray<FWeaponEquip> UnacknowledgedWeaponEquips;

	void ClearAcknowledgedWeaponEquip(const FWeaponEquip& LastWeaponEquip);

	/*
	 *	Aiming
	 */
	UPROPERTY(ReplicatedUsing = "OnRep_IsAiming")
	uint8 bIsAiming : 1;

	UFUNCTION()
	void OnRep_IsAiming();

	UPROPERTY(EditAnywhere, Category="LBlaster|Aiming")
	float ADSMultiplier;

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bInAiming);

	// Locally Controlled 캐릭터에서 실제로 조준하는지
	bool bDesiredIsAiming = false;
	
	/*
	 *	Firing
	 */
	UPROPERTY(Replicated)
	uint8 bIsFiring : 1;

	UFUNCTION(Server, Reliable)
	void ServerSetFiring(bool bInFiring);

	bool CanFire();	
	void Fire();
	void LocalFire(const FVector_NetQuantize& HitTarget);
	void ShotgunLocalFire(const TArray<FVector_NetQuantize>& HitTargets);	
	
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& HitTarget, bool bEnabledSSR);

	bool bDelayedFire = false;
	FVector_NetQuantize CachedHitTarget;
	TArray<FVector_NetQuantize> CachedHitTargets;
	bool bCachedEnabledSSR;

	UFUNCTION(Server, Reliable)
	void ServerShotgunFire(const TArray<FVector_NetQuantize>& HitTargets, bool bEnabledSSR);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& HitTarget);

	UFUNCTION(NetMulticast, Reliable)
    void MulticastShotgunFire(const TArray<FVector_NetQuantize>& HitTargets);

	void TraceUnderCrosshair(FHitResult& TraceHitResult);

	FVector TraceHitTarget;

	UPROPERTY(EditAnywhere, Category = "LBlaster|Firing")
	TMap<EWeaponType, UAnimMontage*> FireMontages;

	void StartDryFireTimer();
	bool CanDryFire();
	void DryFire();

	/*
	 *	Crosshair
	 */
	void SetHUDCrosshair(float DeltaTime);

	FHUDPackage HUDPackage;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	/*
	 *	Sniper Scope
	 */
	void InitSniperScope();

	UPROPERTY(EditAnywhere, Category="LBlaster|Sniper Scope")
	TObjectPtr<USoundBase> SniperScopeZoomInSound;

	UPROPERTY(EditAnywhere, Category="LBlaster|Sniper Scope")
	TObjectPtr<USoundBase> SniperScopeZoomOutSound;

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
	 *	Carried Ammo
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Ammo")
	TMap<EWeaponType, int32> CarriedAmmoMap;

	void AddCarriedAmmo(EWeaponType InWeaponTypeToAdd, int32 InCarriedAmmoToAdd);

	UPROPERTY(EditAnywhere, Category="LBlaster|Ammo")
	TMap<EWeaponType, int32> MaxCarriedAmmoMap;

	void ProcessAddCarriedAmmo(EWeaponType InWeaponTypeToAdd, int32 InCarriedAmmoToAdd);

	/*
	 *	Client-Side Prediction for Carried Ammo
	 */
	UPROPERTY(ReplicatedUsing=OnRep_ServerCarriedAmmoState)
	FCarriedAmmoState ServerCarriedAmmoState;

	UFUNCTION()
	void OnRep_ServerCarriedAmmoState();
	
	FCarriedAmmoChange CreateCarriedAmmoChange(EWeaponType InWeaponType, int32 InCarriedAmmoToAdd);

	UFUNCTION(Server, Reliable)
	void ServerSendCarriedAmmoChange(const FCarriedAmmoChange& InCarriedAmmoChange);

	UPROPERTY()
	TArray<FCarriedAmmoChange> UnacknowledgedCarriedAmmoChanges;

	void ClearAcknowledgedCarriedAmmoChanges(const FCarriedAmmoChange& LastCarriedAmmoChange);

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

	UFUNCTION(NetMulticast, Reliable)
	void MulticastInterruptMontage();

	/*
	 *	Combat State
	 */
	UPROPERTY(VisibleAnywhere, Category = "LBlaster|Combat State")
	ECombatState CombatState;

	void ChangeCombatState(ECombatState InCombatStateToChange);

	void ProcessChangeCombatState(ECombatState InCombatStateToChange);

	/*
	 *	Client-Side Prediction for CombatState
	 */
	UPROPERTY(ReplicatedUsing=OnRep_ServerCombatStateChangedState)
	FCombatStateChangedState ServerCombatStateChangedState;

	UFUNCTION()
	void OnRep_ServerCombatStateChangedState();
	
	FCombatStateChange CreateCombatStateChange(ECombatState InCombatStateToChange);

	UFUNCTION(Server, Reliable)
	void ServerSendCombatStateChange(const FCombatStateChange& InCombatStateChange);

	UPROPERTY()
	TArray<FCombatStateChange> UnacknowledgedCombatStateChanges;

	void ClearAcknowledgedCombatStateChanges(const FCombatStateChange& LastCombatStateChange);
	
	/*
	 *	Grenade
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Grenade")
	TObjectPtr<UAnimMontage> UnEquipBeforeTossGrenadeMontage;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Grenade")
	TObjectPtr<UAnimMontage> TossGrenadeMontage;

	UPROPERTY(EditAnywhere, Category="LBlaster|Grenade")
	TSubclassOf<class AProjectile> GrenadeClass;

	UPROPERTY(EditAnywhere, Category="LBlaster|Grenade")
	int32 MaxGrenadeAmount;

	UPROPERTY(Replicated)
	int32 GrenadeAmount;

	UFUNCTION(Server, Reliable)
	void ServerTossGrenade();

	UFUNCTION(Server, Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize& HitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLaunchGrenade(const FVector_NetQuantize& HitTarget);

	void LocalLaunchGrenade(const FVector_NetQuantize& HitTarget);
	void HandleTossGrenade();
	void HandleUnEquipBeforeTossGrenade();
	void ShowAttachedGrenade(bool bShow);
};
