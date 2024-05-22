// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreboardFreeForAll.h"

#include "ScoreboardRowWithRank.h"
#include "Components/VerticalBox.h"
#include "GameFramework/PlayerState.h"
#include "GameState/LBlasterGameState.h"
#include "Player/LBlasterPlayerState.h"

void UScoreboardFreeForAll::UpdateBoard(bool bPlayerListChanged)
{
	TArray<ALBlasterPlayerState*> LBlasterPlayerStates;

	if (GetWorld() && LeftBox && RightBox && IsValidOwnerPlayerState())
	{
		if (ALBlasterGameState* LBGameState = GetWorld()->GetGameState<ALBlasterGameState>())
		{
			// ALBlasterPlayerState로 캐스팅해 복사
			for (APlayerState* PlayerState : LBGameState->PlayerArray)
			{
				if (ALBlasterPlayerState* LBPlayerState = Cast<ALBlasterPlayerState>(PlayerState))
				{
					LBlasterPlayerStates.Add(LBPlayerState);
				}
			}

			// KDA를 기준으로 내림차순 정렬
			Algo::Sort(LBlasterPlayerStates, [](const ALBlasterPlayerState* A, const ALBlasterPlayerState* B)
			{
				const float A_KDA = A->GetDeath() != 0 ? A->GetScore() / A->GetDeath() : A->GetScore();
				const float B_KDA = B->GetDeath() != 0 ? B->GetScore() / B->GetDeath() : B->GetScore();
				return A_KDA > B_KDA;
			});

			int32 Index = 0;
			for (; Index < LBlasterPlayerStates.Num(); ++Index)
			{
				if (ALBlasterPlayerState* LBPlayerState = LBlasterPlayerStates[Index])
				{
					const UVerticalBox* BoxToUse = Index < LeftBox->GetSlots().Num() ? LeftBox : RightBox;
					if (UScoreboardRowWithRank* Row = Cast<UScoreboardRowWithRank>(BoxToUse->GetSlots()[Index]->Content))
					{
						Row->SetScoreboardRowText(Index + 1, LBPlayerState->GetPlayerName(), LBPlayerState->GetScore(), LBPlayerState->GetDeath());
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
				for (; Index < LeftBox->GetSlots().Num() + RightBox->GetSlots().Num(); ++Index)
				{
					const UVerticalBox* BoxToUse = Index < LeftBox->GetSlots().Num() ? LeftBox : RightBox;
					const int32 IndexToUse = Index < LeftBox->GetSlots().Num() ? Index : Index - LeftBox->GetSlots().Num();
					if (UScoreboardRow* Row = Cast<UScoreboardRow>(BoxToUse->GetSlots()[IndexToUse]->Content))
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
