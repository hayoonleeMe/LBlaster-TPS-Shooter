// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenu.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UPauseMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void MenuSetup();
	void MenuTearDown();

protected:
	/*
	 *	Main Menu Button
	 */
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

private:
	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<class ALBlasterPlayerController> PlayerController;

	bool IsValidPlayerController();

	/*
	 *	Main Menu Button
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> MainMenuButton;

	UFUNCTION()
	void MainMenuButtonClicked();

	UPROPERTY()
	TObjectPtr<class UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;

	/*
	 *	Resume Button
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ResumeButton;

	UFUNCTION()
	void ResumeButtonClicked();
};
