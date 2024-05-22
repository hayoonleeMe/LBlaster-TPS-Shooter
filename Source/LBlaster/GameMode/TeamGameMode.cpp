// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamGameMode.h"

#include "GameState/LBlasterGameState.h"
#include "HUD/LBlasterHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LBlasterPlayerState.h"

ATeamGameMode::ATeamGameMode()
{
	GameStateClass = ALBlasterGameState::StaticClass();
}

void ATeamGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// 도중에 참가한 플레이어는 팀원이 적은 팀으로 배치
	if (ALBlasterGameState* LBGameState = GetGameState<ALBlasterGameState>())
	{
		if (ALBlasterPlayerState* PlayerState = NewPlayer->GetPlayerState<ALBlasterPlayerState>())
		{
			const int32 NumRedTeam = LBGameState->RedTeam.Num();
			const int32 NumBlueTeam = LBGameState->BlueTeam.Num();

			if (NumRedTeam <= NumBlueTeam)
			{
				PlayerState->SetTeam(ETeam::ET_RedTeam);
			}
			else
			{
				PlayerState->SetTeam(ETeam::ET_BlueTeam);
			}
		}
	}
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

			if (GetWorld())
			{
				if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
				{
					if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
					{
						HUD->UpdateScoreboard(true);
					}
				}
			}
		}
	}
}
