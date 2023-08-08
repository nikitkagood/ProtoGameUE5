// Nikita Belov, All rights reserved

#include "Inventory/InvSpecialSlot.h"
#include "Item/ItemBase.h"

#include "Kismet/KismetSystemLibrary.h"

UInvSpecialSlotComponent::UInvSpecialSlotComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = true;
}

bool UInvSpecialSlotComponent::AddItem(UItemBase* item)
{
	Item = item;

	item->SetOuterUpstreamInventory(this);
	item->World = GetWorld();
	item->SetUpperLeftCell({ INDEX_NONE, INDEX_NONE });

	if(item->GetRotated()) //if an item is rotated, roate it back since this component doesn't support rotation
	{
		item->Rotate();
	}

	OnInventoryUpdated.Broadcast();

	return true;
}

void UInvSpecialSlotComponent::Clear()
{
	Item = nullptr;
	OnInventoryUpdated.Broadcast();
}

UItemBase* UInvSpecialSlotComponent::GetItem() const
{
	if(Item == nullptr || Item->GetOuterUpstreamInventory() != this)
	{
		return nullptr;
	}

	return Item;
}

bool UInvSpecialSlotComponent::MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination)
{
	if(destination.GetObject() == this || item != GetItem())
	{
		return false;
	}

	if(destination->ReceiveItem(item))
	{
		Clear();
		return true;
	}

	OnInventoryUpdated.Broadcast(); 

	return false;
}

bool UInvSpecialSlotComponent::MoveItemToInventoryInGrid(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell)
{
	if(destination.GetObject() == this || item != GetItem())
	{
		OnInventoryUpdated.Broadcast(); //Update even if we fail since DragAndDrop operation already destroyed the widget
		return false;
	}

	if(destination->ReceiveItemInGrid(item, new_upper_left_cell))
	{
		Clear();
		return true;
	}

	OnInventoryUpdated.Broadcast(); 

	return false;
}

bool UInvSpecialSlotComponent::AddItemFromWorld(UItemBase* item)
{
	if (item == nullptr)
	{
		return false;
	}
	if (AddItem(item) == false)
	{
		return false;
	}

	//item->ItemActor->Destroy(); //currently ItemActors destroy themselves

	return true;
}

bool UInvSpecialSlotComponent::DropItemToWorld(UItemBase* item)
{
	if(item == nullptr || item != GetItem())
	{
		return false;
	}

	constexpr float DropDistance = 60;

	auto* item_actor = item->SpawnItemActor(GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * DropDistance, GetOwner()->GetActorRotation());
	if(item_actor == nullptr)
	{
		//can't spawn, do not delete from inventory
		UKismetSystemLibrary::PrintString(GetWorld(), "Drop to world is blocked", true, true, FLinearColor(130, 5, 255), 4);

		OnInventoryUpdated.Broadcast(); //Update even if we fail since DragAndDrop operation already destroyed the widget
		return false;
	}

	Clear();

	item->SetOuterItemActor(item_actor);

	return true;
}

bool UInvSpecialSlotComponent::ReceiveItem(UItemBase* item)
{
	if(IsOccupied() == false)
	{
		return AddItem(item);
	}

	return false;
}

TScriptInterface<IInventoryInterface> UInvSpecialSlotComponent::GetOuterUpstreamInventory() const
{
	return GetOuter();
}

bool UInvSpecialSlotComponent::IsOccupied() const
{
	if(GetItem() != nullptr)
	{
		return true;
	}

	return false;
}
