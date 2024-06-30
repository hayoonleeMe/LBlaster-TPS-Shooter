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
	
private:
	/*
	 *	Video Setting Button
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> VideoSettingButton;

	UFUNCTION()
	void VideoSettingButtonClicked();

	/*
	 *	Mouse Setting Button
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> MouseSettingButton;

	UFUNCTION()
	void MouseSettingButtonClicked();

	/*
	 *	Audio Setting Button
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> AudioSettingButton;

	UFUNCTION()
	void AudioSettingButtonClicked();

	/*
	 *	Return Button
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ReturnButton;
	
	UFUNCTION()
	void ReturnButtonClicked();
};
