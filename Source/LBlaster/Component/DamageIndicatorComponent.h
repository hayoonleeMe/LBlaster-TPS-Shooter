// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageIndicatorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LBLASTER_API UDamageIndicatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDamageIndicatorComponent();
	
	void RequestDamageIndication(float InDamage);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<class UDamageIndicator> DamageIndicator;
	
	FTimerHandle ClearTimerHandle;
	float ClearDelay = 3.f;

	float AccumulatedDamage = 0.f;
};
