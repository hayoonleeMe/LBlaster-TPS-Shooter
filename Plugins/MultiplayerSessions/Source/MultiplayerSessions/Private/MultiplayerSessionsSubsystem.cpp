// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"

#include "MultiplayerMapPath.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
	OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnCreateSessionComplete)),
	OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionComplete)),
	OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnJoinSessionComplete)),
	OnDestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnDestroySessionComplete))
{
}

void UMultiplayerSessionsSubsystem::CreateSession(EMatchMode InMatchModeType, int32 InNumMaxPlayer, int32 InGoalKillScore)
{
	if (!IsValidSessionInterface())
	{
		LBOnCreateSessionCompleteDelegate.Broadcast(false);
		return;
	}

	// Session Data Caching
	MatchModeType = InMatchModeType;
	NumPublicConnections = InNumMaxPlayer;
	GoalKillScore = InGoalKillScore;

	// 이미 세션이 존재하면 세션 제거. 제거가 완료되면 다시 CreateSession() 호출됨
	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		bCreateSessionOnDestroy = true;
		DestroySession();
		return;
	}

	// Store the delegate in a FDelegateHandle so we can later remove it from the delegate list
	OnCreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	
	// OnlineSubsystem을 사용하지 않으면 LANMatch
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == TEXT("NULL");

	// 게임에 존재할 수 있는 최대 플레이어의 수
	LastSessionSettings->NumPublicConnections = NumPublicConnections;		

	// 세션이 작동중일 때 다른 플레이어가 참가할 수 있는지 여부
	LastSessionSettings->bAllowJoinInProgress = true;						

	// Presence로 참가할 수 있는지 여부, Presence : 게임을 찾을 때 같은 지역의 플레이어만 참가할 수 있도록 하는 것
	LastSessionSettings->bAllowJoinViaPresence = true;

	// 스팀이 세션을 광고하여 다른 플레이어가 세션을 찾아서 참가할 수 있는지 여부
	LastSessionSettings->bShouldAdvertise = true;

	// 유저 Presence 정보를 표시할 것인지 여부
	LastSessionSettings->bUsesPresence = true;
	
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	
	// MatchModeType을 저장, 이 값들은 개발자 의도대로 관리
	LastSessionSettings->Set(FMultiplayerMapPath::MatchModeKey, static_cast<int32>(MatchModeType), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// GoalKillScore를 저장
	LastSessionSettings->Set(FMultiplayerMapPath::GoalKillScoreKey, GoalKillScore, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	// 1로 설정하면 여러 유저가 각각 고유의 빌드와 호스팅을 할 수 있다. 이후 유효한 게임 세션을 검색할 때 각각의 여러 세션들을 검색하고 참가할 수 있다. 만약 1이 아니면 다른 유저들의 세션들을 볼 수 없고 첫번째로 열리는 게임 세션에 참가하려고 할 것이다.
	LastSessionSettings->BuildUniqueId = 1;	
	
	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
	{
		if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
		{
			// 세션 생성에 실패하면 델리게이트 핸들 초기화
			SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			LBOnCreateSessionCompleteDelegate.Broadcast(false);
		}	
	}
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	LBOnCreateSessionCompleteDelegate.Broadcast(bWasSuccessful);
	
	if (IsValidSessionInterface())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
	}

	if (bWasSuccessful)
	{
		if (UWorld* World = GetWorld())
		{
			World->ServerTravel(FMultiplayerMapPath::LobbyPath);
		}
	}
}

void UMultiplayerSessionsSubsystem::FindSessions(int32 InMaxSearchResults)
{
	if (!IsValidSessionInterface())
    {
		LBOnFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
        return;
    }

    OnFindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

    LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	
	// 최대 세션검색 결과의 수, 현재 Dev App Id를 480으로 쓰므로 크게 설정한다.
    LastSessionSearch->MaxSearchResults = InMaxSearchResults;
	
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == TEXT("NULL");
	
	// 우리가 찾는 세션이 Presence를 사용하는 것을 명시하도록 쿼리세팅 설정
    LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);	

    if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
    {
    	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
    	{
    		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
    		LBOnFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
    	}	
    }
}

