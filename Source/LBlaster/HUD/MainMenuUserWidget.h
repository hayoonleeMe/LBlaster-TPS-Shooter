// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UMainMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void MenuSetup();
	virtual void MenuTearDown();

protected:
	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<APlayerController> OwnerController;

	bool IsValidOwnerController();

	UPROPERTY()
	TObjectPtr<class AMainMenuHUD> OwnerHUD;

	bool IsValidOwnerHUD();
};
