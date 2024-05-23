// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreboardTeamDeathMatch.h"

#include "ScoreboardRow.h"
#include "Components/VerticalBox.h"
#include "GameState/TeamDeathMatchGameState.h"
#include "Player/LBlasterPlayerState.h"

void UScoreboardTeamDeathMatch::UpdateBoard(bool bPlayerListChanged)
{
	if (GetWorld() && RedTeamBox && BlueTeamBox && IsValidOwnerPlayerState())
	{
		if (ATeamDeathMatchGameState* TDMGameState = GetWorld()->GetGameState<ATeamDeathMatchGameState>())
		{
			// KDA로 팀별 PlayerState 정렬
			Algo::Sort(TDMGameState->RedTeam, [](const ALBlasterPlayerState* A, const ALBlasterPlayerState* B)
			{
				const float A_KDA = A->GetDeath() != 0 ? A->GetScore() / A->GetDeath() : A->GetScore();
				const float B_KDA = B->GetDeath() != 0 ? B->GetScore() / B->GetDeath() : B->GetScore();
				return A_KDA > B_KDA;
			});
			Algo::Sort(TDMGameState->BlueTeam, [](const ALBlasterPlayerState* A, const ALBlasterPlayerState* B)
			{
				const float A_Kda = A->GetDeath() != 0 ? A->GetScore() / A->GetDeath() : A->GetScore();
				const float B_Kda = B->GetDeath() != 0 ? B->GetScore() / B->GetDeath() : B->GetScore();
				return A_Kda > B_Kda;
			});

			// 정렬된 PlayerState들로 RedTeam Vertical Box의 Row의 Text 설정
			int32 Index = 0;
			for (; Index < TDMGameState->RedTeam.Num(); ++Index)
			{
				if (ALBlasterPlayerState* LBPlayerState = TDMGameState->RedTeam[Index])
				{
					if (UScoreboardRow* Row = Cast<UScoreboardRow>(RedTeamBox->GetSlots()[Index]->Content))
					{
						Row->SetScoreboardRowText(LBPlayerState->GetPlayerName(), LBPlayerState->GetScore(), LBPlayerState->GetDeath());
						// 로컬 플레이어 업데이트 시 강조 표시
						if (LBPlayerState->GetPlayerName() == OwnerPlayerState->GetPlayerName())
						{
							Row->HighlightRowText();
						}
						else
						{
							Row->UnhighlightRowText();
						}
					}
				}
			}
			if (bPlayerListChanged)
			{
				// Text가 설정된 Row가 남아있으면 모두 Text 제거
				for (; Index < RedTeamBox->GetSlots().Num(); ++Index)
				{
					if (UScoreboardRow* Row = Cast<UScoreboardRow>(RedTeamBox->GetSlots()[Index]->Content))
					{
						if (Row->bTextSet)
						{
							Row->EmptyScoreboardRowText();
						}
					}
				}
			}

			// 정렬된 PlayerState들로 BlueTeam Vertical Box의 Row의 Text 설정
			Index = 0;
			for (; Index < TDMGameState->BlueTeam.Num(); ++Index)
			{
				if (ALBlasterPlayerState* LBPlayerState = TDMGameState->BlueTeam[Index])
				{
					if (UScoreboardRow* Row = Cast<UScoreboardRow>(BlueTeamBox->GetSlots()[Index]->Content))
					{
						Row->SetScoreboardRowText(LBPlayerState->GetPlayerName(), LBPlayerState->GetScore(), LBPlayerState->GetDeath());
						// 로컬 플레이어 업데이트 시 강조 표시
						if (LBPlayerState->GetPlayerName() == OwnerPlayerState->GetPlayerName())
						{
							Row->HighlightRowText();
						}
						else
						{
							Row->UnhighlightRowText();
						}
					}
				}
			}
			if (bPlayerListChanged)
			{
				// Text가 설정된 Row가 남아있으면 모두 Text 제거
				for (; Index < BlueTeamBox->GetSlots().Num(); ++Index)
				{
					if (UScoreboardRow* Row = Cast<UScoreboardRow>(BlueTeamBox->GetSlots()[Index]->Content))
					{
						if (Row->bTextSet)
						{
							Row->EmptyScoreboardRowText();
						}
					}
				}
			}
		}
	}
}
