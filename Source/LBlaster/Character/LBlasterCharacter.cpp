// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LBlasterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/OverheadWidget.h"

ALBlasterCharacter::ALBlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	/* Movement */
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	/* Mesh */
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/LBlaster/Actors/Players/Manny/Meshes/SKM_Manny.SKM_Manny'"));
	if (SkeletalMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(SkeletalMeshRef.Object);
	}
	
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceRef(TEXT("/Script/Engine.AnimBlueprint'/Game/LBlaster/Actors/Players/Manny/ABP_LBlasterAnimInstance.ABP_LBlasterAnimInstance_C'"));
	if (AnimInstanceRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceRef.Class);
	}
	
	/* Camera and SpringArm */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;		// SpringArm은 캐릭터의 회전 입력값을 따른다.

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;	// 카메라는 항상 RelativeRotation을 유지해 캐릭터를 바라본다.

	/* Input */
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/LBlaster/Core/Inputs/IMC_LBContext.IMC_LBContext'"));
	if (InputMappingContextRef.Object)
	{
		DefaultMappingContext = InputMappingContextRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_Move.IA_Move'"));
	if (MoveActionRef.Object)
	{
		MoveAction = MoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_Jump.IA_Jump'"));
	if (JumpActionRef.Object)
	{
		JumpAction = JumpActionRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_Look.IA_Look'"));
	if (LookActionRef.Object)
	{
		LookAction = LookActionRef.Object;
	}

	/* Overhead Widget */
	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidgetComponent"));
	OverheadWidgetComponent->SetupAttachment(GetMesh());
	OverheadWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 190.f));
	OverheadWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	OverheadWidgetComponent->SetDrawAtDesiredSize(true);

	static ConstructorHelpers::FClassFinder<UOverheadWidget> OverheadWidgetClassRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/LBlaster/UI/HUD/WBP_OverheadWidget.WBP_OverheadWidget_C'"));
	if (OverheadWidgetClassRef.Class)
	{
		OverheadWidgetComponent->SetWidgetClass(OverheadWidgetClassRef.Class);
	}
}

void ALBlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	/* Input */
	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	/* Overhead Widget */
	if (UOverheadWidget* OverheadWidget = Cast<UOverheadWidget>(OverheadWidgetComponent->GetUserWidgetObject()))
	{
		OverheadWidget->ShowPlayerName(this);
	}
}

void ALBlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
}

void ALBlasterCharacter::Move(const FInputActionValue& ActionValue)
{
	const FVector2D MovementVector = ActionValue.Get<FVector2D>();

	const FRotator YawRotation(0.f, GetController()->GetControlRotation().Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void ALBlasterCharacter::Look(const FInputActionValue& ActionValue)
{
	const FVector2D LookAxisVector = ActionValue.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ALBlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

