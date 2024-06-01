// Fill out your copyright notice in the Description page of Project Settings.


#include "ResultMenu.h"

#include "Components/TextBlock.h"

void UResultMenu::SetCooldownTimerText(float InTime)
{
	if (TimerText)
	{
		const int32 Seconds = FMath::Max(0, FMath::FloorToInt32(InTime));
		TimerText->SetText(FText::AsNumber(Seconds));
	}
}
