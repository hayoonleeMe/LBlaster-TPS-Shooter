// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GraphicSettingMenu.h"
#include "GraphicSettingMenuForMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UGraphicSettingMenuForMainMenu : public UGraphicSettingMenu
{
	GENERATED_BODY()

public:
	virtual void CloseOverlay() override;
	
private:
	virtual void OnReturnButtonClicked() override;
	virtual void OnNoApplyAlertAcceptButtonClicked() override;
};
