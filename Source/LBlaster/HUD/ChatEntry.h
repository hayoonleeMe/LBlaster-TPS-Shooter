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
	void SetChatEntryPrefix(const FText& InText, const FLinearColor& InLinearColor) const;
	void EmptyChatEntryPrefix() const;
	void SetChatEntryPlayerName(const FText& InText, const FLinearColor& InLinearColor) const;
	void SetChatEntryText(const FText& InText) const;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> ChatEntryPrefix;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ChatEntryPlayerName;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ChatEntryText;
};
