// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LBlasterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "LBlaster.h"
#include "Component/CombatComponent.h"
#include "Component/HealthComponent.h"
#include "Component/LBCharacterMovementComponent.h"
#include "Component/LBlasterCameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/LBlasterGameMode.h"
#include "HUD/OverheadWidget.h"
#include "Net/UnrealNetwork.h"
#include "Player/LBlasterPlayerController.h"
#include "Weapon/Weapon.h"
#include "Weapon/DamageType/DamageType_Explosive.h"

ALBlasterCharacter::ALBlasterCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<ULBCharacterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	/* Replication */
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	/* Actor */
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	BaseEyeHeight = 80.f;
	CrouchedEyeHeight = 50.f;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	
	/* Movement */
	BaseMaxWalkSpeed = 600.f;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;
	GetCharacterMovement()->MaxWalkSpeed = BaseMaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;
	GetCharacterMovement()->GravityScale = 1.f;
	GetCharacterMovement()->MaxAcceleration = 2400.f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.f;
	GetCharacterMovement()->BrakingFriction = 6.f;
	GetCharacterMovement()->GroundFriction = 8.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1400.f;
	GetCharacterMovement()->RotationRate.Yaw = 720.f;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(65.f);
	
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
	
	/* Camera */
	CameraComponent = CreateDefaultSubobject<ULBlasterCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(GetMesh());

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

	static ConstructorHelpers::FObjectFinder<UInputAction> ReloadActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_Reload.IA_Reload'"));
	if (ReloadActionRef.Object)
	{
		ReloadAction = ReloadActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> TossGrenadeActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_Toss_Grenade.IA_Toss_Grenade'"));
	if (TossGrenadeActionRef.Object)
	{
		TossGrenadeAction = TossGrenadeActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> FirstWeaponSlotActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_FirstWeaponSlot.IA_FirstWeaponSlot'"));
	if (FirstWeaponSlotActionRef.Object)
	{
		FirstWeaponSlotAction = FirstWeaponSlotActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> SecondWeaponSlotActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_SecondWeaponSlot.IA_SecondWeaponSlot'"));
	if (SecondWeaponSlotActionRef.Object)
	{
		SecondWeaponSlotAction = SecondWeaponSlotActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ThirdWeaponSlotActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_ThirdWeaponSlot.IA_ThirdWeaponSlot'"));
	if (ThirdWeaponSlotActionRef.Object)
	{
		ThirdWeaponSlotAction = ThirdWeaponSlotActionRef.Object;
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

	/* Grenade */
	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attached Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName(TEXT("GrenadeSocket")));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

	/* Dissolve Effect */
	DissolveTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("Dissolve Timeline Component"));
}

void ALBlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//HideMeshIfCameraClose();
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
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ThisClass::Reload);
	EnhancedInputComponent->BindAction(TossGrenadeAction, ETriggerEvent::Started, this, &ThisClass::TossGrenade);
	EnhancedInputComponent->BindAction(FirstWeaponSlotAction, ETriggerEvent::Started, this, &ThisClass::ChooseFirstWeaponSlot);
	EnhancedInputComponent->BindAction(SecondWeaponSlotAction, ETriggerEvent::Started, this, &ThisClass::ChooseSecondWeaponSlot);
	EnhancedInputComponent->BindAction(ThirdWeaponSlotAction, ETriggerEvent::Started, this, &ThisClass::ChooseThirdWeaponSlot);
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

	/* Animation */
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->LinkAnimClassLayers(BaseAnimLayerClass);
	}

	/* Grenade */
	AttachedGrenade->SetVisibility(false);
}

void ALBlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	/* Input */
	if (GEngine && GetWorld())
	{
		if (const ULocalPlayer* Player = GEngine->GetFirstGamePlayer(GetWorld()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Player))
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}	
		}
	}

	/* Overhead Widget */
	UpdatePlayerNameToOverheadWidget();

	/* Damage */
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}
}

void ALBlasterCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// 클라이언트 캐릭터의 PlayerName 표시
	UpdatePlayerNameToOverheadWidget();
}

void ALBlasterCharacter::SetOverlappingWeapon(AWeapon* InWeapon)
{
	if (!CombatComponent || CombatComponent->GetEquipSlotType() == EEquipSlot::EES_ThirdSlot)
	{
		return;
	}

	AWeapon* LastOverlappingWeapon = OverlappingWeapon;
	OverlappingWeapon = InWeapon;

	if (IsLocallyControlled())
	{
		ShowOverlappingWeaponPickupWidget(LastOverlappingWeapon);
	}
}

void ALBlasterCharacter::SetHUDAmmo(int32 InAmmo)
{
	if (ALBlasterPlayerController* PlayerController = Cast<ALBlasterPlayerController>(Controller))
	{
		PlayerController->SetHUDAmmo(InAmmo);
	}
}

