// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Item/WeaponAttachment.h"

#include "WeaponAttachmentSight.generated.h"


UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, meta=(BlueprintSpawnableComponent), ClassGroup=Item)
class PROTOGAME_API UWeaponAttachmentSight : public UWeaponAttachment
{
	GENERATED_BODY()
	
public:
	UWeaponAttachmentSight();

	virtual bool SetProperties(FDataTableRowHandle handle) override;

private:
	FAttachmentSightInfo attachment_sight_info;

};
