// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CrawlingChaosHUD.generated.h"

UCLASS()
class ACrawlingChaosHUD : public AHUD
{
	GENERATED_BODY()

public:
	ACrawlingChaosHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

