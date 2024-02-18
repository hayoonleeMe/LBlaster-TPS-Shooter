// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LBlasterPlayerState.h"

#include "LBlasterPlayerController.h"
#include "Character/LBlasterCharacter.h"
#include "Net/UnrealNetwork.h"

ALBlasterPlayerState::ALBlasterPlayerState()
{
	NetUpdateFrequency = 10.f;
}

void ALBlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALBlasterPlayerState, Death);
}

void ALBlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	
	if (IsValidOwnerCharacter() && IsValidOwnerController())
	{
		OwnerController->SetHUDScore(GetScore());
	}
}

void ALBlasterPlayerState::OnRep_Death()
{
	if (IsValidOwnerCharacter() && IsValidOwnerController())
	{
		OwnerController->SetHUDDeath(Death);
	}
}

void ALBlasterPlayerState::AddToDeath(int32 InDeathAmount)
{
	Death += InDeathAmount;

	if (IsValidOwnerCharacter() && IsValidOwnerController())
	{
		OwnerController->SetHUDDeath(Death);
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
