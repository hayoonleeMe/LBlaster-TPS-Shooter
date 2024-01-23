// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "LBlasterCharacter.generated.h"

UCLASS()
class LBLASTER_API ALBlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALBlasterCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

protected:
	/*
	 *	Input Section
	 */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	void Move(const FInputActionValue& ActionValue);
	void Look(const FInputActionValue& ActionValue);
	
private:
	/*
	 *	Camera and SpringArm Section
	 */
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<class UCameraComponent> FollowCamera;

	/*
	 *	Overhead Widget
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widget", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> OverheadWidgetComponent;
};
