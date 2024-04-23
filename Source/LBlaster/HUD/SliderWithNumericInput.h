// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SliderWithNumericInput.generated.h"

DECLARE_DELEGATE_OneParam(FOnSliderValueChanged, float /* InSliderValue */);

/**
 * 
 */
UCLASS()
class LBLASTER_API USliderWithNumericInput : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetInitialValue(float InValue);

	FOnSliderValueChanged OnSliderValueChanged;
	
protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USlider> Slider;

	UFUNCTION()
	void OnValueChanged(float Value);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableText> InputValue;

	UFUNCTION()
	void OnInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	float LastInputValue;
};
