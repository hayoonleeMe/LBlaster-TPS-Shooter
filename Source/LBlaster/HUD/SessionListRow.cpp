// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SessionListRow.h"

#include "Components/Border.h"
#include "Components/ListViewBase.h"
#include "Components/TextBlock.h"

static FLinearColor NormalColor(0.f, 0.f, 0.f, 0.6f);
static FLinearColor SelectedColor(0.34375f, 0.34375f, 0.328805f, 0.5f);

void USessionListRow::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (USessionListRow* Data = Cast<USessionListRow>(ListItemObject))
	{
		HostNameString = Data->HostNameString;
		MatchModeString = Data->MatchModeString;
		NumOfCurrentPlayers = Data->NumOfCurrentPlayers;
		NumOfMaxPlayers = Data->NumOfMaxPlayers;
		bClicked = Data->bClicked;

		if (RowBorder)
		{
			RowBorder->SetBrushColor(bClicked ? SelectedColor : NormalColor);
		}
		if (HostNameTextBlock)
		{
			HostNameTextBlock->SetText(FText::FromString(HostNameString));
		}
		if (MatchModeTextBlock)
		{
			MatchModeTextBlock->SetText(FText::FromString(MatchModeString));
		}
		if (NumPlayerTextBlock)
		{
			NumPlayerTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), NumOfCurrentPlayers, NumOfMaxPlayers)));
		}
	}
}

void USessionListRow::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);

	bClicked = bIsSelected;
	if (RowBorder)
	{
		RowBorder->SetBrushColor(bIsSelected ? SelectedColor : NormalColor);
	}
}

void USessionListRow::SetMatchModeString(const FString& InString)
{
	if (InString.Equals(FString(TEXT("FreeForAll"))))
	{
		MatchModeString = FString(TEXT("개인전"));
	}
	else if (InString.Equals(FString(TEXT("TeamDeathMatch"))))
	{
		MatchModeString = FString(TEXT("팀 데스매치"));
	}
}
