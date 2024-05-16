// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterPlayerState.h"
#include "SessionHelperPlayerController.h"
#include "LBTypes/Team.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ALobbyPlayerController : public ASessionHelperPlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Client, Reliable)
	void ClientSendAddPlayerList(ETeam InTeam, const FString& InName);

	bool bFirstTimeConnected = true;

	UFUNCTION(Server, Reliable)
	void ServerSendTeamChangePlayerList(ETeam CurrentTeam, ETeam NewTeam, ALBlasterPlayerState* InPlayerState);
	
	UFUNCTION(Client, Reliable)
	void ClientSendTeamChangePlayerList(ETeam CurrentTeam, ETeam NewTeam, const FString& InName);

	UFUNCTION(Client, Reliable)
	void ClientSendRemovePlayerList(ETeam InTeam, const FString& InName);
};
