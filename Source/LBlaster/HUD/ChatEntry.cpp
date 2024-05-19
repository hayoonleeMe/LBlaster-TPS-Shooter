// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ChatEntry.h"

#include "Components/TextBlock.h"

void UChatEntry::SetChatEntryPrefix(const FText& InText, const FLinearColor& InLinearColor) const
{
	if (ChatEntryPrefix)
	{
		ChatEntryPrefix->SetText(InText);
		ChatEntryPrefix->SetColorAndOpacity(FSlateColor(InLinearColor));
	}
}

void UChatEntry::EmptyChatEntryPrefix() const
{
	if (ChatEntryPrefix)
	{
		ChatEntryPrefix->SetText(FText());
	}
}

void UChatEntry::SetChatEntryPlayerName(const FText& InText, const FLinearColor& InLinearColor) const
{
	if (ChatEntryPlayerName)
	{
		ChatEntryPlayerName->SetText(InText);
		ChatEntryPlayerName->SetColorAndOpacity(FSlateColor(InLinearColor));
	}
}

void UChatEntry::SetChatEntryText(const FText& InText) const
{
	if (ChatEntryText)
	{
		ChatEntryText->SetText(InText);
	}
}
