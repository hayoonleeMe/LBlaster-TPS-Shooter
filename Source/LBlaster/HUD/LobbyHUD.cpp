// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LobbyHUD.h"

#include "LobbyMenu.h"
#include "LobbyMenuTeamDeathMatch.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "Player/LBlasterPlayerState.h"
#include "Player/LobbyPlayerController.h"

void ALobbyHUD::AddNewPlayer(ALBlasterPlayerState* InPlayerState) const
{
	if (LobbyMenu)
	{
		if (MatchMode == EMatchMode::TeamDeathMatch)
		{
			if (ULobbyMenuTeamDeathMatch* LobbyMenuTDM = Cast<ULobbyMenuTeamDeathMatch>(LobbyMenu))
			{
				LobbyMenuTDM->AddNewPlayer(InPlayerState);
			}
		}
		else if (MatchMode == EMatchMode::FreeForAll)
		{
			// TODO : FreeForAll	
		}
	}
}

void ALobbyHUD::AddNewPlayerForClient(ETeam InTeam, const FString& InName)
{
	if (!LobbyMenu || !IsValidOwnerController() || !OwnerController->GetPlayerState<ALBlasterPlayerState>())
	{
		// 모든 요소가 제대로 초기화되기 전에 호출되면 캐싱
		CachedAddNewPlayer.Add(MakeTuple(InTeam, InName));
		return;
	}
	
	if (LobbyMenu)
	{
		if (ULobbyMenuTeamDeathMatch* LobbyMenuTDM = Cast<ULobbyMenuTeamDeathMatch>(LobbyMenu))
		{
			if (InTeam == ETeam::ET_RedTeam)
			{
				LobbyMenuTDM->AddRedTeamForClient(InName);
			}
			else if (InTeam == ETeam::ET_BlueTeam)
			{
				LobbyMenuTDM->AddBlueTeamForClient(InName);
			}	
		}
	}
}

void ALobbyHUD::ChangePlayerTeamFromClient(ETeam CurrentTeam, ETeam NewTeam, ALBlasterPlayerState* InPlayerState)
{
	if (LobbyMenu)
	{
		if (ULobbyMenuTeamDeathMatch* LobbyMenuTDM = Cast<ULobbyMenuTeamDeathMatch>(LobbyMenu))
		{
			LobbyMenuTDM->ChangeTeam(CurrentTeam, NewTeam, InPlayerState);
		}
	}
}

void ALobbyHUD::ChangePlayerTeamForClient(ETeam CurrentTeam, ETeam NewTeam, const FString& InName)
{
	if (LobbyMenu)
	{
		if (ULobbyMenuTeamDeathMatch* LobbyMenuTDM = Cast<ULobbyMenuTeamDeathMatch>(LobbyMenu))
		{
			LobbyMenuTDM->ChangeTeamForClient(CurrentTeam, NewTeam, InName);
		}
	}
}

void ALobbyHUD::RemoveExitingPlayer(ETeam InTeam, const FString& InName, bool bHasAuthority)
{
	if (LobbyMenu)
	{
		if (ULobbyMenuTeamDeathMatch* LobbyMenuTDM = Cast<ULobbyMenuTeamDeathMatch>(LobbyMenu))
		{
			if (InTeam == ETeam::ET_RedTeam)
			{
				LobbyMenuTDM->RemovePlayerFromRedTeam(InName, bHasAuthority);
			}
			else if (InTeam == ETeam::ET_BlueTeam)
			{
				LobbyMenuTDM->RemovePlayerFromBlueTeam(InName, bHasAuthority);
			}
		}
	}
}

void ALobbyHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PollInit();
	
	// ServerTravel 가능
	if (bWantReturnToMainMenu && GetWorld() && GetWorld()->GetNumPlayerControllers() == 1)
	{
		GetWorld()->ServerTravel(FString(TEXT("/Game/LBlaster/Maps/GameStartupMap")));
	}
}

void ALobbyHUD::ReturnToMainMenu()
{
	if (MultiplayerSessionsSubsystem)
	{
		if (IsValidOwnerController())
		{
			if (OwnerController->HasAuthority())
			{
				bWantReturnToMainMenu = true;
				DestroyAllClientSession();
				MultiplayerSessionsSubsystem->DestroySession();
			}
			else
			{
				MultiplayerSessionsSubsystem->DestroySession();
			}
		}
	}
}

void ALobbyHUD::TravelToMatch()
{
	// TODO : Game Instance에 Team 저장, 현재는 임시로 랜덤한 팀 부여
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		if (It->IsValid())
		{
			if (ALBlasterPlayerState* LBPlayerState = It->Get()->GetPlayerState<ALBlasterPlayerState>())
			{
				ETeam Team = FMath::RandBool() ? ETeam::ET_RedTeam : ETeam::ET_BlueTeam;
				LBPlayerState->SetTeam(Team);
			}
		}
	}

	if (UWorld* World = GetWorld())
	{
		World->ServerTravel(FString(TEXT("/Game/LBlaster/Maps/LBlasterMap?listen")));	// TODO : 경로 캐싱
	}
}

void ALobbyHUD::BroadcastAddPlayerList(ETeam InTeam, const FString& InName)
{
	if (UWorld* World = GetWorld())
	{
		for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)
		{
			if (IsValidOwnerController() && OwnerController != It->Get())
			{
				if (ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(It->Get()))
				{
					// 처음 연결된 클라이언트라면 호스트 이름도 적용시킴
					if (LobbyPlayerController->bFirstTimeConnected && IsValidOwnerController())
					{
						if (ALBlasterPlayerState* LBPlayerState = OwnerController->GetPlayerState<ALBlasterPlayerState>())
						{
							LobbyPlayerController->bFirstTimeConnected = false;
							LobbyPlayerController->ClientSendAddPlayerList(LBPlayerState->GetTeam(), LBPlayerState->GetPlayerName());
						}
					}

					// 변경사항을 클라이언트에 적용
					LobbyPlayerController->ClientSendAddPlayerList(InTeam, InName);
				}
			}
		}
	}
}

