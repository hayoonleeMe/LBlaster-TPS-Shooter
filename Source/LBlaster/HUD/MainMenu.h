// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UMainMenu : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:
	virtual void MenuSetup() override;

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
