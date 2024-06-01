// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ResultMenu.h"
#include "ResultMenuTeamDeathMatch.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UResultMenuTeamDeathMatch : public UResultMenu
{
	GENERATED_BODY()

public:
	virtual void MenuSetup() override;
};
