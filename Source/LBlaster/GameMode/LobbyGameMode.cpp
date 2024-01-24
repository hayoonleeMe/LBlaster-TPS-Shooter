// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameMode.h"

#include "Character/LBlasterCharacter.h"
#include "GameFramework/GameStateBase.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;
	
	static ConstructorHelpers::FClassFinder<ALBlasterCharacter> LBlasterCharacterClassRef(TEXT("/Script/Engine.Blueprint'/Game/LBlaster/Actors/Players/Manny/BP_LBlasterCharacter.BP_LBlasterCharacter_C'"));
	if (LBlasterCharacterClassRef.Class)
	{
		DefaultPawnClass = LBlasterCharacterClassRef.Class; 
	}
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GameState->PlayerArray.Num() == 2)
	{
		if (UWorld* World = GetWorld())
		{
			World->ServerTravel(FString(TEXT("/Game/LBlaster/Maps/LBlasterMap?listen")));
		}
	}
}
