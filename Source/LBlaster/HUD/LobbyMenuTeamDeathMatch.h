// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/LobbyMenu.h"
#include "LBTypes/Team.h"
#include "LobbyMenuTeamDeathMatch.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ULobbyMenuTeamDeathMatch : public ULobbyMenu
{
	GENERATED_BODY()

public:
	virtual void MenuSetup() override;
	void AddNewPlayer(class ALBlasterPlayerState* InPlayerState);
	void AddNewPlayerForClient(ETeam InTeam, const FString& InName);
	void AddRedTeam(ALBlasterPlayerState* InPlayerState, bool bDoBroadcast);
	void AddRedTeamForClient(const FString& InName);
	void AddBlueTeam(ALBlasterPlayerState* InPlayerState, bool bDoBroadcast);
	void AddBlueTeamForClient(const FString& InName);
	void ChangeTeam(ETeam CurrentTeam, ETeam NewTeam, ALBlasterPlayerState* InPlayerState);
	void ChangeTeamForClient(ETeam CurrentTeam, ETeam NewTeam, const FString& InName);
	void RemovePlayerFromRedTeam(const FString& InName, bool bDoBroadcast);
	void RemovePlayerFromBlueTeam(const FString& InName, bool bDoBroadcast);
	void RemoveLogoutPlayer(ETeam InTeam, const FString& InName, bool bHasAuthority);
	
private:
	/*
	 *	Player List
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> RedTeamButton;

	UFUNCTION()
	void OnRedTeamButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UVerticalBox> RedTeamBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BlueTeamButton;

	UFUNCTION()
	void OnBlueTeamButtonClicked();
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> BlueTeamBox;

	void BroadcastAddPlayerList(ETeam InTeam, const FString& InName);
	void BroadcastTeamChangePlayerList(ETeam CurrentTeam, ETeam NewTeam, const FString& InName);
	void BroadcastRemovePlayerList(ETeam InTeam, const FString& InName);
};
