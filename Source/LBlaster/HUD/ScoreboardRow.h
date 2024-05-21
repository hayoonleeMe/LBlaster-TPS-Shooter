// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreboardRow.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UScoreboardRow : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetScoreboardRowText(const FString& InPlayerName, int32 InScore, int32 InDeath);

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> PlayerNameText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ScoreText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> DeathText;
};
