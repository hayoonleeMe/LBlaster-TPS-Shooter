// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameMode.h"

#include "Player/SessionHelperPlayerController.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;

	// static ConstructorHelpers::FClassFinder<ASessionHelperPlayerController> SessionHelperPlayerControllerClassRef(TEXT(""));
	// if (SessionHelperPlayerControllerClassRef.Class)
	// {
	// 	PlayerControllerClass = SessionHelperPlayerControllerClassRef.Class;
	// }
	PlayerControllerClass = ASessionHelperPlayerController::StaticClass();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// if (GameState->PlayerArray.Num() == 2)
	// {
	// 	if (UWorld* World = GetWorld())
	// 	{
	// 		World->ServerTravel(FString(TEXT("/Game/LBlaster/Maps/LBlasterMap?listen")));
	// 	}
	// }
}