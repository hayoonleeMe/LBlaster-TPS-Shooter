// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "PauseMenu.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UPauseMenu : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:
	virtual void MenuSetup() override;
	virtual void MenuTearDown() override;

private:
	/*
	 *	Main Menu Button
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ReturnToMainMenuButton;

	UFUNCTION()
	void ReturnToMainMenuButtonClicked();

	/*
	 *	Multiplayer Sessions
	 */
	UPROPERTY()
	TObjectPtr<class UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;

	void OnDestroySessionComplete(bool bWasSuccessful);

	void DestroyAllClientSession();

	/*
	 *	Resume Button
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ResumeButton;

	UFUNCTION()
	void ResumeButtonClicked();

	/*
	 *	Setting Button
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> SettingButton;
	
	UFUNCTION()
	void SettingButtonClicked();

	/*
	 *	Return To Main Menu Alert Overlay
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UOverlay> ReturnToMainMenuAlertOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReturnToMainMenuAlertAcceptButton;

	UFUNCTION()
	void OnReturnToMainMenuAlertAcceptButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReturnToMainMenuAlertCancelButton;

	UFUNCTION()
	void OnReturnToMainMenuAlertCancelButtonClicked();

	/*
	 *	Loading Overlay
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> LoadingOverlay;

	void SetLoadingOverlayVisibility(bool bShow);
};
