// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2DDynamic.h"

#include "EnumItemTypes.h"
#include "ItemThumbnailInfo.h"

#include "InventoryItemInfo.generated.h"

//Contains basic information about items
//Stored in ItemObject
USTRUCT(BlueprintType)
struct PROTOGAME_API FInventoryItemInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	//TODO: not really used; generic ID for all same items
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = -1))
	int32 ItemID;

	//TODO: implement if required; unique ID of an item instance
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = -1))
	//int64 ItemUniqueID;

	//Isn't really used right now
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ItemType Type = ItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FText Name = FText::FromString("DefaultName");

	//Note that item comparison and stacking is dependent on NameShort
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FText NameShort = FText::FromString("DefShort");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", MultiLine = true))
	FText Description = FText::FromString("DefaultDescription");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FText UseActionText = FText::FromString("Use");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = 1, ClampMax = 500))
	FIntPoint Dimensions {1, 1};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = 1))
	int32 CurrentStackSize = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = 1))
	int32 MaxStackSize = 1;

	constexpr static double MassMaxPrecision = 1.0E-4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = 0))
	float Mass = 0.001;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = 0))
	float BasePrice = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UTexture2DDynamic> Thumbnail;
};

//Contains basic information about Item,
//also ItemThumbnailInfo which isn't to be included into ItemObject or ItemActor since we access this information infrequently. 
//So it is stored in data tables only.
USTRUCT(BlueprintType)
struct PROTOGAME_API FItemTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FInventoryItemInfo inventory_item_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FItemThumbnailInfo item_thumbnail_info;
};