// Nikita Belov, All rights reserved


#include "Inventory/InventoryManager.h"

#include "Item/ItemBase.h"
#include "InventoryComponent.h"
#include "InvSpecialSlot.h"
#include "WeaponSpecialSlotComponent.h"

UInventoryManager::UInventoryManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryManager::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UInventoryManager::MoveItemToInventory(UItemBase* item, EManagerInventoryType inventory_type)
{
	TScriptInterface<IInventoryInterface> current_inv = item->GetOuterUpstreamInventory();
	bool result = false;

	if (current_inv == nullptr)
	{
		checkf(false, TEXT("Trying to move item which isn't in any inventory"));
		return false;
	}

	for (auto& i : inventories)
	{
		if (i == nullptr)
		{
			checkf(false, TEXT("Invalid inventory"));
			continue;
		}

		bool is_accpetable_inventory = true;

		switch (inventory_type)
		{
		case EManagerInventoryType::SpecialSlot:
			if (!i.GetObject()->IsA(UInvSpecialSlotComponent::StaticClass()))
			{
				is_accpetable_inventory = false;
			}
			break;
		case EManagerInventoryType::SpecialSlot_Weapon:
			if (!i.GetObject()->IsA(UWeaponSpecialSlotComponent::StaticClass()))
			{
				is_accpetable_inventory = false;
			}
			break;
		case EManagerInventoryType::InventoryComponent:
			if (!i.GetObject()->IsA(UInventoryComponent::StaticClass()))
			{
				is_accpetable_inventory = false;
			}
			break;
		default:
			break;
		}

		if (is_accpetable_inventory)
		{
			result = current_inv->MoveItemToInventory(item, i);
		}
	}

	return result;
}

bool UInventoryManager::MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination)
{
	TScriptInterface<IInventoryInterface> current_inv = item->GetOuterUpstreamInventory();

	if (current_inv == nullptr)
	{
		checkf(false, TEXT("Trying to move item which isn't in any inventory"));
		return false;
	}

	return current_inv->MoveItemToInventory(item, destination);
}

bool UInventoryManager::MoveItemToInventoryInGrid(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell)
{
	//for (uint8 i = 0; i < TNumericLimits<uint8>::Max(); i++)
	//{
	//	TArray<IInventoryInterface*> find_result;

	//	inventories.MultiFind(i, find_result, true);

	//	for (auto& inv : find_result)
	//	{
	//		if (inv->MoveItemToInventoryInGrid(item, destination, new_upper_left_cell) == true)
	//		{
	//			return true;
	//		}
	//	}
	//}

	TScriptInterface<IInventoryInterface> inv = item->GetOuterUpstreamInventory();

	if (inv == nullptr)
	{
		checkf(false, TEXT("Trying to move item (in grid) which isn't in any inventory"));
		return false;
	}

	return inv->MoveItemToInventoryInGrid(item, destination, new_upper_left_cell);
}

bool UInventoryManager::DropItemToWorld(UItemBase* item)
{
	TScriptInterface<IInventoryInterface> inv = item->GetOuterUpstreamInventory();

	if (inv == nullptr)
	{
		checkf(false, TEXT("Trying to drop to world an item which isn't in any inventory"));
		return false;
	}

	return inv->DropItemToWorld(item);
}

bool UInventoryManager::ReceiveItem(UItemBase* item)
{
	//for (uint8 i = TNumericLimits<uint8>::Max(); i != 0; i--)
	//{
	//	TArray<IInventoryInterface*> find_result;

	//	inventories.MultiFind(i, find_result, true);

	//	for (auto& inv : find_result)
	//	{
	//		if (inv->ReceiveItem(item) == true)
	//		{
	//			return true;
	//		}
	//	}
	//}

	for (auto& inv : inventories)
	{
		if (inv->ReceiveItem(item) == true)
		{
			return true;
		}
	}

	return false;
}

bool UInventoryManager::ReceiveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell)
{
	for (auto& inv : inventories)
	{
		if (inv->ReceiveItemInGrid(item, new_upper_left_cell) == true)
		{
			return true;
		}
	}

	return false;
}

void UInventoryManager::UpdateInventory()
{
	for (auto& inv : inventories)
	{
		inv->UpdateInventory();
	}
}

TScriptInterface<IInventoryInterface> UInventoryManager::GetOuterUpstreamInventory() const
{
	//Iventory manager isn't really meant to be within another inventory
	//But techically possibility remains
	return GetOuter();
}

void UInventoryManager::AddInventory(TScriptInterface<IInventoryInterface> inventory)
{
	if (inventory == nullptr)
	{
		checkf(false, TEXT("Inventory is invalid"));
		return;
	}

	if (inventories.Find(inventory) != INDEX_NONE)
	{
		checkf(false, TEXT("Inventory is added already"));
		return;
	}

	inventories.Add(inventory);
}

//void UInventoryManager::SetInventories(TArray<IInventoryInterface*>&& new_inventories)
//{
//	inventories = new_inventories;
//	UE_LOG(LogTemp, Warning, TEXT("Eee"));
//}

void UInventoryManager::RemoveInventory(TScriptInterface<IInventoryInterface> inventory)
{
	if (inventory == nullptr)
	{
		checkf(false, TEXT("Inventory is invalid"));
		return;
	}

	int32 result = inventories.Find(inventory);

	if (result == INDEX_NONE)
	{
		checkf(false, TEXT("This inventory had not been added to manager"));
		return;
	}
	
	inventories.RemoveAt(result);
}

//void UInventoryManager::AddInventory(IInventoryInterface* inventory, uint8 priority)
//{
//	if (inventory == nullptr)
//	{
//		checkf(false, TEXT("Inventory is invalid"));
//		return;
//	}
//
//	inventories.Add(priority, inventory);
//}

