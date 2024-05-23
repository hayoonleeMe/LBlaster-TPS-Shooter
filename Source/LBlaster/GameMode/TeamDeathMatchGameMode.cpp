// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamDeathMatchGameMode.h"

#include "GameState/TeamDeathMatchGameState.h"
#include "HUD/LBlasterHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LBlasterPlayerState.h"

ATeamDeathMatchGameMode::ATeamDeathMatchGameMode()
{
	static ConstructorHelpers::FClassFinder<ATeamDeathMatchGameState> TeamDeathMatchGameStateClassRef(TEXT("/Script/Engine.Blueprint'/Game/LBlaster/Core/States/Game/BP_TeamDeathMatchGameState.BP_TeamDeathMatchGameState_C'"));
	if (TeamDeathMatchGameStateClassRef.Class)
	{
		GameStateClass = TeamDeathMatchGameStateClassRef.Class;
	}
}

void ATeamDeathMatchGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// 도중에 참가한 플레이어는 팀원이 적은 팀으로 배치
	if (ATeamDeathMatchGameState* TDMGameState = GetGameState<ATeamDeathMatchGameState>())
	{
		if (ALBlasterPlayerState* PlayerState = NewPlayer->GetPlayerState<ALBlasterPlayerState>())
		{
			const int32 NumRedTeam = TDMGameState->RedTeam.Num();
			const int32 NumBlueTeam = TDMGameState->BlueTeam.Num();

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

void ATeamDeathMatchGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (ATeamDeathMatchGameState* TDMGameState = GetGameState<ATeamDeathMatchGameState>())
	{
		if (ALBlasterPlayerState* PlayerState = Exiting->GetPlayerState<ALBlasterPlayerState>())
		{
			if (TDMGameState->RedTeam.Contains(PlayerState))
			{
				TDMGameState->RedTeam.Remove(PlayerState);
			}
			if (TDMGameState->BlueTeam.Contains(PlayerState))
			{
				TDMGameState->BlueTeam.Remove(PlayerState);
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
