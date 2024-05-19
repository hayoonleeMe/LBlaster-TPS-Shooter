// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ChatEntry.h"

#include "Components/RichTextBlock.h"

void UChatEntry::SetChatEntryText(FString ChatTarget, const FString& PlayerName, const FText& Text, const FString& TextStyle)
{
	if (ChatEntryText)
	{
		if (!ChatTarget.IsEmpty())
		{
			ChatTarget = FString::Printf(TEXT("<%s>%s</> "), *TextStyle, *ChatTarget);
		}
		const FString FinalPlayerName = FString::Printf(TEXT("<%s>%s:</> "), *TextStyle, *PlayerName);
		ChatEntryText->SetText(FText::FromString(ChatTarget + FinalPlayerName + Text.ToString()));
	}
}
