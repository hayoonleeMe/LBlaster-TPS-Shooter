// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DropDownOption.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UDropDownOption : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetText(const FString& InString);

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> OptionText;
};
