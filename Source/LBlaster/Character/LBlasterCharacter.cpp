// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LBlasterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "LBlaster.h"
#include "Component/CombatComponent.h"
#include "Component/HealthComponent.h"
#include "Component/LBCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/LBlasterGameMode.h"
#include "HUD/OverheadWidget.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"

ALBlasterCharacter::ALBlasterCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<ULBCharacterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	/* Replication */
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	/* Actor */
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	/* Movement */
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;
	BaseMaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxWalkSpeed = BaseMaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;
	GetCharacterMovement()->MaxAcceleration = 2400.f;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;
	GetCharacterMovement()->SetCrouchedHalfHeight(65.f);
	GetCharacterMovement()->BrakingFrictionFactor = 1.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1400.f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->RotationRate.Yaw = 720.f;
	
	/* Capsule Component */
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Destructible, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Ignore);

	/* Mesh */
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/LBlaster/Actors/Manny/Meshes/SKM_Manny.SKM_Manny'"));
	if (SkeletalMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(SkeletalMeshRef.Object);
	}
	
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceRef(TEXT("/Script/Engine.AnimBlueprint'/Game/LBlaster/Actors/Manny/Animations/ABP_LBlasterAnimInstance.ABP_LBlasterAnimInstance_C'"));
	if (AnimInstanceRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceRef.Class);
	}
	
	/* Camera and SpringArm */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;		// SpringArm은 캐릭터의 회전 입력값을 따른다.
	MeshHideThreshold = 200.f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;	// 카메라는 항상 RelativeRotation을 유지해 캐릭터를 바라본다.
	FollowCamera->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
	FollowCamera->PostProcessSettings.DepthOfFieldFocalDistance = 10000.f;
	FollowCamera->PostProcessSettings.bOverride_DepthOfFieldFstop = true;
	FollowCamera->PostProcessSettings.DepthOfFieldFstop = 32.f;

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

	static ConstructorHelpers::FObjectFinder<UInputAction> EquipActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_Equip.IA_Equip'"));
	if (EquipActionRef.Object)
	{
		EquipAction = EquipActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> CrouchActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_Crouch.IA_Crouch'"));
	if (CrouchActionRef.Object)
	{
		CrouchAction = CrouchActionRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> AimActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_Aim.IA_Aim'"));
    if (AimActionRef.Object)
    {
    	AimAction = AimActionRef.Object;
    }

	static ConstructorHelpers::FObjectFinder<UInputAction> FireActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_Fire.IA_Fire'"));
	if (FireActionRef.Object)
	{
		FireAction = FireActionRef.Object;
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

	/* Combat Component */
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));

	/* Animation */
	static ConstructorHelpers::FClassFinder<UAnimInstance> BaseAnimLayerRef(TEXT("/Game/LBlaster/Actors/Manny/Animations/Locomotion/Unarmed/ABP_UnarmedAnimLayers.ABP_UnarmedAnimLayers_C"));
	if (BaseAnimLayerRef.Class)
	{
		BaseAnimLayerClass = BaseAnimLayerRef.Class;
	}

	/* Health */
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));

	/* Elim */
	ElimDelay = 3.f;
}

void ALBlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HideMeshIfCameraClose();
}

void ALBlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::DoJump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ThisClass::EquipWeapon);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ThisClass::DoCrouch);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ThisClass::AimStarted);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ThisClass::AimFinished);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ThisClass::FireStarted);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ThisClass::FireFinished);
}

void ALBlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ALBlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ALBlasterCharacter, LastHitNormal);
}

void ALBlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->LinkAnimClassLayers(BaseAnimLayerClass);
	}
}

void ALBlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	/* Input */
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}	
	}

	/* Overhead Widget */
	if (UOverheadWidget* OverheadWidget = Cast<UOverheadWidget>(OverheadWidgetComponent->GetUserWidgetObject()))
	{
		OverheadWidget->ShowPlayerName(this);
	}

	/* Damage */
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}
}

void ALBlasterCharacter::SetOverlappingWeapon(AWeapon* InWeapon)
{
	AWeapon* LastOverlappingWeapon = OverlappingWeapon;
	OverlappingWeapon = InWeapon;

	if (IsLocallyControlled())
	{
		ShowOverlappingWeaponPickupWidget(LastOverlappingWeapon);
	}
}

void ALBlasterCharacter::AttachWeapon(AWeapon* InEquippedWeapon)
{
	// Called from UCombatComponent::EquipWeapon()
	if (InEquippedWeapon)
	{
		if (const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName(TEXT("RightHandSocket"))))
		{
			HandSocket->AttachActor(InEquippedWeapon, GetMesh());
		}
		InEquippedWeapon->SetOwner(this);
	}
}

void ALBlasterCharacter::SetADSWalkSpeed(bool bEnabled, float InADSMultiplier)
{
	if (bEnabled)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMaxWalkSpeed * InADSMultiplier;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMaxWalkSpeed;
	}
}

void ALBlasterCharacter::SetWeaponAnimLayers(TSubclassOf<UAnimInstance> InWeaponAnimLayer)
{
	if (::IsValid(InWeaponAnimLayer))
	{
		GetMesh()->LinkAnimClassLayers(InWeaponAnimLayer);
	}
}

void ALBlasterCharacter::PlayFireMontage(UAnimMontage* InFireMontage)
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(InFireMontage);	
	}
}

