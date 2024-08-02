// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameMode.h"

#include "Player/LBlasterPlayerState.h"
#include "Player/BasePlayerController.h"

void ABaseGameMode::SendChatText(const FChatParams& ChatParams) const
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABasePlayerController* ReceiverPlayerController = Cast<ABasePlayerController>(It->Get()))
		{
			// 팀 채팅이면 같은 팀 플레이어에게만 전송해줌.
			if (ChatParams.ChatMode == EChatMode::ECM_FriendlyTeam)
			{
				ALBlasterPlayerState* ReceiverPlayerState = ReceiverPlayerController->GetPlayerState<ALBlasterPlayerState>();
				if (ReceiverPlayerState && ChatParams.SenderPlayerTeam == ReceiverPlayerState->GetTeam())
				{
					ReceiverPlayerController->BroadcastChatText(ChatParams);
				}
			}
			else
			{
				ReceiverPlayerController->BroadcastChatText(ChatParams);
			}
		}
	}
}

void ABaseGameMode::SetMatchStateToCooldown()
{
	SetMatchState(MatchState::Cooldown);
}
