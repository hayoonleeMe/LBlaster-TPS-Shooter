// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"

#include "Casing.h"
#include "LBlaster.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/LBlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Player/LBlasterPlayerController.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicatingMovement(true);

	/* Mesh */
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetIsReplicated(false);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECC_FootPlacement, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* Custom Depth */
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
    WeaponMesh->MarkRenderStateDirty();	// 변경 사항을 Refresh
	EnableCustomDepth(true);

	/* Overlap Sphere */
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetIsReplicated(false);
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* Pickup Widget Component */
	PickupWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pickup Widget Component"));
	PickupWidgetComponent->SetupAttachment(RootComponent);
	PickupWidgetComponent->SetIsReplicated(false);
	PickupWidgetComponent->SetRelativeLocation(FVector(0.f, 20.f, 50.f));
	PickupWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	PickupWidgetComponent->SetDrawAtDesiredSize(true);
	PickupWidgetComponent->SetVisibility(false);
	LocOffset = FVector(0.f, 20.f, 50.f);

	static ConstructorHelpers::FClassFinder<UUserWidget> PickupWidgetClassRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/LBlaster/UI/HUD/WBP_PickupWidget.WBP_PickupWidget_C'"));
	if (PickupWidgetClassRef.Class)
	{
		PickupWidgetComponent->SetWidgetClass(PickupWidgetClassRef.Class);
	}

	/* Crosshair */
	static ConstructorHelpers::FObjectFinder<UTexture2D> CenterCrosshairRef(TEXT("/Script/Engine.Texture2D'/Game/LBlaster/UI/Crosshair/Primary/Crosshair_Center.Crosshair_Center'"));
	if (CenterCrosshairRef.Object)
	{
		CenterCrosshair = CenterCrosshairRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> TopCrosshairRef(TEXT("/Script/Engine.Texture2D'/Game/LBlaster/UI/Crosshair/Primary/Crosshair_Top.Crosshair_Top'"));
	if (TopCrosshairRef.Object)
	{
		TopCrosshair = TopCrosshairRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> BottomCrosshairRef(TEXT("/Script/Engine.Texture2D'/Game/LBlaster/UI/Crosshair/Primary/Crosshair_Bottom.Crosshair_Bottom'"));
	if (BottomCrosshairRef.Object)
	{
		BottomCrosshair = BottomCrosshairRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> LeftCrosshairRef(TEXT("/Script/Engine.Texture2D'/Game/LBlaster/UI/Crosshair/Primary/Crosshair_Left.Crosshair_Left'"));
	if (LeftCrosshairRef.Object)
	{
		LeftCrosshair = LeftCrosshairRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> RightCrosshairRef(TEXT("/Script/Engine.Texture2D'/Game/LBlaster/UI/Crosshair/Primary/Crosshair_Right.Crosshair_Right'"));
	if (RightCrosshairRef.Object)
	{
		RightCrosshair = RightCrosshairRef.Object;
	}

	/* Auto Fire */
	bAutomatic = true;
	FireDelay = 0.15f;

	/* Scatter (Minute of Angle) */
	bUseScatter = true;
	DistanceToSphere = 910.f;

	/* Recoil */
	VerticalRecoilValue = 0.3f;

	/* Damage */
	Damage = 20.f;
	HeadshotMultiplier = 1.5f;

	/* ADS FOV */
	ADSFOV = 70.f;

	/* Attach Transform */
	AttachTransform = FTransform::Identity;
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, ServerAmmoState);
	DOREPLIFETIME(AWeapon, ServerWeaponStateChangedState);
	DISABLE_REPLICATED_PRIVATE_PROPERTY(AWeapon, bHidden);
	DISABLE_REPLICATED_PROPERTY(AWeapon, Owner);
}

void AWeapon::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);

	if (NewOwner)
	{
		OwnerCharacter = Cast<ALBlasterCharacter>(NewOwner);
	}
	else
	{
		OwnerCharacter = nullptr;
	}
}

void AWeapon::ShowPickupWidget(bool bInShow) const
{
	if (PickupWidgetComponent)
	{
		// 항상 Weapon 위에 표시되도록 위치 보정
		const FVector RelativeLocation = GetActorRotation().UnrotateVector(LocOffset);
		PickupWidgetComponent->SetRelativeLocation(RelativeLocation);
		PickupWidgetComponent->SetVisibility(bInShow);
	}
}

void AWeapon::SetHUDAmmo()
{
	if (IsValidOwnerCharacter() && bSelected)
	{
		OwnerCharacter->SetHUDAmmo(Ammo);
	}
}

