// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnTimer.h"

#include "Components/ProgressBar.h"

void URespawnTimer::MenuSetup()
{
	Super::MenuSetup();

	if (ProgressBar)
	{
		ProgressBar->SetPercent(0.f);
	}
	SetVisibility(ESlateVisibility::Visible);
}

void URespawnTimer::AddProgressPercent(float InUnitPercent)
{
	if (ProgressBar)
	{
		ProgressBar->SetPercent(ProgressBar->GetPercent() + InUnitPercent);
	}
}
