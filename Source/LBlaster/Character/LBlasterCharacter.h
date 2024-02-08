// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Interface/LBCharacterWeaponInterface.h"
#include "LBlasterCharacter.generated.h"

UCLASS()
class LBLASTER_API ALBlasterCharacter : public ACharacter, public ILBCharacterWeaponInterface
{
	GENERATED_BODY()

public:
	ALBlasterCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool IsAiming() const;	
	
protected:
	virtual void BeginPlay() override;

public:
	/*
	 *	ULBCharacterWeaponInterface
	 */
	virtual void SetOverlappingWeapon(class AWeapon* InWeapon) override;
	virtual void AttachWeapon(AWeapon* InEquippedWeapon) override;
	virtual void SetADSWalkSpeed(bool bEnabled, float InADSMultiplier) override;
	virtual void SetWeaponAnimLayers(TSubclassOf<UAnimInstance> InWeaponAnimLayer) override;

protected:
	/*
	 *	Input
	 */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> EquipAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> AimAction;
	
	void Move(const FInputActionValue& ActionValue);
	void Look(const FInputActionValue& ActionValue);
	void EquipWeapon(const FInputActionValue& ActionValue);
	void DoCrouch(const FInputActionValue& ActionValue);
	void AimStarted(const FInputActionValue& ActionValue);
	void AimFinished(const FInputActionValue& ActionValue);
	
private:
	/*
	 *	Movement
	 */
	UPROPERTY(EditAnywhere)
	float BaseMaxWalkSpeed;
	
	/*
	 *	Camera and SpringArm
	 */
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<class UCameraComponent> FollowCamera;

	/*
	 *	Overhead Widget
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widget", meta = (AllowPrivateAccess = "true"))
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
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCombatComponent> CombatComponent;

	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon();
};
