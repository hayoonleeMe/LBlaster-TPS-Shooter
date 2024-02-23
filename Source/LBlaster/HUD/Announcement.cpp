// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Announcement.h"

#include "Components/TextBlock.h"

void UAnnouncement::SetHUDAnnouncementCountdown(float InCountdownTime)
{
	if (InCountdownTime < 0.f)
	{
		WarmupTimeText->SetText(FText());
		return;
	}		
	
	int32 Minutes = FMath::FloorToInt(InCountdownTime / 60.f);
	int32 Seconds = InCountdownTime - Minutes * 60;
	FString CountdownString = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
	WarmupTimeText->SetText(FText::FromString(CountdownString));
}

void UAnnouncement::SetCooldownAnnouncement()
{
	FString AnnouncementString(TEXT("New Match Starts In :"));
	AnnouncementText->SetText(FText::FromString(AnnouncementString));
	InfoText->SetText(FText());
}
