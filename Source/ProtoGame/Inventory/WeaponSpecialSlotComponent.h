// Nikita Belov, All rights reserved

#pragma once

#include "InvSpecialSlot.h"

#include "WeaponSpecialSlotComponent.generated.h"

//Not used
UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, meta=(BlueprintSpawnableComponent), ClassGroup=(Inventory))
class PROTOGAME_API UWeaponSpecialSlotComponent : public UInvSpecialSlotComponent
{
	GENERATED_BODY()

public:
	UWeaponSpecialSlotComponent();

	void UpdateAssociatedSkeletalMesh();

private:
	USkeletalMeshComponent* AssociatedWeaponSkeletalMesh;
	
};