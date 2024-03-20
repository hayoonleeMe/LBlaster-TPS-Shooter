// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LBlasterHUD.h"

#include "Announcement.h"
#include "CharacterOverlay.h"
#include "LBlaster.h"
#include "PauseMenu.h"
#include "SniperScope.h"
#include "Blueprint/UserWidget.h"
#include "Player/LBlasterPlayerController.h"

ALBlasterHUD::ALBlasterHUD()
{
	/* Crosshair */
	CrosshairSpreadMax = 16.f;
	bEnableCrosshair = true;
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
		SniperScope->AddToViewport();
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
			if (CharacterOverlay)
			{
				CharacterOverlay->SetVisibility(ESlateVisibility::Hidden);
			}
			SniperScope->SetVisibility(ESlateVisibility::Visible);
			SniperScope->ShowSniperScopeWidget();
		}
		else
		{
			bEnableCrosshair = true;
			if (CharacterOverlay)
			{
				CharacterOverlay->SetVisibility(ESlateVisibility::Visible);
			}
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

void ALBlasterHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AddCharacterOverlay();
	HideAnnouncement();
}

bool ALBlasterHUD::IsValidOwnerController()
{
	if (!OwnerController && GetOwningPlayerController())
	{
		OwnerController = Cast<ALBlasterPlayerController>(GetOwningPlayerController());
	}
	return OwnerController != nullptr;
}
