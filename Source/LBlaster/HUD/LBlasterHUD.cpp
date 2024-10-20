// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LBlasterHUD.h"

#include "Announcement.h"
#include "AudioSettingMenu.h"
#include "CharacterOverlay.h"
#include "ChatBox.h"
#include "Crosshair.h"
#include "ElimAnnouncement.h"
#include "HitDirectionIndicator.h"
#include "VideoSettingMenu.h"
#include "LBlaster.h"
#include "MatchTimeTimer.h"
#include "MiniScoreboard.h"
#include "MiniScoreboardFreeForAll.h"
#include "MiniScoreboardTeamDeathMatch.h"
#include "MouseSettingMenu.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "PauseMenu.h"
#include "RespawnTimer.h"
#include "ResultMenu.h"
#include "Scoreboard.h"
#include "SettingsHubMenu.h"
#include "SniperScope.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Player/LBlasterPlayerController.h"
#include "Components/HorizontalBox.h"
#include "HUD/ChatUI.h"

ALBlasterHUD::ALBlasterHUD()
{
	/* Crosshair */
	CrosshairSpreadMax = 16.f;

	/* Elim Announcement */
	ElimAnnouncementTime = 3.5f;
}

void ALBlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	if (Crosshair)
	{
		const float SpreadScaled = CrosshairSpreadMax * CrosshairSpread;
		Crosshair->UpdateCrosshair(SpreadScaled, CrosshairColor);
	}
}

void ALBlasterHUD::UpdateCrosshair(float InCrosshairSpread, const FLinearColor& InCrosshairColor)
{
	CrosshairSpread = InCrosshairSpread;
	CrosshairColor = InCrosshairColor;
}

void ALBlasterHUD::DrawCrosshair(const FCrosshairTexture& CrosshairTexture) const
{
	if (Crosshair)
	{
		Crosshair->DrawCrosshair(CrosshairTexture);
	}
}

void ALBlasterHUD::SetPlayerNameText(const FString& InPlayerName) const
{
	if (Crosshair)
	{
		Crosshair->SetPlayerNameText(InPlayerName);
	}
}

void ALBlasterHUD::MarkPlayerHit(bool bElimination) const
{
	if (Crosshair)
	{
		Crosshair->MarkPlayerHit(bElimination);
	}
}

void ALBlasterHUD::IndicateHitDirection(const FVector& InHitNormal)
{
	if (IsValidOwnerController() && HitDirectionIndicatorClass)
	{
		if (UHitDirectionIndicator* HitDirectionIndicator = CreateWidget<UHitDirectionIndicator>(OwnerController, HitDirectionIndicatorClass))
		{
			HitDirectionIndicator->SetHitNormal(InHitNormal);
			HitDirectionIndicator->MenuSetup();
		}
	}
}

void ALBlasterHUD::SetHUDHealth(float InHealth, float InMaxHealth)
{
	if (CharacterOverlay)
	{
		CharacterOverlay->SetHealthBar(InHealth, InMaxHealth);
		CharacterOverlay->SetHealthText(InHealth, InMaxHealth);
	}
}

void ALBlasterHUD::SetHUDAmmo(int32 InAmmo)
{
	if (CharacterOverlay)
	{
		CharacterOverlay->SetAmmoAmount(InAmmo);
	}
}

void ALBlasterHUD::SetHUDCarriedAmmo(int32 InCarriedAmmo)
{
	if (CharacterOverlay)
	{
		CharacterOverlay->SetCarriedAmmoAmount(InCarriedAmmo);
	}
}

void ALBlasterHUD::SetHUDWeaponTypeText(const FString& InWeaponTypeString)
{
	if (CharacterOverlay)
	{
		CharacterOverlay->SetWeaponTypeText(InWeaponTypeString);
	}
}

void ALBlasterHUD::SetHUDMatchCountdown(float InCountdownTime, bool bPlayAnimation)
{
	if (MatchTimeTimer)
	{
		MatchTimeTimer->SetMatchCountdownText(InCountdownTime, bPlayAnimation);
	}
}

void ALBlasterHUD::SetHUDGrenadeAmount(int32 InGrenadeAmount)
{
	if (CharacterOverlay)
	{
		CharacterOverlay->SetGrenadeAmount(InGrenadeAmount);
	}
}

