// Nikita Belov, All rights reserved

#pragma once

#include "Engine/DataTable.h"
#include "InventoryItemInfo.h"

#include "AmmoInfo.generated.h"

class AProjectile;

UENUM(BlueprintType)
enum class AmmoCaliber : uint8
{
	None = 0				UMETA(DisplayName = "None"),
	//rifle
	a5_56_45		 		UMETA(DisplayName = "5.56x45"),
	a5_45_39		 		UMETA(DisplayName = "5.45x39"),
	a7_62_39				UMETA(DisplayName = "7.62x39"),
	a7_62_51				UMETA(DisplayName = "7.62x51"),
	a6_8_51					UMETA(DisplayName = "6.8x51"),
	a9_39					UMETA(DisplayName = "9x39"),
	//pistol
	a9_19					UMETA(DisplayName = "9x19"),
	a45_acp					UMETA(DisplayName = ".45 ACP"),
	a5_7_28					UMETA(DisplayName = "5.7x28"),
	//heavy mg
	a12_7_99				UMETA(DisplayName = "12.7x99"),

};

USTRUCT(BlueprintType)
struct PROTOGAME_API FAmmoInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<AProjectile> projectile_class;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	AmmoCaliber caliber;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float initial_velocity;
	
	//TODO: implement
	//Either this or drop-off curve
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	//float air_resistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float penetration;

	//angle when maximum ricochet chance is achieved
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float ricochet_angle;
};

USTRUCT(BlueprintType)
struct PROTOGAME_API FAmmoTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FInventoryItemInfo inventory_item_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FItemThumbnailInfo item_thumbnail_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FAmmoInfo ammo_info;
};