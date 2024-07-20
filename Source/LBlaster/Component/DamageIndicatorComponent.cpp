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
		
		int32 AccumulatedDamageInt = FMath::FloorToInt32(AccumulatedDamage);
		// DamagedActor가 무적 상태라 데미지가 0이면 0으로 표시
		if (FMath::FloorToInt32(InDamage) == 0)
		{
			AccumulatedDamageInt = 0;
		}
		
		DamageIndicator->SetDamageText(AccumulatedDamageInt, ClearDelay);
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
