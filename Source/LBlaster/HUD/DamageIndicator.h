// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageIndicator.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UDamageIndicator : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetDamageText(float InDamage, float InDuration);

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> DamageText;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DamageAnimation;

};
