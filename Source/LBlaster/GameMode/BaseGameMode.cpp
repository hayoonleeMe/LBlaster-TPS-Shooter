// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameMode.h"

#include "LBlaster.h"
#include "Player/LBlasterPlayerState.h"
#include "Player/BasePlayerController.h"

void ABaseGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	FChatParams ChatParams;
	ChatParams.ChatMode = EChatMode::ECM_System;
	ChatParams.ChatSystemInfoTemplate = EChatSystemInfoTemplate::ECSIT_Login;
	if (APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>())
	{
		ChatParams.SenderPlayerName = PlayerState->GetPlayerName();
	}
	if (ABasePlayerController* BasePlayerController = Cast<ABasePlayerController>(NewPlayer))
	{
		BasePlayerController->ServerSendChatText(ChatParams);
	}
}

void ABaseGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	FChatParams ChatParams;
	ChatParams.ChatMode = EChatMode::ECM_System;
	ChatParams.ChatSystemInfoTemplate = EChatSystemInfoTemplate::ECSIT_Logout;
	if (APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>())
	{
		ChatParams.SenderPlayerName = PlayerState->GetPlayerName();
	}
	if (ABasePlayerController* BasePlayerController = Cast<ABasePlayerController>(Exiting))
	{
		BasePlayerController->ServerSendChatText(ChatParams);
	}
}

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
