// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrawlingChaosProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

ACrawlingChaosProjectile::ACrawlingChaosProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ACrawlingChaosProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// todo: make projectiles move at current speed + projectile speed
	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 8000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void ACrawlingChaosProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}

void ACrawlingChaosProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
}
