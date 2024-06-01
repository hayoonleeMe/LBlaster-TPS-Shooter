// Fill out your copyright notice in the Description page of Project Settings.


#include "ResultMenuTeamDeathMatch.h"

#include "Components/TextBlock.h"
#include "GameState/TeamDeathMatchGameState.h"
#include "Player/LBlasterPlayerState.h"

void UResultMenuTeamDeathMatch::MenuSetup()
{
	Super::MenuSetup();

	if (IsValidOwnerController() && GetWorld() && ResultText)
	{
		if (ALBlasterPlayerState* LBPlayerState = OwnerController->GetPlayerState<ALBlasterPlayerState>())
		{
			if (ATeamDeathMatchGameState* TDMGameState = GetWorld()->GetGameState<ATeamDeathMatchGameState>())
			{
				const int32 RedTeamScore = TDMGameState->GetRedTeamScore();
				const int32 BlueTeamScore = TDMGameState->GetBlueTeamScore();
				if (RedTeamScore == BlueTeamScore)
				{
					ResultText->SetText(FText::FromString(TEXT("Draw")));
				}
				else
				{
					const ETeam WinningTeam = RedTeamScore > BlueTeamScore ? ETeam::ET_RedTeam : ETeam::ET_BlueTeam;
					if (LBPlayerState->GetTeam() == WinningTeam)
					{
						ResultText->SetText(FText::FromString(TEXT("Win")));
					}
					else
					{
						ResultText->SetText(FText::FromString(TEXT("Lose")));
					}
				}
			}
		}
	}
}
