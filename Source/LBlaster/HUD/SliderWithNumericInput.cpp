// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SliderWithNumericInput.h"

#include "Components/EditableText.h"
#include "Components/Slider.h"

void USliderWithNumericInput::SetInitialValue(float InValue)
{
	if (Slider && InputValue)
	{
		LastInputValue = InValue;
		Slider->SetValue(LastInputValue);
		InputValue->SetText(FText::AsNumber(FMath::RoundToInt32(LastInputValue)));
	}
}

void USliderWithNumericInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (Slider && !Slider->OnValueChanged.IsBound())
	{
		Slider->OnValueChanged.AddDynamic(this, &ThisClass::OnValueChanged);
	}
	if (InputValue && !InputValue->OnTextCommitted.IsBound())
	{
		InputValue->OnTextCommitted.AddDynamic(this, &ThisClass::OnInputTextCommitted);
	}
}

void USliderWithNumericInput::OnValueChanged(float Value)
{
	LastInputValue = Value;
	if (InputValue)
	{
		InputValue->SetText(FText::AsNumber(FMath::RoundToInt32(LastInputValue)));
	}
	OnSliderValueChanged.Execute(LastInputValue);
}

void USliderWithNumericInput::OnInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (!InputValue || !Slider)
	{
		return;
	}

	if (Text.IsNumeric())
	{
		LastInputValue = FMath::Clamp(FCString::Atof(*Text.ToString()), 0.f, 100.f);
		InputValue->SetText(FText::AsNumber(FMath::RoundToInt32(LastInputValue)));
		Slider->SetValue(LastInputValue);
		OnSliderValueChanged.Execute(LastInputValue);
	}
	else
	{
		InputValue->SetText(FText::AsNumber(FMath::RoundToInt32(LastInputValue)));
	}
}
