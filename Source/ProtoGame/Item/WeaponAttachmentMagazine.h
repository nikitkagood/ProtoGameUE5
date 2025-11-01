// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Item/WeaponAttachment.h"

#include "Interfaces/InventoryInterface.h"

#include "WeaponAttachmentMagazine.generated.h"


UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, meta=(BlueprintSpawnableComponent), ClassGroup=Item)
class PROTOGAME_API UWeaponAttachmentMagazine : public UWeaponAttachment, public IInventoryInterface
{
	GENERATED_BODY()

private:
	using DataTableType = FAttachmentMagazineTable;

public:
	UWeaponAttachmentMagazine();

	virtual bool Initialize(FDataTableRowHandle handle) override;

	virtual FItemThumbnailInfo GetItemThumbnailInfoFromDT() override;

	bool Push(UAmmoBase* ammo);
	UAmmoBase* Pop();

	int32 GetCapacity() const { return attachment_magazine_info.capacity; }
	int32 GetAmmoLeft() const { return attachment_magazine_info.storage.Num(); }
	const UAmmoBase* GetTopAmmo() const { return attachment_magazine_info.storage.Last(); }
	const FText& GetTopAmmoName() const { return attachment_magazine_info.storage.Last()->GetItemName(); }

	//Inventory interface
	
	//TODO: change implementation
	virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell) override;
	virtual bool AddItemFromWorld(UItemBase* item) override;
	virtual bool DropItemToWorld(UItemBase* item) override;
	//TODO: change implementation
	virtual bool ReceiveItem(UItemBase* item, FIntPoint new_upper_left_cell) override;
	//TODO: implement
	virtual void UpdateInventory() override { return; };
	virtual TScriptInterface<IInventoryInterface> GetOuterUpstreamInventory() const override;
	virtual AActor* GetInventoryOwner() override { return GetOwner(); };
	bool SetInventoryOwner(UObject* new_owner);

	//Mag unload on use
	virtual bool OnUse(AActor* caller) override;

private:
	//TODO: add inventory function/loading mode
	//just move or load like a person would load rounds one by one

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	FAttachmentMagazineInfo attachment_magazine_info;
};
