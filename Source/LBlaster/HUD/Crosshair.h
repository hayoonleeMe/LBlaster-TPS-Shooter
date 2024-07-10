// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterHUD.h"
#include "LBlasterUserWidget.h"
#include "LBTypes/CrosshairTexture.h"
#include "Crosshair.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UCrosshair : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:
	void DrawCrosshair(const FCrosshairTexture& CrosshairTexture);
	void UpdateCrosshair(float InSpreadScaled, const FLinearColor& InCrosshairColor);
	void SetPlayerNameText(const FString& InPlayerName) const;

	UFUNCTION()
	void OnDesiredCrosshairVisibilityChanged(bool bDesiredCrosshairVisibility);

private:
	/*
	 *	Crosshair
	 */
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	TObjectPtr<class UImage> TopCrosshair;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	TObjectPtr<UImage> BottomCrosshair;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	TObjectPtr<UImage> LeftCrosshair;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	TObjectPtr<UImage> RightCrosshair;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	TObjectPtr<UImage> CenterCrosshair;

	void DrawCrosshair(UImage* ImageToDraw, UTexture2D* InTexture);
	void UpdateCrosshair(UImage* ImageToDraw, const FVector2D& InSpread, const FLinearColor& InCrosshairColor) const;

	/*
	 *	Player Name
	 */
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	TObjectPtr<class UTextBlock> PlayerNameText;

	void UpdatePlayerNameText(const FLinearColor& InCrosshairColor) const;
};
