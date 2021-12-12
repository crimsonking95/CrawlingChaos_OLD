// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"


#include "Components/SphereComponent.h"

// Sets default values
AItem::AItem() :
		OscCurveLength(2.f),
		bCanOscillate(true)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);
	
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(ItemMesh);
	AreaSphere->SetRelativeLocation(FVector{0,0,0});
	AreaSphere->SetHiddenInGame(true);
	AreaSphere->SetVisibility(false);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	InitialLocation = FVector{ GetActorLocation() };

	if (bCanOscillate)
	{
		GetWorldTimerManager().SetTimer(OscTimer, this, &AItem::OscillateCallback, OscCurveLength);	
	}
}

void AItem::OscillateInPlace()
{
	if (OscCurve)
	{
		const FVector Height = FVector{ InitialLocation.X, InitialLocation.Y, InitialLocation.Z +
			50*OscCurve->GetFloatValue(GetWorldTimerManager().GetTimerElapsed(OscTimer))};
		this->SetActorLocation(Height);
	}
}

void AItem::OscillateCallback()
{
	if (bCanOscillate)
	{
		GetWorldTimerManager().SetTimer(OscTimer, this, &AItem::OscillateCallback, OscCurveLength);
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanOscillate)
	{
		OscillateInPlace();	
	}
}

void AItem::Equip()
{
	bCanOscillate = false;
}

