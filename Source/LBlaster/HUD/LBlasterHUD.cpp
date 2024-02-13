// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LBlasterHUD.h"

void ALBlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	if (GEngine)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		if (HUDPackage.TopCrosshair)
		{
			DrawCrosshair(HUDPackage.TopCrosshair, ViewportCenter);
		}
		if (HUDPackage.BottomCrosshair)
		{
			DrawCrosshair(HUDPackage.BottomCrosshair, ViewportCenter);
		}
		if (HUDPackage.LeftCrosshair)
        {
        	DrawCrosshair(HUDPackage.LeftCrosshair, ViewportCenter);
        }
		if (HUDPackage.RightCrosshair)
		{
			DrawCrosshair(HUDPackage.RightCrosshair, ViewportCenter);
		}
		if (HUDPackage.CenterCrosshair)
		{
			DrawCrosshair(HUDPackage.CenterCrosshair, ViewportCenter);
		}
	}
}

void ALBlasterHUD::DrawCrosshair(UTexture2D* InTexture, const FVector2D& ViewportCenter)
{
	const float TextureWidth = InTexture->GetSizeX();
	const float TextureHeight = InTexture->GetSizeY();
	const FVector2D TextureDrawPoint(ViewportCenter.X - (TextureWidth / 2.f), ViewportCenter.Y - (TextureHeight / 2.f));
	DrawTexture(InTexture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f, 1.f, FLinearColor::White);	
}
