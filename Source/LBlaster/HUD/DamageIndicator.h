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
	void SetDamageText(int32 InDamage, float InDuration);

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> DamageText;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DamageAnimation;

	/*
	 *	Color and Font
	 *	Defined in derived Blueprint (WBP_DamageIndicator)
	 */	
	UPROPERTY(EditAnywhere, Category="LBlaster", meta=(AllowPrivateAccess=true))
	FSlateColor NoDamageColor;

	UPROPERTY(EditAnywhere, Category="LBlaster", meta=(AllowPrivateAccess=true))
	FSlateFontInfo NoDamageFont;	
	
	UPROPERTY(EditAnywhere, Category="LBlaster", meta=(AllowPrivateAccess=true))
	FSlateColor DamageColor;

	UPROPERTY(EditAnywhere, Category="LBlaster", meta=(AllowPrivateAccess=true))
	FSlateFontInfo DamageFont;
};
