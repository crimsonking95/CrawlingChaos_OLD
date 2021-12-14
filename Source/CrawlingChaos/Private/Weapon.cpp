// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "../CrawlingChaosCharacter.h"
#include "../CrawlingChaosProjectile.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"


// Sets default values
AWeapon::AWeapon() :
	AutoFireRate(.1f),
	bCanFire(true) // By default, you should be able to shoot the weapon
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// todo: going to need to port item state over
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Set the item properties
	SetItemProperties(EItemState::EIS_Pickup);
}

// Called when the actor is spawned, moved, or a property is changed
void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	const FString weaponTablePath{TEXT("DataTable'/Game/_Game/Weapons/DataTables/WeaponDataTable.WeaponDataTable'")};
	UDataTable* weaponTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(),nullptr,*weaponTablePath));

	if (weaponTableObject)
	{
		FWeaponDataTable* weaponDataRow = nullptr;
		switch (WeaponType)
		{
		case EWeaponType::EWT_Rifle:
			weaponDataRow = weaponTableObject->FindRow<FWeaponDataTable>(FName("Rifle"), TEXT(""));
			break;
		case EWeaponType::EWT_RocketLauncher:
			weaponDataRow = weaponTableObject->FindRow<FWeaponDataTable>(FName("RocketLauncher"), TEXT(""));
			break;
		case EWeaponType::EWT_Shotgun:
			weaponDataRow = weaponTableObject->FindRow<FWeaponDataTable>(FName("Shotgun"), TEXT(""));
			break;
		case EWeaponType::EWT_PlasmaGun:
			weaponDataRow = weaponTableObject->FindRow<FWeaponDataTable>(FName("PlasmaGun"), TEXT(""));
			break;
		default:
			break;
		}

		ItemName = weaponDataRow->ItemName;
		WeaponType = weaponDataRow->WeaponType;
		DamageMode = weaponDataRow->DamageMode;
		GunOffset = weaponDataRow->GunOffset;
		AmmoType = weaponDataRow->AmmoType;
		WeaponAmmo = weaponDataRow->WeaponAmmo;
		NumberOfShots = weaponDataRow->NumberOfShots;
		FireMode = weaponDataRow->FireMode;
		AutoFireRate = weaponDataRow->AutoFireRate;
		ItemMesh->SetSkeletalMesh(weaponDataRow->ItemMesh);
		MaterialInstance = weaponDataRow->MaterialInstance;
		MuzzleFlash = weaponDataRow->MuzzleFlash;
		ProjectileClass = weaponDataRow->Projectile;
		AnimBP = weaponDataRow->AnimBP;
		InventoryIcon = weaponDataRow->InventoryIcon;
		FireSound = weaponDataRow->FireSound;
		PickupSound = weaponDataRow->PickupSound;
		EquipSound = weaponDataRow->EquipSound;
		HitParticleSystem = weaponDataRow->HitParticleSystem;
		HorizontalSpread = weaponDataRow->HorizontalSpread;
		VerticalSpread = weaponDataRow->VerticalSpread;
		TracerParticleSystem = weaponDataRow->TracerParticleSystem;
		
		if (MaterialInstance)
		{
			DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
			GetItemMesh()->SetMaterial(0, DynamicMaterialInstance);
		}
	}
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		const auto Actor = Cast<ACrawlingChaosCharacter>(OtherActor);
		if (Actor)
		{
			bool bShouldDestroy = true;
			Actor->AddAmmoOfType(AmmoType, WeaponAmmo);

			if (!Actor->AlreadyHasWeapon(WeaponType))
			{
				bShouldDestroy = false;
				Actor->AddWeaponToInventory(this);
			}

			if (bShouldDestroy)
			{
				Destroy();
			}
		}
	}
}

float AWeapon::GetRateOfFire() const
{	
	/* Rate of Fire in RPM => Seconds per Round
	 *
	 * (rpm) * 1/60 (m/s) = rounds per second  
	 * 1 / (r/s) = X seconds per round
	 */
	return UKismetMathLibrary::SafeDivide(1.f, (AutoFireRate * (1.f/60.f)));
}

void AWeapon::SetItemState(EItemState NewItemState)
{
	ItemState = NewItemState;
	SetItemProperties(NewItemState);
}

