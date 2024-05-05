// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/MainMenuUserWidget.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMainMenu : public UMainMenuUserWidget
{
	GENERATED_BODY()

public:
	virtual void MenuSetup() override;
	virtual void MenuTearDown() override;

protected:
	virtual void NativeDestruct() override;

private:
	/*
	 *	Widgets
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> StartButton;

	UFUNCTION()
	void OnStartButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SettingButton;

	UFUNCTION()
	void OnSettingButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;

	UFUNCTION()
	void OnQuitButtonClicked();
};
