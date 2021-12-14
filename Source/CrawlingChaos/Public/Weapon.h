// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Enums/AmmoType.h"
#include "Enums/DamageMode.h"
#include "Enums/EItemState.h"
#include "Enums/FireMode.h"
#include "Enums/WeaponType.h"
#include "Item.h"

#include "Weapon.generated.h"

// Forward declarations
class USoundCue;
class ACrawlingChaosProjectile;
class UNiagaraSystem;
class ACrawlingChaosCharacter;

/** Weapon data table struct for ease of adding new weapons */
USTRUCT()
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

	/** Name of the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	/** Type of the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType;

	/** How does the weapon propagate damage? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamageMode DamageMode;
	
	/** Offset used to position the gun properly on the screen */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector GunOffset;

	/** Type of ammo used */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType AmmoType;

	/** Amount of ammo you get when picking up the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponAmmo;

	/** Number of shots per trigger pull */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfShots;

	/** Weapon spread in the horizontal direction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HorizontalSpread;

	/** Weapon spread in the vertical direction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 VerticalSpread;

	/** Fire mode; i.e. full-auto, semi-auto, burst, etc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFireMode FireMode;

	/** Rate of fire in Rounds Per Minute */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoFireRate;

	/** Item mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* ItemMesh;

	/** Texture for the item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* MaterialInstance;

	/** Particle system for the muzzle flash */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* MuzzleFlash;

	/** Type of projectile to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ACrawlingChaosProjectile> Projectile;

	// todo: this will probably just be a montage with a bunch of sections tbh, not a blueprint
	/** Animation blueprint for pickup, swap, fire, reload, etc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimBP;

	// todo: I probably won't need this
	/** Icon used in the inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* InventoryIcon;

	/** Weapon fire sound */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* FireSound;

	/** Sound played on pickup */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* PickupSound;

	/** Sound played on equip */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* HitParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* TracerParticleSystem;
};

UCLASS()
class CRAWLINGCHAOS_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void TraceForHitsAndSpawnAttacks(UWorld* World) const;

	/** Fire the weapon */
	void OnFire();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	/** Called when the area sphere is overlapped */
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	/** Set the item properties based on the current item state. Once the item is picked up,
	*  we don't have to worry about it going /back/ to the pickup state. It's stuck in the inventory */
	void SetItemProperties(EItemState NewItemState);

	/** Perform line traces required for weapon fire */
	FHitResult LineTraceForWeaponFire(const UWorld* World, FVector& MuzzleLocation, FRotator& ProjectileRotation) const;

	/** Spawn weapon projectile (if not hitscan) */
	void SpawnProjectile(UWorld* World, FVector MuzzleLocation, FRotator ProjectileRotation,
						 ACrawlingChaosCharacter* Character) const;

	/** Reset the fire timer if full auto */
	void AutoFireReset();
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Getters
	
	/** Get the projectile class to spawn */
	TSubclassOf<ACrawlingChaosProjectile> GetProjectileClass() const
	{
		return ProjectileClass;
	}

	/** Get the weapon fire sound */
	USoundBase* GetFireSound() const
	{
		return FireSound;
	}

	/** Get the weapon fire animation */
	UAnimMontage* GetFireAnimation() const
	{
		return FireAnimation;
	}

	/** Get the item mesh */
	USkeletalMeshComponent *GetItemMesh() const
	{
		return ItemMesh;
	}

	/** Get the weapon ammo type */
	EAmmoType GetAmmoType() const
	{
		return AmmoType;
	}

	// todo: can probs rename this
	/** Get the ammo per weapon pickup */
	int32 GetWeaponAmmo() const
	{
		return WeaponAmmo;
	}

	/** Get the mode of weapon fire (burst, full-auto, etc.)*/
	EFireMode GetFireMode() const
	{
		return FireMode;
	}

	/** Get the current state of the item */
	EItemState GetItemState() const
	{
		return ItemState;
	}

	/** Get the type of weapon */
	EWeaponType GetWeaponType() const
	{
		return WeaponType;
	}

	/** Get the mode of damage (projectile, hitscan) */
	EDamageMode GetDamageMode() const
	{
		return DamageMode;
	}

	/** Get the Rate of Fire stored in the data table, convert to seconds per round and return */
	float GetRateOfFire() const;

	/** Set the new item state */
	void SetItemState(EItemState NewItemState);

	/** Set the new owner of this weapon */
	void SetPlayer(ACrawlingChaosCharacter* NewOwner)
	{
		this->Player = NewOwner; 
	}

	void SetStartFiring(const bool bFire)
	{
		bStartFiring = bFire;
	}
private:
	/** Item's current state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	EItemState ItemState;
	
	// todo: fix up the uproperty macros
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	TSubclassOf<class ACrawlingChaosProjectile> ProjectileClass;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = true))
	UAnimMontage* FireAnimation;

	/** Type of the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = true))
	EWeaponType WeaponType;

	/** How the weapon propagates damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = true))
	EDamageMode DamageMode;
	
	/** Name of the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = true))
	FString ItemName;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = true))
	FVector GunOffset;

	/** Type of ammo used */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = true))
	EAmmoType AmmoType;

	/** Amount of ammo you get when picking up the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = true))
	int32 WeaponAmmo;

	/** Number of shots per trigger pull */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = true))
	int32 NumberOfShots;

	/** Fire mode; i.e. full-auto, semi-auto, burst, etc. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat Stats", meta = (AllowPrivateAccess = "true"))
	EFireMode FireMode;
	
	/** Rate of fire in Rounds Per Minute */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat Stats", meta = (AllowPrivateAccess = "true"))
	float AutoFireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = true))
	USkeletalMesh* Mesh;
	
	/** Texture for the item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = true))
	UMaterialInstance* MaterialInstance;

	/** Particle system for the muzzle flash */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = true))
	UParticleSystem* MuzzleFlash;

	/** Animation blueprint for pickup, swap, fire, reload, etc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = true))
	TSubclassOf<UAnimInstance> AnimBP;

	/** Icon used in the inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = true))
	UTexture2D* InventoryIcon;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = true))
	USoundBase* FireSound;
	
	/** Sound played on pickup */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = true))
	USoundCue* PickupSound;

	/** Sound played on equip */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = true))
	USoundCue* EquipSound;

	/** Dynamic instance that can be changed at runtime */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicMaterialInstance;

	/** Particle system to spawn on hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* HitParticleSystem;

	/** Particle system to spawn on hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* TracerParticleSystem;

	/** Weapon spread in the horizontal direction */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat Stats", meta = (AllowPrivateAccess = "true"))
	int32 HorizontalSpread;

	/** Weapon spread in the vertical direction */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat Stats", meta = (AllowPrivateAccess = "true"))
	int32 VerticalSpread;

	/** Owner of the weapon */
	UPROPERTY()
	ACrawlingChaosCharacter* Player;
	
	/** Can the weapon fire currently? */
	UPROPERTY()
	bool bCanFire;

	/** Is the weapon now firing? */
	UPROPERTY()
	bool bStartFiring;

	/** Timer used to automatically reset and cause the weapon to be fully automatic */
	UPROPERTY()
	FTimerHandle WeaponFireTimer;
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Non-UPROPERTY class members

};
