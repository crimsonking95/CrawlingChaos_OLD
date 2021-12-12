#pragma once

UENUM(BlueprintType)
enum class EDamageMode: uint8
{
	EDM_HITSCAN UMETA(DisplayName = "Hit Scan"),
	EDM_PROJECTILE UMETA(DisplayName = "Projectile"),

	EDM_DEFAULTMAX UMETA(DisplayName = "Default MAX")
};