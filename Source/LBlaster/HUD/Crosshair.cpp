// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Crosshair.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCrosshair::DrawCrosshair(const FCrosshairTexture& CrosshairTexture)
{
	if (CrosshairTexture.bIsCrosshairVisible)
	{
		SetVisibility(ESlateVisibility::Visible);
		DrawCrosshair(TopCrosshair, CrosshairTexture.TopCrosshair);
		DrawCrosshair(BottomCrosshair, CrosshairTexture.BottomCrosshair);
		DrawCrosshair(LeftCrosshair, CrosshairTexture.LeftCrosshair);
		DrawCrosshair(RightCrosshair, CrosshairTexture.RightCrosshair);
		DrawCrosshair(CenterCrosshair, CrosshairTexture.CenterCrosshair);	
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCrosshair::DrawCrosshair(UImage* ImageToDraw, UTexture2D* InTexture)
{
	if (!ImageToDraw)
	{
		return;
	}
	
	ImageToDraw->SetBrushFromTexture(InTexture, true);
}

void UCrosshair::UpdateCrosshair(float InSpreadScaled, const FLinearColor& InCrosshairColor)
{
	if (IsVisible())
	{
		UpdateCrosshair(TopCrosshair, { 0.f, -InSpreadScaled }, InCrosshairColor);
		UpdateCrosshair(BottomCrosshair, { 0.f, InSpreadScaled }, InCrosshairColor);
		UpdateCrosshair(LeftCrosshair, { -InSpreadScaled, 0.f }, InCrosshairColor);
		UpdateCrosshair(RightCrosshair, { InSpreadScaled, 0.f }, InCrosshairColor);
		UpdateCrosshair(CenterCrosshair, { 0.f, 0.f }, InCrosshairColor);
		UpdatePlayerNameText(InCrosshairColor);
	}
}

void UCrosshair::SetPlayerNameText(const FString& InPlayerName) const
{
	if (PlayerNameText)
	{
		PlayerNameText->SetText(FText::FromString(InPlayerName));
	}
}

void UCrosshair::MarkPlayerHit(bool bElimination)
{
	if (HitMarkerAnim && EliminationMarkerAnim)
	{
		PlayAnimation(bElimination ? EliminationMarkerAnim : HitMarkerAnim);
	}
}

void UCrosshair::OnDesiredCrosshairVisibilityChanged(bool bDesiredCrosshairVisibility)
{
	SetVisibility(bDesiredCrosshairVisibility ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UCrosshair::UpdateCrosshair(UImage* ImageToDraw, const FVector2D& InSpread, const FLinearColor& InCrosshairColor) const
{
	if (!ImageToDraw || ImageToDraw->GetVisibility() != ESlateVisibility::Visible)
	{
		return;
	}

	ImageToDraw->SetBrushTintColor(InCrosshairColor);
	
	if (UCanvasPanelSlot* Canvas = Cast<UCanvasPanelSlot>(ImageToDraw->Slot))
	{
		Canvas->SetPosition(InSpread);
	}
}

void UCrosshair::UpdatePlayerNameText(const FLinearColor& InCrosshairColor) const
{
	if (BottomCrosshair && PlayerNameText)
	{
		PlayerNameText->SetColorAndOpacity(InCrosshairColor);
		
		if (UCanvasPanelSlot* BottomCrosshairCanvas = Cast<UCanvasPanelSlot>(BottomCrosshair->Slot))
		{
			const FVector2D BottomCrosshairPos = BottomCrosshairCanvas->GetPosition();
			if (UCanvasPanelSlot* PlayerNameTextCanvas = Cast<UCanvasPanelSlot>(PlayerNameText->Slot))
			{
				PlayerNameTextCanvas->SetPosition({ BottomCrosshairPos.X, BottomCrosshairPos.Y + 60.f });
			}
		}
	}
}
