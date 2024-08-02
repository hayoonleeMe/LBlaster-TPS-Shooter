// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MatchTimeTimer.h"

#include "Components/TextBlock.h"

UMatchTimeTimer::UMatchTimeTimer()
{
	ThresholdToPlayMatchCountdownBlinkAnim = 10.f;
}

void UMatchTimeTimer::SetMatchCountdownText(float InCountdownTime, bool bPlayAnimation)
{
	if (InCountdownTime < 0.f)
	{
		MatchCountdownText->SetText(FText());
		return;
	}
	
	const int32 Minutes = FMath::FloorToInt(InCountdownTime / 60.f);
	const int32 Seconds = InCountdownTime - Minutes * 60;
	const FString CountdownString = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
	MatchCountdownText->SetText(FText::FromString(CountdownString));

	// 해당 함수는 1초마다 호출되고, 애니메이션의 길이도 1초이므로 매초 호출됨
	if (bPlayAnimation && InCountdownTime <= ThresholdToPlayMatchCountdownBlinkAnim)
	{
		PlayAnimation(MatchCountdownBlink);
	}
}
