// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OptionSelector.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

void UOptionSelector::NativePreConstruct()
{
	Super::NativePreConstruct();

	InitializeOptions();
}

void UOptionSelector::NativeConstruct()
{
	Super::NativeConstruct();

	if (LeftButton && !LeftButton->OnClicked.IsBound())
	{
		LeftButton->OnClicked.AddDynamic(this, &ThisClass::SelectLeftOption);
	}
	if (RightButton && !RightButton->OnClicked.IsBound())
	{
		RightButton->OnClicked.AddDynamic(this, &ThisClass::SelectRightOption);
	}
}

void UOptionSelector::InitializeOptions()
{
	if (!Switcher || Options.IsEmpty())
	{
		return;
	}

	SetActiveIndex(0);
	
	for (const FString& Option : Options)
	{
		if (UTextBlock* TextBlock = WidgetTree->ConstructWidget<UTextBlock>())
		{
			TextBlock->SetText(FText::FromString(Option));
			TextBlock->SetMinDesiredWidth(400.f);
			TextBlock->SetJustification(ETextJustify::Center);
			Switcher->AddChild(TextBlock);
		}
	}
}

void UOptionSelector::SetActiveIndex(int32 InActiveIndex)
{
	if (Switcher)
	{
		Switcher->SetActiveWidgetIndex(InActiveIndex);
	}
}

int32 UOptionSelector::GetActiveIndex() const
{
	if (Switcher)
	{
		return Switcher->GetActiveWidgetIndex();
	}
	return 0;
}

void UOptionSelector::SelectRightOption()
{
	if (Switcher)
	{
		const int32 NewIndex = GetNewIndex(1);
		Switcher->SetActiveWidgetIndex(NewIndex);
		if (OnSwitcherActiveIndexChanged.IsBound())
		{
			OnSwitcherActiveIndexChanged.Execute(NewIndex);
		}
	}
}

void UOptionSelector::SelectLeftOption()
{
	if (Switcher)
	{
		const int32 NewIndex = GetNewIndex(-1);
		Switcher->SetActiveWidgetIndex(NewIndex);
		if (OnSwitcherActiveIndexChanged.IsBound())
		{
			OnSwitcherActiveIndexChanged.Execute(NewIndex);
		}
	}
}

int32 UOptionSelector::GetNewIndex(int32 InAddAmount) const
{
	if (Switcher)
	{
		const int32 OptionsNum = Options.Num();
		return (Switcher->GetActiveWidgetIndex() + InAddAmount + OptionsNum) % OptionsNum;
	}
	return 0;
}
