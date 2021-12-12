// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrawlingChaosGameMode.h"
#include "CrawlingChaosHUD.h"
#include "CrawlingChaosCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACrawlingChaosGameMode::ACrawlingChaosGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/_Game/Character/BP_CrawlingChaosCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ACrawlingChaosHUD::StaticClass();
}
