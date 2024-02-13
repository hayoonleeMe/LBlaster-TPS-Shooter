// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LBlasterHUD.h"

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
			DrawCrosshair(HUDPackage.TopCrosshair, ViewportCenter, Spread);
		}
		if (HUDPackage.BottomCrosshair)
		{
			const FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshair(HUDPackage.BottomCrosshair, ViewportCenter, Spread);
		}
		if (HUDPackage.LeftCrosshair)
        {
			const FVector2D Spread(-SpreadScaled, 0.f);
        	DrawCrosshair(HUDPackage.LeftCrosshair, ViewportCenter, Spread);
        }
		if (HUDPackage.RightCrosshair)
		{
			const FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.RightCrosshair, ViewportCenter, Spread);
		}
		if (HUDPackage.CenterCrosshair)
		{
			const FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CenterCrosshair, ViewportCenter, Spread);
		}
	}
}

void ALBlasterHUD::DrawCrosshair(UTexture2D* InTexture, const FVector2D& InViewportCenter, const FVector2D& InSpread)
{
	const float TextureWidth = InTexture->GetSizeX();
	const float TextureHeight = InTexture->GetSizeY();
	const FVector2D TextureDrawPoint(InViewportCenter.X - (TextureWidth / 2.f) + InSpread.X, InViewportCenter.Y - (TextureHeight / 2.f) + InSpread.Y);
	DrawTexture(InTexture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f, 1.f, FLinearColor::White);	
}
