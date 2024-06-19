// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Interface/HitReceiverInterface.h"
#include "Interface/LBCharacterPickupInterface.h"
#include "Player/LBlasterPlayerState.h"
#include "LBlasterCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS()
class LBLASTER_API ALBlasterCharacter : public ACharacter, public IHitReceiverInterface, public ILBCharacterPickupInterface
{
	GENERATED_BODY()

public:
	ALBlasterCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void OnRep_PlayerState() override;
	
protected:
	virtual void BeginPlay() override;

public:
	/*
	 *	ProjectileHitInterface
	 */
	virtual void SetLastHitNormal(const FVector& InHitNormal) override;

	/*
	 *	ILBCharacterPickupInterface
	 */
	virtual void PickupAmmo(EWeaponType InWeaponType, int32 InAmmoAmount) override;
	virtual void FindNearestOverlappingWeapon() override;

	/*
     *	LBlasterAnimInstance
     */
    bool IsAiming() const;
	bool CanAnimateFiring() const;
	bool IsReloading() const;
	bool IsEquippingWeapon() const;
	void ReloadFinished() const;
	void TossGrenadeFinished() const;
	void LaunchGrenade() const;
	FORCEINLINE const FVector2D& GetMovementVector() const { return MovementVector; };
	FTransform GetWeaponLeftHandTransform() const;
	void StartTossGrenade() const;
	void EquipFinished() const;

	/*
	 *	LBlasterPlayerController
	 */
	void UpdateHUDHealth() const;
	int32 GetGrenadeAmount() const;
	void UpdatePlayerNameToOverheadWidget();
	void EquipDefaultWeapon() const;
	void ReleaseCombatState() const;
	FORCEINLINE void SetInvincible(bool bInInvincible) { bInvincible = bInInvincible; }
	FORCEINLINE float GetInvincibilityTime() const { return InvincibilityTime; }

	/*
	 *	LBlasterPlayerState
	 */
	void SetCharacterMaterialsByTeam(const FTeamCharacterMaterials& TeamCharacterMaterials);
	
	/*
	 *	Combat
	 */
	void SetADSWalkSpeed(bool bEnabled, float InADSMultiplier);
	void SetWeaponAnimLayers(EWeaponType InWeaponType, TSubclassOf<UAnimInstance> InWeaponAnimLayer = nullptr);
	void PlayFireMontage(UAnimMontage* InFireMontage);
	void PlayReloadMontage(UAnimMontage* InReloadMontage);
	void PlayTossGrenadeMontage(UAnimMontage* InTossGrenadeMontage);
	void PlayEquipMontage(UAnimMontage* InEquipMontage);
	void SetBlendWeight(float InWeight) const;
	void SetAdsFov(float InAdsFov) const;
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	FORCEINLINE class AWeapon* GetOverlappingWeapon() const { return OverlappingWeapon; }
	AWeapon* GetEquippingWeapon() const;

	UFUNCTION(BlueprintCallable)
	FString GetCombatInfo();

	/*
	 *	Weapon
	 */
	void SetOverlappingWeapon(AWeapon* InWeapon, bool bBegin);
	void SetHUDAmmo(int32 InAmmo);

	/*
	 *	Elimination
	 */
	void Elim(bool bPlayerLeftGame);
	FORCEINLINE bool IsDead() const { return bIsDead; }

	/*
	 *	Lag Compensation
	 */
	FORCEINLINE TMap<FName, class UBoxComponent*> GetHitCollisionBoxes() const { return HitCollisionBoxes; }
	FORCEINLINE class ULagCompensationComponent* GetLagCompensationComponent() const { return LagCompensationComponent; }
	bool IsServerSideRewindEnabled() const;
	void EnableServerSideRewind(bool bInEnabled) const;

	/*
	 *	Mouse Sensitivity
	 */
	void SetXAxisSensitivityFromUserSettings(float InXAxisSensitivity);
	void SetYAxisSensitivityFromUserSettings(float InYAxisSensitivity);

protected:
	/*
	 *	Input
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<class UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputAction> EquipAction;

	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputAction> AimAction;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
    TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputAction> ReloadAction;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputAction> TossGrenadeAction;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputAction> FirstWeaponSlotAction;

	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputAction> SecondWeaponSlotAction;

	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
    TObjectPtr<UInputAction> ThirdWeaponSlotAction;
	
	void Move(const FInputActionValue& ActionValue);
	void Look(const FInputActionValue& ActionValue);
	void DoJump(const FInputActionValue& ActionValue);
	void EquipWeapon();
	void DoCrouch();
	void DoADS(const FInputActionValue& ActionValue);
	void DoFire(const FInputActionValue& ActionValue);
	void Reload();
	void TossGrenade();
	void ChooseFirstWeaponSlot();
	void ChooseSecondWeaponSlot();
	void ChooseThirdWeaponSlot();

	/*
	 *	Damage
	 */
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);
	
