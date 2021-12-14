#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EWT_Rifle UMETA(DisplayName = "Rifle"),
	EWT_PlasmaGun UMETA(DisplayName = "Plasma Rifle"),


	EWT_DefaultMAX
};