// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "SettingsHubMenu.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API USettingsHubMenu : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:
	virtual void MenuSetup() override;
	
protected:
	/*
	 *	Video Setting	
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> VideoSettingButton;

	/*
	 *	Return Button
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ReturnButton;

private:
	UFUNCTION()
	void VideoSettingButtonClicked();

	UFUNCTION()
	void ReturnButtonClicked();
};
