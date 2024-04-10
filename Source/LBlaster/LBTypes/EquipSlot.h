#pragma once

UENUM(BlueprintType)
enum class EEquipSlot : uint8
{
	EES_FirstSlot UMETA(DisplayName = "First Slot"),
	EES_SecondSlot UMETA(DisplayName = "Second Slot"),
	EES_ThirdSlot UMETA(DisplayName = "Third Slot"),
	EES_MAX UMETA(DisplayName = "DefaultMax")
};
