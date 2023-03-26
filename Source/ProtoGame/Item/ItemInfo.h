// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "EnumItemTypes.h"

#include "Engine/DataTable.h"
#include "Engine/Texture2DDynamic.h"

#include "ItemInfo.generated.h"

USTRUCT(BlueprintType)
struct PROTOGAME_API FItemInfo : public FTableRowBase
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = 0))
	float Mass = 0.001;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = 0))
	float BasePrice = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UTexture2DDynamic> Thumbnail;
};

