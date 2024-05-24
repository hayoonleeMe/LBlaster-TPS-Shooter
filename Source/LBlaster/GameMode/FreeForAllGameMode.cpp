// Fill out your copyright notice in the Description page of Project Settings.


#include "FreeForAllGameMode.h"

#include "Character/LBlasterCharacter.h"
#include "GameState/FreeForAllGameState.h"
#include "HUD/LBlasterHUD.h"
#include "Player/LBlasterPlayerController.h"

AFreeForAllGameMode::AFreeForAllGameMode()
{
	static ConstructorHelpers::FClassFinder<AFreeForAllGameState> FreeForAllGameStateClassRef(TEXT("/Script/Engine.Blueprint'/Game/LBlaster/Core/States/Game/BP_FreeForAllGameState.BP_FreeForAllGameState_C'"));
	if (FreeForAllGameStateClassRef.Class)
	{
		GameStateClass = FreeForAllGameStateClassRef.Class;
	}
}

void AFreeForAllGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

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

void AFreeForAllGameMode::PlayerEliminated(ALBlasterCharacter* EliminatedCharacter, ALBlasterPlayerController* VictimController,
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
				AttackerPlayerState->AddToScore(1.f);
				VictimPlayerState->AddToDeath(1);

				// Update Mini Scoreboard
				if (AFreeForAllGameState* FFAGameState = GetGameState<AFreeForAllGameState>())
				{
					FFAGameState->IncreaseTotalScore();
				}

				// Update Scoreboard
				if (GetWorld())
				{
					if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
					{
						if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
						{
							HUD->UpdateScoreboard(false);
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
