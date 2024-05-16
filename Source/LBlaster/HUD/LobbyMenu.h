// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "LobbyMenu.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ULobbyMenu : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:
	virtual void MenuSetup() override;

	void OnDestroySessionComplete(bool bWasSuccessful);
	void SetNumMaxPlayersText(int32 InNumMaxPlayers);

protected:
	void SetNumPlayersText();
	void SetNumPlayersText(int32 InNumCurrentPlayers, int32 InNumMaxPlayers);
	void AddNumCurrentPlayersText(int32 AmountToAdd);
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> StartButton;

	UFUNCTION()
	void OnStartButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ReturnButton;

	UFUNCTION()
	void OnReturnButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> NumPlayersText;

	int32 NumCurrentPlayers = 0;
	int32 NumMaxPlayers = 0;
};
