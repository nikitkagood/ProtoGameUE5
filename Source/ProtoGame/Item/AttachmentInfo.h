// Nikita Belov, All rights reserved

#pragma once

#include "Engine/DataTable.h"
#include "InventoryItemInfo.h"
#include "AmmoInfo.h"
#include "ItemThumbnailInfo.h"
#include "AmmoBase.h"

#include "AttachmentInfo.generated.h"

class UAmmoBase;

//Naming convention: 
//Usually *PlatformName*_Compatible
//And *PlatformName*_Only, *PlatformName*_*WeaponName*_Only
//UENUM(BlueprintType)
//enum class AttachmentSlotClass : uint8
//{
//	None = 0				UMETA(DisplayName = "None"),
//	//Mounts
//	Picatinny				UMETA(DisplayName = "Picatinny"),
//	Dovetail				UMETA(DisplayName = "Dovetail"),
//	MLOK					UMETA(DisplayName = "MLOK"),
//	//AR-15 Platform
//	AR15_Compatible			UMETA(DisplayName = "AR15_Compatible"),
//	AR15_Only				UMETA(DisplayName = "AR15_Only"),
//	AR15_M4_Only			UMETA(DisplayName = "AR15_M4_Only"),
//	//AR-10 Platform
//	AR10_Compatible			UMETA(DisplayName = "AR10_Compatible"),
//	AR10_Only				UMETA(DisplayName = "AR10_Only"),
//	//AK Platform
//	AK_Compatible			UMETA(DisplayName = "AK_Compatible"),
//	AK_Only					UMETA(DisplayName = "AK_Only"),
//	AK_AK74_Only			UMETA(DisplayName = "AK_AK74_Only"),
//};

//UENUM(BlueprintType)
//enum class AttachmentSlotType : uint8
//{
//	None = 0					UMETA(DisplayName = "None"),
//	//
//	Magazine					UMETA(DisplayName = "Magazine"),
//	MagazineSpecial				UMETA(DisplayName = "MagazineSpecial"),
//	Belt						UMETA(DisplayName = "Belt"),
//	//sight or sight mount
//	SightBig					UMETA(DisplayName = "SightBig"),
//	SightMedium					UMETA(DisplayName = "SightMedium"),
//	SightSmall					UMETA(DisplayName = "SightSmall"),
//	SightSpecial				UMETA(DisplayName = "SightSpecial"),
//	//
//	TacticalDeviceBig			UMETA(DisplayName = "TacticalDeviceBig"),
//	TacticalDeviceSmall			UMETA(DisplayName = "TacticalDeviceSmall"),
//	TacticalDeviceSpecial		UMETA(DisplayName = "TacticalDeviceSpecial"),
//	//
//	TacticalGripBig				UMETA(DisplayName = "TacticalGripBig"),
//	TacticalGripSmall			UMETA(DisplayName = "TacticalGripSmall"),
//	TacticalGripSpecial			UMETA(DisplayName = "TacticalGripSpecial"),
//	//
//	TacticalDeviceOrGripBig		UMETA(DisplayName = "TacticalDeviceOrGripBig"),
//	TacticalDeviceOrGripSmall	UMETA(DisplayName = "TacticalDeviceOrGripSmall"),
//	TacticalDeviceOrGripSpecial	UMETA(DisplayName = "TacticalDeviceOrGripSpecial"),
//	//
//	Handguard					UMETA(DisplayName = "Handguard"),
//	HandguardSpecial			UMETA(DisplayName = "HandguardSpecial"),
//	//
//	Stock						UMETA(DisplayName = "Stock"),
//	StockSpecial				UMETA(DisplayName = "StockSpecial"),
//	//
//	MuzzleDevice				UMETA(DisplayName = "MuzzleDevice"),
//	MuzzleDeviceSpecial			UMETA(DisplayName = "MuzzleDeviceSpecial"),
//	//
//	GasTube						UMETA(DisplayName = "GasTube"),
//	GasTubeSpecial				UMETA(DisplayName = "GasTubeSpecial"),
//};

USTRUCT(BlueprintType)
struct PROTOGAME_API FAttachmentSlot : public FTableRowBase
{
	GENERATED_BODY()

	//For hashing system and TMap compatibility
	FAttachmentSlot() {};
	FAttachmentSlot(FGameplayTagContainer slot_class, FGameplayTag slot_type, int32 slot_number) : slot_compatibility(slot_class), slot_type(slot_type) {};
	//FAttachmentSlot(const FAttachmentSlot& other) : FAttachmentSlot(other.slot_compatibility, other.slot_type) {};

	bool operator == (const FAttachmentSlot& other) const
	{
		return Equals(other);
	}

	bool Equals(const FAttachmentSlot& other) const
	{
		return slot_compatibility == other.slot_compatibility
			&& slot_type == other.slot_type 
			/* && slot_number == other.slot_number*/;
	}

	//compatibility, like AK only, or picatinny, special sight mount etc
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, Categories = "Weapon.Attachment.Compat"))
	FGameplayTagContainer slot_compatibility;

	//like magazine, sight, handguard etc
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, Categories = "Weapon.Attachment.Type"))
	FGameplayTag slot_type;

	//Used to distinguish the same type slots and sockets
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	//int32 slot_number = 0;
};

FORCEINLINE uint32 GetTypeHash(const FAttachmentSlot& Thing)
{
	uint32 Hash = 0;

	Hash = HashCombineFast(Hash, GetTypeHash(Thing.slot_compatibility.GetGameplayTagArray()));
	Hash = HashCombineFast(Hash, GetTypeHash(Thing.slot_type));
	//Hash = HashCombineFast(Hash, GetTypeHash(Thing.slot_number));
	return Hash;
}

USTRUCT(BlueprintType)
struct PROTOGAME_API FAttachmentInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FAttachmentSlot slot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FName socket_name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TSoftObjectPtr<UStaticMesh> static_mesh;
};

USTRUCT(BlueprintType)
struct PROTOGAME_API FAttachmentMagazineInfo : public FTableRowBase
{
	GENERATED_BODY()

	FAttachmentMagazineInfo()
	{
		storage.Reserve(Capacity);
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FGameplayTag Caliber;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 Capacity = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<UAmmoBase*> storage;

	//per round
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float LoadTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float UnloadTime;
};

USTRUCT(BlueprintType)
struct PROTOGAME_API FAttachmentMagazineTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FInventoryItemInfo inventory_item_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FItemThumbnailInfo item_thumbnail_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FAttachmentInfo attachment_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FAttachmentMagazineInfo attachment_magazine_info;
};

USTRUCT(BlueprintType)
struct PROTOGAME_API FAttachmentSightInfo : public FTableRowBase
{
	GENERATED_BODY()

	//TODO: Might be redundant; Check whether it's possible to get this information from attached SM -> SM socket
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector aim_point;

	//SkeletalMesh
	//AnimBP
};

USTRUCT(BlueprintType)
struct PROTOGAME_API FAttachmentSightTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FInventoryItemInfo inventory_item_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FItemThumbnailInfo item_thumbnail_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FAttachmentInfo attachment_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FAttachmentSightInfo attachment_sight_info;
};