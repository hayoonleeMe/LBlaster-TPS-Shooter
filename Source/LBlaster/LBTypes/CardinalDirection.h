#pragma once

UENUM(BlueprintType)
enum class ECardinalDirection : uint8
{
    ECD_Forward UMETA(DisplayName = "Forward"),
    ECD_Backward UMETA(DisplayName = "Backward"),
    ECD_Left UMETA(DisplayName = "Left"),
    ECD_Right UMETA(DisplayName = "Right"),
    ECD_MAX UMETA(DisplayName = "DefaultMax")
};