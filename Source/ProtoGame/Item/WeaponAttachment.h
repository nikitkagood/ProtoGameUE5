// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"

#include "AttachmentInfo.h"
#include "ItemBase.h"

#include "WeaponAttachment.generated.h"

class UWeaponBase;

//This class is only base for inherited classes, which should represent certain attachment types
UCLASS(Abstract, NotBlueprintable, DefaultToInstanced, ClassGroup=Item)
class PROTOGAME_API UWeaponAttachment : public UItemBase
{
	GENERATED_BODY()

public:	
	UWeaponAttachment();

	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool Initialize(FDataTableRowHandle handle) override;

	void SetOuterWeapon(UWeaponBase* weapon);

	UWeaponBase* GetOuterWeapon() const;

	const FAttachmentSlot& GetAttachmentSlot() { return attachment_info.slot; };
	TSoftObjectPtr<UStaticMesh> GetStaticMesh() { return attachment_info.static_mesh; };

	FName GetSocketName();

	//static bool isAttachmentCompatible(FAttachmentSlot slot, FAttachmentSlot other_slot);

	//TODO: simple implementation now
	bool IsAttachmentCompatible(FAttachmentSlot slot) const;

protected:
	//virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = true))
	FAttachmentInfo attachment_info;
};
