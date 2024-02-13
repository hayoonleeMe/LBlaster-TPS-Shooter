// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LBlasterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

	UPROPERTY()	
	TObjectPtr<UTexture2D> TopCrosshair;

	UPROPERTY()
	TObjectPtr<UTexture2D> BottomCrosshair;

	UPROPERTY()
	TObjectPtr<UTexture2D> LeftCrosshair;

	UPROPERTY()
	TObjectPtr<UTexture2D> RightCrosshair;

	UPROPERTY()
	TObjectPtr<UTexture2D> CenterCrosshair;
};

/**
 * 
 */
UCLASS()
class LBLASTER_API ALBlasterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	FORCEINLINE void SetHUDPackage(const FHUDPackage& InPackage) { HUDPackage = InPackage; }

private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* InTexture, const FVector2D& ViewportCenter);
};
