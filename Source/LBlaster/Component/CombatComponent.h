// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUD/LBlasterHUD.h"
#include "LBTypes/CombatState.h"
#include "LBTypes/CrosshairTexture.h"
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
	uint8 bPlayEquipMontage : 1;

	UPROPERTY()
	uint8 bShouldPlayUnarmedEquipMontage : 1;

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
	uint8 bPlayEquipMontage : 1;

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
	FORCEINLINE void TriggerAimingKey(bool bPressed) { bAimingKeyPressed = bPressed; }
	FORCEINLINE bool CanAnimateFiring() const { return bCanAnimateFiring; }
	FORCEINLINE bool IsReloading() const { return CombatState == ECombatState::ECS_Reloading; }
	FORCEINLINE class AWeapon* GetEquippingWeapon() { return GetEquippingWeapon(EquipSlotType); }
	AWeapon* GetEquippingWeapon(EEquipSlot InEquipSlotType);
	FTransform GetWeaponLeftHandTransform();
	FORCEINLINE int32 GetGrenadeAmount() const { return GrenadeAmount; }
	FORCEINLINE EEquipSlot GetEquipSlotType() const { return EquipSlotType; }

	UFUNCTION(Server, Reliable)
	void ServerEquipDefaultWeapon();

	void ChooseWeaponSlot(EEquipSlot InEquipSlotType);
	void EquipOverlappingWeapon(class AWeapon* InWeapon);
	// 현재 캐릭터와 Overlapping 중인 Weapon을 장착
	void EquipNewOverlappingWeapon();
	
	void SetAiming(bool bInAiming);
	void SetFiring(bool bInFiring);
	UAnimMontage* SelectHitReactMontage(const FVector& HitNormal);
	UAnimMontage* SelectDeathMontage(const FVector& HitNormal);
	UAnimMontage* SelectReloadMontage();
	void ElimWeapon();
	void ElimAllWeapon();
	void Reload();
	void ReloadFinished();
	void ShowSniperScopeWidget(bool bShowScope);
	void TossGrenade(bool bPressed);
	void TossGrenadeFinished();
	void StartTossGrenade(bool bShouldJumpToSection);
	void LaunchGrenade();
	void UpdateHUDGrenadeAmount();
	void PickupAmmo(EWeaponType InWeaponType, int32 InAmmoAmount);
	void EquipFinished();
	void HideCrosshair();
	void ShowCrosshair(EWeaponType InWeaponType);
	void DrawGrenadeTrajectory();

	FString GetCombatInfo();

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
	void ProcessEquipWeapon(EEquipSlot InEquipSlotType, EEquipMode InEquipMode, AWeapon* InWeaponToEquip, bool bPlayEquipMontage, bool bCanSendCombatStateRPC = true);
	void HolsterWeapon(EEquipSlot InEquipSlotType);

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

	void OnEquipDelayTimerFinished();

	UPROPERTY(EditAnywhere, Category="LBlaster|Weapon")
	float EquipDelay = 0.4f;

	bool bCanEquipWeapon = true;

	/*
	 *	Client-Side Prediction for Equip
	 */
	UPROPERTY(ReplicatedUsing=OnRep_ServerWeaponEquipState)
	FWeaponEquipState ServerWeaponEquipState;

	UFUNCTION()
	void OnRep_ServerWeaponEquipState();

	FWeaponEquip CreateWeaponEquip(EEquipSlot InEquipSlotType, EEquipMode InEquipMode, AWeapon* InWeaponToEquip, bool bInPlayEquipMontage);

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

	bool bAimingKeyPressed = false;
	
	/*
	 *	Firing
	 */
	UPROPERTY(ReplicatedUsing=OnRep_IsFiring)
	uint8 bIsFiring : 1;

	UFUNCTION()
	void OnRep_IsFiring();

	UFUNCTION(Server, Reliable)
	void ServerSetFiring(bool bInFiring, bool bInCanAnimateFiring);

	// Locally Controlled 캐릭터에서 실제로 조준하는지
	bool bDesiredIsFiring = false;

	bool CanFire();	
	void Fire();
	void LocalFire(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const FVector_NetQuantize& HitTarget);
	void ShotgunLocalFire(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const TArray<FVector_NetQuantize>& HitTargets);	
	
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const FVector_NetQuantize& HitTarget, bool bEnabledSSR);

	UFUNCTION(Server, Reliable)
	void ServerShotgunFire(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const TArray<FVector_NetQuantize>& HitTargets, bool bEnabledSSR);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const FVector_NetQuantize& HitTarget);

	UFUNCTION(NetMulticast, Reliable)
    void MulticastShotgunFire(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const TArray<FVector_NetQuantize>& HitTargets);

	void TraceUnderCrosshair(FHitResult& TraceHitResult);

	FVector TraceHitTarget;

	UPROPERTY(EditAnywhere, Category = "LBlaster|Firing")
	TMap<EWeaponType, UAnimMontage*> FireMontages;

	void StartDryFireTimer();
	bool CanDryFire();
	void DryFire();

	bool CanReloadOnFire();

	bool bCanAnimateFiring = false;

	/*
	 *	Crosshair
	 */
	bool bShowCrosshair = true;

	void SetHUDCrosshair(EWeaponType InWeaponType);
	void UpdateHUDCrosshair(float DeltaTime);
	void SetHUDCrosshairColor(AActor* TracedActor);
	void SetHUDPlayerNameTextUnderCrosshair(const FString& InPlayerName = FString());

	float CrosshairSpread;
	FLinearColor CrosshairColor;
	
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	UPROPERTY(EditAnywhere, Category="LBlaster|Crosshair")
	FCrosshairTexture DefaultCrosshair;

	UPROPERTY(EditAnywhere, Category="LBlaster|Crosshair")
	FCrosshairTexture ShotgunCrosshair;

	FCrosshairTexture GetCrosshairTexture(EWeaponType InWeaponType) const;

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
	UPROPERTY()
	bool bCanFire;
	
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

	/*
	 *	Combat State
	 */
	UPROPERTY(VisibleAnywhere, Category = "LBlaster|Combat State")
	ECombatState CombatState;

	void ChangeCombatState(ECombatState InCombatStateToChange, bool bPlayEquipMontage = true, bool bShouldPlayUnarmedEquipMontage = false, bool bCanSendCombatStateRPC = true);

	void ProcessChangeCombatState(ECombatState InCombatStateToChange, bool bPlayEquipMontage = true, bool bShouldPlayUnarmedEquipMontage = false);
	void OnChangedCombatState(bool bPlayEquipMontage, bool bShouldPlayUnarmedEquipMontage);

	/*
	 *	Client-Side Prediction for CombatState
	 */
	UPROPERTY(ReplicatedUsing=OnRep_ServerCombatStateChangedState)
	FCombatStateChangedState ServerCombatStateChangedState;

	UFUNCTION()
	void OnRep_ServerCombatStateChangedState();
	
	FCombatStateChange CreateCombatStateChange(ECombatState InCombatStateToChange, bool bInPlayEquipMontage, bool bInShouldPlayUnarmedEquipMontage);

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
	float MaxGrenadeThrowDistance = 1300.f;

	UPROPERTY(EditAnywhere, Category="LBlaster|Grenade")
	float GrenadePathArcValue = 0.8f;

	UPROPERTY(EditAnywhere, Category="LBlaster|Grenade")
	int32 MaxGrenadeAmount;

	UPROPERTY(ReplicatedUsing=OnRep_GrenadeAmount)
	int32 GrenadeAmount;

	UFUNCTION()
	void OnRep_GrenadeAmount();

	UFUNCTION(Server, Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize& LaunchLocation, const FVector_NetQuantize& LaunchVelocity);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLaunchGrenade(const FVector_NetQuantize& LaunchLocation, const FVector_NetQuantize& LaunchVelocity);

	void LocalLaunchGrenade(const FVector_NetQuantize& LaunchLocation, const FVector_NetQuantize& LaunchVelocity);
	void HandleUnEquipBeforeTossGrenade();
	void ShowAttachedGrenade(bool bShow);

	bool bDrawGrenadeTrajectory = false;
	bool bCanLaunchGrenade = false;
	
	FVector GrenadeLaunchLocation;
	FVector GrenadeLaunchVelocity;

	UPROPERTY()
	TArray<TObjectPtr<class USplineMeshComponent>> SplineMeshes;

	void HideAllSplineMesh();

	UPROPERTY(EditAnywhere, Category="LBlaster|Grenade")
	TObjectPtr<UStaticMesh> GrenadeTrajectorySM;
	
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> GrenadeTrajectoryPointMesh;

	UPROPERTY(EditAnywhere, Category="LBlaster|Grenade")
	TObjectPtr<UStaticMesh> GrenadeTrajectoryPointSM;
};
