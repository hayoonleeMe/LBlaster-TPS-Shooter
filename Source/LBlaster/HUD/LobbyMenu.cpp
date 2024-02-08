// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LobbyMenu.h"

void ULobbyMenu::Travel()
{
	if (UWorld* World = GetWorld())
	{
		World->ServerTravel(FString(TEXT("/Game/LBlaster/Maps/LBlasterMap?listen")));
	}
}
