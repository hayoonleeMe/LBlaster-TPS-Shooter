// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainMenuUserWidget.h"
#include "LBlasterUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ULBlasterUserWidget : public UMainMenuUserWidget
{
	GENERATED_BODY()
	
protected:
	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<class ALBlasterPlayerController> OwnerLBController;

	bool IsValidOwnerLBController();

	UPROPERTY()
	TObjectPtr<class ALBlasterHUD> LBlasterHUD;

	bool IsValidLBlasterHUD();
};
