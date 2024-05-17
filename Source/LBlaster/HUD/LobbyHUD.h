// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseHUD.h"
#include "MatchModeTypes.h"
#include "LBTypes/Team.h"
#include "LobbyHUD.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ALobbyHUD : public ABaseHUD
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;

	void ReturnToMainMenu();
	void TravelToMatch();
	virtual void ReturnMenu(bool bForceReturn) override;
	
	void AddNewPlayer(class ALBlasterPlayerState* InPlayerState) const;
	void AddNewPlayerForClient(ETeam InTeam, const FString& InName);
	void ChangePlayerTeamFromClient(ETeam CurrentTeam, ETeam NewTeam, ALBlasterPlayerState* InPlayerState);
	void ChangePlayerTeamForClient(ETeam CurrentTeam, ETeam NewTeam, const FString& InName);
	void RemoveExitingPlayer(ETeam InTeam, const FString& InName, bool bHasAuthority);
	
	void BroadcastAddPlayerList(ETeam InTeam, const FString& InName);
	void BroadcastTeamChangePlayerList(ETeam CurrentTeam, ETeam NewTeam, const FString& InName);
	void BroadcastRemovePlayerList(ETeam InTeam, const FString& InName);

protected:
	virtual void BeginPlay() override;

private:
	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<APlayerController> OwnerController;

	bool IsValidOwnerController();

	bool bFirstTimeInit = false;
	void PollInit();

	/*
	 *	Multiplayer Sessions
	 */
	TObjectPtr<class UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;
	
	void OnDestroySessionComplete(bool bWasSuccessful);
	
	/*
	 *	Lobby Menu
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Lobby Menu")
	TMap<EMatchMode, TSubclassOf<class ULobbyMenu>> LobbyMenuClassByMatchModeMap; 

	UPROPERTY()
	TObjectPtr<ULobbyMenu> LobbyMenu;
	
	void AddLobbyMenu(int32 NumMaxPlayers);
	
	// 서버에서 QuitButton을 눌렀을 때 true로 설정됨. Tick에서 ServerTravel을 할 수 있는지 체크하는 데 사용됨.
	bool bWantReturnToMainMenu = false;
	
	void DestroyAllClientSession();

	TArray<TTuple<ETeam, FString>> CachedAddNewPlayer;

	/*
	 *	MatchMode
	 */
	EMatchMode MatchMode;
};
