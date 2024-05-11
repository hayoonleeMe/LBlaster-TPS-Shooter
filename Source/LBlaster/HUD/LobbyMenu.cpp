// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LobbyMenu.h"

#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/SessionHelperPlayerController.h"

void ULobbyMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton && !StartButton->OnClicked.IsBound())
	{
		StartButton->OnClicked.AddDynamic(this, &ThisClass::OnStartButtonClicked);
	}
	if (QuitButton && !QuitButton->OnClicked.IsBound())
	{
		QuitButton->OnClicked.AddDynamic(this, &ThisClass::OnQuitButtonClicked);
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->LBOnDestroySessionCompleteDelegate.AddUObject(this, &ThisClass::OnDestroySessionComplete);
	}
}

void ULobbyMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// ServerTravel 가능
	if (bWantServerTravel && GetWorld() && GetWorld()->GetNumPlayerControllers() == 1)
	{
		GetWorld()->ServerTravel(FString(TEXT("/Game/LBlaster/Maps/GameStartupMap")));
	}
}

void ULobbyMenu::OnDestroySessionComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		return;
	}

	// DestroySession 성공하면 MainMenu Map으로 이동
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		if (PlayerController->HasAuthority())
		{
			// 해당 세션에 연결된 모든 클라이언트가 세션에서 빠져나가고 호스트만 남으면 메인메뉴로 이동
			if (GetWorld()->GetNumPlayerControllers() == 1)
			{
				GetWorld()->ServerTravel(FString(TEXT("/Game/LBlaster/Maps/GameStartupMap")));	// TODO : 경로 캐싱
			}
		}
		else
		{
			PlayerController->ClientTravel(FString(TEXT("/Game/LBlaster/Maps/GameStartupMap")), TRAVEL_Absolute);	// TODO : 경로 캐싱
		}
	}
}

void ULobbyMenu::OnStartButtonClicked()
{
	Travel();
}

void ULobbyMenu::Travel()
{
	if (UWorld* World = GetWorld())
	{
		World->ServerTravel(FString(TEXT("/Game/LBlaster/Maps/LBlasterMap?listen")));	// TODO : 경로 캐싱
	}
}

void ULobbyMenu::OnQuitButtonClicked()
{
	if (MultiplayerSessionsSubsystem)
	{
		if (APlayerController* OwnerController = GetOwningPlayer())
		{
			if (OwnerController->HasAuthority())
			{
				bWantServerTravel = true;
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

void ULobbyMenu::DestroyAllClientSession()
{
	// 모든 클라이언트의 플레이어 컨트롤러에서 Client RPC를 호출해 DestroySession을 호출하게 한다.
	if (APlayerController* OwnerController = GetOwningPlayer())
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
