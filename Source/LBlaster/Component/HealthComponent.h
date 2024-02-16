// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LBLASTER_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ReceiveDamage(float InDamage, AController* InstigatorController);
	void UpdateHUDHealth();

protected:
	virtual void BeginPlay() override;

private:
	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<class ALBlasterCharacter> OwnerCharacter;

	bool IsValidOwnerCharacter();
	
	/*
	 *	Health
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Health")
	float MaxHealth;

	UPROPERTY(ReplicatedUsing=OnRep_Health, VisibleAnywhere, Category="LBlaster|Health")
	float Health;

	UFUNCTION()
	void OnRep_Health();
};