private:
	/*
	 *	Core
	 */
	bool bFirstTimeInit = false;

	void PollInit();
	
	/*
	 *	Movement
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Movement")
	float BaseMaxWalkSpeed;

	FVector2D MovementVector;
	
	/*
	 *	Camera
	 */
	UPROPERTY(VisibleAnywhere, Category="LBlaster|Camera")
	TObjectPtr<class ULBlasterCameraComponent> CameraComponent;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Camera")
	float MeshHideThreshold;

	void HideMeshIfCameraClose();

	/*
	 *	Overhead Widget
	 */
	UPROPERTY(VisibleAnywhere, Category="LBlaster|Widget")
	TObjectPtr<class UWidgetComponent> OverheadWidgetComponent;

	void UpdateOverheadWidgetTransform();

	/*
	 *	Weapon
	 */
	UPROPERTY()
	TObjectPtr<AWeapon> OverlappingWeapon;

	/*
	 *	Grenade
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> AttachedGrenade;

	/*
	 *	Combat
	 */
	UPROPERTY(VisibleAnywhere, Category="LBlaster|Combat")
	TObjectPtr<class UCombatComponent> CombatComponent;

	/*
	 *	Animation
	 */
	UPROPERTY(EditDefaultsOnly, Category="LBlaster|Animation")
	TSubclassOf<UAnimInstance> BaseAnimLayerClass;

	/*
	 *	HitReact
	 */
	void PlayHitReactMontage(const FVector& HitNormal) const;

	UPROPERTY()
	FVector LastHitNormal;

	/*
	 *	Health
	 */
	UPROPERTY(VisibleAnywhere, Category="LBlaster|Health")
	TObjectPtr<class UHealthComponent> HealthComponent;

	/*
	 *	Invincibility
	 */
	bool bInvincible = false;

	UPROPERTY(EditAnywhere, Category="LBlaster|Invincibility")
	float InvincibilityTime;
	
	/*
	 *	Elimination
	 */
	void PlayDeathMontage(const FVector& HitNormal);
	void Ragdoll();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim(bool bPlayerLeftGame);

	UPROPERTY(EditDefaultsOnly, Category="LBlaster|Elim")
	float ElimDelay;

	UPROPERTY(EditDefaultsOnly, Category="LBlaster|Elim")
	float RespawnTimerUpdateFrequency;
	
	FTimerHandle ElimTimer;
	void ElimTimerFinished();

	uint8 bIsDead : 1;

	/*
	 *	Dissolve Effect
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> DissolveTimelineComponent;

	UPROPERTY(EditAnywhere, Category="LBlaster|Dissolve Effect")
	TObjectPtr<UCurveFloat> DissolveCurve;

	FOnTimelineFloat DissolveTrack;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();

	UPROPERTY(EditAnywhere, Category="LBlaster|Dissolve Effect")
	TArray<UMaterialInstance*> DissolveMaterialInstances;
	
	UPROPERTY(VisibleAnywhere, Category="LBlaster|Dissolve Effect")
	TArray<UMaterialInstanceDynamic*> DynamicDissolveMaterialInstances;

	/*
	 *	Leave Game
	 */
	bool bLeftGame = false;

	/*
	 *	Lag Compensation
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class ULagCompensationComponent> LagCompensationComponent;

	UPROPERTY()
	TMap<FName, class UBoxComponent*> HitCollisionBoxes;

	/*
	 *	Hit boxes used for Server-Side Rewind
	 */
	void InitializeHitBoxes();
	
	#pragma region HitBoxes
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBoxComponent> head;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> pelvis;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> spine_03;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> spine_04;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> spine_05;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> upperarm_l;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> lowerarm_l;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> hand_l;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> upperarm_r;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> lowerarm_r;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> hand_r;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> thigh_l;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> calf_l;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> foot_l;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> thigh_r;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> calf_r;

	UPROPERTY(EditAnywhere)
    TObjectPtr<UBoxComponent> foot_r;
	#pragma endregion
};
