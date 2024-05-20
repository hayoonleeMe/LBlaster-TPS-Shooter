// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/LobbyMenu.h"
#include "LobbyMenuFreeForAll.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ULobbyMenuFreeForAll : public ULobbyMenu
{
	GENERATED_BODY()

public:
	void AddNewPlayer(const FString& InName);
	void AddNewPlayerForClient(const FString& InName);
	void AddLeftList(const FString& InName, bool bDoBroadcast);
	void AddRightList(const FString& InName, bool bDoBroadcast);
	void RemoveLogoutPlayer(const FString& InName, bool bHasAuthority);
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UVerticalBox> LeftBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> RightBox;

	void BroadcastAddPlayerList(const FString& InName);
	void BroadcastRemovePlayerList(const FString& InName);
};