void AWeapon::SetItemProperties(EItemState NewItemState)
{
	switch (NewItemState)
	{
	case EItemState::EIS_Pickup:
		// We want to be able to see the mesh, and overlap, but that's it
		ItemMesh->SetOnlyOwnerSee(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		bCanOscillate = true;
		break;
	case EItemState::EIS_PickedUp:
		// No collision, can't see it, etc.
		ItemMesh->SetOnlyOwnerSee(false);
		ItemMesh->SetVisibility(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		bCanOscillate = false;
		break;
		
	case EItemState::EIS_Equipped:
		// No shadows, but we can see the mesh and not collide with it
		ItemMesh->SetOnlyOwnerSee(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->CastShadow = false;
		ItemMesh->bCastDynamicShadow = false;
		
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		bCanOscillate = false;
		break;

	default:
		// Hide the mesh, disable physics and collision
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::TraceForHitsAndSpawnAttacks(UWorld* const World) const
{
	FVector MuzzleLocation;
	FRotator ProjectileRotation;
	const FHitResult HitResult = LineTraceForWeaponFire(World, MuzzleLocation, ProjectileRotation);

	if (ProjectileClass != nullptr && DamageMode == EDamageMode::EDM_PROJECTILE)
	{
		SpawnProjectile(World, MuzzleLocation, ProjectileRotation, Player);
	}
	else
	{
		// todo: add surface specific effects here
		if (HitResult.bBlockingHit && HitParticleSystem)
		{
			// todo: spawn a projectile that the tracer particle is attached to, so you can see the bullet
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitParticleSystem, HitResult.Location);
			
			if (TracerParticleSystem != nullptr)
			{
				const bool bShouldSpawnTracer = FMath::RandRange(0, 3) == 2;
				if (bShouldSpawnTracer)
				{
					const FVector BulletDirection{UKismetMathLibrary::GetDirectionUnitVector(MuzzleLocation, HitResult.Location)};
					const float TracerSpawnMultiplier = FMath::RandRange(30, 400);
					const FVector Start{MuzzleLocation + BulletDirection*TracerSpawnMultiplier};
					const FVector End{MuzzleLocation + BulletDirection*(TracerSpawnMultiplier + 150)};
				
					const auto Tracer = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TracerParticleSystem, Start);
					Tracer->SetNiagaraVariableVec3(FString{"BeamEnd"}, End);
				}
			}
			if (HitResult.GetActor()) 
			{
				if(HitResult.GetActor()->IsRootComponentMovable()) {
					const FVector CameraForward{Player->GetActorForwardVector()};
					UStaticMeshComponent* MeshRootComp = Cast<UStaticMeshComponent>(HitResult.GetActor()->GetRootComponent());

					
					MeshRootComp->AddForceAtLocation(CameraForward*25000*MeshRootComp->GetMass(), HitResult.Location);
				}
			}
		}
	}
}

void AWeapon::OnFire()
{
	if (!bStartFiring || !bCanFire) return;
	if (ItemState != EItemState::EIS_Equipped) return; // sanity check
	if (Player == nullptr) return;
	if (Player->GetAmmo(AmmoType) <= 0) return;
	
	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		// Set this to false, it'll only reset if the timer pops. Prevents the player from doing annoying
		// cheese like scroll-wheel shooting
		bCanFire = false;

		size_t i = 0;
		do
		{
			TraceForHitsAndSpawnAttacks(World);
			i++;
		} while (i < NumberOfShots);
		

		Player->DecrementInventoryValue(AmmoType, NumberOfShots);

		// try and play the sound if specified
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			Player->PlayWeaponFireAnimation(FireAnimation);
		}

		// Timer for automatic fire and/rate of fire enforcement
		GetWorld()->GetTimerManager().SetTimer(WeaponFireTimer,
			this,
			&AWeapon::AutoFireReset,
			GetRateOfFire());
	}
}

FHitResult AWeapon::LineTraceForWeaponFire(const UWorld* World, FVector& MuzzleLocation, FRotator& ProjectileRotation) const
{
	MuzzleLocation = ItemMesh->GetSocketLocation("Muzzle");
	FVector2D ViewPort;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewPort);
	}

	// Get our screen center in world coordinates
	float spreadX = FMath::RandRange(-HorizontalSpread, HorizontalSpread);
	float spreadY = FMath::RandRange(-VerticalSpread, VerticalSpread);
	const FVector2D ScreenCenter{ (ViewPort.X / 2) + spreadX, (ViewPort.Y / 2) + spreadY };
	FVector WorldPosition;
	FVector WorldDirection;
	const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
											 ScreenCenter, WorldPosition, WorldDirection);
	if (bScreenToWorld)
	{
		// Start from the center in world coordinates and go out by 50'000
		const FVector TraceStart{WorldPosition};
		const FVector TraceEnd{WorldPosition + (WorldDirection * (50'000))};
		FVector Location = TraceEnd;
		FHitResult HitResult;

		// See if our line trace finds a hit
		World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd,
										ECollisionChannel::ECC_Visibility);
		if (HitResult.bBlockingHit)
		{
			Location = HitResult.Location;
		}

		// Now see if anything is in the way of the trace when it's from muzzle to the original hit
		FHitResult MuzzleTraceHit;
		const FVector TraceEndWithMuzzleLength{ Location - MuzzleLocation };
		const FVector MuzzleToEnd{ MuzzleLocation + TraceEndWithMuzzleLength * 1.25f };
		World->LineTraceSingleByChannel(MuzzleTraceHit, MuzzleLocation,
										MuzzleToEnd, ECollisionChannel::ECC_Visibility);
		if (MuzzleTraceHit.bBlockingHit)
		{
			Location = MuzzleTraceHit.Location;
		}

		ProjectileRotation = UKismetMathLibrary::FindLookAtRotation(MuzzleLocation, Location);
		return MuzzleTraceHit;
	}
	// Return an empty hit result
	return FHitResult{};
}

void AWeapon::SpawnProjectile(UWorld* const World, const FVector MuzzleLocation, const FRotator ProjectileRotation, ACrawlingChaosCharacter* Character) const
{
	// Set Spawn Collision Handling Override
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	ActorSpawnParams.Owner = Character;
			
	// Spawn the projectile at the muzzle
	const auto Projectile = World->SpawnActor<ACrawlingChaosProjectile>(ProjectileClass, MuzzleLocation, ProjectileRotation,
																	ActorSpawnParams);

	if (Character)
	{
		Character->GetCapsuleComponent()->IgnoreActorWhenMoving(Projectile, true);
	}
}

void AWeapon::AutoFireReset()
{
	bCanFire = true;
	if (bStartFiring && FireMode == EFireMode::EFM_FullAuto && Player->GetAmmo(AmmoType) > 0)
	{
		OnFire();
	}
}
