// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreboardRowWithRank.h"

#include "Components/TextBlock.h"

void UScoreboardRowWithRank::SetScoreboardRowText(int32 InRank, const FString& InPlayerName, int32 InScore, int32 InDeath)
{
	if (RankText)
	{
		RankText->SetText(FText::AsNumber(InRank));
	}
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
	bTextSet = true;
}

void UScoreboardRowWithRank::EmptyScoreboardRowText()
{
	Super::EmptyScoreboardRowText();

	if (RankText)
	{
		RankText->SetText(FText());
	}
}
