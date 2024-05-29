#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_TossingGrenade UMETA(DisplayName = "Tossing Grenade"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),
	ECS_MAX UMETA(DisplayName = "DefaultMax")
};

inline const TCHAR* LexToString(ECombatState InType)
{
	switch (InType)
	{
	case ECombatState::ECS_Unoccupied:
		return TEXT("ECS_Unoccupied");
	case ECombatState::ECS_Reloading:
		return TEXT("ECS_Reloading");
	case ECombatState::ECS_TossingGrenade:
		return TEXT("ECS_TossingGrenade");
	case ECombatState::ECS_Equipping:
		return TEXT("ECS_Equipping");
	case ECombatState::ECS_MAX:
		return TEXT("ECS_MAX");
	}
	return TEXT("Unknown");
}
