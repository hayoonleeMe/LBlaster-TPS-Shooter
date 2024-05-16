// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PlayerListRow.h"

#include "Components/TextBlock.h"

void UPlayerListRow::SetNameText(const FString& InName)
{
	if (NameText)
	{
		NameText->SetText(FText::FromString(InName));
		PlayerName = InName;
	}
}

void UPlayerListRow::HighlightNameText()
{
	if (NameText)
	{
		NameText->SetColorAndOpacity(FSlateColor(HighlightColor));
	}
}
