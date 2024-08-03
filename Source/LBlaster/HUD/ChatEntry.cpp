// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ChatEntry.h"

#include "Components/RichTextBlock.h"

void UChatEntry::SetChatEntryText(const FChatParams& ChatParams, bool bInFriendly) const
{
	if (!ChatEntryText)
	{
		return;
	}

	const FString ChatTargetPrefix = ChatTextStyle::GetChatTargetPrefix(ChatParams.ChatMode);
	const FString TextStyle = ChatTextStyle::GetChatTextStyle(ChatParams.ChatMode, bInFriendly);
	const FString FinalPrefix = FString::Printf(TEXT("<%s>%s %s:</> "), *TextStyle, *ChatTargetPrefix, *ChatParams.SenderPlayerName);
	
	const FText FinalText = FText::FromString(FinalPrefix + ChatParams.Content);
	ChatEntryText->SetText(FinalText);
}

void UChatEntry::SetChatEntryTextForSystem(const FChatParams& ChatParams) const
{
	if (!ChatEntryText)
	{
		return;
	}

	const FString ChatTargetPrefix = ChatTextStyle::GetChatTargetPrefix(ChatParams.ChatMode);
	const FString TextStyle = ChatTextStyle::GetChatTextStyle(ChatParams.ChatMode);
	const FString FinalPrefix = FString::Printf(TEXT("<%s>%s %s</> "), *TextStyle, *ChatTargetPrefix, *ChatParams.SenderPlayerName);
	
	const FString SystemInfoTemplate = ChatSystemInfoTemplate::GetChatSystemInfoTemplate(ChatParams.ChatSystemInfoTemplate);
	const FString FinalTemplate = FString::Printf(TEXT("<%s>%s</>"), *TextStyle, *SystemInfoTemplate);
	
	const FText FinalText = FText::FromString(FinalPrefix + FinalTemplate);
	ChatEntryText->SetText(FinalText);
}
