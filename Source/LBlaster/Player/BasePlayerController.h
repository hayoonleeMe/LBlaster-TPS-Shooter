// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LBTypes/ChatParams.h"
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
	void ServerSendChatText(const FChatParams& ChatParams);
	
	UFUNCTION(Client, Reliable)
	void ClientAddChatText(const FChatParams& ChatParams);
};
