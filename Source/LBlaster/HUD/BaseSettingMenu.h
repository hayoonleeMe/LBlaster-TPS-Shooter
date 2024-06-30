// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "BaseSettingMenu.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UBaseSettingMenu : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:
	virtual void MenuSetup() override;

protected:
	UPROPERTY()
	TObjectPtr<class ULBGameUserSettings> GameUserSettings;

	/*
	 *	Apply Button
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ApplyButton;

	UFUNCTION()
	virtual void OnApplyButtonClicked();
	
	void EnableApplyButton();
	virtual bool ShouldApplyChange() const;

	/*
	 *	Return Button
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReturnButton;

	UFUNCTION()
	void OnReturnButtonClicked();

	/*
	 *	NoApplyAlertOverlay
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UOverlay> NoApplyAlertOverlay;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NoApplyAlertAcceptButton;

	UFUNCTION()
	virtual void OnNoApplyAlertAcceptButtonClicked();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NoApplyAlertCancelButton;

	UFUNCTION()
	void OnNoApplyAlertCancelButtonClicked();
};
