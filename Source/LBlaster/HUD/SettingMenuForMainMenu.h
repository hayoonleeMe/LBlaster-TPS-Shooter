// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/SettingMenu.h"
#include "SettingMenuForMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API USettingMenuForMainMenu : public USettingMenu
{
	GENERATED_BODY()

public:
	virtual void MenuSetup() override;

private:
	UFUNCTION()
	void MainMenuGraphicSettingButtonClicked();

	UFUNCTION()
	void MainMenuReturnButtonClicked();
};
