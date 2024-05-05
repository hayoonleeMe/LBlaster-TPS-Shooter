// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MultiplayerSessionsSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FLBOnCreateSessionCompleteDelegate, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FLBOnFindSessionsCompleteDelegate, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FLBOnJoinSessionCompleteDelegate, EOnJoinSessionCompleteResult::Type Result);
DECLARE_MULTICAST_DELEGATE_OneParam(FLBOnDestroySessionCompleteDelegate, bool bWasSuccessful);

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionsSubsystem();

	/*
	* To handle session functionality. The Menu class will call these
	*/
	void SessionSetup(int32 InNumPublicConnections, const FString& InMatchType, const FString& InLobbyPath);
	void CreateSession();
	void FindSessions(int32 InMaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void DestroySession();
	void StartSession();

	FLBOnCreateSessionCompleteDelegate LBOnCreateSessionCompleteDelegate;
	FLBOnFindSessionsCompleteDelegate LBOnFindSessionsCompleteDelegate;
	FLBOnJoinSessionCompleteDelegate LBOnJoinSessionCompleteDelegate;
	FLBOnDestroySessionCompleteDelegate LBOnDestroySessionCompleteDelegate;

protected:
	/*
	* Internal callbacks for the delegates we'll add to the Online Session Interface delegate list.
	* This don't need to be called outside this class.
	*/
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

private:
	// Pointer to the online session interface
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	
	bool IsValidSessionInterface();

	/*
	 *	To add to the Online Session Interface delegate list.
	 *	We'll bind our MultiplayerSessionsSubsystem internal callbacks to these.
	 */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	
	int32 NumPublicConnections{ 4 };
	FString MatchType{ TEXT("FreeForAll") };
	FString LobbyPath{ TEXT("") };
	
	/*
	 *	세션을 생성할 때 이미 세션이 존재하면 그 세션 제거가 완료될 때 다시 세션 생성하기 위한 트리거
	 */
	bool bCreateSessionOnDestroy = false;
};