FTransform ALBlasterCharacter::GetWeaponLeftHandTransform() const
{
	if (CombatComponent)
	{
		return CombatComponent->GetWeaponLeftHandTransform();
	}
	return FTransform::Identity;
}

void ALBlasterCharacter::ShowWeapon() const
{
	if (CombatComponent)
	{
		CombatComponent->ShowWeapon();
	}
}

void ALBlasterCharacter::StartTossGrenade() const
{
	if (CombatComponent)
	{
		CombatComponent->StartTossGrenade();
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

void ALBlasterCharacter::SetWeaponAnimLayers(EWeaponType InWeaponType, TSubclassOf<UAnimInstance> InWeaponAnimLayer)
{
	// BaseAnimLayerClass
	if (InWeaponType == EWeaponType::EWT_Unarmed)
	{
		if (BaseAnimLayerClass)
		{
			GetMesh()->LinkAnimClassLayers(BaseAnimLayerClass);
		}
	}
	else if (InWeaponAnimLayer)
	{
		GetMesh()->LinkAnimClassLayers(InWeaponAnimLayer);
	}
	
	if (CombatComponent)
	{
		if (UAnimMontage* MontageToPlay = CombatComponent->GetEquipMontage(InWeaponType))
		{
			PlayEquipMontage(MontageToPlay);
		}
	}
}

void ALBlasterCharacter::PlayFireMontage(UAnimMontage* InFireMontage)
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(InFireMontage);	
	}
}

void ALBlasterCharacter::PlayReloadMontage(UAnimMontage* InReloadMontage)
{
	if (InReloadMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(InReloadMontage);
		}	
	}
}

void ALBlasterCharacter::PlayTossGrenadeMontage(UAnimMontage* InTossGrenadeMontage)
{
	if (InTossGrenadeMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(InTossGrenadeMontage);
		}
	}
}

void ALBlasterCharacter::PlayEquipMontage(UAnimMontage* InEquipMontage)
{
	if (InEquipMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(InEquipMontage);
		}
	}
}

void ALBlasterCharacter::SetBlendWeight(float InWeight) const
{
	if (CameraComponent)
	{
		CameraComponent->SetBlendWeight(InWeight);
	}
}

void ALBlasterCharacter::SetADSFOV(float InADSFOV)
{
	if (CameraComponent)
	{
		CameraComponent->SetADSFOV(InADSFOV);
	}
}

void ALBlasterCharacter::Elim()
{
	GetWorldTimerManager().SetTimer(ElimTimer, this, &ThisClass::ElimTimerFinished, ElimDelay);
	MulticastElim();
}

void ALBlasterCharacter::UpdateHUDHealth() const
{
	if (HealthComponent)
	{
		HealthComponent->UpdateHUDHealth();
	}
}

int32 ALBlasterCharacter::GetGrenadeAmount() const
{
	if (CombatComponent)
	{
		return CombatComponent->GetGrenadeAmount();
	}
	return 0;
}

