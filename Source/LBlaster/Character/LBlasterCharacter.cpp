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
#include "Component/LagCompensationComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameInstance/LBGameInstance.h"
#include "GameMode/LBlasterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Player/LBlasterPlayerController.h"
#include "Player/LBlasterPlayerState.h"
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
	RespawnTimerUpdateFrequency = 0.05f;

	/* Dissolve Effect */
	DissolveTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("Dissolve Timeline Component"));

	/* Lag Compensation */
	LagCompensationComponent = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("Lag Compensation Component"));
	
	/* Hit boxes used for Server-Side Rewind */
	InitializeHitBoxes();
}

void ALBlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PollInit();
}

void ALBlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::DoJump);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ThisClass::DoCrouch);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ThisClass::DoADS);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ThisClass::DoFire);
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ThisClass::Reload);
	EnhancedInputComponent->BindAction(TossGrenadeAction, ETriggerEvent::Triggered, this, &ThisClass::TossGrenade);
	EnhancedInputComponent->BindAction(FirstWeaponSlotAction, ETriggerEvent::Triggered, this, &ThisClass::ChooseFirstWeaponSlot);
	EnhancedInputComponent->BindAction(SecondWeaponSlotAction, ETriggerEvent::Triggered, this, &ThisClass::ChooseSecondWeaponSlot);
	EnhancedInputComponent->BindAction(ThirdWeaponSlotAction, ETriggerEvent::Triggered, this, &ThisClass::ChooseThirdWeaponSlot);
}

void ALBlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALBlasterCharacter, bInvincible);
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

	/* Damage */
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}
}

void ALBlasterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	/* Invincibility */
	if (ALBlasterPlayerController* PlayerController = GetController<ALBlasterPlayerController>())
	{
		if (PlayerController->bCanSetInvincibilityInBeginPlay && HasAuthority())
		{
			PlayerController->StartInvincibilityTimer();
		}
		if (!PlayerController->bCanSetInvincibilityInBeginPlay)
		{
			PlayerController->bCanSetInvincibilityInBeginPlay = true;
		}
	}
}

AWeapon* ALBlasterCharacter::GetEquippingWeapon() const
{
	if (CombatComponent)
	{
		return CombatComponent->GetEquippingWeapon();
	}
	return nullptr;
}

FString ALBlasterCharacter::GetCombatInfo()
{
	if (CombatComponent)
	{
		return CombatComponent->GetCombatInfo();
	}
	return TEXT("Invalid CombatComponent");
}

void ALBlasterCharacter::SetHUDAmmo(int32 InAmmo)
{
	if (ALBlasterPlayerController* PlayerController = Cast<ALBlasterPlayerController>(Controller))
	{
		PlayerController->SetHUDAmmo(InAmmo);
	}
}

void ALBlasterCharacter::EquipOverlappingWeapon(AWeapon* InWeapon)
{
	if (!CombatComponent)
	{
		return;
	}
	
	// 3번 슬롯인 상태에서 Weapon Overlap 방지; End Overlap은 가능
	if (CombatComponent->GetEquipSlotType() == EEquipSlot::EES_ThirdSlot)
	{
		return;
	}
	CombatComponent->EquipOverlappingWeapon(InWeapon);
}

FTransform ALBlasterCharacter::GetWeaponLeftHandTransform() const
{
	if (CombatComponent)
	{
		return CombatComponent->GetWeaponLeftHandTransform();
	}
	return FTransform::Identity;
}

void ALBlasterCharacter::StartTossGrenade() const
{
	if (CombatComponent)
	{
		CombatComponent->StartTossGrenade();
	}
}

