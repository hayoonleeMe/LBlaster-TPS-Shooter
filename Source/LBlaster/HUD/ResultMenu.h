// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "ResultMenu.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UResultMenu : public ULBlasterUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> ResultText;
};
