// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LBGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ULBGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

private:
	inline const static FString SaveGameFileName{ TEXT("MyGame") };
};
