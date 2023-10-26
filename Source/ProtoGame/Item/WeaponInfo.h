// Nikita Belov, All rights reserved

#pragma once

#include "InventoryItemInfo.h"
#include "AttachmentInfo.h"
#include "ItemThumbnailInfo.h"
#include "Effects/EffectBase.h"

#include "WeaponInfo.generated.h"

class UAnimInstance;
class AProjectile;
class UWeaponAttachment;

UENUM(BlueprintType)
enum class WeaponFireMode : uint8
{
	//safe is for special occasions only, by default it's redundant
	Safe = 0			UMETA(DisplayName = "Safe"),

	Single = 1			UMETA(DisplayName = "Single"),
	Auto				UMETA(DisplayName = "Auto"),
	AutoSlow			UMETA(DisplayName = "AutoSlow"),
	TwoRoundBurst		UMETA(DisplayName = "Two round burst"),
	ThreeRoundBurst     UMETA(DisplayName = "Three round burst"),
	FourRoundBurst		UMETA(DisplayName = "Four round burst"),
	FiveRoundBurst      UMETA(DisplayName = "Five round burst"),
	SpecialMode1        UMETA(DisplayName = "Special mode 1"),
	SpecialMode2        UMETA(DisplayName = "Special mode 2"),
	SpecialMode3        UMETA(DisplayName = "Special mode 3"),
};

USTRUCT(BlueprintType)
struct PROTOGAME_API FWeaponInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	//TSoftObjectPtr<USkeletalMesh> weapon_mesh;

	//Animations

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	TSubclassOf<UAnimInstance> AnimClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	UAnimMontage* HoldAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	UAnimMontage* AimDownSights;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadGun;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	UAnimMontage* ChargingHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	UAnimMontage* ChargingHandleGun;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	TSubclassOf<AEffectBase> MuzzleLight;
	//

	//Sounds, effects

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = true))
	USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = true))
	USoundBase* EmptyClickSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = true))
	USoundBase* CasingSound;
	//

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bFunctional = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = WeaponSpecifications)
	bool bHasChamber = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = WeaponSpecifications)
	UAmmoBase* Chamber;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = WeaponSpecifications)
	TArray<WeaponFireMode> FireModesAvailable {WeaponFireMode::Single};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = WeaponSpecifications)
	WeaponFireMode FireMode = FireModesAvailable[0];

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = WeaponSpecifications)
	AmmoCaliber Caliber;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = WeaponSpecifications)
	//TSubclassOf<AProjectile> AmmoType;

	//Fire rate threshold is 1000 RPM 
	//TODO: after the threshold multiple bullets better be spawned per each Fire call
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0.01, ClampMax = 1000), Category = WeaponSpecifications)
	float FireRate = 50;

	//Vertical muzzle climb
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 1500), Category = WeaponSpecifications)
	float VerticalRecoil;

	//Horizontal muzzle movement
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 1500), Category = WeaponSpecifications)
	float HorizontalRecoil;

	//Screen shake
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 1, ClampMax = 1500), Category = WeaponSpecifications)
	float PerceivedRecoil;

	//How muzzle (and sight respectively) shakes in characters hands 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 1500), Category = WeaponSpecifications)
	float GunSway;

	//Now in artificial units 
	//TODO: Must be in MOA
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 1000), Category = WeaponSpecifications)
	float Dispersion;

	//ADS

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 75), Category = ADS)
	float DistanceFromSight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 3000), Category = ADS)
	float ADS_InAnimTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 3000), Category = ADS)
	float ADS_OutAnimTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 1200), Category = ADS)
	float ADS_InInterpSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 1200), Category = ADS)
	float ADS_OutInterpSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 1200), Category = ADS)
	float ADS_AccumulatedAnimationTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 1200), Category = ADS)
	float ADS_VertSway;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 1200), Category = ADS)
	float ADS_HorSway;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = ADS)
	bool ADS_ResetNeeded;
	//

	//Reload

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = Reload)
	bool bReloading;

	//Combined. Mag in and mag out. Pulling charging handle or other additional manipulations aren't included.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 300), Category = Reload)
	float ReloadTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 300), Category = Reload)
	float ChargingHandleTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 300), Category = Reload)
	float LoadAmmoStraightIntoChamberTime;
	//

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 1200), Category = Handling)
	float DrawSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 1200), Category = Handling)
	float HolsterSpeed;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0), Category = Handling)
	//float Length;

	//It's not percentage. Each gun must have absolute nubmer, so players can easily understand which weapon is more durable
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 10000), Category = Durability)
	float Durability;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0), Category = Durability)
	float Heat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0), Category = Durability)
	float HeatingSpeed;
};

//Data tables only
//Represents only base weapon
USTRUCT(BlueprintType)
struct PROTOGAME_API FWeaponTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FInventoryItemInfo inventory_item_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FWeaponInfo weapon_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TArray<FAttachmentSlot> attachment_slots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FItemThumbnailInfo item_thumbnail_info;
};

//Data tables only
//Allows making weapons presets with attachments included
USTRUCT(BlueprintType)
struct PROTOGAME_API FWeaponPresetTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FDataTableRowHandle weapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TArray<FDataTableRowHandle> attachment_preset;
};