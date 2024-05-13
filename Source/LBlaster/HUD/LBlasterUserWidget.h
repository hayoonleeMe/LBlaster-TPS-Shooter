// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LBlasterUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ULBlasterUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void MenuSetup();
	virtual void MenuTearDown();

	virtual bool IsOverlayOpened();
	virtual void CloseOverlay();
	virtual bool CanReturn();
	
protected:
	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<APlayerController> OwnerController;

	bool IsValidOwnerController();

	UPROPERTY()
	TObjectPtr<class ABaseHUD> OwnerHUD;

	bool IsValidOwnerHUD();
};
