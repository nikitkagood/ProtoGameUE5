// Nikita Belov, All rights reserved


#include "Inventory/InventoryManager.h"

#include "Item/ItemBase.h"
#include "InventoryComponent.h"
#include "InvSpecialSlot.h"
#include "WeaponSpecialSlotComponent.h"

#include <functional>

UInventoryManager::UInventoryManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryManager::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryManager::AddInventory(const UClass* inventory_class, const FName& name)
{
	auto* t = NewObject<UObject>(this, inventory_class, name);
	//inventories.Add(t);
}

void UInventoryManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UInventoryManager::MoveItemToInventory(UItemBase* item, TArray<UClass*> inventory_types, bool exclude_types)
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

		if (inventory_types.IsEmpty())
		{
			result = current_inv->MoveItemToInventory(item, i);
		}
		else 
		{
			if (inventory_types.Contains(i.GetObject()->GetClass()))
			{
				result = current_inv->MoveItemToInventory(item, i);
			}
		}


		if (result == true)
		{
			break;
		}
	}

	return result;
}

bool UInventoryManager::AddItemFromWorld(UItemBase* item, EManagerInventoryType inventory_type)
{
	//typedef bool (*Predicate_AddItemFromWorld) (TScriptInterface<IInventoryInterface> inv);

	auto AddItemFromWorld_if = [&](std::function<bool(TScriptInterface<IInventoryInterface>)> predicate)
	{
		for (auto& inv : inventories)
		{
			if (inv == nullptr)
			{
				checkf(false, TEXT("Invalid inventory"));
				continue;
			}

			if (predicate(inv) == false)
			{
				continue;
			}

			if (inv.GetInterface()->AddItemFromWorld(item))
			{
				return true;
			}
		}

		return false;
	};

	switch (inventory_type)
	{
	case EManagerInventoryType::Any:

		for (auto& inv : inventories)
		{
			if (inv.GetInterface()->AddItemFromWorld(item))
			{
				return true;
			}
		}

		break;
	case EManagerInventoryType::SpecialSlot:

		if (AddItemFromWorld_if( [&](TScriptInterface<IInventoryInterface> inv)
			{ return inv.GetObject()->IsA<UInvSpecialSlotComponent>(); }) )
		{
			return true;
		}

		break;
	case EManagerInventoryType::InventoryComponent:

		if (
			AddItemFromWorld_if( [&](TScriptInterface<IInventoryInterface> inv)
			{ return inv.GetObject()->IsA<UInventoryComponent>(); }) )
		{
			return true;
		}
		break;
	default:
		break;
	}

	return false;
}

bool UInventoryManager::MoveItemToInventoryDestination(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell = 0)
{
	TScriptInterface<IInventoryInterface> inv = item->GetOuterUpstreamInventory();

	if (inv == nullptr)
	{
		checkf(false, TEXT("Trying to move item (to destination) which isn't in any inventory"));
		return false;
	}

	return inv->MoveItemToInventoryDestination(item, destination, new_upper_left_cell);
}

//bool UInventoryManager::AddItemFromWorld(UItemBase* item)
//{
//	for (auto& inv : inventories)
//	{
//		if(inv.GetInterface()->AddItemFromWorld(item))
//		{
//			return true;
//		}
//	}
//
//	return false;
//}

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

//bool UInventoryManager::ReceiveItem(UItemBase* item)
//{
//
//	for (auto& inv : inventories)
//	{
//		if (inv->ReceiveItem(item) == true)
//		{
//			return true;
//		}
//	}
//
//	return false;
//}
//
//bool UInventoryManager::ReceiveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell)
//{
//	for (auto& inv : inventories)
//	{
//		if (inv->ReceiveItemInGrid(item, new_upper_left_cell) == true)
//		{
//			return true;
//		}
//	}
//
//	return false;
//}

//void UInventoryManager::UpdateInventory()
//{
//	for (auto& inv : inventories)
//	{
//		inv->UpdateInventory();
//	}
//}

void UInventoryManager::AddExistingInventory(TScriptInterface<IInventoryInterface> inventory)
{
	if (inventory == nullptr)
	{
		checkf(false, TEXT("UInventoryManager::AddExistingInventory: Inventory is invalid"));
		return;
	}

	if (inventories.Find(inventory) != INDEX_NONE)
	{
		checkf(false, TEXT("UInventoryManager::AddExistingInventory: Inventory is added already"));
		return;
	}

	inventories.Add(inventory);
}


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

