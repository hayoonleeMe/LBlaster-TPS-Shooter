// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LBlasterPlayerState.h"

#include "LBlasterPlayerController.h"
#include "Character/LBlasterCharacter.h"

void ALBlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	
	if (IsValidOwnerCharacter() && IsValidOwnerController())
	{
		OwnerController->SetHUDScore(GetScore());
	}
}

void ALBlasterPlayerState::AddToScore(float InScoreAmount)
{
	SetScore(GetScore() + InScoreAmount);

	if (IsValidOwnerCharacter() && IsValidOwnerController())
	{
		OwnerController->SetHUDScore(GetScore());
	}
}

bool ALBlasterPlayerState::IsValidOwnerCharacter()
{
	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ALBlasterCharacter>(GetPawn());
	}
	return OwnerCharacter != nullptr;
}

bool ALBlasterPlayerState::IsValidOwnerController()
{
	if (!OwnerController)
	{
		OwnerController = Cast<ALBlasterPlayerController>(OwnerCharacter->Controller);
	}
	return OwnerController != nullptr;
}
