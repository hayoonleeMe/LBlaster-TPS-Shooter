// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniScoreboardTeamDeathMatch.h"

#include "Components/TextBlock.h"

void UMiniScoreboardTeamDeathMatch::SetRedTeamScoreText(const int32 InTeamScore)
{
	if (RedTeamScoreText)
	{
		RedTeamScoreText->SetText(FText::AsNumber(InTeamScore));
	}
}

void UMiniScoreboardTeamDeathMatch::SetBlueTeamScoreText(const int32 InTeamScore)
{
	if (BlueTeamScoreText)
	{
		BlueTeamScoreText->SetText(FText::AsNumber(InTeamScore));
	}
}
