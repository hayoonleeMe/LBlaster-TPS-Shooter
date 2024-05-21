// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "MatchModeTypes.h"
#include "StartMenu.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UStartMenu : public ULBlasterUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void MenuSetup() override;
	virtual bool IsOverlayOpened() override;
	virtual void CloseOverlay() override;
	virtual bool CanReturn() override;
	
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

	void OnGameModeSelectorChanged(int32 InActiveIndex);

	EMatchMode GetMatchModeType();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UDropDown> MaxPlayerDropDown;
	
	int32 GetMaxPlayerValue() const;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> CreateSessionAlertCreateButton;

	UFUNCTION()
	void OnCreateSessionAlertCreateButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> CreateSessionAlertCancelButton;

	UFUNCTION()
	void OnCreateSessionAlertCancelButtonClicked();

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
