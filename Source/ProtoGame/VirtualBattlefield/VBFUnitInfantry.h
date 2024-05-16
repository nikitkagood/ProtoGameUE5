// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "VBFUnitLand.h"
#include "VBFUnitInfantry.generated.h"

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, meta = (BlueprintSpawnableComponent), ClassGroup = VirtualBattlefield)
class PROTOGAME_API UVBFUnitInfantry : public UVBFUnitLand
{
	GENERATED_BODY()
	
};
