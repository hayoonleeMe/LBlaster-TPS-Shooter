// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MatchModeTypes.h"
#include "GameFramework/HUD.h"
#include "BaseHUD.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ABaseHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void CreateSettingMenu();
	virtual void CreateGraphicSettingMenu();
	virtual void ReturnMenu(bool bForceReturn = false);
	virtual void AddNewMenuToStack(class ULBlasterUserWidget* InNewMenu);

	FORCEINLINE EMatchMode GetMatchModeType() const { return MatchModeType; }
	
protected:
	virtual void PostInitializeComponents() override;

protected:
	UPROPERTY()
	TArray<TObjectPtr<ULBlasterUserWidget>> MenuStack;

	/*
	 * Match Mode
	 */
	EMatchMode MatchModeType;
};
