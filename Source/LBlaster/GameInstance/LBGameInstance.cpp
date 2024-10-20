// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/LBGameInstance.h"

#include "GameUserSettings/LBGameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"

ULBGameInstance::ULBGameInstance()
{
	/* Sound Mix */
	static ConstructorHelpers::FObjectFinder<USoundMix> MasterSoundMixRef(TEXT("/Script/Engine.SoundMix'/Game/LBlaster/Core/Audio/SoundClasses/Mix_MasterSound.Mix_MasterSound'"));
	if (MasterSoundMixRef.Object)
	{
		MasterSoundMix = MasterSoundMixRef.Object;
	}
	
	/* Sound Class */
	static ConstructorHelpers::FObjectFinder<USoundClass> MasterSoundClassRef(TEXT("/Script/Engine.SoundClass'/Engine/EngineSounds/Master.Master'"));
	if (MasterSoundClassRef.Object)
	{
		MasterSoundClass = MasterSoundClassRef.Object;
	}
}

void ULBGameInstance::Init()
{
	Super::Init();

	if (!GEngine)
	{
		return;
	}

	if (ULBGameUserSettings* GameUserSettings = Cast<ULBGameUserSettings>(GEngine->GetGameUserSettings()))
	{
		if (GameUserSettings->IsFirstExecute())
		{
			GameUserSettings->SetGraphicOptionByAutoDetect();
		}
		else
		{
			GameUserSettings->LoadSettings();
			GameUserSettings->ApplyCustomSettings(false, GetWorld());
		}
	}
}

void ULBGameInstance::SetOverallSoundVolume(const UObject* WorldContextObject, float Volume) const
{
	if (WorldContextObject && MasterSoundMix && MasterSoundClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(WorldContextObject, MasterSoundMix, MasterSoundClass, Volume, 1.f, 0.f);
		UGameplayStatics::PushSoundMixModifier(WorldContextObject, MasterSoundMix);
	}
}
