// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/CharacterOverlay.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UCharacterOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	/* Weapon Slot Icon */
	WeaponSlotImages.Emplace(EEquipSlot::EES_FirstSlot, WeaponSlot1);
	WeaponSlotImages.Emplace(EEquipSlot::EES_SecondSlot, WeaponSlot2);
	WeaponSlotImages.Emplace(EEquipSlot::EES_ThirdSlot, WeaponSlot3);
}

void UCharacterOverlay::SetHealthBar(float InHealth, float InMaxHealth)
{
	const float HealthPercent = InHealth / InMaxHealth;
	HealthBar->SetPercent(HealthPercent);
}

void UCharacterOverlay::SetHealthText(float InHealth, float InMaxHealth)
{
	// TODO : 임시로 HealthText 표시 안함. 결정 필요 
	return;
	const FString HealthString = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(InHealth), FMath::CeilToInt(InMaxHealth));
	HealthText->SetText(FText::FromString(HealthString));
}

void UCharacterOverlay::SetKillScoreAmount(int32 InKillScore)
{
	KillScoreAmount->SetText(FText::AsNumber(InKillScore));
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
	// TODO : 임시로 Weapon Type Text 사용안함. 완전히 결정 필요 
	return;
	
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
	if (InCountdownTime < 0.f)
	{
		MatchCountdownText->SetText(FText());
		return;
	}
	
	int32 Minutes = FMath::FloorToInt(InCountdownTime / 60.f);
	int32 Seconds = InCountdownTime - Minutes * 60;
	FString CountdownString = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
	MatchCountdownText->SetText(FText::FromString(CountdownString));

	// 해당 함수는 1초마다 호출되고, 애니메이션의 길이도 1초이므로 매초 호출됨
	if (InCountdownTime <= ThresholdToPlayMatchCountdownBlinkAnim)
	{
		PlayAnimation(MatchCountdownBlink);
	}
}

void UCharacterOverlay::SetGrenadeAmount(int32 InGrenadeAmount) const
{
	const FString GrenadeAmountString = FString::Printf(TEXT("%d"), InGrenadeAmount);
	GrenadeAmount->SetText(FText::FromString(GrenadeAmountString));
}

void UCharacterOverlay::HighPingWarning(float InDuration)
{
	if (HighPingImage && HighPingImageBlink)
	{
		PlayAnimation(HighPingImageBlink, 0.f, FMath::CeilToInt32(InDuration / HighPingImageBlink->GetEndTime()));
	}
}

void UCharacterOverlay::SetWeaponSlotIcon(EEquipSlot InEquipSlot, EWeaponType InWeaponType)
{
	if (WeaponSlotIcons.Contains(InWeaponType) && WeaponSlotImages.Contains(InEquipSlot))
	{
		if (UTexture2D* WeaponTexture = WeaponSlotIcons[InWeaponType])
		{
			if (UImage* WeaponImage = WeaponSlotImages[InEquipSlot])
			{
				WeaponImage->SetBrushFromTexture(WeaponTexture, true);
			}
		}	
	}
}

void UCharacterOverlay::ChooseWeaponSlot(EEquipSlot InEquipSlot)
{
	for (const TTuple<EEquipSlot, UImage*>& Pair : WeaponSlotImages)
	{
		if (UImage* Image = Pair.Value)
		{
			if (Pair.Key == InEquipSlot)
			{
				Image->SetRenderOpacity(1.f);
			}
			else
			{
				Image->SetRenderOpacity(0.5f);
			}	
		}
	}
}
