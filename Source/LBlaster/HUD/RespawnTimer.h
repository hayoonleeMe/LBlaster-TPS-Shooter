// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "RespawnTimer.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API URespawnTimer : public ULBlasterUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void MenuSetup() override;
	void AddProgressPercent(float InUnitPercent);

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UProgressBar> ProgressBar;
};
