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
	FString FinalContent = ChatParams.Content;
	if (ChatParams.ChatMode == EChatMode::ECM_System)
	{
		FinalContent = FString::Printf(TEXT("<%s>%s</>"), *TextStyle, *ChatParams.Content);
	}
	const FText FinalText = FText::FromString(FinalPrefix + FinalContent);
	ChatEntryText->SetText(FinalText);
}
