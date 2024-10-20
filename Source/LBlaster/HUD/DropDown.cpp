// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/DropDown.h"

#include "DropDownOption.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ComboBoxString.h"

void UDropDown::NativeConstruct()
{
	Super::NativeConstruct();

	if (ComboBox && !ComboBox->OnGenerateWidgetEvent.IsBound())
	{
		ComboBox->OnGenerateWidgetEvent.BindDynamic(this, &ThisClass::OnComboBoxGenerateWidgetEvent);
	}
}

void UDropDown::InitializeOptions(const TArray<FString>& Options)
{
	if (!ComboBox)
	{
		return;
	}

	ComboBox->ClearOptions();

	for (const FString& Option : Options)
	{
		ComboBox->AddOption(Option);
	}
	ComboBox->SetSelectedIndex(0);
}

int32 UDropDown::GetSelectedValue() const
{
	if (ComboBox)
	{
		return FCString::Atoi(*ComboBox->GetSelectedOption());
	}
	return 0;
}

UWidget* UDropDown::OnComboBoxGenerateWidgetEvent(FString Item)
{
	if (IsValidOwnerController() && DropDownOptionClass)
	{
		if (UDropDownOption* Option = CreateWidget<UDropDownOption>(OwnerController, DropDownOptionClass))
		{
			Option->SetText(Item);
			return Option;
		}
	}
	return nullptr;
}
