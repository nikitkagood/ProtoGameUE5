// Nikita Belov, All rights reserved

#include "Inventory/InvSpecialSlot.h"
#include "Inventory/InventoryManager.h"
#include "Item/ItemBase.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UInvSpecialSlotComponent::UInvSpecialSlotComponent()
{

}

bool UInvSpecialSlotComponent::AddItem(UItemBase* item)
{
	if (IsOccupied())
	{
		return false;
	}

	//if (!IsItemCompatibleClass(item))
	//{
	//	//Incompatible class
	//	return false;
	//}

	if (item->SetOuterUpstreamInventory(this) == false)
	{
		return false;
	}

	Item = item;

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
	if(!IsValid(Item) || Item->GetOuterUpstreamInventory() != this)
	{
		return nullptr;
	}

	return Item;
}

//bool UInvSpecialSlotComponent::MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination)
//{
//	if(destination.GetObject() == this || item != GetItem())
//	{
//		return false;
//	}
//
//	if(destination->ReceiveItem(item))
//	{
//		Clear();
//		return true;
//	}
//
//	OnInventoryUpdated.Broadcast(); 
//
//	return false;
//}

bool UInvSpecialSlotComponent::MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell)
{
	if(destination.GetObject() == this || item != GetItem())
	{
		OnInventoryUpdated.Broadcast(); //Update even if we fail
		return false;
	}

	if(destination->ReceiveItem(item, new_upper_left_cell))
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
	
	AActor* owner_actor = nullptr;
	UInventoryManager* inv_manager = nullptr;
	float EndDropDistance = 60; //by default it's 60

	if (GetInventoryOwner()->IsA(UInventoryManager::StaticClass()))
	{
		inv_manager = Cast<UInventoryManager>(GetInventoryOwner());
		owner_actor = Cast<AActor>(inv_manager->GetOwner());

		EndDropDistance = inv_manager->GetDropDistance();
	}
	else
	{
		owner_actor = Cast<AActor>(GetInventoryOwner());
	}


	if (owner_actor == nullptr)
	{
		return false;
	}

	FItemActorSpawnParameters spawn_params;
	spawn_params.MoveOwnershipItemObject = true;
	spawn_params.EnablePhysics = true;
	spawn_params.Interactible = true;

	auto* item_actor = item->SpawnItemActor(owner_actor->GetActorLocation() + owner_actor->GetActorForwardVector() * EndDropDistance, owner_actor->GetActorRotation(), spawn_params);
	if(item_actor == nullptr)
	{
		//can't spawn, do not delete from inventory
		UKismetSystemLibrary::PrintString(owner_actor, "Drop to world is blocked", true, true, FLinearColor(130, 5, 255), 4);

		OnInventoryUpdated.Broadcast(); //Update even if we fail since DragAndDrop operation already destroyed the widget
		return false;
	}

	Clear();

	item->SetOuterItemActor(item_actor);

	return true;
}

bool UInvSpecialSlotComponent::ReceiveItem(UItemBase* item, FIntPoint new_upper_left_cell)
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

bool UInvSpecialSlotComponent::SetInventoryOwner(UObject* new_owner)
{
	if (IsValid(new_owner) == false)
	{
		checkf(false, TEXT("New owner is not valid"))
		return false;
	}

	if (Rename(nullptr, new_owner) == false)
	{
		return false;
	}

	//GetOwner()->RemoveOwnedComponent(this);

	//if (auto new_owner_actor = Cast<AActor>(new_owner))
	//{
	//	new_owner_actor->AddOwnedComponent(this);
	//}

	return true;
}

//bool UInvSpecialSlotComponent::IsItemCompatibleClass(UItemBase* item)
//{
//	if (CompatibleClasses.IsEmpty())
//	{
//		return true;
//	}
//
//	for (auto& i : CompatibleClasses)
//	{
//		if (i.Key == item->GetClass())
//		{
//			return true;
//		}
//
//		if (i.Value.include_subclasses == true)
//		{
//			if (UKismetMathLibrary::ClassIsChildOf(item->GetClass(), i.Key))
//			{
//				return true;
//			}
//		}
//	}
//
//	return false;
//}

bool UInvSpecialSlotComponent::IsOccupied() const
{
	if(GetItem() != nullptr)
	{
		return true;
	}

	return false;
}

