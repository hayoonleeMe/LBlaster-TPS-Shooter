// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageIndicator.h"

#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"

void UDamageIndicator::SetDamageText(float InDamage, float InDuration)
{
	if (DamageText)
	{
		DamageText->SetText(FText::AsNumber(FMath::FloorToInt32(InDamage)));
	}
	if (DamageAnimation)
	{
		const float EndTime = DamageAnimation->GetEndTime();
		const float PlaybackSpeed = EndTime / InDuration;
		PlayAnimation(DamageAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, PlaybackSpeed);
	}
}
