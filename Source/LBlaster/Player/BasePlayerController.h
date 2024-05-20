// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LBTypes/ChatMode.h"
#include "LBTypes/Team.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/*
	 *	MultiplayerSessions
	 */
	UFUNCTION(Client, Reliable)
	void ClientDestroySession();

	/*
	 *	ChatBox
	 */
	UFUNCTION(Server, Reliable)
	void ServerSendChatTextToAll(const FString& InPlayerName, const FText& InText, EChatMode InChatMode);

	UFUNCTION(Server, Reliable)
	void ServerSendChatTextToSameTeam(const FString& InPlayerName, const FText& InText, EChatMode InChatMode);

	void BroadcastChatText(const FString& InPlayerName, const FText& InText, EChatMode InChatMode, ETeam SourceTeam = ETeam::ET_MAX);

	UFUNCTION(Client, Reliable)
	void ClientAddChatText(const FString& InPlayerName, const FText& InText, EChatMode InChatMode, ETeam SourceTeam = ETeam::ET_MAX);
};
