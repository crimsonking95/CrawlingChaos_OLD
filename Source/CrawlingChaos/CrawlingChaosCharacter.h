// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enums/AmmoType.h"
#include "Enums/WeaponType.h"
#include "GameFramework/Character.h"

#include "CrawlingChaosCharacter.generated.h"


class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;
class AWeapon;
class UNiagaraSystem;

UCLASS(config=Game)
class ACrawlingChaosCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACrawlingChaosCharacter();

protected:
	/** Begin Play override */
	virtual void BeginPlay() override;

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	
	/** Called when the fire button is pressed */
	void PrimaryFireButtonPressed();

	/** Called when the fire button is released */
	void PrimaryFireButtonReleased();

	/** Called when equipping a weapon */
	void EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping = false);

	/** Swap the current weapon with the intended one */
	void SwapWeapons(EWeaponType WeaponTypeToSwap);

	/** Equip the weapon in the first slot */
	void WeaponOneEquip();

	/** Equip the weapon in the second slot */
	void WeaponTwoEquip();

	/** Equip the weapon in the third slot */
	void WeaponThreeEquip();
	void WeaponFourEquip();
private:
	bool bCanFire;
	
	FTimerHandle WeaponFireTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	TSubclassOf<AWeapon> DefaultWeaponClass;
	
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Ammo map */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;
	
	uint32 StartingAmmoVal;

	/** Inventory of weapons the character is currently holding */
	UPROPERTY()
	TMap<EWeaponType, AWeapon*> WeaponInventory;
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Getters
	
	/** Returns Mesh1P sub-object **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	
	/** Returns FirstPersonCameraComponent sub-object **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	int32 GetAmmo(EAmmoType AmmoType)
	{
		if (AmmoMap.Contains(AmmoType))
		{
			return AmmoMap[AmmoType];
		}
		return 0;
	}

	/** Returns true if the player already has the weapon of that type, or false if not */
	bool AlreadyHasWeapon(EWeaponType WeaponType) const
	{
		return WeaponInventory.Contains(WeaponType);
	};

	void AddWeaponToInventory(AWeapon* WeaponToAdd);

	void AddAmmoOfType(EAmmoType AmmoType, int32 AmmoAmount) 
	{
		if (AmmoMap.Contains(AmmoType))
		{
			AmmoMap[AmmoType] += AmmoAmount;
		}
		else
		{
			AmmoMap.Add(AmmoType, AmmoAmount);
		}
	}

	/** Decrement the AmmoType by the input value Amount */
	void DecrementInventoryValue(EAmmoType Type, int32 Amount);

	void PlayWeaponFireAnimation(UAnimMontage* AnimMontage) const;
};

