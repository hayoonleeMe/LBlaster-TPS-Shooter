// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniScoreboardFreeForAll.h"

#include "Components/TextBlock.h"

void UMiniScoreboardFreeForAll::SetTotalScoreText(const int32 InTotalScore)
{
	if (TotalScoreText)
	{
		TotalScoreText->SetText(FText::AsNumber(InTotalScore));
	}
}
