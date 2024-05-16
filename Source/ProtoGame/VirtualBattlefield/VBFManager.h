// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "VBFUnitBase.h"
#include "VBFMap.h"

#include "VBFManager.generated.h"

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced)
class PROTOGAME_API UUVBFManager : public UObject
{
	GENERATED_BODY()

	using UnitOwnerID = uint32;

	//VBFMap
	
private:
	//UVBFMap* vbf_map;

	TMap<UnitOwnerID, UVBFUnitBase*> units;
};
