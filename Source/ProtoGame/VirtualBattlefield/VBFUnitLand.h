// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "VirtualBattlefield/VBFUnitBase.h"

#include "VBFUnitLand.generated.h"

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, meta = (BlueprintSpawnableComponent), ClassGroup = VirtualBattlefield)
class PROTOGAME_API UVBFUnitLand : public UVBFUnitBase
{
	GENERATED_BODY()
	
	virtual bool TryMoveTo(FVector new_position) override;
};
