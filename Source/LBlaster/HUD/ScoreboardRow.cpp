// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreboardRow.h"

#include "Components/TextBlock.h"

void UScoreboardRow::SetScoreboardRowText(const FString& InPlayerName, int32 InScore, int32 InDeath)
{
	if (PlayerNameText)
	{
		PlayerNameText->SetText(FText::FromString(InPlayerName));
	}
	if (ScoreText)
	{
		ScoreText->SetText(FText::AsNumber(InScore));
	}
	if (DeathText)
	{
		DeathText->SetText(FText::AsNumber(InDeath));		
	}
}
