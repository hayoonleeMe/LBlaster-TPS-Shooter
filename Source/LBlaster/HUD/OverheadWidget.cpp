// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverheadWidget.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}

void UOverheadWidget::SetDisplayText(const FString& InText)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(InText));
	}
}

void UOverheadWidget::ShowPlayerName(APawn* InPawn)
{
	if (InPawn)
	{
		if (APlayerState* PlayerState = InPawn->GetPlayerState<APlayerState>())
		{
			SetDisplayText(PlayerState->GetPlayerName());
		}
	}
}
