// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSIndicator.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UFPSIndicator : public UUserWidget
{
	GENERATED_BODY()

public:

protected:
	virtual void NativeConstruct() override;

private:
	/*
	 *	FPS Indicator
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> FPSIndicatorText;

	void OnFPSIndicatorEnabledChanged(bool bFPSIndicatorEnabled);
	void UpdateFPSIndicator();

	int32 CurrentFPSRate = -1;

	FTimerHandle FPSIndicatorTimer;

	float FPSIndicatorTimerRate = 1.f;
};
