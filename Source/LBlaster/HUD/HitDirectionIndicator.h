// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/LBlasterUserWidget.h"
#include "HitDirectionIndicator.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UHitDirectionIndicator : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void MenuSetup() override;
	FORCEINLINE void SetHitNormal(const FVector& InHitNormal) { HitNormal = -InHitNormal; }
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBorder> HitCircleBorder;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> StartAnim;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> EndAnim;

	FTimerHandle AnimTimer;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

	FVector HitNormal;
};
