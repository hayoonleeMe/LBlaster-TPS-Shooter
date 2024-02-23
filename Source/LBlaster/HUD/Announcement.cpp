// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Announcement.h"

#include "Components/TextBlock.h"

void UAnnouncement::SetWarmupCountdownText(float InCountdownTime)
{
	int32 Minutes = FMath::FloorToInt(InCountdownTime / 60.f);
	int32 Seconds = InCountdownTime - Minutes * 60;
	FString CountdownString = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
	WarmupTimeText->SetText(FText::FromString(CountdownString));
}
