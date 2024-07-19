// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageIndicatorComponent.h"

#include "LBlaster.h"
#include "Character/LBlasterCharacter.h"
#include "Components/WidgetComponent.h"
#include "HUD/DamageIndicator.h"
#include "Kismet/GameplayStatics.h"


UDamageIndicatorComponent::UDamageIndicatorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDamageIndicatorComponent::RequestDamageIndication(float InDamage)
{
	if (DamageIndicator)
	{
		// Accumulate Damage
		if (GetWorld()->GetTimerManager().IsTimerActive(ClearTimerHandle))
		{
			AccumulatedDamage += InDamage;
		}
		// Clear Accumulated Damage
		else
		{
			AccumulatedDamage = InDamage;
		}
		DamageIndicator->SetDamageText(AccumulatedDamage, ClearDelay);
		GetWorld()->GetTimerManager().SetTimer(ClearTimerHandle, ClearDelay, false);
	}
}

void UDamageIndicatorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ALBlasterCharacter* OwnerCharacter = Cast<ALBlasterCharacter>(GetOwner()))
	{
		if (UWidgetComponent* DamageIndicatorWidgetComponent = OwnerCharacter->GetDamageIndicatorWidgetComponent())
		{
			DamageIndicatorWidgetComponent->InitWidget();
			DamageIndicator = Cast<UDamageIndicator>(DamageIndicatorWidgetComponent->GetWidget());
		}
	}
}
