// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Announcement.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UAnnouncement : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetWarmupCountdownText(float InCountdownTime);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> WarmupTimeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AnnouncementText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InfoText;
};