void ALBlasterCharacter::Elim()
{
	GetWorldTimerManager().SetTimer(ElimTimer, this, &ThisClass::ElimTimerFinished, ElimDelay);
	MulticastElim();
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

void ALBlasterCharacter::DoJump(const FInputActionValue& ActionValue)
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		if (bIsCrouched)
		{
			UnCrouch();
		}
		else
		{
			Jump();
		}
	}
}

void ALBlasterCharacter::EquipWeapon(const FInputActionValue& ActionValue)
{
	if (!OverlappingWeapon)
	{
		return;
	}
	
	ServerEquipWeapon();
}

void ALBlasterCharacter::DoCrouch(const FInputActionValue& ActionValue)
{
	if (!bIsCrouched && GetCharacterMovement()->IsMovingOnGround())
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

void ALBlasterCharacter::AimStarted(const FInputActionValue& ActionValue)
{
	if (CombatComponent)
	{
		CombatComponent->SetAiming(true);
	}	
}

void ALBlasterCharacter::AimFinished(const FInputActionValue& ActionValue)
{
	if (CombatComponent)
	{
		CombatComponent->SetAiming(false);
	}
}

void ALBlasterCharacter::FireStarted(const FInputActionValue& ActionValue)
{
	if (CombatComponent)
	{
		CombatComponent->SetFiring(true);
	}
}

void ALBlasterCharacter::FireFinished(const FInputActionValue& ActionValue)
{
	if (CombatComponent)
	{
		CombatComponent->SetFiring(false);
	}
}

void ALBlasterCharacter::HideMeshIfCameraClose()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	// 캐릭터와 카메라가 임계값보다 가까워지면 캐릭터의 메시를 숨긴다.
	if ((GetActorLocation() - FollowCamera->GetComponentLocation()).Size() < MeshHideThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (CombatComponent && CombatComponent->GetEquippingWeapon() && CombatComponent->GetEquippingWeapon()->GetWeaponMesh())
		{
			CombatComponent->GetEquippingWeapon()->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		// 평소 상황이므로 early return
		if (GetMesh()->IsVisible())
		{
			return;
		}
		
		GetMesh()->SetVisibility(true);
		if (CombatComponent && CombatComponent->GetEquippingWeapon() && CombatComponent->GetEquippingWeapon()->GetWeaponMesh())
		{
			CombatComponent->GetEquippingWeapon()->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void ALBlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastOverlappingWeapon) const
{
	ShowOverlappingWeaponPickupWidget(LastOverlappingWeapon);
}

void ALBlasterCharacter::ShowOverlappingWeaponPickupWidget(AWeapon* LastOverlappingWeapon) const
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastOverlappingWeapon)
	{
		LastOverlappingWeapon->ShowPickupWidget(false);
	}
}

void ALBlasterCharacter::PlayHitReactMontage(const FVector& HitNormal)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (CombatComponent && AnimInstance)
	{
		if (UAnimMontage* MontageToPlay = CombatComponent->SelectHitReactMontage(HitNormal))
		{
			AnimInstance->Montage_Play(MontageToPlay);
		}
	}
}

void ALBlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController,
                                       AActor* DamageCauser)
{
	if (HealthComponent)
	{
		HealthComponent->ReceiveDamage(Damage, InstigatorController);
	}
}

bool ALBlasterCharacter::IsAiming() const
{
	return CombatComponent && CombatComponent->IsAiming();
}

bool ALBlasterCharacter::IsFiring() const
{
	return CombatComponent && CombatComponent->IsFiring();
}

FTransform ALBlasterCharacter::GetLeftHandTransform() const
{
	if (CombatComponent->GetEquippingWeapon() && GetMesh())
	{
		if (USkeletalMeshComponent* EquippingWeaponMesh = CombatComponent->GetEquippingWeapon()->GetWeaponMesh())
		{
			FTransform LeftHandTransform = EquippingWeaponMesh->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
			FVector OutPosition;
			FRotator OutRotation;
			GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
			LeftHandTransform.SetLocation(OutPosition);
			LeftHandTransform.SetRotation(FQuat(OutRotation));
			return LeftHandTransform;
		}	
	}
	return FTransform();
}

void ALBlasterCharacter::SetLastHitNormal(const FVector& InHitNormal)
{
	LastHitNormal = InHitNormal;
	PlayHitReactMontage(LastHitNormal);
}

void ALBlasterCharacter::OnRep_LastHitNormal()
{
	PlayHitReactMontage(LastHitNormal);
}

void ALBlasterCharacter::PlayDeathMontage(const FVector& HitNormal)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (CombatComponent && AnimInstance)
	{
		if (UAnimMontage* MontageToPlay = CombatComponent->SelectDeathMontage(HitNormal))
		{
			AnimInstance->Montage_Play(MontageToPlay);
		}
	}
}

void ALBlasterCharacter::Ragdoll()
{
	CameraBoom->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALBlasterCharacter::ElimTimerFinished()
{
	if (ALBlasterGameMode* GameMode = GetWorld()->GetAuthGameMode<ALBlasterGameMode>())
	{
		GameMode->RequestRespawn(this, Controller);
	}
}

void ALBlasterCharacter::MulticastElim_Implementation()
{
	PlayDeathMontage(LastHitNormal);
    	
    const float RagdollDelay = FMath::FRandRange(0.1f, 0.6f);
    FTimerHandle RagdollDelayTimer;
    GetWorldTimerManager().SetTimer(RagdollDelayTimer, this, &ThisClass::Ragdoll, RagdollDelay);
}

AController* ALBlasterCharacter::GetController()
{
	return Controller;
}

void ALBlasterCharacter::ServerEquipWeapon_Implementation()
{
	if (CombatComponent && OverlappingWeapon)
	{
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}
}

