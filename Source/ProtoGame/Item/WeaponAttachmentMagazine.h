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

	int32 GetCapacity() const { return attachment_magazine_info.capacity; }
	int32 GetRoundsLeft() const { return attachment_magazine_info.storage.Num(); }
	const FText& GetTopRoundName() const { return attachment_magazine_info.storage.Last()->GetItemName(); }

	//Inventory interface
	
	//In Move- functions item parameter isn't used
	virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination) override;
	virtual bool MoveItemToInventoryInGrid(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell) override;
	virtual bool AddItemFromWorld(UItemBase* item) override;
	virtual bool DropItemToWorld(UItemBase* item) override;
	virtual bool ReceiveItem(UItemBase* item) override;
	virtual void UpdateInventory() override { /*does nothing*/ };
	virtual TScriptInterface<IInventoryInterface> GetOuterUpstreamInventory() const override;

	//Mag unload on use
	virtual bool OnUse(AActor* caller) override;
private:
	//Not supported
	virtual bool ReceiveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell) override { check(false); return false; };
public:

	bool Push(UAmmoBase* ammo);
	UAmmoBase* Pop();
private:
	FAttachmentMagazineInfo attachment_magazine_info;
};
