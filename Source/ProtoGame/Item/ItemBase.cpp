// Nikita Belov, All rights reserved


#include "ItemBase.h"

#include "Character/GameCharacterBase.h"
#include "Inventory/InventoryManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "ItemActor.h"

#include "Engine/StaticMesh.h"

UItemBase::UItemBase()
{
	upper_left_cell = { -1, -1 };
	bRotated = false;
}

void UItemBase::PostInitProperties()
{
	Super::PostInitProperties();

	if (!item_guid.guid.IsValid())
	{
		item_guid = CreateItemGuid();
	}
}

bool UItemBase::Initialize(FDataTableRowHandle handle)
{ 
	auto* ptr_row = handle.GetRow<DataTableType>("UItemBase::Initialize");

	if(ptr_row != nullptr)
	{
		inventory_item_info = ptr_row->inventory_item_info;
		return true;
	}
	else 
	{
		//return InitializeFromInstancedStruct(handle);
	}

	return false;
}

//bool UItemBase::InitializeFromInstancedStruct(FDataTableRowHandle handle)
//{
//	auto* ptr_row_inst = handle.GetRow<FItemInfoInstanced>("UItemBase::Initialize");
//
//	if (ptr_row_inst != nullptr)
//	{
//		inventory_item_info = ptr_row_inst->inventory_item_info;
//
//		//EXAMPLE:
//		//for (const auto&inst_struct : ptr_row_inst->intstanced_structs)
//		//{
//		//	if (inst_struct.GetPtr</*YOUR_TYPE_HERE*/>() != nullptr)
//		//	{
//
//		//	}
//		//}
//
//		return true;
//	}
//
//	return false;
//}


bool UItemBase::OnInteracted(AActor* caller, EInteractionActions action)
{
	auto inv_manager = caller->FindComponentByClass<UInventoryManager>();

	//Inventory manager dependent actions
	if(IsValid(inv_manager))
	{
		auto cached_item_actor = GetOuterItemActor();

		switch (action)
		{
		case EInteractionActions::Default:
			if (inv_manager->AddItemFromWorld(this))
			{
				cached_item_actor->Destroy();
				return true;
			}
			break;
		case EInteractionActions::Equip:
			if (inv_manager->AddItemFromWorld(this, EManagerInventoryType::SpecialSlot))
			{
				cached_item_actor->Destroy();
				return true;
			}
			break;
		case EInteractionActions::Take:
			if (inv_manager->AddItemFromWorld(this, EManagerInventoryType::InventoryComponent))
			{
				cached_item_actor->Destroy();
				return true;
			}
			break;
		default:
			ensureMsgf(false, TEXT("UItemBase::OnInteracted: no match for action"));
			break;
		}
	}

	//Other actions
	switch (action)
	{
	case EInteractionActions::Use:
		OnUse(caller);
		break;
	case EInteractionActions::Consume:
		OnUse(caller);
		break;
	default:
		ensureMsgf(false, TEXT("UItemBase::OnInteracted: no match for action"));
		break;
	}

	return false;
}

TArray<EInteractionActions> UItemBase::GetInteractionActions() const
{
	return TArray<EInteractionActions>{ EInteractionActions::Default };
}

bool UItemBase::OnUse(AActor* caller)
{
	ensureMsgf(false, TEXT("UItemBase::OnUse is meant to be overriden")); 
	return false;
}

FItemGuid UItemBase::CreateItemGuid()
{
	if (item_guid.guid.IsValid())
	{
		return item_guid;
	}

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		return FItemGuid(FGuid::NewGuid());
	}

	return {};
}

bool UItemBase::IsEqualForStack(UItemBase* other_item, const FName& tag_filter) const
{
	bool result = false;
	
	bool is_dimensions_equal = (inventory_item_info.Dimensions.X == other_item->inventory_item_info.Dimensions.X && inventory_item_info.Dimensions.Y == other_item->inventory_item_info.Dimensions.Y)
		|| (inventory_item_info.Dimensions.X == other_item->inventory_item_info.Dimensions.Y && inventory_item_info.Dimensions.Y == other_item->inventory_item_info.Dimensions.X);

	result = is_dimensions_equal &&
		GetClass() == other_item->GetClass() &&
		ItemActorClass == other_item->ItemActorClass &&
		inventory_item_info.Type == other_item->inventory_item_info.Type;

	if (result == true)
	{
		result = CompareTags(other_item, tag_filter);
	}

	return result;
}

