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
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	/* Mesh */
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* Custom Depth */
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);
    WeaponMesh->MarkRenderStateDirty();	// 변경 사항을 Refresh
	EnableCustomDepth(true);

	/* Overlap Sphere */
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* Auto Fire */
	bAutomatic = true;
	FireDelay = 0.15f;

	/* Scatter (Minute of Angle) */
	bUseScatter = true;
	DistanceToSphere = 910.f;

	/* Spread */
	CrosshairSpreadRecoverySpeed = 5.f;
	CrosshairSpreadShootingGain = 0.8f;

	/* Recoil */
	VerticalRecoilValue = 0.3f;
	SpreadRecoilFactor = 1.f;

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

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Overlap Event
	if (AreaSphere)
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereBeginOverlap);	
	}
	SetActorEnableCollision(false);
}

void AWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Recovery CrosshairSpreadShootingFactor
	CrosshairSpreadShootingFactor = FMath::FInterpTo(CrosshairSpreadShootingFactor, 0.f, DeltaSeconds, CrosshairSpreadRecoverySpeed);
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

void AWeapon::OnWeaponEquipped(bool bInSelected)
{
	Super::OnWeaponEquipped(bInSelected);
	
	bSelected = bInSelected;

	// 무기가 장착된 상태라면 Overlapping Event 발생 중지
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnableCustomDepth(false);

	// Spread 초기화
	CrosshairSpreadShootingFactor = 0.f;
}

void AWeapon::SetWeaponVisibility(bool bInVisible)
{
	SetActorHiddenInGame(!bInVisible);
}

float AWeapon::GetMinuteOfAngle() const
{
	return MinuteOfAngle * 2.54f + CrosshairSpreadShootingFactor * SpreadRecoilFactor;
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

bool AWeapon::GetMuzzleFlashLocationForRep(FVector_NetQuantize& OutMuzzleFlashLocation, FRotator& OutMuzzleFlashRotation) const
{
	if (const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(TEXT("MuzzleFlash")))
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		OutMuzzleFlashLocation = SocketTransform.GetLocation();
		OutMuzzleFlashRotation = SocketTransform.GetRotation().Rotator();
		return true;
	}
	return false;
}

bool AWeapon::GetMuzzleFlashLocationAndDirection(FVector& OutMuzzleFlashLocation, FVector& OutMuzzleFlashDirection) const
{
	if (const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(TEXT("MuzzleFlash")))
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		OutMuzzleFlashLocation = SocketTransform.GetLocation();
		OutMuzzleFlashDirection = SocketTransform.GetRotation().Vector();
		return true;
	}
	return false;
}

void AWeapon::Fire(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const FVector& HitTarget)
{
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

	// Update CrosshairShootingFactor
	CrosshairSpreadShootingFactor += CrosshairSpreadShootingGain;
	
	SpendRound();
}

FVector AWeapon::TraceEndWithScatter(const FVector_NetQuantize& TraceStart, const FVector& HitTarget) const
{
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, GetMinuteOfAngle());
	const FVector EndLoc = SphereCenter + RandVec;
	const FVector ToEndLoc = (EndLoc - TraceStart).GetSafeNormal();
	
	return TraceStart + ToEndLoc * TRACE_LENGTH;
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

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                   bool bFromSweep, const FHitResult& SweepResult)
{
	// 소유자가 없을때만 착용하도록
	if (GetOwner())
	{
		return;
	}
	
	if (ALBlasterCharacter* OtherCharacter = Cast<ALBlasterCharacter>(OtherActor))
	{
		OtherCharacter->EquipOverlappingWeapon(this);
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
