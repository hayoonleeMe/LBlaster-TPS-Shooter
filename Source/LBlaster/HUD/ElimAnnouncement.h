// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ElimAnnouncement.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UElimAnnouncement : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetElimAnnouncementText(const FString& InAttackerName, const FString& InVictimName);
	FORCEINLINE class UHorizontalBox* GetAnnouncementBox() const { return AnnouncementBox; }

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> AnnouncementBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> AnnouncementText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> TextFadeOut;
};
