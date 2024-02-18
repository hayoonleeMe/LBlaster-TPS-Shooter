// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthBar(float InHealth, float InMaxHealth);
	void SetHealthText(float InHealth, float InMaxHealth);
	void SetScoreAmount(float InScore);
	void SetDeathAmount(int32 InDeath);
	void SetAmmoAmount(int32 InAmmo);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> HealthText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreAmount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DeathAmount;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoAmount;
};
