// Nikita Belov, All rights reserved

#pragma once

#include "InventoryItemInfo.h"
#include "AttachmentInfo.h"
#include "ItemThumbnailInfo.h"
#include "Effects/EffectBase.h"


#include "Engine/DataAsset.h"

#include "WeaponInfo.generated.h"

class UAnimInstance;
class UAnimMontage;
class AProjectile;
class UWeaponAttachment;
struct FWeaponGunActorInfo;
struct FWeaponGunInfo;

UENUM(BlueprintType)
enum class EWeaponFireMode : uint8
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

//Since different skeletal meshes have different bone names
//We explicitly tell AnimInsance what is what
USTRUCT(BlueprintType)
struct PROTOGAME_API FGunBoneNames : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bones")
	FName MuzzleEnd = "muzzle_end";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bones")
	FName MuzzleDevice = "muzzle_device";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bones")
	FName GunBarrel = "muzzle_end";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bones")
	FName Magazine = "magazine";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bones")
	FName MagRelease = "mag_release";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bones")
	FName ChargingHandle = "charging_handle";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bones")
	FName Bolt = "bolt";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bones")
	FName BoltCover = "bolt_cover";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bones")
	FName BoltRelease = "bolt_release";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bones")
	FName FireModeSwitch = "fire_mode_switch";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bones")
	FName Stock = "stock";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bones")
	FName SafetySwitch = "safety_switch";

};

//It should contain only weapon logic values
USTRUCT(BlueprintType)
struct PROTOGAME_API FWeaponGunInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	//TSoftObjectPtr<USkeletalMesh> weapon_mesh;

	//Ideally guns will be made out of parts, so by default they aren't functional
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bFunctional = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = WeaponSpecifications)
	bool bHasChamber = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = WeaponSpecifications)
	UAmmoBase* Chamber;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = WeaponSpecifications)
	TArray<EWeaponFireMode> FireModesAvailable { EWeaponFireMode::Single };

	//Fire mode of a gun
	//By default single fire, but it's not always available
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = WeaponSpecifications)
	EWeaponFireMode FireMode = FireModesAvailable[0];

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = WeaponSpecifications)
	FGameplayTag Caliber;

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
	bool bReloading = false;

	//Combined. Mag in and mag out. Pulling charging handle or other additional manipulations aren't included.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 300), Category = Reload)
	float ReloadTime = 3;

	//I.e. if a gun is fully empty, ReloadTime and ChargingHandleTime are combined
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 300), Category = Reload)
	float ChargingHandleTime = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 300), Category = Reload)
	float LoadAmmoStraightIntoChamberTime = 3;
	//

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 1200), Category = Handling)
	float DrawSpeed = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 1200), Category = Handling)
	float HolsterSpeed = 1;


	//Absolute nubmer. So players can easily understand which weapon is more durable
	//Default value is 500
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, ClampMax = 10000), Category = Durability)
	float Durability = 500;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0), Category = Durability)
	float Heat = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0), Category = Durability)
	float HeatingSpeed = 1;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0), Category = Handling)
	//float Length;
};

USTRUCT(BlueprintType)
struct PROTOGAME_API FWeaponGunActorInfo : public FTableRowBase
{
	GENERATED_BODY()

	//Animations

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	TSubclassOf<UAnimInstance> AnimClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	FGunBoneNames gun_bone_names;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	UAnimMontage* HoldAnimChar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	UAnimMontage* AimDownSightsChar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadChar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadGun;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	UAnimMontage* ChargingHandleChar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = true))
	UAnimMontage* ChargingHandleGun;

	//Sounds

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = true))
	USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = true))
	USoundBase* DryFireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = true))
	USoundBase* CasingDropSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = true))
	USoundBase* FireSelectorSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = true))
	USoundBase* MagInSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = true))
	USoundBase* MagOutSound;

	//Effects

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = true))
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = true))
	TSubclassOf<AEffectBase> MuzzleLight;
};

UCLASS(BlueprintType)
class PROTOGAME_API UFWeaponGunActorData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FWeaponGunActorInfo gun_actor_info;
};


//Data tables only
//Represents only base weapon
USTRUCT(BlueprintType)
struct PROTOGAME_API FWeaponGunTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FInventoryItemInfo inventory_item_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FWeaponGunInfo weapon_gun_info;

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