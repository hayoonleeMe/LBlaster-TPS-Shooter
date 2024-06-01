// Fill out your copyright notice in the Description page of Project Settings.


#include "ResultMenuFreeForAll.h"

#include "Components/TextBlock.h"
#include "GameState/FreeForAllGameState.h"
#include "Player/LBlasterPlayerState.h"

void UResultMenuFreeForAll::MenuSetup()
{
	Super::MenuSetup();

	if (IsValidOwnerController() && GetWorld() && ResultText)
	{
		if (ALBlasterPlayerState* LBPlayerState = OwnerController->GetPlayerState<ALBlasterPlayerState>())
		{
			if (AFreeForAllGameState* FFAGameState = GetWorld()->GetGameState<AFreeForAllGameState>())
			{
				for (int32 Index = 0; Index < FFAGameState->LBPlayerArray.Num(); ++Index)
				{
					if (LBPlayerState == FFAGameState->LBPlayerArray[Index])
					{
						ResultText->SetText(FText::AsNumber(Index + 1));
						return;
					}
				}
			}
		}
	}
}
