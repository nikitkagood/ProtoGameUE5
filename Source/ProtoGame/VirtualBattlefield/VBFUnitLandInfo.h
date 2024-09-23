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
};

USTRUCT(BlueprintType)
struct PROTOGAME_API FVBFUnitLandInfoTable : public FTableRowBase
{
	GENERATED_BODY()

	FVBFUnitLandInfoTable();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVBFUnitInfoTable unit_info_table;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVBFUnitLandInfo unit_land_info;
};