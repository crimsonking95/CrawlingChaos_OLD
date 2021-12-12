// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class CRAWLINGCHAOS_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Oscillate the item in place while it's on the map and not picked up*/
	void OscillateInPlace();

	/** Callback that resets the oscillate timer */
	void OscillateCallback();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetCanOscillate(bool bShouldOscillate) { bCanOscillate = bShouldOscillate; };
	bool GetCanOscillate() const { return bCanOscillate; };

	void Equip();
protected:
	/** Item mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* ItemMesh;

	/** Area sphere for overlapping item pickup */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess = true))
	class USphereComponent* AreaSphere;

	/** Item oscillation while on the map */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item, meta = (AllowPrivateAccess = true))
	UCurveFloat* OscCurve;
	FTimerHandle OscTimer;
	float OscCurveLength;
	FVector InitialLocation;
	bool bCanOscillate;
};
