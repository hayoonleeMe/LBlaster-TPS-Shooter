// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LBlasterHUD.h"

#include "Announcement.h"
#include "CharacterOverlay.h"
#include "ChatBox.h"
#include "ElimAnnouncement.h"
#include "GraphicSettingMenu.h"
#include "LBlaster.h"
#include "PauseMenu.h"
#include "SettingMenu.h"
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
	bEnableCrosshair = true;

	/* Elim Announcement */
	ElimAnnouncementTime = 3.5f;
}

void ALBlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	if (GEngine && bEnableCrosshair)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		const float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (HUDPackage.TopCrosshair)
		{
			const FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(HUDPackage.TopCrosshair, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.BottomCrosshair)
		{
			const FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshair(HUDPackage.BottomCrosshair, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.LeftCrosshair)
        {
			const FVector2D Spread(-SpreadScaled, 0.f);
        	DrawCrosshair(HUDPackage.LeftCrosshair, ViewportCenter, Spread, HUDPackage.CrosshairColor);
        }
		if (HUDPackage.RightCrosshair)
		{
			const FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.RightCrosshair, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CenterCrosshair)
		{
			const FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CenterCrosshair, ViewportCenter, Spread, HUDPackage.CrosshairColor);
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

void ALBlasterHUD::SetHUDScore(float InScore)
{
	if (CharacterOverlay)
	{
		CharacterOverlay->SetScoreAmount(InScore);
	}
}

void ALBlasterHUD::SetHUDDeath(int32 InDeath)
{
	if (CharacterOverlay)
	{
		CharacterOverlay->SetDeathAmount(InDeath);
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

void ALBlasterHUD::SetHUDMatchCountdown(float InCountdownTime)
{
	if (CharacterOverlay)
	{
		CharacterOverlay->SetMatchCountdownText(InCountdownTime);
	}
}

void ALBlasterHUD::SetHUDAnnouncementCountdown(float InCountdownTime)
{
	if (Announcement)
	{
		Announcement->SetHUDAnnouncementCountdown(InCountdownTime);
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

			SetHUDScore(0.f);
			SetHUDDeath(0);
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

void ALBlasterHUD::DrawCrosshair(UTexture2D* InTexture, const FVector2D& InViewportCenter, const FVector2D& InSpread, const FLinearColor& InLinearColor)
{
	const float TextureWidth = InTexture->GetSizeX();
	const float TextureHeight = InTexture->GetSizeY();
	const FVector2D TextureDrawPoint(InViewportCenter.X - (TextureWidth / 2.f) + InSpread.X, InViewportCenter.Y - (TextureHeight / 2.f) + InSpread.Y);
	DrawTexture(InTexture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f, 1.f, InLinearColor);	
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
		if (MenuStack.IsValidIndex(LastMenuIndex))
		{
			MenuStack[LastMenuIndex]->SetVisibility(ESlateVisibility::Hidden);
		}	
	}
	
	MenuStack.Emplace(InNewMenu);
	InNewMenu->MenuSetup();
}

void ALBlasterHUD::ReturnMenu()
{
	int32 LastMenuIndex = MenuStack.Num() - 1;
	if (MenuStack.IsValidIndex(LastMenuIndex))
	{
		MenuStack[LastMenuIndex]->MenuTearDown();
		MenuStack.RemoveAt(LastMenuIndex);
		--LastMenuIndex;
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
		MenuStack[LastMenuIndex]->SetVisibility(ESlateVisibility::Visible);
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
		if (ChatUI)
		{
			ChatUI->AddToViewport();
		}
	}
		
}

void ALBlasterHUD::SetCooldownAnnouncement()
{
	if (Announcement)
	{
		Announcement->SetVisibility(ESlateVisibility::Visible);
		Announcement->SetCooldownAnnouncement();
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
			bEnableCrosshair = false;
			SniperScope->SetVisibility(ESlateVisibility::Visible);
			SniperScope->ShowSniperScopeWidget();
		}
		else
		{
			bEnableCrosshair = true;
			SniperScope->SetVisibility(ESlateVisibility::Hidden);
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
	if (ChatUI)
	{
		ChatUI->ChatBox->FocusChatEdit();
	}
}

void ALBlasterHUD::AddChatMessage(const FText& InText) const
{
	if (ChatUI)
	{
		ChatUI->ChatBox->AddChatMessage(InText);
	}
}

void ALBlasterHUD::ScrollChatBox(float InScrollValue) const
{
	if (ChatUI)
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

void ALBlasterHUD::CreateSettingMenu()
{
	if (SettingMenuClass && !SettingMenu)
	{
		if (IsValidOwnerController())
		{
			SettingMenu = CreateWidget<USettingMenu>(OwnerController, SettingMenuClass);
		}
	}

	AddNewMenuToStack(SettingMenu);
}

void ALBlasterHUD::CreateGraphicSettingMenu()
{
	if (GraphicSettingMenuClass && !GraphicSettingMenu)
	{
		if (IsValidOwnerController())
		{
			GraphicSettingMenu = CreateWidget<UGraphicSettingMenu>(OwnerController, GraphicSettingMenuClass);
		}
	}

	AddNewMenuToStack(GraphicSettingMenu);
}

void ALBlasterHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AddCharacterOverlay();
	HideAnnouncement();
	AddChatUI();
}

bool ALBlasterHUD::IsValidOwnerController()
{
	if (!OwnerController && GetOwningPlayerController())
	{
		OwnerController = Cast<ALBlasterPlayerController>(GetOwningPlayerController());
	}
	return OwnerController != nullptr;
}
