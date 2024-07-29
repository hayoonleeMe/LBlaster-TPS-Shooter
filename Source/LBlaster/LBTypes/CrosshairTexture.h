#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "CrosshairTexture.generated.h"

USTRUCT(BlueprintType)
struct FCrosshairTexture
{
	GENERATED_BODY()

	FCrosshairTexture()
		: bIsCrosshairVisible(false)
	{}

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> CenterCrosshair;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> TopCrosshair;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> BottomCrosshair;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> LeftCrosshair;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> RightCrosshair;

	UPROPERTY(EditAnywhere)
	bool bIsCrosshairVisible;
};
