// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SessionHelperPlayerController.h"

#include "MultiplayerSessionsSubsystem.h"

void ASessionHelperPlayerController::ClientDestroySession_Implementation()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>())
		{
			MultiplayerSessionsSubsystem->DestroySession();
		}
	}
}
