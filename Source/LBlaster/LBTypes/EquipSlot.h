#pragma once

UENUM(BlueprintType)
enum class EEquipSlot : uint8
{
	EES_FirstSlot UMETA(DisplayName = "First Slot"),
	EES_SecondSlot UMETA(DisplayName = "Second Slot"),
	EES_ThirdSlot UMETA(DisplayName = "Third Slot"),
	EES_MAX UMETA(DisplayName = "DefaultMax")
};

inline const TCHAR* LexToString(EEquipSlot InType)
{
	switch (InType)
	{
	case EEquipSlot::EES_FirstSlot:
		return TEXT("EES_FirstSlot");
	case EEquipSlot::EES_SecondSlot:
		return TEXT("EES_SecondSlot");
	case EEquipSlot::EES_ThirdSlot:
		return TEXT("EES_ThirdSlot");
	case EEquipSlot::EES_MAX:
		return TEXT("EES_MAX");
	}
	return TEXT("Unknown");
}
