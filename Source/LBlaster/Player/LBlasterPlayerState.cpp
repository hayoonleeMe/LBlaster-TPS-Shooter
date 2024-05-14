// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LBlasterPlayerState.h"

#include "LBlasterPlayerController.h"
#include "Character/LBlasterCharacter.h"
#include "GameState/LBlasterGameState.h"
#include "Net/UnrealNetwork.h"

ALBlasterPlayerState::ALBlasterPlayerState()
{
	NetUpdateFrequency = 10.f;

	/* Team */
	Team = ETeam::ET_MAX;
}

void ALBlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALBlasterPlayerState, Death);
	DOREPLIFETIME(ALBlasterPlayerState, Team);
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

void ALBlasterPlayerState::InitTeam()
{
	if (ALBlasterGameState* GameState = GetWorld()->GetGameState<ALBlasterGameState>())
	{
		if (Team == ETeam::ET_RedTeam)
		{
			GameState->RedTeam.Add(this);
		}
		else if (Team == ETeam::ET_BlueTeam)
		{
			GameState->BlueTeam.Add(this);
		}
	}
}

FTeamCharacterMaterials ALBlasterPlayerState::GetCharacterMaterials() const
{
	if (TeamCharacterMaterialsMap.Contains(Team))
	{
		return TeamCharacterMaterialsMap[Team];
	}
	return FTeamCharacterMaterials();
}

void ALBlasterPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	// 새로 사용할 PlayerState에 이전 PlayerState의 값을 복사한다.
	if (ALBlasterPlayerState* OutPlayerState = Cast<ALBlasterPlayerState>(PlayerState))
	{
		OutPlayerState->Team = Team;
	}
}

bool ALBlasterPlayerState::IsValidOwnerCharacter()
{
	if (!OwnerCharacter && GetPawn())
	{
		OwnerCharacter = Cast<ALBlasterCharacter>(GetPawn());
	}
	return OwnerCharacter != nullptr;
}

bool ALBlasterPlayerState::IsValidOwnerController()
{
	if (!OwnerController && IsValidOwnerCharacter() && OwnerCharacter->Controller)
	{
		OwnerController = Cast<ALBlasterPlayerController>(OwnerCharacter->Controller);
	}
	return OwnerController != nullptr;
}
