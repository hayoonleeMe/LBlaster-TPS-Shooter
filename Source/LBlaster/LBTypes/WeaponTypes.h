#pragma once

#define TRACE_LENGTH 80000.f;
#define CUSTOM_DEPTH_PURPLE 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Unarmed UMETA(DisplayName = "Unarmed"),
	EWT_Rifle UMETA(DisplayName = "Rifle"),
	EWT_RocketLauncher UMETA(DisplayName = "RocketLauncher"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_SMG UMETA(DisplayName = "SMG"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_SniperRifle UMETA(DisplayName = "SniperRifle"),
	EWT_GrenadeLauncher UMETA(DisplayName = "GrenadeLauncher"),
	EWT_MAX UMETA(DisplayName = "DefaultMax")
};

inline const TCHAR* LexToString(EWeaponType InType)
{
	switch (InType)
	{
	case EWeaponType::EWT_Unarmed:
		return TEXT("EWT_Unarmed");
	case EWeaponType::EWT_Rifle:
		return TEXT("EWT_Rifle");
	case EWeaponType::EWT_RocketLauncher:
		return TEXT("EWT_RocketLauncher");
	case EWeaponType::EWT_Pistol:
		return TEXT("EWT_Pistol");
	case EWeaponType::EWT_SMG:
		return TEXT("EWT_SMG");
	case EWeaponType::EWT_Shotgun:
		return TEXT("EWT_Shotgun");
	case EWeaponType::EWT_SniperRifle:
		return TEXT("EWT_SniperRifle");
	case EWeaponType::EWT_GrenadeLauncher:
		return TEXT("EWT_GrenadeLauncher");
	case EWeaponType::EWT_MAX:
		return TEXT("EWT_MAX");
	}
	return TEXT("Unknown");
}
