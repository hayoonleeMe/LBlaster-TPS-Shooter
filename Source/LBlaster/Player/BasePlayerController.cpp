// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"

#include "LBlasterPlayerState.h"
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

void ABasePlayerController::ServerSendChatTextToAll_Implementation(const FString& InPlayerName, const FText& InText, EChatMode InChatMode)
{
	if (ALBlasterPlayerState* LBPlayerState = GetPlayerState<ALBlasterPlayerState>())
	{
		if (ABaseGameMode* BaseGameMode = Cast<ABaseGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			BaseGameMode->SendChatTextToAll(InPlayerName, InText, InChatMode, LBPlayerState->GetTeam());
		}
	}
}

void ABasePlayerController::ServerSendChatTextToSameTeam_Implementation(const FString& InPlayerName, const FText& InText, EChatMode InChatMode)
{
	if (ALBlasterPlayerState* LBPlayerState = GetPlayerState<ALBlasterPlayerState>())
	{
		if (ABaseGameMode* BaseGameMode = Cast<ABaseGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			BaseGameMode->SendChatTextToSameTeam(InPlayerName, InText, InChatMode, LBPlayerState->GetTeam());
		}
	}
}

void ABasePlayerController::BroadcastChatText(const FString& InPlayerName, const FText& InText, EChatMode InChatMode, ETeam SourceTeam)
{
	ClientAddChatText(InPlayerName, InText, InChatMode, SourceTeam);
}

void ABasePlayerController::ClientAddChatText_Implementation(const FString& InPlayerName, const FText& InText, EChatMode InChatMode, ETeam SourceTeam)
{
	if (ABaseHUD* BaseHUD = Cast<ABaseHUD>(GetHUD()))
	{
		BaseHUD->AddChatMessage(InPlayerName, InText, InChatMode, SourceTeam);
	}
}