void ALobbyHUD::BroadcastTeamChangePlayerList(ETeam CurrentTeam, ETeam NewTeam, const FString& InName)
{
	if (UWorld* World = GetWorld())
	{
		for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)
		{
			if (IsValidOwnerController() && OwnerController != It->Get())
			{
				if (ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(It->Get()))
				{
					// 변경사항을 클라이언트에 적용
					LobbyPlayerController->ClientSendTeamChangePlayerList(CurrentTeam, NewTeam, InName);
				}
			}
		}
	}
}

void ALobbyHUD::BroadcastRemovePlayerList(ETeam InTeam, const FString& InName)
{
	if (UWorld* World = GetWorld())
	{
		for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)
		{
			if (IsValidOwnerController() && OwnerController != It->Get())
			{
				if (ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(It->Get()))
				{
					// 변경사항을 클라이언트에 적용
					LobbyPlayerController->ClientSendRemovePlayerList(InTeam, InName);
				}
			}
		}
	}
}

void ALobbyHUD::BeginPlay()
{
	Super::BeginPlay();

	/* MultiplayerSessionsSubsystem */
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->LBOnDestroySessionCompleteDelegate.AddUObject(this, &ThisClass::OnDestroySessionComplete);

		if (FNamedOnlineSession* NamedOnlineSession = MultiplayerSessionsSubsystem->GetNamedOnlineSession())
		{
			int32 Value;
			NamedOnlineSession->SessionSettings.Get(UMultiplayerSessionsSubsystem::MatchModeKey, Value);
			MatchMode = static_cast<EMatchMode>(Value);
		}
	}

	AddLobbyMenu();
}

void ALobbyHUD::PollInit()
{
	// 모든 요소가 제대로 초기화될 때 캐싱된 작업 수행
	if (!bFirstTimeInit)
	{
		if (LobbyMenu && IsValidOwnerController() && OwnerController->GetPlayerState<ALBlasterPlayerState>())
		{
			bFirstTimeInit = true;
			
			// 제대로 초기화되기 전에 AddNewPlayer가 호출되어 캐싱된 작업을 수행
			if (CachedAddNewPlayer.Num() > 0)
			{
				for (const TTuple<ETeam, FString>& Tuple : CachedAddNewPlayer)
				{
					AddNewPlayerForClient(Tuple.Get<0>(), Tuple.Get<1>());
				}
				CachedAddNewPlayer.Empty();
			}
		}	
	}
}

bool ALobbyHUD::IsValidOwnerController()
{
	if (!OwnerController && GetOwningPlayerController())
	{
		OwnerController = Cast<APlayerController>(GetOwningPlayerController());
	}
	return OwnerController != nullptr;
}

void ALobbyHUD::OnDestroySessionComplete(bool bWasSuccessful)
{
	if (LobbyMenu)
	{
		LobbyMenu->OnDestroySessionComplete(bWasSuccessful);
	}
	
	// DestroySession 성공하면 MainMenu Map으로 이동
	if (bWasSuccessful)
	{
		if (IsValidOwnerController())
		{
			if (OwnerController->HasAuthority())
			{
				// 해당 세션에 연결된 모든 클라이언트가 세션에서 빠져나가고 호스트만 남으면 메인메뉴로 이동
				if (GetWorld()->GetNumPlayerControllers() == 1)
				{
					GetWorld()->ServerTravel(FString(TEXT("/Game/LBlaster/Maps/GameStartupMap")));	// TODO : 경로 캐싱
				}
			}
			else
			{
				OwnerController->ClientTravel(FString(TEXT("/Game/LBlaster/Maps/GameStartupMap")), TRAVEL_Absolute);	// TODO : 경로 캐싱
			}
		}	
	}
}

void ALobbyHUD::AddLobbyMenu()
{
	if (LobbyMenuClassByMatchModeMap.Contains(MatchMode) && LobbyMenuClassByMatchModeMap[MatchMode] && !LobbyMenu)
	{
		if (IsValidOwnerController())
		{
			LobbyMenu = CreateWidget<ULobbyMenu>(OwnerController, LobbyMenuClassByMatchModeMap[MatchMode]);
		}
	}

	if (LobbyMenu)
	{
		LobbyMenu->MenuSetup();

		// 호스트를 리스트에 추가
		if (IsValidOwnerController() && OwnerController->HasAuthority())
		{
			if (ALBlasterPlayerState* LBPlayerState = OwnerController->GetPlayerState<ALBlasterPlayerState>())
			{
				if (MatchMode == EMatchMode::TeamDeathMatch)
				{
					if (ULobbyMenuTeamDeathMatch* LobbyMenuTDM = Cast<ULobbyMenuTeamDeathMatch>(LobbyMenu))
					{
						LobbyMenuTDM->AddNewPlayer(LBPlayerState);
					}
				}
				else if (MatchMode == EMatchMode::FreeForAll)
				{
					// TODO : FreeForAll	
				}				
			}
		}
	}
}

void ALobbyHUD::DestroyAllClientSession()
{
	// 모든 클라이언트의 플레이어 컨트롤러에서 Client RPC를 호출해 DestroySession을 호출하게 한다.
	if (IsValidOwnerController())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PlayerController = It->Get(); PlayerController != OwnerController)
			{
				if (ASessionHelperPlayerController* SHController = Cast<ASessionHelperPlayerController>(PlayerController))
				{
					SHController->ClientDestroySession();
				}
			}
		}	
	}
}
