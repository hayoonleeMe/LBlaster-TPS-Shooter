// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/LBlasterUserWidget.h"
#include "MatchTimeTimer.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UMatchTimeTimer : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:
	UMatchTimeTimer();
	void SetMatchCountdownText(float InCountdownTime, bool bPlayAnimation);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> MatchCountdownText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> MatchCountdownBlink;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Miscellaneous")
	float ThresholdToPlayMatchCountdownBlinkAnim;
};
