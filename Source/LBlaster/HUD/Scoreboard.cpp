// Fill out your copyright notice in the Description page of Project Settings.


#include "Scoreboard.h"

#include "Player/LBlasterPlayerState.h"

void UScoreboard::UpdateBoard()
{
}

void UScoreboard::SetScoreboardForResultMenu()
{
	UpdateBoard();
	bUsedForResultMenu = true;
	SetVisibility(ESlateVisibility::Visible);
}

bool UScoreboard::IsValidOwnerPlayerState()
{
	if (!OwnerPlayerState && IsValidOwnerController())
	{
		OwnerPlayerState = OwnerController->GetPlayerState<ALBlasterPlayerState>();
	}
	return OwnerPlayerState != nullptr;
}
