// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LBLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(class AWeapon* InWeapon);
	FORCEINLINE bool IsEquippedWeapon() const { return EquippedWeapon != nullptr; }
	FORCEINLINE bool IsAiming() const { return bIsAiming; }
	void SetAiming(bool bInAiming);

protected:
	virtual void BeginPlay() override;

private:
	/*
	 *	Weapon
	 */
	UPROPERTY(Replicated)
	TObjectPtr<AWeapon> EquippedWeapon;

	/*
	 *	Aiming
	 */
	UPROPERTY(Replicated)
	uint8 bIsAiming : 1;

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bInAiming);
};
