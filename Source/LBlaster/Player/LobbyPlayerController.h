// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerController.h"
#include "LBTypes/Team.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ALobbyPlayerController : public ABasePlayerController
{
	GENERATED_BODY()

public:
	ALobbyPlayerController();
	
	UFUNCTION(Client, Reliable)
	void ClientSendAddPlayerList(ETeam InTeam, const FString& InName);
	
	UFUNCTION(Client, Reliable)
	void ClientSendAddPlayerListFFA(const FString& InName);

	bool bFirstTimeConnected = true;

	UFUNCTION(Server, Reliable)
	void ServerSendTeamChangePlayerList(ETeam CurrentTeam, ETeam NewTeam, class ALBlasterPlayerState* InPlayerState);
	
	UFUNCTION(Client, Reliable)
	void ClientSendTeamChangePlayerList(ETeam CurrentTeam, ETeam NewTeam, const FString& InName);

	UFUNCTION(Client, Reliable)
	void ClientSendRemovePlayerList(ETeam InTeam, const FString& InName);

	UFUNCTION(Client, Reliable)
	void ClientSendRemovePlayerListFFA(const FString& InName);

protected:
	virtual void BeginPlay() override;

	/*
	 *	Input
	 */
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<class UInputMappingContext> MenuMappingContext;

	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<class UInputAction> FocusChatAction;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputAction> ReturnMenuAction;

	void FocusChat();
	void ReturnMenu();

private:
	/*
	 *	Owning
	 */
	UPROPERTY()
	TObjectPtr<class ALobbyHUD> OwningHUD;

	bool IsValidOwningHUD();
};
