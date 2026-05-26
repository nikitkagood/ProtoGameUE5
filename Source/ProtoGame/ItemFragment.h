// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "ItemFragment.generated.h"

//A dynamically added value to an item
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew, Abstract)
class PROTOGAME_API UItemFragment : public UObject
{
	GENERATED_BODY()
	
};


UCLASS(BlueprintType, EditInlineNew)
class PROTOGAME_API UDurabilityFragment : public UItemFragment
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fragment")
    float CurrentDurability = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fragment")
    float MaxDurability = 100.0f;
};