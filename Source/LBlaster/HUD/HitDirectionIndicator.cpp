// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HitDirectionIndicator.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Border.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

void UHitDirectionIndicator::NativeConstruct()
{
	Super::NativeConstruct();

	DynamicMaterial = HitCircleBorder->GetDynamicMaterial();
}

void UHitDirectionIndicator::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValidOwnerController())
	{
		return;
	}

	const ACharacter* OwnerCharacter = OwnerController->GetCharacter();
	if (OwnerCharacter && DynamicMaterial)
	{
		const FVector CharacterForwardVector = OwnerCharacter->GetActorForwardVector();

		const float Y = CharacterForwardVector.X * HitNormal.Y - CharacterForwardVector.Y * HitNormal.X;
		const float X = CharacterForwardVector.Dot(HitNormal);

		const float Angle = FMath::GetMappedRangeValueClamped(FVector2D(180.f, -180.f), FVector2D(0.f, 1.f), UKismetMathLibrary::DegAtan2(Y, X));
		DynamicMaterial->SetScalarParameterValue(FName(TEXT("Angle")), Angle);
	}
}

void UHitDirectionIndicator::MenuSetup()
{
	Super::MenuSetup();

	// StartAnim => EndAnim => Remove Self
	if (StartAnim && EndAnim)
	{
		PlayAnimationForward(StartAnim);
		GetWorld()->GetTimerManager().SetTimer(AnimTimer, FTimerDelegate::CreateLambda([&]()
		{
			PlayAnimationForward(EndAnim);
			GetWorld()->GetTimerManager().SetTimer(AnimTimer, FTimerDelegate::CreateLambda([&]()
			{
				MenuTearDown();
			}), EndAnim->GetEndTime(), false);

		}), StartAnim->GetEndTime(), false);
	}
}
