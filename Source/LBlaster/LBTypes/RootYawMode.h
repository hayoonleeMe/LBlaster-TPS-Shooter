#pragma once

UENUM(BlueprintType)
enum class ERootYawOffsetMode : uint8
{
	ERM_BlendOut UMETA(DisplayName = "BlendOut"),
	ERM_Hold UMETA(DisplayName = "Hold"),
	ERM_Accumulate UMETA(DisplayName = "Accumulate"),
	ERM_MAX UMETA(DisplayName = "DefaultMax")
};