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

	DropDistance = 60;
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
			result = current_inv->MoveItemToInventory(item, i, { -1, -1 });
		}
		else 
		{
			if (inventory_types.Contains(i->GetClass()))
			{
				result = current_inv->MoveItemToInventory(item, i, {-1, -1});
			}
		}


		if (result == true)
		{
			break;
		}
	}

	return result;
}

bool UInventoryManager::MoveItemToInventoryDestination(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell = 0)
{
	TScriptInterface<IInventoryInterface> inv = item->GetOuterUpstreamInventory();

	if (inv == nullptr)
	{
		checkf(false, TEXT("Trying to move item (to destination) which isn't in any inventory"));
		return false;
	}

	return inv->MoveItemToInventory(item, destination, new_upper_left_cell);
}

bool UInventoryManager::AddItemFromWorld(UItemBase* item, EManagerInventoryType inventory_type)
{
	//auto AddItemFromWorld_if = [&](std::function<bool(TScriptInterface<IInventoryInterface>)> predicate)
	//Call AddItemFrom world if passed predicate is true
	//auto AddItemFromWorld_if = [item, this](std::function<bool(IInventoryInterface*)> predicate)
	//{
	//	for (auto& i : inventories)
	//	{
	//		IInventoryInterface* inv = Cast<IInventoryInterface>(i);

	//		if (inv == nullptr)
	//		{
	//			checkf(false, TEXT("Invalid inventory"));
	//			continue;
	//		}

	//		if (predicate(inv) == false)
	//		{
	//			continue;
	//		}

	//		if (inv->AddItemFromWorld(item))
	//		{
	//			return true;
	//		}
	//	}

	//	return false;
	//};

	auto AddItemFromWorld_Type = [item, this](TSubclassOf<UObject> type)
		{
			for (auto& i : inventories)
			{
				if (i->IsA(type) == false)
				{
					continue;
				}

				IInventoryInterface* inv = Cast<IInventoryInterface>(i);

				if (inv == nullptr)
				{
					checkf(false, TEXT("Invalid inventory"));
					continue;
				}

				if (inv->AddItemFromWorld(item))
				{
					return true;
				}
			}

			return false;
		};

	switch (inventory_type)
	{
	case EManagerInventoryType::Any:

		for (auto& i : inventories)
		{
			IInventoryInterface* inv = Cast<IInventoryInterface>(i);

			if (inv == nullptr)
			{
				checkf(false, TEXT("Invalid inventory"));
				continue;
			}

			if (inv->AddItemFromWorld(item))
			{
				return true;
			}
		}

		break;
	case EManagerInventoryType::SpecialSlot:
		//Call lambda, pass as a parameter another lambda (predicate)
		//if (AddItemFromWorld_if( [](IInventoryInterface* inv)
		//	{ return inv->_getUObject()->IsA<UInvSpecialSlotComponent>(); }) )
		//{
		//	return true;
		//}

		if (AddItemFromWorld_Type(UInvSpecialSlotComponent::StaticClass()))
		{
			return true;
		}

		break;
	case EManagerInventoryType::InventoryComponent:
		//if (AddItemFromWorld_if( [](IInventoryInterface* inv)
		//	{ return inv->_getUObject()->IsA<UInventoryComponent>(); }) )
		//{
		//	return true;
		//}

		if (AddItemFromWorld_Type(UInventoryComponent::StaticClass()))
		{
			return true;
		}

		break;
	default:
		break;
	}

	return false;
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

TScriptInterface<IInventoryInterface> UInventoryManager::FindInventoryHasTag(FGameplayTag tag, bool exact)
{
	for (auto* inv : inventories)
	{
		if (exact)
		{
			if (Cast<IInventoryInterface>(inv)->GetInventoryTags().HasTagExact(tag))
			{
				return inv;
			}
		}
		else
		{
			if (Cast<IInventoryInterface>(inv)->GetInventoryTags().HasTag(tag))
			{
				return inv;
			}
		}
	}

	return {};
}

TArray<TScriptInterface<IInventoryInterface>> UInventoryManager::FindAllInventoriesHasTag(FGameplayTag tag, bool exact)
{
	TArray<TScriptInterface<IInventoryInterface>> arr;

	for (auto* inv : inventories)
	{
		if (exact)
		{
			if (Cast<IInventoryInterface>(inv)->GetInventoryTags().HasTagExact(tag))
			{
				arr.Push(inv);
			}
		}
		else
		{
			if (Cast<IInventoryInterface>(inv)->GetInventoryTags().HasTag(tag))
			{
				arr.Push(inv);
			}
		}
	}

	return arr;
}

TScriptInterface<IInventoryInterface> UInventoryManager::FindInventoryHasAllTags(FGameplayTagContainer tags, bool exact)
{
	for (auto* inv : inventories)
	{
		if (exact)
		{
			if (Cast<IInventoryInterface>(inv)->GetInventoryTags().HasAllExact(tags))
			{
				return inv;
			}
		}
		else
		{
			if (Cast<IInventoryInterface>(inv)->GetInventoryTags().HasAll(tags))
			{
				return inv;
			}
		}
	}

	return {};
}


void UInventoryManager::AddExistingInventory(TScriptInterface<IInventoryInterface> inventory)
{
	if (inventory == nullptr)
	{
		checkf(false, TEXT("UInventoryManager::AddExistingInventory: Inventory is invalid"));
		return;
	}

	if (inventories.Find(inventory.GetObject()) != INDEX_NONE)
	{
		checkf(false, TEXT("UInventoryManager::AddExistingInventory: Inventory is added already"));
		return;
	}

	inventories.Add(inventory.GetObject());
}


void UInventoryManager::RemoveInventory(TScriptInterface<IInventoryInterface> inventory)
{
	if (inventory == nullptr)
	{
		checkf(false, TEXT("Inventory is invalid"));
		return;
	}

	int32 result = inventories.Find(inventory.GetObject());

	if (result == INDEX_NONE)
	{
		checkf(false, TEXT("This inventory had not been added to manager"));
		return;
	}
	
	inventories.RemoveAt(result);
}

