// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "VBFUnitLand.h"
#include "VBFUnitInfantryInfo.h"

#include "VBFUnitInfantry.generated.h"

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, meta = (BlueprintSpawnableComponent), ClassGroup = VirtualBattlefield)
class PROTOGAME_API UVBFUnitInfantry : public UVBFUnitLand
{
	GENERATED_BODY()

	using DataTableType = FVBFUnitInfantryInfoTable;

public:
	virtual bool Initialize(FDataTableRowHandle handle) override;

	//bool SpawnUnitActor_Implementation(const FTransform& transform, FDataTableRowHandle handle, UWorld* world_optional = nullptr);


protected:

	FVBFUnitInfantryInfo unit_infantry_info;
	
};
