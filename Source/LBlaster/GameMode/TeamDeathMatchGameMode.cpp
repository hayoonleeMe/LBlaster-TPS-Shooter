﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamDeathMatchGameMode.h"

#include "Character/LBlasterCharacter.h"
#include "GameState/TeamDeathMatchGameState.h"
#include "HUD/LBlasterHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LBlasterPlayerController.h"
#include "Player/LBlasterPlayerState.h"
#include "PlayerStart/BlueTeamPlayerStart.h"
#include "PlayerStart/RedTeamPlayerStart.h"

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

	if (ATeamDeathMatchGameState* TDMGameState = GetGameState<ATeamDeathMatchGameState>())
	{
		if (ALBlasterPlayerState* PlayerState = NewPlayer->GetPlayerState<ALBlasterPlayerState>())
		{
			// 도중에 참가한 플레이어는 팀원이 적은 팀으로 배치
			const int32 NumRedTeam = TDMGameState->GetRedTeamNum();
			const int32 NumBlueTeam = TDMGameState->GetBlueTeamNum();

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
			TDMGameState->RemoveRedTeamPlayer(PlayerState);
			TDMGameState->RemoveBlueTeamPlayer(PlayerState);

			if (GetWorld())
			{
				if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
				{
					if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
					{
						HUD->UpdateScoreboard();
					}
				}
			}
		}
	}
}

void ATeamDeathMatchGameMode::PlayerEliminated(ALBlasterCharacter* EliminatedCharacter, ALBlasterPlayerController* VictimController,
	ALBlasterPlayerController* AttackerController)
{
	// 게임 중일 때만 Kill 가능
	if (!IsMatchInProgress())
	{
		return;
	}
	// 이미 죽었으면 return
	if (!EliminatedCharacter || EliminatedCharacter->IsDead())
	{
		return;
	}
	
	// Attacker 점수 획득
	if (ALBlasterPlayerState* AttackerPlayerState = Cast<ALBlasterPlayerState>(AttackerController->PlayerState))
	{
		if (ALBlasterPlayerState* VictimPlayerState = Cast<ALBlasterPlayerState>(VictimController->PlayerState))
		{
			if (AttackerPlayerState != VictimPlayerState)
			{
				AttackerPlayerState->AddToKillScore(1);
				VictimPlayerState->AddToDeath(1);

				// Update Mini Scoreboard
				if (ATeamDeathMatchGameState* TDMGameState = GetGameState<ATeamDeathMatchGameState>())
				{
					if (AttackerPlayerState->GetTeam() == ETeam::ET_RedTeam)
					{
						TDMGameState->IncreaseRedTeamScore();
					}
					else if (AttackerPlayerState->GetTeam() == ETeam::ET_BlueTeam)
					{
						TDMGameState->IncreaseBlueTeamScore();
					}
				}

				// Update Scoreboard
				if (GetWorld())
				{
					if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
					{
						if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
						{
							HUD->UpdateScoreboard();
						}	
					}
				}
			}
			
			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
			{
				if (ALBlasterPlayerController* PlayerController = Cast<ALBlasterPlayerController>(*It))
				{
					PlayerController->BroadcastElim(AttackerPlayerState, VictimPlayerState);
				}
			}
		}
	}	

	EliminatedCharacter->Elim(false);
}

AActor* ATeamDeathMatchGameMode::FindPlayerStartTDM(AController* Player)
{
	if (ALBlasterPlayerState* LBPlayerState = Player->GetPlayerState<ALBlasterPlayerState>())
	{
		const ETeam Team = LBPlayerState->GetTeam();
		if (Team == ETeam::ET_RedTeam)
		{
			TArray<AActor*> RedTeamPlayerStarts;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARedTeamPlayerStart::StaticClass(), RedTeamPlayerStarts);

			const int32 RandIndex = FMath::RandRange(0, RedTeamPlayerStarts.Num() - 1);
			if (RedTeamPlayerStarts.IsValidIndex(RandIndex))
			{
				return RedTeamPlayerStarts[RandIndex];
			}
		}
		else if (Team == ETeam::ET_BlueTeam)
		{
			TArray<AActor*> BlueTeamPlayerStarts;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlueTeamPlayerStart::StaticClass(), BlueTeamPlayerStarts);

			const int32 RandIndex = FMath::RandRange(0, BlueTeamPlayerStarts.Num() - 1);
			if (BlueTeamPlayerStarts.IsValidIndex(RandIndex))
			{
				return BlueTeamPlayerStarts[RandIndex];
			}
		}
	}
	return nullptr;
}
