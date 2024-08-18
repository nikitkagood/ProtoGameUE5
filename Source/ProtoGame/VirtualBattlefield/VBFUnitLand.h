// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "VirtualBattlefield/VBFUnitBase.h"

#include "VBFUnitLandInfo.h"

#include "VBFUnitLand.generated.h"

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, meta = (BlueprintSpawnableComponent), ClassGroup = VirtualBattlefield)
class PROTOGAME_API UVBFUnitLand : public UVBFUnitBase
{
	GENERATED_BODY()
public:
	using DataTableType = FVBFUnitLandInfoTable;

protected:
	virtual bool Initialize(FDataTableRowHandle handle) override;
public:
	bool SpawnUnitActor_Implementation(const FTransform& transform, FDataTableRowHandle handle, UWorld* world_optional = nullptr);

	//virtual bool TryMoveTo(FVector new_position) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FVBFUnitLandInfo unit_land_info;
};
