#pragma once

UENUM(BlueprintType)
enum class EItemState : uint8 
{
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),

	EIS_MAX UMETA(DisplayName = "DefaultMAX")
};