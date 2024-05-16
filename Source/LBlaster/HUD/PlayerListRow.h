// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerListRow.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UPlayerListRow : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetNameText(const FString& InName);
	FORCEINLINE FString GetPlayerName() const { return PlayerName; }
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBorder> RowBorder;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> NameText;

	UPROPERTY()
	FString PlayerName;
};
