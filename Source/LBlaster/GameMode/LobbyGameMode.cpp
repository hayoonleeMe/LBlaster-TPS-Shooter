// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameMode.h"

#include "Player/LBlasterPlayerState.h"
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

	static ConstructorHelpers::FClassFinder<ALBlasterPlayerState> PlayerStateClassRef(TEXT("/Script/Engine.Blueprint'/Game/LBlaster/Core/States/Player/BP_LBlasterPlayerState.BP_LBlasterPlayerState_C'"));
	if (PlayerStateClassRef.Class)
	{
		PlayerStateClass = PlayerStateClassRef.Class;
	}
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