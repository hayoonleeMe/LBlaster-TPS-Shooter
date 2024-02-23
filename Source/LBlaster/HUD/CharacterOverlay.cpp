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

void UCharacterOverlay::SetScoreAmount(float InScore)
{
	const FString ScoreString = FString::Printf(TEXT("%d"), FMath::FloorToInt(InScore));
	ScoreAmount->SetText(FText::FromString(ScoreString));
}

void UCharacterOverlay::SetDeathAmount(int32 InDeath)
{
	const FString DeathString = FString::Printf(TEXT("%d"), InDeath);
	DeathAmount->SetText(FText::FromString(DeathString));
}

void UCharacterOverlay::SetAmmoAmount(int32 InAmmo)
{
	const FString AmmoString = FString::Printf(TEXT("%d"), InAmmo);
	AmmoAmount->SetText(FText::FromString(AmmoString));
}

void UCharacterOverlay::SetCarriedAmmoAmount(int32 InCarriedAmmo)
{
	const FString CarriedAmmoString = FString::Printf(TEXT("%d"), InCarriedAmmo);
	CarriedAmmoAmount->SetText(FText::FromString(CarriedAmmoString));
}

void UCharacterOverlay::SetWeaponTypeText(const FString& InWeaponTypeString)
{
	if (InWeaponTypeString == FString())
	{
		WeaponTypeText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		WeaponTypeText->SetVisibility(ESlateVisibility::Visible);
		WeaponTypeText->SetText(FText::FromString(InWeaponTypeString));
	}
}

void UCharacterOverlay::SetMatchCountdownText(float InCountdownTime)
{
	int32 Minutes = FMath::FloorToInt(InCountdownTime / 60.f);
	int32 Seconds = InCountdownTime - Minutes * 60;
	FString CountdownString = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
	MatchCountdownText->SetText(FText::FromString(CountdownString));
}
