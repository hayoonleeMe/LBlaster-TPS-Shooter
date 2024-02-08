// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LBTypes/WeaponTypes.h"
#include "CombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LBLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FORCEINLINE bool IsEquippingWeapon() const { return EquippingWeapon != nullptr; }
	FORCEINLINE bool IsAiming() const { return bIsAiming; }
	FORCEINLINE class AWeapon* GetEquippingWeapon() const { return EquippingWeapon; }
	EWeaponType GetEquippingWeaponType() const;
	void EquipWeapon(class AWeapon* InWeapon);	
	void SetAiming(bool bInAiming);

protected:
	virtual void BeginPlay() override;

private:
	/*
	 *	Weapon
	 */
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippingWeapon;

	UFUNCTION()
	void OnRep_EquippedWeapon();
	
	/*
	 *	Aiming
	 */
	UPROPERTY(Replicated)
	uint8 bIsAiming : 1;

	UPROPERTY(EditAnywhere)
	float ADSMultiplier;

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bInAiming);
};
