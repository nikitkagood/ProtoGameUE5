// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "VBFWeapon.h"
#include "VBFUnitLandInfo.h"

#include "VBFUnitInfantryInfo.generated.h"

USTRUCT(BlueprintType)
struct PROTOGAME_API FVBFUnitInfantryInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 MaxSoldierCount = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 SoldierCount = MaxSoldierCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<UVBFWeapon*> Weapons;
};

USTRUCT(BlueprintType)
struct PROTOGAME_API FVBFUnitInfantryInfoTable : public FTableRowBase
{
	GENERATED_BODY()

	FVBFUnitInfantryInfoTable();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVBFUnitLandInfoTable unit_land_table;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVBFUnitInfantryInfo unit_infantry_info;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//bool OverrideSoldierMesh = false;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//TArray<UStreamableRenderAsset*> SoldierMesh;
};