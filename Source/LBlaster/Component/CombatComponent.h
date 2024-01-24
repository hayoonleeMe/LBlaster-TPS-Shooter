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

	void EquipWeapon(class AWeapon* InWeapon);

protected:
	virtual void BeginPlay() override;

private:
	/*
	 *	Weapon
	 */
	UPROPERTY()
	TObjectPtr<AWeapon> EquippedWeapon;
};
