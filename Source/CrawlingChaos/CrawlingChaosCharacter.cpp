// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrawlingChaosCharacter.h"
#include "CrawlingChaosProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMyProjectCharacter

ACrawlingChaosCharacter::ACrawlingChaosCharacter() :
	bCanFire(false),
	StartingAmmoVal(120)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->FieldOfView = 110;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	AmmoMap.Add(EAmmoType::EAT_Rifle, StartingAmmoVal);
	AmmoMap.Add(EAmmoType::EAT_Pistol, StartingAmmoVal);
	AmmoMap.Add(EAmmoType::EAT_Plasma, StartingAmmoVal);
	AmmoMap.Add(EAmmoType::EAT_Shotgun, StartingAmmoVal);
	AmmoMap.Add(EAmmoType::EAT_Rocket, StartingAmmoVal);
}

void ACrawlingChaosCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (DefaultWeaponClass)
	{
		const auto WeaponToAdd = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
		AddWeaponToInventory(WeaponToAdd);
		EquipWeapon(WeaponToAdd);
	}

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	Mesh1P->SetHiddenInGame(false, true);
}

/////////////////////////////////////////////////////////////////////////////
/// Weapon equipping/swapping

void ACrawlingChaosCharacter::WeaponOneEquip()
{
	SwapWeapons(EWeaponType::EWT_Rifle);
}

void ACrawlingChaosCharacter::WeaponTwoEquip()
{
	SwapWeapons(EWeaponType::EWT_RocketLauncher);
}

void ACrawlingChaosCharacter::WeaponThreeEquip()
{
	SwapWeapons(EWeaponType::EWT_Shotgun);
}

void ACrawlingChaosCharacter::WeaponFourEquip()
{
	SwapWeapons(EWeaponType::EWT_PlasmaGun);
}

void ACrawlingChaosCharacter::SwapWeapons(EWeaponType WeaponTypeToSwap)
{
	if (EquippedWeapon == nullptr)
	{
		// Equip it and return immediately
		EquipWeapon(WeaponInventory[WeaponTypeToSwap]);
		return;
	}
	
	if (!WeaponInventory.Contains(WeaponTypeToSwap)) return;
	if (WeaponTypeToSwap == EquippedWeapon->GetWeaponType()) return;

	if (EquippedWeapon != nullptr)
	{
		// Set item state back to picked up
		EquippedWeapon->SetItemState(EItemState::EIS_PickedUp);	
	}

	// Set the new Equipped weapon to the item in the map
	EquipWeapon(WeaponInventory[WeaponTypeToSwap]);
}

void ACrawlingChaosCharacter::EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping)
{
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	EquippedWeapon->GetItemMesh()->AttachToComponent(Mesh1P, 
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), 
		TEXT("GripPoint"));
}

void ACrawlingChaosCharacter::AddWeaponToInventory(AWeapon* WeaponToAdd)
{
	if (WeaponInventory.Contains(WeaponToAdd->GetWeaponType())) return;
	WeaponInventory.Add(WeaponToAdd->GetWeaponType(), WeaponToAdd);
	WeaponToAdd->SetItemState(EItemState::EIS_PickedUp);
	WeaponToAdd->SetPlayer(this);
} 


//////////////////////////////////////////////////////////////////////////
/// Input

void ACrawlingChaosCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACrawlingChaosCharacter::PrimaryFireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACrawlingChaosCharacter::PrimaryFireButtonReleased);

	PlayerInputComponent->BindAction("Weapon1", IE_Pressed, this, &ACrawlingChaosCharacter::WeaponOneEquip);
	PlayerInputComponent->BindAction("Weapon2", IE_Pressed, this, &ACrawlingChaosCharacter::WeaponTwoEquip);
	PlayerInputComponent->BindAction("Weapon3", IE_Pressed, this, &ACrawlingChaosCharacter::WeaponThreeEquip);
	PlayerInputComponent->BindAction("Weapon4", IE_Pressed, this, &ACrawlingChaosCharacter::WeaponFourEquip);
}

/////////////////////////////////////////////////////////////////////////
/// Weapon Fire

void ACrawlingChaosCharacter::DecrementInventoryValue(const EAmmoType Type, int32 Amount)
{
	if (EquippedWeapon->GetFireMode() == EFireMode::EFM_Burst)
	{
		for (size_t i = 0; i < Amount; i++)
		{
			if (AmmoMap[Type] != 0)
			{
				--AmmoMap[Type];
			}
		}
	}
	else
	{
		--AmmoMap[Type];
	}
	
}

void ACrawlingChaosCharacter::PlayWeaponFireAnimation(UAnimMontage* AnimMontage) const
{
	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
	if (AnimInstance != nullptr)
	{
		AnimInstance->Montage_Play(EquippedWeapon->GetFireAnimation(), 1.f);
	}
}

void ACrawlingChaosCharacter::PrimaryFireButtonPressed()
{
	// Avoid crashing the game lol
	if (EquippedWeapon == nullptr) return;
	if (AmmoMap[EquippedWeapon->GetAmmoType()] == 0) return;

	EquippedWeapon->SetStartFiring(true);
	EquippedWeapon->OnFire();
}

// Disabling this because you can't bind a button press function if it's const
// ReSharper disable once CppMemberFunctionMayBeConst
void ACrawlingChaosCharacter::PrimaryFireButtonReleased()
{
	EquippedWeapon->SetStartFiring(false);
}
