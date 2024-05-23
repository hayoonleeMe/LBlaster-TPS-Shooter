// Fill out your copyright notice in the Description page of Project Settings.


#include "FreeForAllGameMode.h"

#include "GameState/FreeForAllGameState.h"
#include "HUD/LBlasterHUD.h"

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