void UMultiplayerSessionsSubsystem::OnFindSessionComplete(bool bWasSuccessful)
{
	LBOnFindSessionsCompleteDelegate.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
	
	if (IsValidSessionInterface())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
	}
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!IsValidSessionInterface())
	{
		LBOnJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}
	
	OnJoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
		LBOnJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	LBOnJoinSessionCompleteDelegate.Broadcast(Result);

	if (!IsValidSessionInterface())
	{
		return;
	}

	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
	
	FString Address;
	if (SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
		{
			PlayerController->ClientTravel(Address, TRAVEL_Absolute);
		}
	}
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!IsValidSessionInterface())
	{
		LBOnDestroySessionCompleteDelegate.Broadcast(false);
		return;
	}

	OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

	// 제거할 세션이 없으면 true 반환하고 끝냄
	if (!GetNamedOnlineSession())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		LBOnDestroySessionCompleteDelegate.Broadcast(true);
	}
	else if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		LBOnDestroySessionCompleteDelegate.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	LBOnDestroySessionCompleteDelegate.Broadcast(bWasSuccessful);
	
	if (IsValidSessionInterface())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
	}

	// 세션을 성공적으로 삭제했고, 세션 생성 중에 지운 경우라면 다시 세션 생성
	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		CreateSession(MatchModeType, NumPublicConnections, GoalKillScore);
	}
}

FNamedOnlineSession* UMultiplayerSessionsSubsystem::GetNamedOnlineSession(FName SessionName)
{
	if (SessionInterface)
	{
		return SessionInterface->GetNamedSession(SessionName);
	}
	return nullptr;
}

FString UMultiplayerSessionsSubsystem::GetSessionInfo()
{
	if (!SessionInterface)
	{
		return FString(TEXT("Invalid SessionInterface"));
	}

	if (FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(NAME_GameSession))
	{
		FString Str;
		Str += FString::Printf(TEXT("Valid NamedOnlineSession, [SessionName %s]  "), *NamedSession->SessionName.ToString());

		Str += TEXT("[SessionId ");
		if (NamedSession->SessionInfo.IsValid() && NamedSession->SessionInfo->GetSessionId().IsValid())
		{
			Str += NamedSession->SessionInfo->GetSessionId().ToString();
		}
		else
		{
			Str += TEXT("Invalid");
		}
		Str += TEXT("]  ");

		Str += TEXT("[OwningUserId ");
		if (NamedSession->OwningUserId.IsValid())
		{
			Str += NamedSession->OwningUserId->ToString();
		}
		else
		{
			Str += TEXT("Invalid");
		}
		Str += TEXT("]  ");

		Str += TEXT("[LocalOwnerId ");
		if (NamedSession->LocalOwnerId.IsValid())
		{
			Str += NamedSession->LocalOwnerId->ToString();
		}
		else
		{
			Str += TEXT("Invalid");
		}
		Str += TEXT("]  ");
		
		Str += FString::Printf(TEXT("RegisteredPlayers : [RegisteredPlayers.Num() %d, "), NamedSession->RegisteredPlayers.Num());
		for (int32 Index = 0; Index < NamedSession->RegisteredPlayers.Num(); ++Index)
		{
			Str += FString::Printf(TEXT("%d : "), Index);
			FUniqueNetIdRef IdRef = NamedSession->RegisteredPlayers[Index];
			
			if (IdRef->IsValid())
			{
				Str += IdRef->ToString() + TEXT(", ");
			}
			else
			{
				Str += TEXT("Invalid");
			}
		}
		Str += TEXT("]  ");

		return Str;
	}
	return FString(TEXT("Invalid NamedOnlineSession"));
}

bool UMultiplayerSessionsSubsystem::IsValidSessionInterface()
{
	if (!SessionInterface)
	{
		if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
		{
			SessionInterface = OnlineSubsystem->GetSessionInterface();
		}
	}
	return SessionInterface.IsValid();
}
