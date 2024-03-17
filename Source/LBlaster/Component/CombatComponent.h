// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUD/LBlasterHUD.h"
#include "LBTypes/CombatState.h"
#include "LBTypes/WeaponTypes.h"
#include "CombatComponent.generated.h"

UENUM(BlueprintType)
enum class EEquipSlot : uint8
{
	EES_FirstSlot UMETA(DisplayName = "First Slot"),
	EES_SecondSlot UMETA(DisplayName = "Second Slot"),
	EES_ThirdSlot UMETA(DisplayName = "Third Slot"),
	EES_MAX UMETA(DisplayName = "DefaultMax")
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
	FORCEINLINE class AWeapon* GetEquippingWeapon() { return EquipSlots[static_cast<int8>(EquipSlotType)]; }
	FTransform GetWeaponLeftHandTransform();
	FORCEINLINE int32 GetGrenadeAmount() const { return GrenadeAmount; }

	UFUNCTION(Server, Reliable)
	void ServerEquipOverlappingWeapon();

	UFUNCTION(Server, Reliable)
	void ServerEquipDefaultWeapon();
	
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

	UFUNCTION(Server, Reliable)
	void ServerChooseWeaponSlot(EEquipSlot InEquipSlotType);

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
	void EquipWeapon(AWeapon* InWeapon);
	void SwapWeapon(AWeapon* InWeapon);
	void HolsterWeapon();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSwitchToUnarmedState();

	static FString GetWeaponTypeString(EWeaponType InWeaponType = EWeaponType::EWT_Unarmed);
	void AttachWeapon();
	void AttachWeaponToLeftHand();

	UPROPERTY(EditAnywhere, Category="LBlaster|Weapon")
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY(Replicated)
	EEquipSlot EquipSlotType;

	UPROPERTY(ReplicatedUsing="OnRep_EquipSlots")
	TArray<AWeapon*> EquipSlots;

	UFUNCTION()
    void OnRep_EquipSlots();

	void SetEquippingWeapon(AWeapon* InWeapon);

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

	bool CanFire();	
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

	void StartDryFireTimer();
	bool CanDryFire();

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
	 *	Ammo
	 */
	UPROPERTY(ReplicatedUsing=OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	UPROPERTY(EditAnywhere, Category="LBlaster|Ammo")
	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere, Category="LBlaster|Ammo")
	TMap<EWeaponType, int32> MaxCarriedAmmoMap;

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
	UPROPERTY(ReplicatedUsing=OnRep_CombatState)
	ECombatState CombatState;

	UFUNCTION()
	void OnRep_CombatState();

	/*
	 *	Grenade
	 */
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

	void HandleTossGrenade();
	void ShowAttachedGrenade(bool bShow);
};
