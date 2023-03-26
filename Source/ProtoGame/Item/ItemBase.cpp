// Nikita Belov, All rights reserved


#include "ItemBase.h"

#include "Character/GameCharacter.h"
#include "Inventory/InventoryComponent.h"
#include "ItemActor.h"

#include "Engine/StaticMesh.h"

UItemBase::UItemBase()
{
	SetupDefaults();
}

UItemBase* UItemBase::StaticCreateObject(AItemActor* outer, TSubclassOf<UItemBase> item_base_class, ItemObjectCreationMethod item_object_creation_method, FDataTableRowHandle dt_item_properties)
{
	UItemBase* item_object = nullptr;

	if(item_object_creation_method == ItemObjectCreationMethod::CreateItemObjectFromDataTable)
	{
		item_object = NewObject<UItemBase>(outer, item_base_class);
		if(item_object == nullptr)
		{
			UE_LOG(LogTemp, Fatal, TEXT("Failed to create ItemObject. Maybe there is something wrong with assigned class."));
			return nullptr;
		}
		if(item_object->SetProperties(dt_item_properties) == false)
		{
			UE_LOG(LogTemp, Fatal, TEXT("Failed to set item properties. Maybe there is something wrong with the data table."));
			return nullptr;
		}
		item_object->SetItemActorClass(outer->GetClass()); //what ItemActor to spawn when ItemBase is dropped to world
		item_object->SetOuterItemActor(outer);
	}
	else if(item_object_creation_method == ItemObjectCreationMethod::CreateItemObjectFromItemBaseBP)
	{
		//TODO: might now work now
		item_object = NewObject<UItemBase>(outer, item_base_class);
		item_object->SetItemActorClass(outer->GetClass()); //what ItemActor to spawn when ItemBase is dropped to world
		item_object->SetOuterItemActor(outer);
	}

	return item_object;
}

bool UItemBase::SetProperties(FDataTableRowHandle handle)
{ 
	auto* ptr_row = handle.GetRow<FItemInfo>("UItemBase::SetProperties");

	if(ptr_row != nullptr)
	{
		item_info = *ptr_row;
		return true;
	}

	return false;
}


bool UItemBase::OnInteract(AActor* caller)
{
	auto ptr = Cast<AGameCharacterBase>(caller);

	if(ptr != nullptr)
	{
		if(ptr->GetInventoryComponent()->AddItemFromWorld(this))
		{
			return true; 
		}
	}

	return false;
}

bool UItemBase::OnUse(AActor* caller)
{
	//checkf(false, TEXT("ItemBase::OnUse isn't meant to be called")); 
	return false;
}

AItemActor* UItemBase::SpawnItemActor(const FVector& location, const FRotator& rotation)
{
	return AItemActor::StaticCreateObject(GetWorld(), ItemActorClass, this, location, rotation);
}

AItemActor* UItemBase::SpawnItemActorVisualOnly(const FVector& location, const FRotator& rotation)
{
	return AItemActor::StaticCreateObjectVisualOnly(GetWorld(), ItemActorClass, this, location, rotation);
}

void UItemBase::Rotate()
{
	Swap(item_info.Dimensions.X, item_info.Dimensions.Y);
	bRotated = !bRotated;
}

bool UItemBase::StackAdd(UItemBase* other)
{
	if(other == nullptr)
	{
		checkf(false, TEXT("Item is invalid"))
		return false;
	}

	//remainder <= 0 means we filled this stack and other doesn't exist anymore
	// == 0 no free stack space left
	// < 0 means how much free space left
	//remainder > 0 means how much other stack has left
	int32 stack_remainder = this->GetCurrentStackSize() + other->GetCurrentStackSize() - this->GetMaxStackSize();

	if(stack_remainder <= 0)
	{
		this->item_info.CurrentStackSize += other->GetCurrentStackSize();
		other->item_info.CurrentStackSize = 0;

		return true;
	}
	else
	{
		this->item_info.CurrentStackSize = this->GetMaxStackSize();
		other->item_info.CurrentStackSize = stack_remainder;

		return true;
	}
}

void UItemBase::SetupDefaults()
{
	//item_info.ItemID = -1;
	//item_info.Name.FromString("DefaultName");
	//item_info.Description.FromString("DefaultDescription");
	//item_info.UseActionText.FromString("DefaultAction");
	//item_info.Type = ItemType::None;
	//item_info.Dimensions = { 1, 1 };
	//item_info.MaxStackSize = 1;

	upper_left_cell = { -1, -1 };
	bRotated = false;
}

UStaticMesh* UItemBase::GetStaticMeshFromItemActorCDO() const
{
	//Default way to get the mesh. Since it's already used by ItemActor.
	return ItemActorClass.GetDefaultObject()->GetStaticMeshComp()->GetStaticMesh();
}

USkeletalMesh* UItemBase::GetSkeletalMeshFromItemActorCDO() const
{
	//Default way to get the mesh. Since it's already used by ItemActor.
	return ItemActorClass.GetDefaultObject()->GetSkeletalMeshComp()->SkeletalMesh;
}

TScriptInterface<IInventoryInterface> UItemBase::GetOuterUpstreamInventory() const
{
	return GetOuter();
}

AItemActor* UItemBase::GetOuterItemActor() const
{
	return Cast<AItemActor>(GetOuter()); 
}

void UItemBase::SetOuterUpstreamInventory(TScriptInterface<IInventoryInterface> inventory)
{
	this->Rename(nullptr, inventory.GetObject());
}

void UItemBase::SetOuterItemActor(AItemActor* item_actor)
{ 
	this->Rename(nullptr, item_actor->_getUObject()); 
}

UWorld* UItemBase::GetWorldFromOuter() const
{
	if(GIsEditor && !GIsPlayInEditorWorld)
	{
		return nullptr; 
	}
	if(GetOuter())
	{
		return GetOuter()->GetWorld(); 
	}
	else
	{
		return nullptr; 
	}
}

bool operator==(const UItemBase& lhs, const UItemBase& rhs)
{
	return (lhs.item_info.NameShort.EqualTo(rhs.item_info.NameShort, ETextComparisonLevel::Quinary) 
		&& lhs.item_info.Mass == rhs.item_info.Mass
		&& lhs.item_info.BasePrice == rhs.item_info.BasePrice
		&& lhs.item_info.Dimensions == rhs.item_info.Dimensions
		);
}
