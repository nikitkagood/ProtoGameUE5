// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemActor.h"

#include "WeaponBaseActor.generated.h"


UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, meta = (BlueprintSpawnableComponent), ClassGroup = (Item))
class PROTOGAME_API AWeaponBaseActor : public AItemActor
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	virtual void OnAttack() { check(false); };
	
};
