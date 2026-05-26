// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2DDynamic.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"

#include "ItemThumbnailInfo.h"
#include "ItemFragment.h"

#include "InventoryItemInfo.generated.h"

//Contains basic information about items
//Stored in ItemObject
//Do NOT place DataTable only values here
USTRUCT(BlueprintType)
struct PROTOGAME_API FInventoryItemInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FText Name = FText::FromString("DefaultName");

	//Displayed when UI space is limited
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FText NameShort = FText::FromString("DefShort");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", MultiLine = true))
	FText Description = FText::FromString("DefaultDescription");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FText UseActionText = FText::FromString("Use");

	//Has to be 1 or more, other values are INVALID
	//Max value isn't fixed, 100000 is chosen arbitrarily, int32 Max is the hard limit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = 1, ClampMax = 100000))
	FIntPoint Dimensions {1, 1};

	//Has to be 1 or more, other values are INVALID and will likely trigger item instance destruction
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = 1))
	int32 CurrentStackSize = 1;

	//IsStackable: MaxStackSize > 1; There is no separate bool for this
	//Has to be 1 or more, other values are INVALID and will likely trigger item instance destruction
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = 1))
	int32 MaxStackSize = 1;

	//how many decimal places
	constexpr static double MassMaxPrecision = 4;

	//in kg, only MassMaxPrecision decimal places are relevant, everything else is rounded
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = 0))
	float Mass = 0.001;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = 0))
	float BasePrice = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UTexture2DDynamic> Thumbnail;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Item", AllowPrivateAccess = "true"))
	FGameplayTag Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer Tags;

	//Fragment is any dynamically added value
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//TArray<UItemFragment*> ItemFragments;
};

//Contains basic information about Item,
//also ItemThumbnailInfo which isn't to be included into ItemObject or ItemActor since we access this information infrequently. 
//So it is stored in data tables only.
USTRUCT(BlueprintType)
struct PROTOGAME_API FItemTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FInventoryItemInfo inventory_item_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FItemThumbnailInfo item_thumbnail_info;

	//Might be useful
	//As far as I can tell (item)Fragment is a dynamically added value + logic (class)
	//List of fragments added to Item by default from DT
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowAbstract = "false", AllowPrivateAccess = "true"))
	//TArray<TSubclassOf<UItemFragment>> ItemDefalutFragments;
};

USTRUCT(BlueprintType)
struct PROTOGAME_API FItemInfoInstanced : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FInventoryItemInfo inventory_item_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FItemThumbnailInfo item_thumbnail_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FInstancedStruct> intstanced_structs;
};