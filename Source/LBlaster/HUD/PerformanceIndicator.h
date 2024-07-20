// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "PerformanceIndicator.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UPerformanceIndicator : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:

protected:
	virtual void NativeConstruct() override;

private:
	void OnPerformanceIndicatorEnabledChanged(bool bPerformanceIndicatorEnabled);

	FTimerHandle PerformanceIndicatorTimer;

	float PerformanceIndicatorTimerRate = 2.f;
	
	void UpdatePerformanceIndicator();
	
	/*
	 *	FPS Indicator
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> FPSIndicatorText;

	int32 CurrentFPSRate = -1;

	/*
	 *	Ping Indicator
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> PingIndicatorText;

	int32 CurrentPing = -1;
};
