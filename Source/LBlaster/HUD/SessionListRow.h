// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MatchModeTypes.h"
#include "OnlineSessionSettings.h"
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
	void SetSessionResult(const FOnlineSessionSearchResult& InSessionResult);
	FORCEINLINE FOnlineSessionSearchResult GetSessionResult() const { return SessionResult; }

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	
private:
	/*
	 *	Data
	 */
	FOnlineSessionSearchResult SessionResult;
	
	UPROPERTY()
	FString HostNameString;

	UPROPERTY()
	EMatchMode MatchModeType;

	UPROPERTY()
	int32 NumOfCurrentPlayers;

	UPROPERTY()
	int32 NumOfMaxPlayers;

	/*
	 *	Toggle
	 */
	bool bClicked = false;

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
