// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SniperScope.h"

void USniperScope::ShowSniperScopeWidget()
{
	if (ScopeZoomIn)
	{
		PlayAnimation(ScopeZoomIn);
	}
}
