// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameMode.h"

#include "Player/LBlasterPlayerState.h"
#include "Player/BasePlayerController.h"

void ABaseGameMode::SendChatTextToAll(const FString& InPlayerName, const FText& InText, EChatMode InChatMode, ETeam SourceTeam) const
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABasePlayerController* BasePlayerController = Cast<ABasePlayerController>(It->Get()))
		{
			BasePlayerController->BroadcastChatText(InPlayerName, InText, InChatMode, SourceTeam);
		}
	}
}

void ABaseGameMode::SendChatTextToSameTeam(const FString& InPlayerName, const FText& InText, EChatMode InChatMode, ETeam SourceTeam) const
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABasePlayerController* BasePlayerController = Cast<ABasePlayerController>(It->Get()))
		{
			if (ALBlasterPlayerState* LBPlayerState = It->Get()->GetPlayerState<ALBlasterPlayerState>())
			{
				if (LBPlayerState->GetTeam() == SourceTeam)
				{
					BasePlayerController->BroadcastChatText(InPlayerName, InText, InChatMode, SourceTeam);	
				}
			}			
		}
	}
}