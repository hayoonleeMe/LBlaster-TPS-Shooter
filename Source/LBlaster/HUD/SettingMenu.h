// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "SettingMenu.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API USettingMenu : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:
	virtual void MenuSetup() override;
	
protected:
	/*
	 *	Graphic Setting	
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> GraphicSettingButton;

	/*
	 *	Return Button
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ReturnButton;

private:
	UFUNCTION()
	void GraphicSettingButtonClicked();

	UFUNCTION()
	void ReturnButtonClicked();
};