bool UItemBase::CompareTags(UItemBase* other_item, const FName& tag_filter) const
{
	if (tag_filter == FName{} || tag_filter.IsNone())
	{
		inventory_item_info.Tags == other_item->inventory_item_info.Tags;
	}

	FGameplayTag ParentTag = FGameplayTag::RequestGameplayTag(tag_filter);

	FGameplayTagContainer FilteredA;
	FGameplayTagContainer FilteredB;

	for (const auto& tag : inventory_item_info.Tags)
	{
		if (tag.MatchesTag(ParentTag))
		{
			FilteredA.AddTag(tag);
		}
	}

	for (const auto& tag : other_item->inventory_item_info.Tags)
	{
		if (tag.MatchesTag(ParentTag))
		{
			FilteredB.AddTag(tag);
		}
	}

	return FilteredA == FilteredB;
}

void UItemBase::SetCurrentStackSize(int32 new_size)
{
	auto inventory = GetOuterUpstreamInventory();

	if(inventory != nullptr)
	{
		inventory->UpdateStackDependencies(this, new_size);
	}

	inventory_item_info.CurrentStackSize = new_size;
}

AItemActor* UItemBase::SpawnItemActor(const FVector& location, const FRotator& rotation, const FItemActorSpawnParameters& spawn_parameters)
{
	return AItemActor::StaticCreateObject(GetWorld(), ItemActorClass, this, spawn_parameters, location, rotation);
}

//AItemActor* UItemBase::SpawnItemActorVisualOnly(const FVector& location, const FRotator& rotation)
//{
//	return AItemActor::StaticCreateObjectVisualOnly(GetWorld(), ItemActorClass, this, location, rotation);
//}

void UItemBase::Rotate()
{
	Swap(inventory_item_info.Dimensions.X, inventory_item_info.Dimensions.Y);
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
		this->inventory_item_info.CurrentStackSize += other->GetCurrentStackSize();
		other->inventory_item_info.CurrentStackSize = 0;

		return true;
	}
	else
	{
		this->inventory_item_info.CurrentStackSize = this->GetMaxStackSize();
		other->inventory_item_info.CurrentStackSize = stack_remainder;

		return true;
	}
}

void UItemBase::SetMass(float new_mass)
{
	inventory_item_info.Mass = new_mass;

	//round to MassMaxPrecision
	const float multiplier = FMath::Pow(10.0f, FInventoryItemInfo::MassMaxPrecision);
	inventory_item_info.Mass = FMath::RoundHalfFromZero(inventory_item_info.Mass * multiplier) / multiplier;
}

void UItemBase::ChangeMass(float change_in_mass)
{
	inventory_item_info.Mass += change_in_mass;

	//round to MassMaxPrecision
	const float multiplier = FMath::Pow(10.0f, FInventoryItemInfo::MassMaxPrecision);
	inventory_item_info.Mass = FMath::RoundHalfFromZero(inventory_item_info.Mass * multiplier) / multiplier;
	
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

UItemBase* UItemBase::StackGetSplit(int32 amount, UObject* new_outer)
{
	int32 remainder = inventory_item_info.CurrentStackSize - amount;

	if(amount < 1 || remainder < 0)
	{
		checkf(false, TEXT("Error: StackGetSplit: Wrong amount"))
	}

	//if(remainder == 0)
	//{
	//	return this;
	//}

	//UItemBase* new_obj = DuplicateObject<UItemBase>(this, new_outer);
	UItemBase* new_obj = StaticCreateObject<UItemBase>(new_outer, this->GetClass(), ItemObjectCreationMethod::CreateItemObjectFromDataTable, ItemActorClass.GetDefaultObject()->GetItemProperites());
	new_obj->inventory_item_info.CurrentStackSize = amount;

	//this->inventory_item_info.CurrentStackSize = remainder;
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

bool UItemBase::SetOuterUpstreamInventory(TScriptInterface<IInventoryInterface> inventory)
{
	return this->Rename(nullptr, inventory.GetObject());;
}

bool UItemBase::SetOuterItemActor(AItemActor* item_actor)
{ 
	return this->Rename(nullptr, Cast<UObject>(item_actor));
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

void UItemBase::OnDestroy()
{
	auto owning_item_actor = GetOuterItemActor();
	if (owning_item_actor)
	{
		owning_item_actor->Destroy();
	}

	MarkAsGarbage();
}

AActor* UItemBase::GetOwner() const
{
	auto* actor_comp_owner = Cast<UActorComponent>(GetOuter());

	if (actor_comp_owner != nullptr)
	{
		return actor_comp_owner->GetOwner();
	}

	return Cast<AActor>(GetOuter());
}

//TODO: some items may have different dimensions, like weapons
//bool operator==(const UItemBase& lhs, const UItemBase& rhs)
//{
//	return (lhs.inventory_item_info.NameShort.EqualTo(rhs.inventory_item_info.NameShort, ETextComparisonLevel::Quinary) 
//		&& lhs.inventory_item_info.Mass == rhs.inventory_item_info.Mass
//		&& lhs.inventory_item_info.BasePrice == rhs.inventory_item_info.BasePrice
//		&& lhs.inventory_item_info.Dimensions == rhs.inventory_item_info.Dimensions
//		);
//}
