// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Interface/InteractWithCrosshairInterface.h"
#include "Interface/LBCharacterWeaponInterface.h"
#include "Interface/HitReceiverInterface.h"
#include "LBlasterCharacter.generated.h"

UCLASS()
class LBLASTER_API ALBlasterCharacter : public ACharacter, public ILBCharacterWeaponInterface, public IInteractWithCrosshairInterface, public IHitReceiverInterface
{
	GENERATED_BODY()

public:
	ALBlasterCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	
protected:
	virtual void BeginPlay() override;

public:
	/*
	 *	ULBCharacterWeaponInterface
	 */
	virtual void SetOverlappingWeapon(AWeapon* InWeapon) override;

	/*
	 *	ProjectileHitInterface
	 */
	virtual void SetLastHitNormal(const FVector& InHitNormal) override;
	virtual AController* GetController() override;

	/*
     *	LBlasterAnimInstance
     */
    bool IsAiming() const;
    bool IsFiring() const;
    FTransform GetLeftHandTransform() const;

	/*
	 *	Combat
	 */
	void AttachWeapon(AWeapon* InEquippedWeapon);
	void SetADSWalkSpeed(bool bEnabled, float InADSMultiplier);
	void SetWeaponAnimLayers(TSubclassOf<UAnimInstance> InWeaponAnimLayer);
	void PlayFireMontage(UAnimMontage* InFireMontage);

	/*
	 *	Camera
	 */
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/*
	 *	Elimination
	 */
	void Elim();
	
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
	
	void Move(const FInputActionValue& ActionValue);
	void Look(const FInputActionValue& ActionValue);
	void DoJump(const FInputActionValue& ActionValue);
	void EquipWeapon(const FInputActionValue& ActionValue);
	void DoCrouch(const FInputActionValue& ActionValue);
	void AimStarted(const FInputActionValue& ActionValue);
	void AimFinished(const FInputActionValue& ActionValue);
	void FireStarted(const FInputActionValue& ActionValue);
	void FireFinished(const FInputActionValue& ActionValue);

	/*
	 *	Damage
	 */
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);
	
private:
	/*
	 *	Movement
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Movement")
	float BaseMaxWalkSpeed;
	
	/*
	 *	Camera and SpringArm
	 */
	UPROPERTY(VisibleAnywhere, Category="LBlaster|Camera")
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category="LBlaster|Camera")
	TObjectPtr<class UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, Category="LBlaster|Camera")
	float MeshHideThreshold;
	
	void HideMeshIfCameraClose();

	/*
	 *	Overhead Widget
	 */
	UPROPERTY(VisibleAnywhere, Category="LBlaster|Widget")
	TObjectPtr<class UWidgetComponent> OverheadWidgetComponent;

	/*
	 *	Weapon
	 */
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	TObjectPtr<AWeapon> OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastOverlappingWeapon) const;
	void ShowOverlappingWeaponPickupWidget(AWeapon* LastOverlappingWeapon) const;

	/*
	 *	Combat
	 */
	UPROPERTY(VisibleAnywhere, Category="LBlaster|Combat")
	TObjectPtr<class UCombatComponent> CombatComponent;

	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon();

	/*
	 *	Animation
	 */
	UPROPERTY(EditDefaultsOnly, Category="LBlaster|Animation")
	TSubclassOf<UAnimInstance> BaseAnimLayerClass;

	/*
	 *	HitReact
	 */
	void PlayHitReactMontage(const FVector& HitNormal);

	UPROPERTY(ReplicatedUsing=OnRep_LastHitNormal)
	FVector LastHitNormal;

	UFUNCTION()
	void OnRep_LastHitNormal();

	/*
	 *	Health
	 */
	UPROPERTY(VisibleAnywhere, Category="LBlaster|Health")
	TObjectPtr<class UHealthComponent> HealthComponent;

	/*
	 *	Elimination
	 */
	void PlayDeathMontage(const FVector& HitNormal);
	void Ragdoll();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	UPROPERTY(EditDefaultsOnly, Category="LBlaster|Elim")
	float ElimDelay;
	
	FTimerHandle ElimTimer;
	void ElimTimerFinished();
};