void ALBlasterCharacter::Move(const FInputActionValue& ActionValue)
{
	MovementVector = ActionValue.Get<FVector2D>();

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

	if (CombatComponent)
	{
		CombatComponent->SetAiming(false);
		CombatComponent->ServerEquipOverlappingWeapon();
	}
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

void ALBlasterCharacter::Reload(const FInputActionValue& ActionValue)
{
	if (CombatComponent)
	{
		CombatComponent->Reload();
	}
}

void ALBlasterCharacter::TossGrenade(const FInputActionValue& ActionValue)
{
	if (CombatComponent)
	{
		CombatComponent->TossGrenade();
	}
}

void ALBlasterCharacter::ChooseFirstWeaponSlot(const FInputActionValue& ActionValue)
{
	if (CombatComponent)
	{
		CombatComponent->SetAiming(false);
		CombatComponent->ServerChooseWeaponSlot(EEquipSlot::EES_FirstSlot);
	}
}

void ALBlasterCharacter::ChooseSecondWeaponSlot(const FInputActionValue& ActionValue)
{
	if (CombatComponent)
	{
		CombatComponent->SetAiming(false);
		CombatComponent->ServerChooseWeaponSlot(EEquipSlot::EES_SecondSlot);
	}
}

void ALBlasterCharacter::ChooseThirdWeaponSlot(const FInputActionValue& ActionValue)
{
	if (CombatComponent)
	{
		CombatComponent->SetAiming(false);
		CombatComponent->ServerChooseWeaponSlot(EEquipSlot::EES_ThirdSlot);
	}
}

void ALBlasterCharacter::HideMeshIfCameraClose()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	// 캐릭터와 카메라가 임계값보다 가까워지면 캐릭터의 메시를 숨긴다.
	if ((GetActorLocation() - CameraComponent->GetComponentLocation()).Size() < MeshHideThreshold)
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

void ALBlasterCharacter::UpdatePlayerNameToOverheadWidget()
{
	if (UOverheadWidget* OverheadWidget = Cast<UOverheadWidget>(OverheadWidgetComponent->GetUserWidgetObject()))
	{
		OverheadWidget->ShowPlayerName(this);
	}
}

void ALBlasterCharacter::EquipDefaultWeapon() const
{
	if (CombatComponent)
	{
		CombatComponent->ServerEquipDefaultWeapon();
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

void ALBlasterCharacter::PlayHitReactMontage(const FVector& HitNormal) const
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
	// 폭발 데미지면 랜덤한 HitReact Montage 재생
	if (DamageType->IsA(UDamageType_Explosive::StaticClass()))
	{
		SetLastHitNormal(FVector(FMath::FRandRange(0.f, 1.f), FMath::FRandRange(0.f, 1.f), FMath::FRandRange(0.f, 1.f)).GetSafeNormal());
	}
	
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

bool ALBlasterCharacter::IsReloading() const
{
	return CombatComponent && CombatComponent->IsReloading();
}

bool ALBlasterCharacter::IsEquippingWeapon() const
{
	return CombatComponent && CombatComponent->GetEquippingWeapon();
}

void ALBlasterCharacter::ReloadFinished() const
{
	if (CombatComponent)
	{
		CombatComponent->ReloadFinished();
	}
}

void ALBlasterCharacter::TossGrenadeFinished() const
{
	if (CombatComponent)
	{
		CombatComponent->TossGrenadeFinished();
	}
}

void ALBlasterCharacter::LaunchGrenade() const
{
	if (CombatComponent)
	{
		CombatComponent->LaunchGrenade();
	}
}

void ALBlasterCharacter::SetLastHitNormal(const FVector& InHitNormal)
{
	LastHitNormal = InHitNormal;
	PlayHitReactMontage(LastHitNormal);
}

void ALBlasterCharacter::PickupAmmo(EWeaponType InWeaponType, int32 InAmmoAmount)
{
	if (CombatComponent)
	{
		CombatComponent->PickupAmmo(InWeaponType, InAmmoAmount);
	}
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
	if (CameraComponent)
	{
		CameraComponent->bIsDead = true;
		CameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
	
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

void ALBlasterCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	for (UMaterialInstanceDynamic* MID : DynamicDissolveMaterialInstances)
	{
		MID->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void ALBlasterCharacter::StartDissolve()
{
	// Dissolve Curve를 사용하기 위한 셋업
	DissolveTrack.BindDynamic(this, &ThisClass::UpdateDissolveMaterial);
	if (DissolveTimelineComponent && DissolveCurve)
	{
		// 매 프레임마다 Dissolve Track에 바인딩된 함수에 DissolveCurve의 값이 전달되며 호출된다.
		DissolveTimelineComponent->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimelineComponent->Play();
	}
}

void ALBlasterCharacter::MulticastElim_Implementation()
{
	if (CombatComponent)
	{
		// Aiming 상태 해제
		if (IsLocallyControlled())
		{
			CombatComponent->SetAiming(false);
		}
		// 소지 중인 무기 모두 해제
		if (HasAuthority())
		{
			CombatComponent->ElimWeapon();
		}
	}
	
	PlayDeathMontage(LastHitNormal);
	
	// 죽는 중에 중복 타격되지 않도록 충돌 제거
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 탄약 UI 0으로 업데이트
	if (ALBlasterPlayerController* PlayerController = Cast<ALBlasterPlayerController>(Controller))
	{
		PlayerController->SetHUDAmmo(0);
		PlayerController->SetHUDCarriedAmmo(0);
		PlayerController->SetHUDWeaponTypeText();
	}

	/* Ragdoll */
    const float RagdollDelay = FMath::FRandRange(0.1f, 0.6f);
    FTimerHandle RagdollDelayTimer;
    GetWorldTimerManager().SetTimer(RagdollDelayTimer, this, &ThisClass::Ragdoll, RagdollDelay);

	/* Dissolve Effect */
	for (int32 i = 0; i < DissolveMaterialInstances.Num(); ++i)
	{
		if (UMaterialInstance* MI = DissolveMaterialInstances[i])
		{
			DynamicDissolveMaterialInstances.Emplace(UMaterialInstanceDynamic::Create(MI, this));
			if (UMaterialInstanceDynamic* MID = DynamicDissolveMaterialInstances[i])
			{
				GetMesh()->SetMaterial(i, MID);
				MID->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
				MID->SetScalarParameterValue(TEXT("Glow"), 200.f);
			}
		}
	}
	StartDissolve();
}

