// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/LBGameInstance.h"

#include "GameUserSettings/LBGameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/LBSaveGame.h"

void ULBGameInstance::Init()
{
	Super::Init();

	if (!GEngine)
	{
		return;
	}
	
	if (UGameplayStatics::DoesSaveGameExist(SaveGameFileName, 0))
	{
		if (ULBGameUserSettings* GameUserSettings = Cast<ULBGameUserSettings>(GEngine->GetGameUserSettings()))
		{
			GameUserSettings->LoadSettings();
			GameUserSettings->ApplyCustomSettings(false, GetWorld());
		}
	}
	else
	{
		// 첫 실행
		if (ULBGameUserSettings* GameUserSettings = Cast<ULBGameUserSettings>(GEngine->GetGameUserSettings()))
		{
			GameUserSettings->SetGraphicOptionByAutoDetect(true);
		}
		
		if (USaveGame* SaveGame = UGameplayStatics::CreateSaveGameObject(ULBSaveGame::StaticClass()))
		{
			UGameplayStatics::SaveGameToSlot(SaveGame, SaveGameFileName, 0);
		}
	}
}
