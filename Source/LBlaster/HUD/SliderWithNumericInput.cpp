// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SliderWithNumericInput.h"

#include "Components/EditableText.h"
#include "Components/Slider.h"

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

void USliderWithNumericInput::InitializeValues(float InInitialValue, float InSliderMinValue, float InSliderMaxValue, float InSliderStepSize)
{
	SetInitialValue(InInitialValue);
	
	if (Slider)
	{
		Slider->SetMinValue(InSliderMinValue);
		Slider->SetMaxValue(InSliderMaxValue);
		Slider->SetStepSize(InSliderStepSize);	
	}
}

void USliderWithNumericInput::SetInitialValue(float InValue)
{
	LastInputValue = InValue;
	
	if (Slider && InputValue)
	{
		Slider->SetValue(LastInputValue);
		InputValue->SetText(FText::AsNumber(FMath::RoundToInt32(LastInputValue)));
	}
}

void USliderWithNumericInput::OnValueChanged(float Value)
{
	LastInputValue = Value;
	
	if (InputValue)
	{
		InputValue->SetText(FText::AsNumber(FMath::RoundToInt32(LastInputValue)));
	}
	if (OnSliderValueChanged.IsBound())
	{
		OnSliderValueChanged.Execute(LastInputValue);
	}
}

void USliderWithNumericInput::OnInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (!InputValue || !Slider)
	{
		return;
	}

	if (Text.IsNumeric())
	{
		LastInputValue = FMath::Clamp(FCString::Atof(*Text.ToString()), Slider->GetMinValue(), Slider->GetMaxValue());
		InputValue->SetText(FText::AsNumber(FMath::RoundToInt32(LastInputValue)));
		Slider->SetValue(LastInputValue);
		if (OnSliderValueChanged.IsBound())
		{
			OnSliderValueChanged.Execute(LastInputValue);
		}
	}
	else
	{
		InputValue->SetText(FText::AsNumber(FMath::RoundToInt32(LastInputValue)));
	}
}
