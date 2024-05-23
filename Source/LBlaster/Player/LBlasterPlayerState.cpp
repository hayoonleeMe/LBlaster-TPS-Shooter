// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LBlasterPlayerState.h"

#include "LBlasterPlayerController.h"
#include "Character/LBlasterCharacter.h"
#include "GameInstance/LBGameInstance.h"
#include "GameState/TeamDeathMatchGameState.h"
#include "HUD/LBlasterHUD.h"
#include "Net/UnrealNetwork.h"

ALBlasterPlayerState::ALBlasterPlayerState()
{
	NetUpdateFrequency = 10.f;

	/* Team */
	Team = ETeam::ET_MAX;
}

void ALBlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALBlasterPlayerState, Death);
	DOREPLIFETIME(ALBlasterPlayerState, Team);
}

void ALBlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	
	if (IsValidOwnerCharacter() && IsValidOwnerController())
	{
		OwnerController->SetHUDScore(GetScore());
	}

	if (GetWorld())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
			{
				HUD->UpdateScoreboard(false);
			}	
		}
	}
}

void ALBlasterPlayerState::OnRep_Death()
{
	if (IsValidOwnerCharacter() && IsValidOwnerController())
	{
		OwnerController->SetHUDDeath(Death);
	}

	if (GetWorld())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
			{
				HUD->UpdateScoreboard(false);
			}	
		}
	}
}

void ALBlasterPlayerState::ServerSetTeam_Implementation(ETeam InTeam)
{
	SetTeam(InTeam);
}

void ALBlasterPlayerState::AddToDeath(int32 InDeathAmount)
{
	Death += InDeathAmount;

	if (IsValidOwnerCharacter() && IsValidOwnerController())
	{
		OwnerController->SetHUDDeath(Death);
	}
}

void ALBlasterPlayerState::AddToScore(float InScoreAmount)
{
	SetScore(GetScore() + InScoreAmount);

	if (IsValidOwnerCharacter() && IsValidOwnerController())
	{
		OwnerController->SetHUDScore(GetScore());
	}
}

void ALBlasterPlayerState::InitTeam()
{
	if (HasAuthority())
	{
		if (ATeamDeathMatchGameState* TDMGameState = GetWorld()->GetGameState<ATeamDeathMatchGameState>())
		{
			if (Team == ETeam::ET_RedTeam)
			{
				TDMGameState->AddUniqueRedTeamPlayer(this);
			}
			else if (Team == ETeam::ET_BlueTeam)
			{
				TDMGameState->AddUniqueBlueTeamPlayer(this);
			}

			if (GetWorld())
			{
				if (APlayerController* FirstPlayerController = GetWorld()->GetFirstPlayerController())
				{
					if (ALBlasterHUD* HUD = FirstPlayerController->GetHUD<ALBlasterHUD>())
					{
						HUD->UpdateScoreboard(false);
					}		
				}
			}
		}		
	}
}

void ALBlasterPlayerState::InitTeamFromGameInstance()
{
	// 게임에 참가하기 전에 유저가 GameInstance에 캐싱했던 팀 값을 Autonomous Proxy에서 업데이트
	if (ULBGameInstance* GameInstance = GetGameInstance<ULBGameInstance>())
	{
		FUniqueNetIdRepl NetIdRepl = GetUniqueId();
		if (NetIdRepl.IsValid())
		{
			if (GameInstance->MidGameJoinedPlayerTeamMap.Contains(NetIdRepl))
			{
				ServerSetTeam(GameInstance->MidGameJoinedPlayerTeamMap[NetIdRepl]);
				GameInstance->MidGameJoinedPlayerTeamMap.Remove(NetIdRepl);
			}
		}
	}
}

FTeamCharacterMaterials ALBlasterPlayerState::GetCharacterMaterials() const
{
	if (TeamCharacterMaterialsMap.Contains(Team))
	{
		return TeamCharacterMaterialsMap[Team];
	}
	return FTeamCharacterMaterials();
}

void ALBlasterPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	// 새로 사용할 PlayerState에 이전 PlayerState의 값을 복사한다.
	if (ALBlasterPlayerState* OutPlayerState = Cast<ALBlasterPlayerState>(PlayerState))
	{
		OutPlayerState->Team = Team;
	}
}

bool ALBlasterPlayerState::IsValidOwnerCharacter()
{
	if (!OwnerCharacter && GetPawn())
	{
		OwnerCharacter = Cast<ALBlasterCharacter>(GetPawn());
	}
	return OwnerCharacter != nullptr;
}

bool ALBlasterPlayerState::IsValidOwnerController()
{
	if (!OwnerController && IsValidOwnerCharacter() && OwnerCharacter->Controller)
	{
		OwnerController = Cast<ALBlasterPlayerController>(OwnerCharacter->Controller);
	}
	return OwnerController != nullptr;
}
