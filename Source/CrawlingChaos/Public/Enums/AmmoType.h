#pragma once

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_Pistol UMETA(DisplayName = "Pistol"),
	EAT_Rifle UMETA(DisplayName = "Rifle"),
	EAT_Shotgun UMETA(DisplayName = "Shotgun"),
	EAT_Plasma UMETA(DisplayName = "Plasma"),
	EAT_Rocket UMETA(DisplayName = "Rocket"),
	EAT_Grenade UMETA(DisplayName = "Grenade"),

	EAT_MAX UMETA(DisplayName = "DefaultMax")
};