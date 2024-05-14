#pragma once

UENUM(BlueprintType)
enum class ETeam : uint8
{
	ET_NoTeam UMETA(DisplayName = "NoTeam"),
	ET_RedTeam UMETA(DisplayName = "RedTeam"),
	ET_BlueTeam UMETA(DisplayName = "BlueTeam"),
	ET_MAX UMETA(DisplayName = "DefaultMax")
};