void ALBlasterCharacter::EquipFinished() const
{
	if (CombatComponent)
	{
		CombatComponent->EquipFinished();
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

void ALBlasterCharacter::SetAdsFov(float InAdsFov) const
{
	if (CameraComponent)
	{
		CameraComponent->SetAdsFov(InAdsFov);
	}
}

void ALBlasterCharacter::Elim(bool bPlayerLeftGame)
{
	bIsDead = true;
	MulticastElim(bPlayerLeftGame);
}

bool ALBlasterCharacter::IsServerSideRewindEnabled() const
{
	if (LagCompensationComponent)
	{
		return LagCompensationComponent->IsServerSideRewindEnabled();
	}
	return false;
}

void ALBlasterCharacter::EnableServerSideRewind(bool bInEnabled) const
{
	if (LagCompensationComponent)
	{
		LagCompensationComponent->EnableServerSideRewind(bInEnabled);
	}
}

void ALBlasterCharacter::SetXAxisSensitivityFromUserSettings(float InXAxisSensitivity)
{
	if (LookAction)
	{
		for (UInputModifier* Modifer : LookAction->Modifiers)
		{
			if (UInputModifierScalar* ScalarModifier = Cast<UInputModifierScalar>(Modifer))
			{
				ScalarModifier->Scalar.X = InXAxisSensitivity;
				return;
			}
		}
	}
}

void ALBlasterCharacter::SetYAxisSensitivityFromUserSettings(float InYAxisSensitivity)
{
	if (LookAction)
	{
		for (UInputModifier* Modifer : LookAction->Modifiers)
		{
			if (UInputModifierScalar* ScalarModifier = Cast<UInputModifierScalar>(Modifer))
			{
				ScalarModifier->Scalar.Y = InYAxisSensitivity;
				return;
			}
		}
	}
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
	AddControllerPitchInput(LookAxisVector.Y * -1.f);
}

void ALBlasterCharacter::DoJump(const FInputActionValue& ActionValue)
{
	if (ActionValue.Get<bool>())
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
	else
	{
		StopJumping();
	}
}

void ALBlasterCharacter::DoCrouch()
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

void ALBlasterCharacter::DoADS(const FInputActionValue& ActionValue)
{
	if (CombatComponent)
	{
		CombatComponent->TriggerAimingKey(ActionValue.Get<bool>());
		CombatComponent->SetAiming(ActionValue.Get<bool>());
	}
}

void ALBlasterCharacter::DoFire(const FInputActionValue& ActionValue)
{
	if (CombatComponent)
	{
		CombatComponent->SetFiring(ActionValue.Get<bool>());
	}
}

void ALBlasterCharacter::Reload()
{
	if (CombatComponent)
	{
		CombatComponent->Reload();
	}
}

void ALBlasterCharacter::TossGrenade()
{
	if (CombatComponent)
	{
		CombatComponent->TossGrenade();
	}
}

void ALBlasterCharacter::ChooseFirstWeaponSlot()
{
	if (CombatComponent)
	{
		CombatComponent->ChooseWeaponSlot(EEquipSlot::EES_FirstSlot);
	}
}

void ALBlasterCharacter::ChooseSecondWeaponSlot()
{
	if (CombatComponent)
	{
		CombatComponent->ChooseWeaponSlot(EEquipSlot::EES_SecondSlot);
	}
}

void ALBlasterCharacter::ChooseThirdWeaponSlot()
{
	if (CombatComponent)
	{
		CombatComponent->ChooseWeaponSlot(EEquipSlot::EES_ThirdSlot);
	}
}

void ALBlasterCharacter::EquipDefaultWeapon() const
{
	if (CombatComponent)
	{
		CombatComponent->ServerEquipDefaultWeapon();
	}
}

void ALBlasterCharacter::ReleaseCombatState() const
{
	if (CombatComponent)
	{
		CombatComponent->SetAiming(false);
		CombatComponent->SetFiring(false);
	}
}

void ALBlasterCharacter::SetCharacterMaterialsByTeam(const FTeamCharacterMaterials& TeamCharacterMaterials)
{
	if (GetMesh())
	{
		GetMesh()->SetMaterial(0, TeamCharacterMaterials.MaterialInst2);
		GetMesh()->SetMaterial(1, TeamCharacterMaterials.MaterialInst1);
		DissolveMaterialInstances.Empty(2);
		DissolveMaterialInstances.Add(TeamCharacterMaterials.DissolveInst1);
		DissolveMaterialInstances.Add(TeamCharacterMaterials.DissolveInst2);
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
	// 스폰 무적
	if (bInvincible)
	{
		return;
	}
	
	// 팀 데스매치에서 아군사격 방지 (폭발 데미지)
	if (ALBlasterPlayerState* VictimState = GetPlayerState<ALBlasterPlayerState>())
	{
		if (ALBlasterPlayerState* AttackerState = InstigatorController->GetPlayerState<ALBlasterPlayerState>())
		{
			if (VictimState->GetTeam() != ETeam::ET_MAX && VictimState->GetTeam() == AttackerState->GetTeam())
			{
				return;
			}
		}
	}
	
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

void ALBlasterCharacter::PollInit()
{
	if (!bFirstTimeInit)
	{
		if (GetMesh() && GetPlayerState())
		{
			if (ALBlasterPlayerState* LBPlayerState = GetPlayerState<ALBlasterPlayerState>())
			{
				if (LBPlayerState->GetTeam() != ETeam::ET_MAX)
				{
					bFirstTimeInit = true;
					SetCharacterMaterialsByTeam(LBPlayerState->GetCharacterMaterials());
				}
			}
		}
	}
}

bool ALBlasterCharacter::IsAiming() const
{
	return CombatComponent && CombatComponent->IsAiming();
}

bool ALBlasterCharacter::CanAnimateFiring() const
{
	return CombatComponent && CombatComponent->CanAnimateFiring();
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
	if (!bInvincible)
	{
		LastHitNormal = InHitNormal;
		PlayHitReactMontage(LastHitNormal);	
	}
}

void ALBlasterCharacter::PickupAmmo(EWeaponType InWeaponType, int32 InAmmoAmount)
{
	if (CombatComponent)
	{
		CombatComponent->PickupAmmo(InWeaponType, InAmmoAmount);
	}
}

void ALBlasterCharacter::EquipNewOverlappingWeapon()
{
	if (CombatComponent)
	{
		CombatComponent->EquipNewOverlappingWeapon();
	}
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
	// Respawn Timer 위젯을 숨김
	if (HasAuthority())
	{
		if (ALBlasterPlayerController* PlayerController = GetController<ALBlasterPlayerController>())
		{
			if (PlayerController->IsLocalController())
			{
				PlayerController->HideRespawnTimer();
			}
			else
			{
				PlayerController->ClientHideRespawnTimer();
			}
		}
	}
	
	if (ALBlasterGameMode* GameMode = GetWorld()->GetAuthGameMode<ALBlasterGameMode>())
	{
		if (!bLeftGame)
		{
			GameMode->RequestRespawn(this, Controller);
		}
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

void ALBlasterCharacter::MulticastElim_Implementation(bool bPlayerLeftGame)
{
	if (CombatComponent)
	{
		// Aiming 상태 해제
		if (IsLocallyControlled())
		{
			CombatComponent->SetAiming(false);
		}
		// 소지 중인 무기 모두 해제
		CombatComponent->ElimAllWeapon();

		if (IsLocallyControlled())
		{
			// 크로스헤어 숨김
			CombatComponent->HideCrosshair();
		}
	}
	
	PlayDeathMontage(LastHitNormal);
	
	// 죽는 중에 중복 타격되지 않도록 충돌 제거
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 매치가 종료됐는지
	bool bIsMatchInCooldown = false;
	
	if (ALBlasterPlayerController* PlayerController = Cast<ALBlasterPlayerController>(Controller))
	{
		// 탄약 UI 0으로 업데이트
		PlayerController->SetHUDAmmo(0);
		PlayerController->SetHUDCarriedAmmo(0);
		PlayerController->SetHUDWeaponTypeText();
		PlayerController->SetWeaponSlotIcon(EEquipSlot::EES_FirstSlot, EWeaponType::EWT_Unarmed);
		PlayerController->SetWeaponSlotIcon(EEquipSlot::EES_SecondSlot, EWeaponType::EWT_Unarmed);
		
		// 매치가 종료되면 리스폰 타이머 활성화 제한
		bIsMatchInCooldown = PlayerController->IsMatchInCooldown();
		// Start Respawn Timer
		if (!bIsMatchInCooldown && IsLocallyControlled() && !bPlayerLeftGame)
		{
			PlayerController->StartRespawnTimer(ElimDelay, RespawnTimerUpdateFrequency);
		}
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

	bLeftGame = bPlayerLeftGame;

	// 매치가 종료되면 리스폰 제한
	if (!bIsMatchInCooldown)
	{
		GetWorldTimerManager().SetTimer(ElimTimer, this, &ThisClass::ElimTimerFinished, ElimDelay);
	}
}

#pragma region HitBoxes
void ALBlasterCharacter::InitializeHitBoxes()
{
	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), FName(TEXT("head")));
	HitCollisionBoxes.Emplace(FName("head"), head);

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName(TEXT("pelvis")));
	HitCollisionBoxes.Emplace(FName("pelvis"), pelvis);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName(TEXT("spine_03")));
	HitCollisionBoxes.Emplace(FName("spine_03"), spine_03);

	spine_04 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_04"));
	spine_04->SetupAttachment(GetMesh(), FName(TEXT("spine_04")));
	HitCollisionBoxes.Emplace(FName("spine_04"), spine_04);

	spine_05 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_05"));
	spine_05->SetupAttachment(GetMesh(), FName(TEXT("spine_05")));
	HitCollisionBoxes.Emplace(FName("spine_05"), spine_05);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName(TEXT("upperarm_l")));
	HitCollisionBoxes.Emplace(FName("upperarm_l"), upperarm_l);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName(TEXT("lowerarm_l")));
	HitCollisionBoxes.Emplace(FName("lowerarm_l"), lowerarm_l);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName(TEXT("hand_l")));
	HitCollisionBoxes.Emplace(FName("hand_l"), hand_l);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName(TEXT("upperarm_r")));
	HitCollisionBoxes.Emplace(FName("upperarm_r"), upperarm_r);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName(TEXT("lowerarm_r")));
	HitCollisionBoxes.Emplace(FName("lowerarm_r"), lowerarm_r);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName(TEXT("hand_r")));
	HitCollisionBoxes.Emplace(FName("hand_r"), hand_r);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName(TEXT("thigh_l")));
	HitCollisionBoxes.Emplace(FName("thigh_l"), thigh_l);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName(TEXT("calf_l")));
	HitCollisionBoxes.Emplace(FName("calf_l"), calf_l);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName(TEXT("foot_l")));
	HitCollisionBoxes.Emplace(FName("foot_l"), foot_l);

	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName(TEXT("thigh_r")));
	HitCollisionBoxes.Emplace(FName("thigh_r"), thigh_r);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName(TEXT("calf_r")));
	HitCollisionBoxes.Emplace(FName("calf_r"), calf_r);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
    foot_r->SetupAttachment(GetMesh(), FName(TEXT("foot_r")));
	HitCollisionBoxes.Emplace(FName("foot_r"), foot_r);

	for (const TTuple<FName, UBoxComponent*>& Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
            Box.Value->SetCollisionResponseToAllChannels(ECR_Ignore);
            Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}
#pragma endregion