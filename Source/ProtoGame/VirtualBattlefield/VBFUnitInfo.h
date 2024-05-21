// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "VBFUnitInfo.generated.h"

//class UVBFUnitBase;

USTRUCT(Blueprintable, BlueprintType)
struct PROTOGAME_API FVBFUnitInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FText DisplayName = FText::FromString("DefaultUnitDisplayName");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool IsEverCommandable = true;

	//Movement

	//Position on a map as if in a world, not a grid
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector Position;

	//Things cannot occupy the same space so we have to account for it
	//Also may be useful for other things
	//In meters
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ClampMin = 0, UIMax = 100))
	FVector Size {1., 1., 1.};

	//In m/s
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float Speed;

	//In m/s
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, UIMax = 1000))
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FRotator Rotation;
};

USTRUCT(BlueprintType)
struct PROTOGAME_API FVBFUnitInfoTable : public FTableRowBase
{
	GENERATED_BODY()

	FVBFUnitInfoTable();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, NoClear, meta = (AllowPrivateAccess = true))
	TSubclassOf<class UVBFUnitBase> unit_class;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UTexture2D* thumbnail;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FVBFUnitInfo inventory_item_info;
};