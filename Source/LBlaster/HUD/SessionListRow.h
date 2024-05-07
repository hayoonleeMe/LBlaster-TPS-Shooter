// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "SessionListRow.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API USessionListRow : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

	FORCEINLINE void SetHostNameString(const FString& InString) { HostNameString = InString; }
	void SetMatchModeString(const FString& InString);
	FORCEINLINE void SetNumOfCurrentPlayers(int32 InValue) { NumOfCurrentPlayers = InValue; }
	FORCEINLINE void SetNumOfMaxPlayers(int32 InValue) { NumOfMaxPlayers = InValue; }
	
private:
	/*
	 *	Data
	 */
	UPROPERTY()
	FString HostNameString;

	UPROPERTY()
	FString MatchModeString;

	UPROPERTY()
	int32 NumOfCurrentPlayers;

	UPROPERTY()
	int32 NumOfMaxPlayers;

	/*
	 *	Widgets
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBorder> RowBorder;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> HostNameTextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> MatchModeTextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> NumPlayerTextBlock;
};
