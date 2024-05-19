// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatEntry.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UChatEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetChatEntryText(FString ChatTarget, const FString& PlayerName, const FText& Text, const FString& TextStyle);

	inline const static FString FriendlyTeamTextStyle{ TEXT("FriendlyTeam") };
	inline const static FString OpponentTeamTextStyle{ TEXT("OpponentTeam") };
	inline const static FString DefaultTextStyle{ TEXT("Default") };
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URichTextBlock> ChatEntryText;
};
