// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniScoreboard.h"

#include "Components/TextBlock.h"

void UMiniScoreboard::SetGoalKillScoreText(const int32 InGoalKillScore)
{
	if (GoalKillScoreText)
	{
		GoalKillScoreText->SetText(FText::AsNumber(InGoalKillScore));
	}
}
