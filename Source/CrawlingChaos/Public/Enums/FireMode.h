#pragma once

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	EFM_Semi UMETA(DisplayName = "Semi-Automatic"),
	EFM_Burst UMETA(DisplayName = "Burst"), // todo: handle burst weapons
	EFM_FullAuto UMETA(DisplayName = "Fully-Automatic"),

	EFM_MAX UMETA(DisplayName = "DefaultMax")
};