// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LBlasterHUD.h"

#include "CharacterOverlay.h"
#include "Blueprint/UserWidget.h"

ALBlasterHUD::ALBlasterHUD()
{
	/* Crosshair */
	CrosshairSpreadMax = 16.f;
}

void ALBlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	if (GEngine)
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

void ALBlasterHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
	
	SetHUDScore(0.f);
	SetHUDDeath(0);
	SetHUDAmmo(0);
	SetHUDCarriedAmmo(0);
}

void ALBlasterHUD::AddCharacterOverlay()
{
	if (APlayerController* PlayerController = GetOwningPlayerController())
	{
		if (CharacterOverlayClass)
		{
			CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
			CharacterOverlay->AddToViewport();
		}
	}
}

void ALBlasterHUD::DrawCrosshair(UTexture2D* InTexture, const FVector2D& InViewportCenter, const FVector2D& InSpread, const FLinearColor& InLinearColor)
{
	const float TextureWidth = InTexture->GetSizeX();
	const float TextureHeight = InTexture->GetSizeY();
	const FVector2D TextureDrawPoint(InViewportCenter.X - (TextureWidth / 2.f) + InSpread.X, InViewportCenter.Y - (TextureHeight / 2.f) + InSpread.Y);
	DrawTexture(InTexture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f, 1.f, InLinearColor);	
}
