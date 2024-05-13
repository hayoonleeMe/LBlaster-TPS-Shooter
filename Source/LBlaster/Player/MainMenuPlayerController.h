// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMainMenuPlayerController();
	
protected:
	virtual void BeginPlay() override;
	
	/*
	 *	Input
	 */
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<class UInputMappingContext> MenuMappingContext;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<class UInputAction> ReturnMenuAction;

	void ReturnMenu();

private:
	/*
	 *	Owning
	 */
	UPROPERTY()
	TObjectPtr<class AMainMenuHUD> OwningHUD;

	bool IsValidOwningHUD();
};
