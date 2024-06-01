// Fill out your copyright notice in the Description page of Project Settings.


#include "Scoreboard.h"

#include "Components/Border.h"
#include "Player/LBlasterPlayerState.h"

void UScoreboard::UpdateBoard()
{
}

void UScoreboard::SetScoreboardForResultMenu()
{
	UpdateBoard();
	bUsedForResultMenu = true;
	SetVisibility(ESlateVisibility::Visible);

	if (Border)
	{
		FVector2D Translation = Border->GetRenderTransform().Translation;
		Translation.Y += 100.f;
		Border->SetRenderTranslation(Translation);
	}
}

bool UScoreboard::IsValidOwnerPlayerState()
{
	if (!OwnerPlayerState && IsValidOwnerController())
	{
		OwnerPlayerState = OwnerController->GetPlayerState<ALBlasterPlayerState>();
	}
	return OwnerPlayerState != nullptr;
}
