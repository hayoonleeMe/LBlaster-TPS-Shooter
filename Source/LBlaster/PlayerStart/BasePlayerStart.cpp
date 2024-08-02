// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerStart.h"


ABasePlayerStart::ABasePlayerStart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	RespawnDelay = 3.f;
}

void ABasePlayerStart::StartRespawnDelayTimer()
{
	bCanRespawn = false;
	GetWorldTimerManager().SetTimer(RespawnDelayTimer, FTimerDelegate::CreateLambda([&]()
	{
		bCanRespawn = true;
	}), RespawnDelay, false);
}
