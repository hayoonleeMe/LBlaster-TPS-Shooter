// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamGameMode.h"

#include "GameState/LBlasterGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LBlasterPlayerState.h"

ATeamGameMode::ATeamGameMode()
{
	GameStateClass = ALBlasterGameState::StaticClass();
}

void ATeamGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (ALBlasterGameState* LBGameState = GetGameState<ALBlasterGameState>())
	{
		if (ALBlasterPlayerState* PlayerState = Exiting->GetPlayerState<ALBlasterPlayerState>())
		{
			if (LBGameState->RedTeam.Contains(PlayerState))
			{
				LBGameState->RedTeam.Remove(PlayerState);
			}
			if (LBGameState->BlueTeam.Contains(PlayerState))
			{
				LBGameState->BlueTeam.Remove(PlayerState);
			}
		}
	}
}