void ALBlasterHUD::AddCharacterOverlay()
{
	if (CharacterOverlay)
	{
		return;
	}
	
	if (IsValidOwnerController())
	{
		if (CharacterOverlayClass)
		{
			CharacterOverlay = CreateWidget<UCharacterOverlay>(OwnerController, CharacterOverlayClass);
			CharacterOverlay->AddToViewport();

			SetHUDAmmo(0);
			SetHUDCarriedAmmo(0);
			SetHUDWeaponTypeText(FString());
			SetWeaponSlotIcon(EEquipSlot::EES_FirstSlot, EWeaponType::EWT_Unarmed);
			SetWeaponSlotIcon(EEquipSlot::EES_SecondSlot, EWeaponType::EWT_Unarmed);
			SetWeaponSlotIcon(EEquipSlot::EES_ThirdSlot, EWeaponType::EWT_Unarmed);
			OwnerController->UpdateHUDHealth();
			OwnerController->UpdateHUDGrenadeAmount();
		}
	}
}

void ALBlasterHUD::RemoveCharacterOverlay()
{
	if (CharacterOverlay)
	{
		CharacterOverlay->RemoveFromParent();
	}
}

void ALBlasterHUD::AddMatchTimeTimer()
{
	if (!MatchTimeTimer && MatchTimeTimerClass && IsValidOwnerController())
	{
		MatchTimeTimer = CreateWidget<UMatchTimeTimer>(OwnerController, MatchTimeTimerClass);
	}
	if (MatchTimeTimer)
	{
		MatchTimeTimer->MenuSetup();
	}
}

void ALBlasterHUD::AddCrosshair()
{
	if (!Crosshair && CrosshairClass && IsValidOwnerController())
	{
		Crosshair = CreateWidget<UCrosshair>(OwnerController, CrosshairClass);
	}
	if (Crosshair)
	{
		Crosshair->MenuSetup();
		OnDesiredCrosshairVisibilityChanged.BindUObject(Crosshair, &UCrosshair::OnDesiredCrosshairVisibilityChanged);
	}
}

void ALBlasterHUD::ElimAnnouncementTimerFinished(UElimAnnouncement* MessageToRemove)
{
	if (MessageToRemove)
	{
		MessageToRemove->RemoveFromParent();

		if (ElimMessages.Contains(MessageToRemove))
		{
			ElimMessages.Remove(MessageToRemove);
		}
	}
}

