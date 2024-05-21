// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "VBFUnitInfo.h"

#include "VBFUnitLandInfo.generated.h"

USTRUCT(BlueprintType)
struct PROTOGAME_API FVBFUnitLandInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float UnitLandTestValue;
	
	//TArray<VBFWeapon>;
};

USTRUCT(BlueprintType)
struct PROTOGAME_API FVBFUnitLandInfoTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVBFUnitLandInfo unit_land_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVBFUnitInfoTable unit_info;
};