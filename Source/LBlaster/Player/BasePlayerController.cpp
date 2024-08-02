// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"

#include "MultiplayerSessionsSubsystem.h"
#include "GameMode/BaseGameMode.h"
#include "HUD/BaseHUD.h"
#include "Kismet/GameplayStatics.h"

void ABasePlayerController::ClientDestroySession_Implementation()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>())
		{
			MultiplayerSessionsSubsystem->DestroySession();
		}
	}
}

void ABasePlayerController::ServerSendChatText_Implementation(const FChatParams& ChatParams)
{
	if (ABaseGameMode* BaseGameMode = Cast<ABaseGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		BaseGameMode->SendChatText(ChatParams);
	}
}

void ABasePlayerController::BroadcastChatText(const FChatParams& ChatParams)
{
	ClientAddChatText(ChatParams);
}

void ABasePlayerController::ClientAddChatText_Implementation(const FChatParams& ChatParams)
{
	if (ABaseHUD* BaseHUD = Cast<ABaseHUD>(GetHUD()))
	{
		BaseHUD->AddChatMessage(ChatParams);
	}
}