void AWeapon::AddAmmo(int32 InAmmoToAdd)
{
	if (!IsValidOwnerCharacter())
	{
		return;
	}
	
	FAmmoChange AmmoChange = CreateAmmoChange(InAmmoToAdd);
	if (OwnerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
	{
		ProcessAddAmmo(InAmmoToAdd);
		UnacknowledgedAmmoChanges.Add(AmmoChange);
		ServerSendAmmoChange(AmmoChange);
	}
	if (OwnerCharacter->HasAuthority() && OwnerCharacter->IsLocallyControlled())
	{
		ServerSendAmmoChange(AmmoChange);
	}
}

void AWeapon::SetWeaponVisibility(bool bInVisible)
{
	SetActorEnableCollision(bInVisible);
	SetActorHiddenInGame(!bInVisible);
}

float AWeapon::GetSphereRadius() const
{
	return MOA * 2.54f;
}

void AWeapon::SpendRound()
{
	AddAmmo(-1);
}

void AWeapon::ProcessAddAmmo(int32 InAmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo + InAmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
}

FAmmoChange AWeapon::CreateAmmoChange(int32 InAmmoToAdd)
{
	if (AGameStateBase* GameStateBase = GetWorld()->GetGameState())
	{
		FAmmoChange AmmoChange;
		AmmoChange.AmmoToAdd = InAmmoToAdd;
		AmmoChange.Time = GameStateBase->GetServerWorldTimeSeconds();
		return AmmoChange;
	}
	return FAmmoChange();
}

void AWeapon::ClearAcknowledgedAmmoChanges(const FAmmoChange& LastAmmoChange)
{
	TArray<FAmmoChange> NewArray;

	for (const FAmmoChange& AmmoChange : UnacknowledgedAmmoChanges)
	{
		if (AmmoChange.Time > LastAmmoChange.Time)
		{
			NewArray.Add(AmmoChange);
		}
	}
	UnacknowledgedAmmoChanges = NewArray;
}

void AWeapon::OnRep_ServerAmmoState()
{
	Ammo = ServerAmmoState.Ammo;

	ClearAcknowledgedAmmoChanges(ServerAmmoState.LastAmmoChange);

	// Unacknowledged Ammo Change 다시 적용
	for (const FAmmoChange& AmmoChange : UnacknowledgedAmmoChanges)
	{
		Ammo = FMath::Clamp(Ammo + AmmoChange.AmmoToAdd, 0, MagCapacity);
	}
	SetHUDAmmo();

	if (IsValidOwnerCharacter() && OwnerCharacter->GetLocalRole() == ROLE_SimulatedProxy)
	{
		ProcessAddAmmo(ServerAmmoState.LastAmmoChange.AmmoToAdd);
	}
}

void AWeapon::ServerSendAmmoChange_Implementation(const FAmmoChange& InAmmoChange)
{
	ProcessAddAmmo(InAmmoChange.AmmoToAdd);

	ServerAmmoState.Ammo = Ammo;
	ServerAmmoState.LastAmmoChange = InAmmoChange;
}

void AWeapon::CallServerScoreRequest(ALBlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity,
                                     float HitTime, float InDamage, float InHeadshotMultiplier, float InProjectileGravityScale)
{
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}

	if (CasingClass)
	{
		if (const USkeletalMeshSocket* Socket = WeaponMesh->GetSocketByName(TEXT("AmmoEject")))
		{
			const FTransform SocketTransform = Socket->GetSocketTransform(WeaponMesh);
			
			if (UWorld* World = GetWorld())
			{
				World->SpawnActor<ACasing>(CasingClass, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator());
			}
		}
	}

	SpendRound();
}

FVector AWeapon::TraceEndWithScatter(const FVector& HitTarget) const
{
	if (const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName(TEXT("MuzzleFlash"))))
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector TraceStart = SocketTransform.GetLocation();

		const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
		const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, GetSphereRadius());
		const FVector EndLoc = SphereCenter + RandVec;
		const FVector ToEndLoc = EndLoc - TraceStart;

		return TraceStart + ToEndLoc / ToEndLoc.Size() * TRACE_LENGTH;
	}
	return FVector::ZeroVector;
}

void AWeapon::Dropped()
{
	ChangeWeaponState(EWeaponState::EWS_Dropped);
	bSelected = false;

	const FDetachmentTransformRules DetachRule(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRule);
	SetOwner(nullptr);
}

void AWeapon::Holstered()
{
	SetWeaponVisibility(false);
	bSelected = false;

	const FDetachmentTransformRules DetachRule(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRule);
}

