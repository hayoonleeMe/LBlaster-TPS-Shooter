// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUD/MainMenuUserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MatchModeTypes.h"
#include "StartMenu.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UStartMenu : public UMainMenuUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void MenuSetup() override;
	
	void OnCreateSessionComplete(bool bWasSuccessful);

private:
	/*
	 *	Widgets
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CreateSessionButton;

	UFUNCTION()
	void OnCreateSessionButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> FindSessionsButton;

	UFUNCTION()
	void OnFindSessionsButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReturnButton;

	UFUNCTION()
	void OnReturnButtonClicked();

	/*
	 *	Create Session Alert Overlay
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UOverlay> CreateSessionAlertOverlay;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UOptionSelector> GameModeSelector;

	EMatchMode GetMatchModeType();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class USliderWithNumericInput> MaxPlayerSlider;

	int32 GetMaxPlayerValue();

	float SliderInitialValue = 1.f;
	float SliderMinValue = 1.f;
	float SliderMaxValue = 10.f;
	float SliderStepSize = 1.f;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> AlertCreateSessionButton;

	UFUNCTION()
	void OnAlertCreateSessionButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> AlertCancelButton;

	UFUNCTION()
	void OnAlertCancelButtonClicked();

	/*
	 *	Loading Overlay
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUserWidget> LoadingOverlay;

	void SetLoadingOverlayVisibility(bool bShow);

	/*
	 *	Create Session Fail Alert Overlay
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> CreateSessionFailAlertOverlay;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> CreateFailAlertReturnButton;

	UFUNCTION()
	void OnCreateFailAlertReturnButton();
};
