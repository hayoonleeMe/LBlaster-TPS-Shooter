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

	void SetNumPlayersText(int32 NumCurrentPlayers, int32 NumMaxPlayers);
};
