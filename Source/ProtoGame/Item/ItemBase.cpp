// Nikita Belov, All rights reserved


#include "ItemBase.h"

#include "Character/GameCharacterBase.h"
//#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryManager.h"
#include "ItemActor.h"

#include "Engine/StaticMesh.h"

UItemBase::UItemBase()
{
	upper_left_cell = { -1, -1 };
	bRotated = false;
}

bool UItemBase::Initialize(FDataTableRowHandle handle)
{ 
	auto* ptr_row = handle.GetRow<DataTableType>("UItemBase::Initialize");

	if(ptr_row != nullptr)
	{
		item_info = ptr_row->item_info;
		return true;
	}

	return false;
}


bool UItemBase::Interact(AActor* caller)
{
	auto ptr = Cast<AGameCharacterBase>(caller);

	if(ptr != nullptr)
	{
		if(ptr->GetInventoryManger()->AddItemFromWorld(this))
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

void UItemBase::SetCurrentStackSize(int32 new_size)
{
	auto inventory = GetOuterUpstreamInventory();

	if(inventory != nullptr)
	{
		inventory->UpdateStackDependencies(this, new_size);
	}

	item_info.CurrentStackSize = new_size;
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
		checkf(false, TEXT("Eror: Item is invalid"))
		return false;
	}

	if(this->GetClass() != other->GetClass())
	{
		checkf(false, TEXT("Error: StackAdd: types aren't equal"))
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

UStaticMesh* UItemBase::GetStaticMeshFromItemActorCDO() const
{
	//Default way to get the mesh. Since it's already used by ItemActor.
	return ItemActorClass.GetDefaultObject()->GetStaticMeshComp()->GetStaticMesh();
}

USkeletalMesh* UItemBase::GetSkeletalMeshFromItemActorCDO() const
{
	//Default way to get the mesh. Since it's already used by ItemActor.
	return ItemActorClass.GetDefaultObject()->GetSkeletalMeshComp()->GetSkeletalMeshAsset();
}

UItemBase* UItemBase::StackGet(int32 amount, UObject* new_outer)
{
	int32 remainder = item_info.CurrentStackSize - amount;

	if(amount < 1 || remainder < 0)
	{
		checkf(false, TEXT("Error: StackGet: Wrong amount"))
	}

	//if(remainder == 0)
	//{
	//	return this;
	//}

	//UItemBase* new_obj = DuplicateObject<UItemBase>(this, new_outer);
	UItemBase* new_obj = StaticCreateObject<UItemBase>(new_outer, this->GetClass(), ItemObjectCreationMethod::CreateItemObjectFromDataTable, ItemActorClass.GetDefaultObject()->GetItemProperites());
	new_obj->item_info.CurrentStackSize = amount;

	//this->item_info.CurrentStackSize = remainder;
	this->SetCurrentStackSize(remainder);

	return new_obj;
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

FItemThumbnailInfo UItemBase::GetItemThumbnailInfoFromDT()
{
	return GetItemThumbnailInfoFromDT_Impl<DataTableType>();
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

//TODO: some items may have different dimensions, like weapons
bool operator==(const UItemBase& lhs, const UItemBase& rhs)
{
	return (lhs.item_info.NameShort.EqualTo(rhs.item_info.NameShort, ETextComparisonLevel::Quinary) 
		&& lhs.item_info.Mass == rhs.item_info.Mass
		&& lhs.item_info.BasePrice == rhs.item_info.BasePrice
		&& lhs.item_info.Dimensions == rhs.item_info.Dimensions
		);
}
