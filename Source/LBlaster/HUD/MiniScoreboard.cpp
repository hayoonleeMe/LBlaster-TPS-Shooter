// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniScoreboard.h"

#include "Components/TextBlock.h"

void UMiniScoreboard::SetGoalScoreText(const int32 InGoalScore)
{
	if (GoalScoreText)
	{
		GoalScoreText->SetText(FText::AsNumber(InGoalScore));
	}
}
