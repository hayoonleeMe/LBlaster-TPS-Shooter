// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ElimAnnouncement.h"

#include "Components/TextBlock.h"

void UElimAnnouncement::SetElimAnnouncementText(const FString& InAttackerName, const FString& InVictimName)
{
	if (AnnouncementText)
	{
		const FString Text = FString::Printf(TEXT("%s elimmed %s"), *InAttackerName, *InVictimName);
		AnnouncementText->SetText(FText::FromString(Text));
	}
	
}
