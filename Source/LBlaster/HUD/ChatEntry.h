// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LBTypes/ChatParams.h"
#include "ChatEntry.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UChatEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetChatEntryText(const FChatParams& ChatParams, bool bInFriendly) const;
	void SetChatEntryTextForSystem(const FChatParams& ChatParams) const;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URichTextBlock> ChatEntryText;
};
