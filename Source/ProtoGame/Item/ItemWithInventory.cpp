// Nikita Belov, All rights reserved


#include "Item/ItemWithInventory.h"
#include "Inventory/InventoryManager.h"
#include "Character/GameCharacterBase.h"

UItemWithInventory::UItemWithInventory()
{
    //InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

bool UItemWithInventory::Initialize(FDataTableRowHandle handle)
{
    //auto* ptr_row = handle.GetRow<DataTableType>("UItemWithInventory::Initialize");

    //if (ptr_row != nullptr)
    //{
    //    inventory_item_info = ptr_row->inventory_item_info;
    //    InventoryComponent->Initialize(ptr_row->InventoryDimensions, ptr_row->InventoryName);

    //    return true;
    //}

    return false;
}

FItemThumbnailInfo UItemWithInventory::GetItemThumbnailInfoFromDT()
{
    return GetItemThumbnailInfoFromDT_Impl<DataTableType>();
}

bool UItemWithInventory::SetOuterUpstreamInventory(TScriptInterface<IInventoryInterface> inventory)
{
	//When we move InventoryItem, we want to transfer ownership of ActorComponents (like InventoryComponent) since they don't follow UObject garbage collection pattern
	//We don't change ownership of SceneComponents, which represent visual state and are to be deleted if they are no longer owned by ItemActor

	//if (inventory.GetObject() == InventoryComponent)
	//{
	//	//Self recursion
	//	return false;
	//}

	//if (IsValid(GetInventoryComponent()))
	//{
	//	if (this->SetInventoryOwner(inventory->GetInventoryOwner()) == false)
	//	{
	//		return false;
	//	}
	//}

	//return this->Rename(nullptr, inventory.GetObject());
	return false;
}

bool UItemWithInventory::SetOuterItemActor(AItemActor* item_actor)
{
	//if (IsValid(GetInventoryComponent()))
	//{
	//	if (GetInventoryComponent()->SetInventoryOwner(item_actor) == false)
	//	{
	//		return false;
	//	}
	//}
	//{
	//	return false;
	//}

	//return this->Rename(nullptr, item_actor);
	return false;
}

bool UItemWithInventory::MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell)
{
    //return InventoryComponent->MoveItemToInventory(item, destination, new_upper_left_cell);
	return false;
}

bool UItemWithInventory::AddItemFromWorld(UItemBase* item)
{
    //return InventoryComponent->AddItemFromWorld(item);
	return false;
}

bool UItemWithInventory::DropItemToWorld(UItemBase* item)
{
    //return InventoryComponent->DropItemToWorld(item);
	return false;
}

bool UItemWithInventory::ReceiveItem(UItemBase* item, FIntPoint new_upper_left_cell)
{
    //return InventoryComponent->ReceiveItem(item, new_upper_left_cell);
	return false;
}

void UItemWithInventory::UpdateStackDependencies(UItemBase* item, int32 new_stack_size)
{
    //InventoryComponent->UpdateStackDependencies(item, new_stack_size);
}

void UItemWithInventory::UpdateInventory()
{
    //InventoryComponent->UpdateInventory();
}

TScriptInterface<IInventoryInterface> UItemWithInventory::GetOuterUpstreamInventory() const
{
    TScriptInterface<IInventoryInterface> res;
    res.SetInterface(Cast<IInventoryInterface>(GetOuter()));
    return res;
}

bool UItemWithInventory::SetInventoryOwner(UObject* new_owner)
{
	//if (IsValid(new_owner) == false)
	//{
	//	checkf(false, TEXT("New owner is not valid"))
	//	return false;
	//}

	//if (IsValid(GetInventoryComponent()))
	//{
	//	if (GetInventoryComponent()->SetInventoryOwner(new_owner) == false)
	//	{
	//		return false;
	//	}
	//}
	//else
	//{
	//	return false;
	//}

	//return Rename(nullptr, new_owner);
	return false;
}


