// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameMode.h"

#include "HUD/LobbyHUD.h"
#include "Player/LBlasterPlayerState.h"
#include "Player/LobbyPlayerController.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;

	PlayerControllerClass = ALobbyPlayerController::StaticClass();

	static ConstructorHelpers::FClassFinder<ALBlasterPlayerState> PlayerStateClassRef(TEXT("/Script/Engine.Blueprint'/Game/LBlaster/Core/States/Player/BP_LBlasterPlayerState.BP_LBlasterPlayerState_C'"));
	if (PlayerStateClassRef.Class)
	{
		PlayerStateClass = PlayerStateClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<ALobbyHUD> LobbyHUDClassRef(TEXT("/Script/Engine.Blueprint'/Game/LBlaster/Core/HUD/BP_LobbyHUD.BP_LobbyHUD_C'"));
	if (LobbyHUDClassRef.Class)
	{
		HUDClass = LobbyHUDClassRef.Class;
	}
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (ALBlasterPlayerState* LBPlayerState = NewPlayer->GetPlayerState<ALBlasterPlayerState>())
	{
		if (UWorld* World = GetWorld())
		{
			if (APlayerController* PlayerController = World->GetFirstPlayerController())
			{
				if (ALobbyHUD* LobbyHUD = PlayerController->GetHUD<ALobbyHUD>())
				{
					LobbyHUD->AddNewPlayer(LBPlayerState);
				}
			}
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (ALBlasterPlayerState* LBPlayerState = Exiting->GetPlayerState<ALBlasterPlayerState>())
	{
		if (UWorld* World = GetWorld())
		{
			if (APlayerController* PlayerController = World->GetFirstPlayerController())
			{
				if (ALobbyHUD* LobbyHUD = PlayerController->GetHUD<ALobbyHUD>())
				{
					LobbyHUD->RemoveExitingPlayer(LBPlayerState->GetTeam(), LBPlayerState->GetPlayerName(), true);
				}
			}
		}
	}
}
