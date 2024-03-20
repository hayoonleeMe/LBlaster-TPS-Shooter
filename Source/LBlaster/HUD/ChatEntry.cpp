// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ChatEntry.h"

#include "Components/TextBlock.h"

void UChatEntry::SetChatEntryText(const FText& InText) const
{
	if (ChatEntryText)
	{
		ChatEntryText->SetText(InText);
	}
}
