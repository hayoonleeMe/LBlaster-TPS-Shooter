// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/DropDownOption.h"

#include "Components/TextBlock.h"

void UDropDownOption::SetText(const FString& InString)
{
	if (OptionText)
	{
		OptionText->SetText(FText::FromString(InString));
	}
}