void ALBlasterHUD::ReturnMenu(bool bForceReturn)
{
	int32 LastMenuIndex = MenuStack.Num() - 1;
	if (MenuStack.IsValidIndex(LastMenuIndex) && MenuStack[LastMenuIndex])
	{
		// 현재 메뉴에서 Return할 수 없는 작업을 수행 중이면 Early Return
		if (!MenuStack[LastMenuIndex]->CanReturn())
		{
			return;
		}

		// 이전 메뉴로 이동
		if (!MenuStack[LastMenuIndex]->IsOverlayOpened() || bForceReturn)
		{
			MenuStack[LastMenuIndex]->MenuTearDown();
			MenuStack.RemoveAt(LastMenuIndex);
			--LastMenuIndex;
		}
		// 열린 Overlay 닫음
		else
		{
			MenuStack[LastMenuIndex]->CloseOverlay();
		}
	}

	// Pause Menu로 돌아감
	if (MenuStack.Num() == 0)
	{
		PauseMenu->SetVisibility(ESlateVisibility::Visible);

		if (IsValidOwnerController())
		{
			OwnerController->DisableMenuMappingContext();
		}
	}
	else
	{
		if (MenuStack[LastMenuIndex])
		{
			MenuStack[LastMenuIndex]->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ALBlasterHUD::AddNewMenuToStack(ULBlasterUserWidget* InNewMenu)
{
	if (!InNewMenu)
	{
		return;
	}
	
	// Pause Menu에서 새로운 메뉴 오픈
	if (MenuStack.Num() == 0)
	{
		PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		
		if (IsValidOwnerController())
		{
			OwnerController->EnableMenuMappingContext();
		}
	}
	else
	{
		const int32 LastMenuIndex = MenuStack.Num() - 1;
		if (MenuStack.IsValidIndex(LastMenuIndex) && MenuStack[LastMenuIndex])
		{
			MenuStack[LastMenuIndex]->SetVisibility(ESlateVisibility::Hidden);
		}	
	}
	
	MenuStack.Emplace(InNewMenu);
	InNewMenu->MenuSetup();
}

void ALBlasterHUD::AddChatMessage(const FChatParams& ChatParams)
{
	if (ChatUI && ChatUI->ChatBox)
    {
    	ChatUI->ChatBox->AddChatMessage(ChatParams);
    }
}

void ALBlasterHUD::UpdateScoreboard()
{
	if (Scoreboard && Scoreboard->IsVisible())
	{
		Scoreboard->UpdateBoard();
		
		if (ResultMenu)
		{
			ResultMenu->MenuSetup();
		}
	}
}

void ALBlasterHUD::AddAnnouncement()
{
	if (IsValidOwnerController())
	{
		if (AnnouncementClass)
		{
			Announcement = CreateWidget<UAnnouncement>(OwnerController, AnnouncementClass);
			Announcement->AddToViewport();
		}
	}
}

void ALBlasterHUD::HideAnnouncement()
{
	if (Announcement)
	{
		Announcement->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ALBlasterHUD::AddElimAnnouncement(const FString& AttackerName, const FString& VictimName)
{
	if (IsValidOwnerController() && ElimAnnouncementClass)
	{
		if (UElimAnnouncement* ElimAnnouncement = CreateWidget<UElimAnnouncement>(OwnerController, ElimAnnouncementClass))
		{
			ElimAnnouncement->SetElimAnnouncementText(AttackerName, VictimName);
			ElimAnnouncement->AddToViewport();

			// 한칸 씩 밑으로 내림
			for (const UElimAnnouncement* Message : ElimMessages)
			{
				if (Message && Message->GetAnnouncementBox())
				{
					if (UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Message->GetAnnouncementBox()))
					{
						const FVector2D Position = CanvasSlot->GetPosition();
						const FVector2D NewPosition(Position.X, Position.Y + CanvasSlot->GetSize().Y);
						CanvasSlot->SetPosition(NewPosition);
					}
				}
			}
			ElimMessages.Add(ElimAnnouncement);

			FTimerHandle Timer;
			FTimerDelegate Delegate;
			// 파라미터가 있는 함수를 바인딩
			Delegate.BindUFunction(this, FName(TEXT("ElimAnnouncementTimerFinished")), ElimAnnouncement);
			GetWorld()->GetTimerManager().SetTimer(Timer, Delegate, ElimAnnouncementTime, false);
		}
	}
}

void ALBlasterHUD::AddChatUI()
{
	if (IsValidOwnerController() && ChatUIClass)
	{
		ChatUI = CreateWidget<UChatUI>(OwnerController, ChatUIClass);
		if (ChatUI && ChatUI->ChatBox)
		{
			ChatUI->AddToViewport();

			if (GetMatchModeType() == EMatchMode::TeamDeathMatch)
			{
				ChatUI->ChatBox->InitializeChatBox(EChatMode::ECM_FriendlyTeam, false);
			}
			else if (GetMatchModeType() == EMatchMode::FreeForAll)
			{
				ChatUI->ChatBox->InitializeChatBox(EChatMode::ECM_FreeForAll, false);
			}
		}
	}
}

void ALBlasterHUD::AddScoreboard()
{
	if (ScoreboardClassByMatchModeMap.Contains(GetMatchModeType()) && ScoreboardClassByMatchModeMap[GetMatchModeType()])
	{
		if (IsValidOwnerController())
		{
			Scoreboard = CreateWidget<UScoreboard>(OwnerController, ScoreboardClassByMatchModeMap[GetMatchModeType()]);
			if (Scoreboard)
			{
				Scoreboard->MenuSetup();
				Scoreboard->SetVisibility(ESlateVisibility::Collapsed);
			}	
		}
	}
}

void ALBlasterHUD::AddMiniScoreboard()
{
	if (MiniScoreboardClassByMatchModeMap.Contains(GetMatchModeType()) && MiniScoreboardClassByMatchModeMap[GetMatchModeType()])
	{
		if (IsValidOwnerController())
		{
			MiniScoreboard = CreateWidget<UMiniScoreboard>(OwnerController, MiniScoreboardClassByMatchModeMap[GetMatchModeType()]);
			if (MiniScoreboard)
			{
				MiniScoreboard->MenuSetup();
			}	
		}
	}
}

void ALBlasterHUD::UpdateHUDRespawnTimer()
{
	if (RespawnTimer)
	{
		RespawnTimer->AddProgressPercent(RespawnTimerUnitPercent);
	}
}

void ALBlasterHUD::AddHelpInfo()
{
	if (!HelpInfo && HelpInfoClass && IsValidOwnerController())
	{
		HelpInfo = CreateWidget<UUserWidget>(OwnerController, HelpInfoClass);
	}
	if (HelpInfo)
	{
		HelpInfo->AddToViewport();
		SetHelpInfoVisibility(true);
	}
}

void ALBlasterHUD::SetScoreboardVisibility(bool bVisible)
{
	if (Scoreboard)
	{
		if (bVisible)
		{
			Scoreboard->SetVisibility(ESlateVisibility::Visible);
			UpdateScoreboard();		
		}
		else
		{
			Scoreboard->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void ALBlasterHUD::SetGoalKillScoreMiniScoreboard(const int32 InGoalKillScore) const
{
	if (MiniScoreboard)
	{
		MiniScoreboard->SetGoalKillScoreText(InGoalKillScore);
	}
}

void ALBlasterHUD::UpdateTeamScoreMiniScoreboard(const ETeam InTeam, const int32 InTeamScore) const
{
	if (MiniScoreboard)
	{
		if (UMiniScoreboardTeamDeathMatch* MiniScoreboardTDM = Cast<UMiniScoreboardTeamDeathMatch>(MiniScoreboard))
		{
			if (InTeam == ETeam::ET_RedTeam)
			{
				MiniScoreboardTDM->SetRedTeamScoreText(InTeamScore);
			}
			else if (InTeam == ETeam::ET_BlueTeam)
			{
				MiniScoreboardTDM->SetBlueTeamScoreText(InTeamScore);
			}
		}
	}
}

void ALBlasterHUD::UpdateTotalScoreMiniScoreboard(const int32 InTotalScore) const
{
	if (MiniScoreboard)
	{
		if (UMiniScoreboardFreeForAll* MiniScoreboardFFA = Cast<UMiniScoreboardFreeForAll>(MiniScoreboard))
		{
			MiniScoreboardFFA->SetTotalScoreText(InTotalScore);
		}
	}
}

void ALBlasterHUD::StartRespawnTimer(float InElimDelay, float InRespawnTimerUpdateFrequency)
{
	if (!RespawnTimer && RespawnTimerClass && IsValidOwnerController())
	{
		RespawnTimer = CreateWidget<URespawnTimer>(OwnerController, RespawnTimerClass);
	}
	if (RespawnTimer)
	{
		RespawnTimer->MenuSetup();
	}
	
	RespawnTimerUnitPercent = InRespawnTimerUpdateFrequency / InElimDelay;
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ThisClass::UpdateHUDRespawnTimer, InRespawnTimerUpdateFrequency, true);
	}
}

void ALBlasterHUD::HideRespawnTimer()
{
	if (RespawnTimer)
	{
		RespawnTimer->SetVisibility(ESlateVisibility::Collapsed);
		GetWorldTimerManager().ClearTimer(RespawnTimerHandle);
	}
}

void ALBlasterHUD::AddResultMenu()
{
	if (Scoreboard)
	{
		Scoreboard->SetScoreboardForResultMenu();
	}
	if (MiniScoreboard)
	{
		MiniScoreboard->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if (IsValidOwnerController() && ResultMenuClassByMatchModeMap.Contains(GetMatchModeType()) && ResultMenuClassByMatchModeMap[GetMatchModeType()])
	{
		ResultMenu = CreateWidget<UResultMenu>(OwnerController, ResultMenuClassByMatchModeMap[GetMatchModeType()]);
	}
	if (ResultMenu)
	{
		ResultMenu->MenuSetup();
	}
	
	OnDesiredCrosshairVisibilityChanged.ExecuteIfBound(false);
}

void ALBlasterHUD::SetHUDMatchCooldown(float InTime)
{
	if (ResultMenu)
	{
		ResultMenu->SetCooldownTimerText(InTime);
	}
}

void ALBlasterHUD::InitSniperScope(const TSubclassOf<UUserWidget>& InSniperScopeClass)
{
	if (SniperScope || !InSniperScopeClass)
	{
		return;
	}

	if (IsValidOwnerController())
	{
		SniperScope = CreateWidget<USniperScope>(OwnerController, InSniperScopeClass);
		SniperScope->AddToViewport(-999);
		SniperScope->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ALBlasterHUD::ShowSniperScopeWidget(bool bShowScope)
{
	if (SniperScope)
	{
		if (bShowScope)
		{
			SniperScope->SetVisibility(ESlateVisibility::Visible);
			SniperScope->ShowSniperScopeWidget();
			if (Crosshair)
			{
				Crosshair->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		else
		{
			SniperScope->SetVisibility(ESlateVisibility::Hidden);
			if (Crosshair)
			{
				Crosshair->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void ALBlasterHUD::HighPingWarning(float InDuration) const
{
	if (CharacterOverlay)
	{
		CharacterOverlay->HighPingWarning(InDuration);
	}
}

bool ALBlasterHUD::ShowPauseMenu()
{
	if (PauseMenuClass && !PauseMenu)
	{
		if (IsValidOwnerController())
		{
			PauseMenu = CreateWidget<UPauseMenu>(OwnerController, PauseMenuClass);
		}
	}

	if (PauseMenu)
	{
		if (!bShowedPauseMenu)
		{
			PauseMenu->MenuSetup();
		}
		else
		{
			PauseMenu->MenuTearDown();
		}
		bShowedPauseMenu = !bShowedPauseMenu;
	}
	return bShowedPauseMenu;
}

void ALBlasterHUD::FocusChat() const
{
	if (ChatUI && ChatUI->ChatBox)
	{
		ChatUI->ChatBox->FocusChatEdit();
	}
}

void ALBlasterHUD::ScrollChatBox(float InScrollValue) const
{
	if (ChatUI && ChatUI->ChatBox)
	{
		ChatUI->ChatBox->Scroll(InScrollValue);
	}
}

void ALBlasterHUD::SetWeaponSlotIcon(EEquipSlot InEquipSlot, EWeaponType InWeaponType) const
{
	if (CharacterOverlay)
	{
		CharacterOverlay->SetWeaponSlotIcon(InEquipSlot, InWeaponType);
	}
}

void ALBlasterHUD::ChooseWeaponSlot(EEquipSlot InEquipSlot) const
{
	if (CharacterOverlay)
	{
		CharacterOverlay->ChooseWeaponSlot(InEquipSlot);
	}
}

void ALBlasterHUD::ChangeChatMode() const
{
	if (ChatUI && ChatUI->ChatBox)
	{
		ChatUI->ChatBox->ChangeChatMode();
	}
}

void ALBlasterHUD::SetHelpInfoVisibility(bool bVisible)
{
	if (HelpInfo)
	{
		HelpInfo->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		OnDesiredCrosshairVisibilityChanged.ExecuteIfBound(!bVisible);
	}
}

void ALBlasterHUD::CreateSettingMenu()
{
	if (SettingMenuClass && !SettingMenu)
	{
		if (IsValidOwnerController())
		{
			SettingMenu = CreateWidget<USettingsHubMenu>(OwnerController, SettingMenuClass);
		}
	}

	AddNewMenuToStack(SettingMenu);
}

void ALBlasterHUD::CreateVideoSettingMenu()
{
	if (VideoSettingMenuClass && !VideoSettingMenu)
	{
		if (IsValidOwnerController())
		{
			VideoSettingMenu = CreateWidget<UVideoSettingMenu>(OwnerController, VideoSettingMenuClass);
		}
	}

	AddNewMenuToStack(VideoSettingMenu);
}

void ALBlasterHUD::CreateMouseSettingMenu()
{
	if (MouseSettingMenuClass && !MouseSettingMenu)
	{
		if (IsValidOwnerController())
		{
			MouseSettingMenu = CreateWidget<UMouseSettingMenu>(OwnerController, MouseSettingMenuClass);
		}
	}

	AddNewMenuToStack(MouseSettingMenu);
}

void ALBlasterHUD::CreateAudioSettingMenu()
{
	if (AudioSettingMenuClass && !AudioSettingMenu)
	{
		if (IsValidOwnerController())
		{
			AudioSettingMenu = CreateWidget<UAudioSettingMenu>(OwnerController, AudioSettingMenuClass);
		}
	}

	AddNewMenuToStack(AudioSettingMenu);
}

void ALBlasterHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AddCharacterOverlay();
	HideAnnouncement();
	AddCrosshair();

	// Add UI by MatchModeType for Debugging
#if !WITH_EDITOR
	AddChatUI();
	AddScoreboard();
	AddMiniScoreboard();
	AddHelpInfo();
	AddMatchTimeTimer();
#endif
}

void ALBlasterHUD::BeginPlay()
{
	Super::BeginPlay();

	// Add UI by MatchModeType for Debugging
#if WITH_EDITOR
	AddChatUI();
	AddScoreboard();
	AddMiniScoreboard();
	AddHelpInfo();
	AddMatchTimeTimer();
#endif
}

bool ALBlasterHUD::IsValidOwnerController()
{
	if (!OwnerController && GetOwningPlayerController())
	{
		OwnerController = Cast<ALBlasterPlayerController>(GetOwningPlayerController());
	}
	return OwnerController != nullptr;
}