void AWeapon::EnableCustomDepth(bool bEnable) const
{
	if (WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

float AWeapon::GetDamageFallOffMultiplier(float InDistance) const
{
	if (DamageFallOffCurve)
	{
		return DamageFallOffCurve->GetFloatValue(InDistance);
	}
	return 0.f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Overlap Event 판정, 실제 무기 착용은 서버에서만 이루어지게 하고, Pickup Widget은 로컬에서 표시해 높은 핑에서도 바로 볼 수 있게 함.
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereBeginOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                   bool bFromSweep, const FHitResult& SweepResult)
{
	if (ALBlasterCharacter* OtherCharacter = Cast<ALBlasterCharacter>(OtherActor))
	{
		OtherCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ALBlasterCharacter* OtherCharacter = Cast<ALBlasterCharacter>(OtherActor))
	{
		OtherCharacter->SetOverlappingWeapon(nullptr);
	}
}

bool AWeapon::IsValidOwnerCharacter()
{
	if (!OwnerCharacter && GetOwner())
	{
		OwnerCharacter = Cast<ALBlasterCharacter>(GetOwner());
	}
	return OwnerCharacter != nullptr;
}

bool AWeapon::IsValidOwnerController()
{
	if (!OwnerController && IsValidOwnerCharacter() && OwnerCharacter->GetController())
	{
		OwnerController = Cast<ALBlasterPlayerController>(OwnerCharacter->GetController());
	}
	return OwnerController != nullptr;
}

void AWeapon::OnRep_WeaponState()
{
	OnChangedWeaponState();
}

void AWeapon::OnChangedWeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		// 무기가 장착된 상태라면 Pickup Widget을 숨기고 Pickup Overlap 발생 중지
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		EnableCustomDepth(false);
		break;
		
	case EWeaponState::EWS_Dropped:
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		EnableCustomDepth(true);
		break;
	}
}

void AWeapon::ChangeWeaponState(EWeaponState InWeaponStateToChange)
{
	if (!IsValidOwnerCharacter())
	{
		return;
	}

	FWeaponStateChange WeaponStateChange = CreateWeaponStateChange(InWeaponStateToChange);
	if (!OwnerCharacter->HasAuthority())
	{
		// 게임 초기에 Default Weapon을 생성하고 착용할 때 Simulated Proxy에서도 ProcessEquipWeapon()을 통해 이 함수를 호출하므로 작업을 수행할 수 있게 한다.
		ProcessChangeWeaponState(InWeaponStateToChange);
		if (OwnerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
		{
			UnacknowledgedWeaponStateChanges.Add(WeaponStateChange);
			// ChangeWeaponState는 클라에서의 EquipWeapon에 의해 서버로 전달된 RPC에서 수행되므로 Autonomous Proxy에서는 Server RPC를 전송하지 않도록 한다.
		}	
	}
	
	// Autonomous Proxy에 의해 전송된 Equip Server RPC에 의해 이 함수가 호출됐을 때도 수행해줘야 한다.
	if (OwnerCharacter->HasAuthority() && (OwnerCharacter->IsLocallyControlled() || OwnerCharacter->GetRemoteRole() == ROLE_AutonomousProxy))
	{
		ServerSendWeaponStateChange(WeaponStateChange);
	}
}

void AWeapon::ProcessChangeWeaponState(EWeaponState InWeaponStateToChange)
{
	WeaponState = InWeaponStateToChange;
	OnChangedWeaponState();
}

void AWeapon::OnRep_ServerWeaponStateChangedState()
{
	ClearAcknowledgedWeaponStateChanges(ServerWeaponStateChangedState.LastWeaponStateChange);

	// Unacknowledged Combat State Change 다시 적용
	if (UnacknowledgedWeaponStateChanges.IsValidIndex(UnacknowledgedWeaponStateChanges.Num() - 1))
	{
		ProcessChangeWeaponState(UnacknowledgedWeaponStateChanges[UnacknowledgedWeaponStateChanges.Num() - 1].WeaponStateToChange);
	}

	if (IsValidOwnerCharacter() && OwnerCharacter->GetLocalRole() == ROLE_SimulatedProxy)
	{
		ProcessChangeWeaponState(ServerWeaponStateChangedState.LastWeaponStateChange.WeaponStateToChange);
	}
}

FWeaponStateChange AWeapon::CreateWeaponStateChange(EWeaponState InWeaponStateToChange)
{
	if (AGameStateBase* GameStateBase = GetWorld()->GetGameState())
	{
		FWeaponStateChange WeaponStateChange;
		WeaponStateChange.WeaponStateToChange = InWeaponStateToChange;
		WeaponStateChange.Time = GameStateBase->GetServerWorldTimeSeconds();
		return WeaponStateChange;
	}
	return FWeaponStateChange();
}

void AWeapon::ServerSendWeaponStateChange_Implementation(const FWeaponStateChange& InWeaponStateChange)
{
	ProcessChangeWeaponState(InWeaponStateChange.WeaponStateToChange);

	ServerWeaponStateChangedState.WeaponState = WeaponState;
	ServerWeaponStateChangedState.LastWeaponStateChange = InWeaponStateChange;
}

void AWeapon::ClearAcknowledgedWeaponStateChanges(const FWeaponStateChange& LastWeaponStateChange)
{
	TArray<FWeaponStateChange> NewArray;

	for (const FWeaponStateChange& WeaponStateChange : UnacknowledgedWeaponStateChanges)
	{
		if (WeaponStateChange.Time > LastWeaponStateChange.Time)
		{
			NewArray.Add(WeaponStateChange);
		}
	}
	UnacknowledgedWeaponStateChanges = NewArray;
}
