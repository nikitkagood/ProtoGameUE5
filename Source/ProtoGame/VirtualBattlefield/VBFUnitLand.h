// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "VirtualBattlefield/VBFUnitBase.h"

#include "VBFUnitLandInfo.h"

#include "VBFUnitLand.generated.h"

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, ClassGroup = VirtualBattlefield)
class PROTOGAME_API UVBFUnitLand : public UVBFUnitBase
{
	GENERATED_BODY()
public:
	using DataTableType = FVBFUnitLandInfoTable;

protected:
	virtual bool Initialize(FDataTableRowHandle handle) override;
public:
	bool SpawnUnitActor_Implementation(const FTransform& transform, const FVector& normal, FDataTableRowHandle handle, UWorld* world_optional = nullptr);

	//Currently we simply check whether MaxSpeed was set above zero
	bool IsEverMovable_Implementation() const;
	bool CanMove_Implementation() const;


	bool TryMoveTo_Implementation(const FVector& location);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FVBFUnitLandInfo unit_land_info;
};

