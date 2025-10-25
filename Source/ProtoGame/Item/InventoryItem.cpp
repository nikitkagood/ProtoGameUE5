// Nikita Belov, All rights reserved


#include "Item/InventoryItem.h"
#include "Inventory/InventoryManager.h"
#include "Character/GameCharacterBase.h"

UInventoryItem::UInventoryItem()
{
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

bool UInventoryItem::Initialize(FDataTableRowHandle handle)
{
    auto* ptr_row = handle.GetRow<DataTableType>("UInventoryItem::Initialize");

    if (ptr_row != nullptr)
    {
        inventory_item_info = ptr_row->inventory_item_info;
        InventoryComponent->Initialize(ptr_row->InventoryDimensions, ptr_row->InventoryName);

        return true;
    }

    return false;
}

FItemThumbnailInfo UInventoryItem::GetItemThumbnailInfoFromDT()
{
    return GetItemThumbnailInfoFromDT_Impl<DataTableType>();
}

bool UInventoryItem::SetOuterUpstreamInventory(TScriptInterface<IInventoryInterface> inventory)
{
	//When we move InventoryItem, we want to transfer ownership of ActorComponents (like InventoryComponent) since they don't follow UObject garbage collection pattern
	//We don't change ownership of SceneComponents, which represent visual state and are to be deleted if they are no longer owned by ItemActor

	if (inventory.GetObject() == InventoryComponent)
	{
		//Self recursion
		return false;
	}

	if (IsValid(GetInventoryComponent()))
	{
		if (this->SetInventoryOwner(inventory->GetInventoryOwner()) == false)
		{
			return false;
		}
	}

	return this->Rename(nullptr, inventory.GetObject());;
}

bool UInventoryItem::SetOuterItemActor(AItemActor* item_actor)
{
	if (IsValid(GetInventoryComponent()))
	{
		if (GetInventoryComponent()->SetInventoryOwner(item_actor) == false)
		{
			return false;
		}
	}
	{
		return false;
	}

	return this->Rename(nullptr, item_actor);
}

bool UInventoryItem::Interact(AActor* caller, EInteractionActions action)
{
	auto character = Cast<AGameCharacterBase>(caller);

	if (character != nullptr)
	{
		switch (action)
		{
		case EInteractionActions::Unspecified:
			if (character->GetInventoryManger()->AddItemFromWorld(this))
			{
				return true;
			}
			break;
		case EInteractionActions::Open:
			//Since it heavily relies on UI, it's in BP

			//Take a look into contents of this inventory (while it's not equipped)
			//character->OpenInventoryUI();
			//create grid inv in slot
			//interaction range check
			break;
		case EInteractionActions::Close:
			//Not meaningful. There is no need to close inventory via action.
			break;
		case EInteractionActions::Equip:
			if (character->GetInventoryManger()->AddItemFromWorld(this, EManagerInventoryType::SpecialSlot))
			{
				return true;
			}
			break;
		case EInteractionActions::Take:
			if (character->GetInventoryManger()->AddItemFromWorld(this, EManagerInventoryType::InventoryComponent))
			{
				return true;
			}
			break;
		case EInteractionActions::Lock:
			//Not implemented
			break;
		case EInteractionActions::Unlock:
			//Not implemented
			break;
		default:
			break;
		}
	}

	return false;
}

bool UInventoryItem::MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination)
{
    return InventoryComponent->MoveItemToInventory(item, destination);
}

bool UInventoryItem::MoveItemToInventoryDestination(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell)
{
    return InventoryComponent->MoveItemToInventoryDestination(item, destination, new_upper_left_cell);
}

bool UInventoryItem::AddItemFromWorld(UItemBase* item)
{
    return InventoryComponent->AddItemFromWorld(item);
}

bool UInventoryItem::DropItemToWorld(UItemBase* item)
{
    return InventoryComponent->DropItemToWorld(item);
}

bool UInventoryItem::ReceiveItem(UItemBase* item)
{
    return InventoryComponent->ReceiveItem(item);
}

bool UInventoryItem::ReceiveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell)
{
    return InventoryComponent->ReceiveItemInGrid(item, new_upper_left_cell);
}

void UInventoryItem::UpdateStackDependencies(UItemBase* item, int32 new_stack_size)
{
    InventoryComponent->UpdateStackDependencies(item, new_stack_size);
}

void UInventoryItem::UpdateInventory()
{
    InventoryComponent->UpdateInventory();
}

TScriptInterface<IInventoryInterface> UInventoryItem::GetOuterUpstreamInventory() const
{
    TScriptInterface<IInventoryInterface> res;
    res.SetInterface(Cast<IInventoryInterface>(GetOuter()));
    return res;
}

bool UInventoryItem::SetInventoryOwner(UObject* new_owner)
{
	if (IsValid(new_owner) == false)
	{
		checkf(false, TEXT("New owner is not valid"))
		return false;
	}

	if (IsValid(GetInventoryComponent()))
	{
		if (GetInventoryComponent()->SetInventoryOwner(new_owner) == false)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return Rename(nullptr, new_owner);
}


