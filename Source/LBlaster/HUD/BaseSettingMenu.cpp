// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSettingMenu.h"

#include "BaseHUD.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "GameUserSettings/LBGameUserSettings.h"

void UBaseSettingMenu::MenuSetup()
{
	Super::MenuSetup();

	if (!GameUserSettings && GEngine)
	{
		GameUserSettings = Cast<ULBGameUserSettings>(GEngine->GetGameUserSettings());
	}

	/* Apply Button */
	if (ApplyButton && !ApplyButton->OnClicked.IsBound())
	{
		ApplyButton->OnClicked.AddDynamic(this, &ThisClass::OnApplyButtonClicked);
	}
	ApplyButton->SetIsEnabled(false);

	/* Return Button */
	if (ReturnButton && !ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnButtonClicked);
	}

	/* NoApplyAlertOverlay */
	if (NoApplyAlertOverlay)
	{
		NoApplyAlertOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
	
	if (NoApplyAlertAcceptButton && !NoApplyAlertAcceptButton->OnClicked.IsBound())
	{
		NoApplyAlertAcceptButton->OnClicked.AddDynamic(this, &ThisClass::OnNoApplyAlertAcceptButtonClicked);
	}

	if (NoApplyAlertCancelButton && !NoApplyAlertCancelButton->OnClicked.IsBound())
	{
		NoApplyAlertCancelButton->OnClicked.AddDynamic(this, &ThisClass::OnNoApplyAlertCancelButtonClicked);
	}
}

void UBaseSettingMenu::OnApplyButtonClicked()
{
}

void UBaseSettingMenu::EnableApplyButton()
{
	if (GameUserSettings && ApplyButton)
	{
		ApplyButton->SetIsEnabled(ShouldApplyChange());
	}
}

bool UBaseSettingMenu::ShouldApplyChange() const
{
	return false;
}

void UBaseSettingMenu::OnReturnButtonClicked()
{
	if (ShouldApplyChange())
	{
		// 변경사항 있다고 alert
		if (NoApplyAlertOverlay)
		{
			NoApplyAlertOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		if (IsValidOwnerHUD())
		{
			OwnerHUD->ReturnMenu();
		}	
	}
}

void UBaseSettingMenu::OnNoApplyAlertAcceptButtonClicked()
{
}

void UBaseSettingMenu::OnNoApplyAlertCancelButtonClicked()
{
	if (NoApplyAlertOverlay)
	{
		NoApplyAlertOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}
