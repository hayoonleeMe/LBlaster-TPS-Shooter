// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/CharacterOverlay.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UCharacterOverlay::SetHealthBar(float InHealth, float InMaxHealth)
{
	const float HealthPercent = InHealth / InMaxHealth;
	HealthBar->SetPercent(HealthPercent);
}

void UCharacterOverlay::SetHealthText(float InHealth, float InMaxHealth)
{
	const FString HealthString = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(InHealth), FMath::CeilToInt(InMaxHealth));
	HealthText->SetText(FText::FromString(HealthString));
}
