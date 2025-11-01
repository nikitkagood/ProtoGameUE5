// Nikita Belov, All rights reserved


#include "Item/WeaponAttachmentMagazine.h"
#include "ItemActor.h"

UWeaponAttachmentMagazine::UWeaponAttachmentMagazine()
{
	inventory_item_info.UseActionText = FText::FromString("Unload");
}

bool UWeaponAttachmentMagazine::Initialize(FDataTableRowHandle handle)
{
	auto* ptr_row = handle.GetRow<DataTableType>("UWeaponAttachmentMagazine::Initialize");

	if(ptr_row != nullptr)
	{
		inventory_item_info = ptr_row->inventory_item_info;
		attachment_info = ptr_row->attachment_info;
		attachment_magazine_info = ptr_row->attachment_magazine_info;

		return true;
	}

    return false;
}

FItemThumbnailInfo UWeaponAttachmentMagazine::GetItemThumbnailInfoFromDT()
{
	return GetItemThumbnailInfoFromDT_Impl<DataTableType>();
}

//bool UWeaponAttachmentMagazine::MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell)
//{
//	//Item parameter isn't used
//
//	if(destination.GetObject() == this)
//	{
//		return false;
//	}
//
//	auto* res = attachment_magazine_info.storage.Last();
//
//	if(res != nullptr && destination->ReceiveItem(res, new_upper_left_cell))
//	{
//		Pop();
//		return true;
//	}
//
//	return false;
//}

bool UWeaponAttachmentMagazine::MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell)
{
	//Item parameter isn't used

	if(destination.GetObject() == this)
	{
		return false;
	}

	auto* res = attachment_magazine_info.storage.Last();

	if(res != nullptr && destination->ReceiveItem(res, new_upper_left_cell))
	{
		Pop();
		return true;
	}

	return false;
}

bool UWeaponAttachmentMagazine::AddItemFromWorld(UItemBase* item)
{
	if (item == nullptr)
	{
		return false;
	}

	UAmmoBase* ammo = Cast<UAmmoBase>(item);

	if (ammo == nullptr || Push(ammo) == false)
	{
		return false;
	}

	//item->ItemActor->Destroy(); //currently ItemActors destroy themselves

	return true;
}
bool UWeaponAttachmentMagazine::DropItemToWorld(UItemBase* item)
{
	return false;
}

bool UWeaponAttachmentMagazine::ReceiveItem(UItemBase* item, FIntPoint new_upper_left_cell)
{

	return Push(Cast<UAmmoBase>(item));
}

bool UWeaponAttachmentMagazine::SetInventoryOwner(UObject* new_owner)
{
	if (IsValid(new_owner) == false)
	{
		checkf(false, TEXT("New owner is not valid"))
		return false;
	}

	return Rename(nullptr, new_owner);
}

TScriptInterface<IInventoryInterface> UWeaponAttachmentMagazine::GetOuterUpstreamInventory() const
{
	return GetOuter();
}

bool UWeaponAttachmentMagazine::OnUse(AActor* caller)
{
	//Mag unload

	auto upstream_inventory = GetOuterUpstreamInventory();

	if(upstream_inventory == nullptr)
	{
		return false;
	}

	bool at_least_one = false;

	int32 ammo_left = GetAmmoLeft();

	for(int32 i = 0; i < ammo_left; i++)
	{
		bool local = false;

		local = MoveItemToInventory(nullptr, upstream_inventory, {-1, -1});

		if(at_least_one != true && local == true)
		{
			at_least_one = true;
		}

		if(local == false)
		{
			break;
		}
	}

	return at_least_one;
}

bool UWeaponAttachmentMagazine::Push(UAmmoBase* ammo)
{
	if(ammo != nullptr && attachment_magazine_info.storage.Num() < attachment_magazine_info.capacity)
	{
		attachment_magazine_info.storage.Push(ammo);
		return true;
	}

	return false;
}

UAmmoBase* UWeaponAttachmentMagazine::Pop()
{
	if(attachment_magazine_info.storage.Num() > 0)
	{
		return attachment_magazine_info.storage.Pop();
	}
	
	return nullptr;
}
