// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "Scoreboard.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UScoreboard : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:
	virtual void UpdateBoard();
	virtual void SetScoreboardForResultMenu();

protected:
	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<class ALBlasterPlayerState> OwnerPlayerState;

	bool IsValidOwnerPlayerState();

	bool bUsedForResultMenu = false;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBorder> Border;
	
private:
	UPROPERTY(EditAnywhere, Category="LBlaster")
	TSubclassOf<class UScoreboardRow> ScoreboardRowClass;
};
