// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MatchModeTypes.h"
#include "GameFramework/HUD.h"
#include "LBTypes/ChatMode.h"
#include "LBTypes/Team.h"
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
	virtual void CreateVideoSettingMenu();
	virtual void CreateMouseSettingMenu();
	virtual void CreateAudioSettingMenu();
	virtual void ReturnMenu(bool bForceReturn = false);
	virtual void AddNewMenuToStack(class ULBlasterUserWidget* InNewMenu);
	virtual void AddChatMessage(const FString& InPlayerName, const FText& InText, EChatMode InChatMode, ETeam SourceTeam);

	FORCEINLINE EMatchMode GetMatchModeType() const { return MatchModeType; }
	FORCEINLINE int32 GetGoalKillScore() const { return GoalKillScore; }
	
protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

protected:
	UPROPERTY()
	TArray<TObjectPtr<ULBlasterUserWidget>> MenuStack;

	/*
	 * Match Mode
	 */
	EMatchMode MatchModeType;

	/*
	 *	Goal Kill Score
	 */
	int32 GoalKillScore;
};
