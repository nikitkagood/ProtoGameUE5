// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Interfaces/InteractionInterface.h"
#include "Interfaces/UseInterface.h"
#include "EnumItemTypes.h"
#include "InventoryItemInfo.h"
#include "ItemActor.h"

#include "ItemBase.generated.h"

class UStaticMesh;
class AGameCharacterBase;
class AItemActor;
class IInventoryInterface;
enum class ItemObjectCreationMethod : uint8;

//Unique ID for each item instance
USTRUCT(BlueprintType)
struct FItemGuid {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear)
	FGuid guid;
};

//This class represents items without phyisical location, world-independent - ItemObject.
//ItemActor - is used to represent items in a world.
// 
//ItemObject is supposed to contain all the information and behaviour of an Item,
// unless it's strictly world-dependent.
//Usually represented by an Inventory.
//
//You can combine ItemObject with something else, like ItemObject + SkeletalMesh = weapon in hands of a character.
UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, meta=(BlueprintSpawnableComponent), ClassGroup=Item)
class PROTOGAME_API UItemBase : public UObject, public IUseInterface
{
	GENERATED_BODY()

private:
	//What type data table has which stores information needed for this class
	using DataTableType = FItemTable;

public:	 
	UItemBase();

	template<typename T>
	static T* StaticCreateObject(UObject* outer, TSubclassOf<UItemBase> item_base_class, ItemObjectCreationMethod item_object_creation_method, FDataTableRowHandle dt_item_properties = {});

	virtual void PostInitProperties() override;

	//IMPORTANT: To be overriden in child classes: 

	virtual bool Initialize(FDataTableRowHandle handle);

	//TODO: it's not really used yet
	//Initialize from FInstancedStruct
	//virtual bool InitializeFromInstancedStruct(FDataTableRowHandle handle);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual FItemThumbnailInfo GetItemThumbnailInfoFromDT();

	////To be overriden in child classes end

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FInventoryItemInfo& GetInventoryItemInfo() { return inventory_item_info; }

	void SetItemActorClass(TSubclassOf<AItemActor> value) { ItemActorClass = value; };

protected:
	//We use this function to avoid code duplication
	//But to reamain BP interface and class-specific implementation
	template<typename DataTableType>
	FItemThumbnailInfo GetItemThumbnailInfoFromDT_Impl();

	void SetCurrentStackSize(int32 new_size);

public:
	UFUNCTION(BlueprintCallable)
	const FItemGuid& GetItemGuid() const { return item_guid; };
	UFUNCTION(BlueprintCallable)
	FGameplayTag GetType() const { return inventory_item_info.Type; };
	UFUNCTION(BlueprintCallable)
	const FText& GetItemName() const { return inventory_item_info.Name; }
	UFUNCTION(BlueprintCallable)
	const FText& GetItemNameShort() const { return inventory_item_info.NameShort; }
	UFUNCTION(BlueprintCallable)
	const FText& GetDescription() const { return inventory_item_info.Description; };
	UFUNCTION(BlueprintCallable)
	const FText& GetUseActionText() const { return inventory_item_info.UseActionText; };
	UFUNCTION(BlueprintCallable)
	float GetMassOneUnit() const { return inventory_item_info.Mass; };
	UFUNCTION(BlueprintCallable)
	float GetMassTotal() const { return inventory_item_info.Mass * inventory_item_info.CurrentStackSize; };
	UFUNCTION(BlueprintCallable)
	virtual FIntPoint GetDimensions() const { return inventory_item_info.Dimensions; };
	UFUNCTION(BlueprintCallable)
	TSoftObjectPtr<UTexture2DDynamic> GetThumbnail() const { return inventory_item_info.Thumbnail; };
	UFUNCTION(BlueprintCallable)
	void SetThumbnail(UTexture2DDynamic* dynamic_texture) { inventory_item_info.Thumbnail = dynamic_texture; };
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentStackSize() const { return inventory_item_info.CurrentStackSize; };
	UFUNCTION(BlueprintCallable)
	int32 GetMaxStackSize() const { return inventory_item_info.MaxStackSize; };

public:

	UFUNCTION(BlueprintCallable)
	void SetMass(float new_mass);
	UFUNCTION(BlueprintCallable)
	void ChangeMass(float change_in_mass);

	//Default way to get the mesh. Since it's already used by ItemActor.
	UFUNCTION(BlueprintCallable)
	UStaticMesh* GetStaticMeshFromItemActorCDO() const;
	UFUNCTION(BlueprintCallable)
	USkeletalMesh* GetSkeletalMeshFromItemActorCDO() const;

	//We use Outer to determine who this ItemObject belongs to
	//InventoryInterface and ItemActor are 2 supported variants
	//Any other Outer isn't strictly prohibited but not supported either
	UFUNCTION(BlueprintCallable)
	TScriptInterface<IInventoryInterface> GetOuterUpstreamInventory() const; 
	UFUNCTION(BlueprintCallable)
	AItemActor* GetOuterItemActor() const;

	//Also checks whether this Outer can be set
	UFUNCTION(BlueprintCallable)
	virtual bool SetOuterUpstreamInventory(TScriptInterface<IInventoryInterface> inventory);
	UFUNCTION(BlueprintCallable)
	virtual bool SetOuterItemActor(AItemActor* item_actor);

	UFUNCTION(BlueprintCallable)
	FIntPoint GetUpperLeftCell() const { return upper_left_cell; }
	UFUNCTION()
	void SetUpperLeftCell(FIntPoint value) { upper_left_cell = value; };
	UFUNCTION(BlueprintCallable)
	FIntPoint GetLowerRightCell() const { return { upper_left_cell.X + GetDimensions().X - 1, upper_left_cell.Y + GetDimensions().Y - 1 }; }
	UFUNCTION(BlueprintCallable)
	bool GetRotated() const { return bRotated; }

	UFUNCTION(BlueprintCallable)
	virtual USkeletalMeshComponent* CreateSKForSceneCapture(USceneComponent* outer) { check(false); return nullptr; };

	UPROPERTY(Transient)
	class UWorld* World; //must be set by InventoryComponent

	UFUNCTION(BlueprintCallable)
	virtual class UWorld* GetWorld() const { return World; }

	UFUNCTION(BlueprintCallable)
	virtual class UWorld* GetWorldFromOuter() const;

	UFUNCTION(BlueprintPure)
	virtual AActor* GetOwner() const;

	//Spawn ItemActor and set it as ItemBase Outer
	//TODO: Currently this system isn't reliable enough. Though collision is handled, it's probably still possible to spawn small objects inside walls.
	UFUNCTION()
	virtual AItemActor* SpawnItemActor(const FVector& location, const FRotator& rotation);

	//Spawn ItemActor which is purely visual
	UFUNCTION(BlueprintCallable)
	virtual AItemActor* SpawnItemActorVisualOnly(const FVector& location, const FRotator& rotation);

    UFUNCTION(BlueprintCallable)
	void Rotate();

	//Adds another stack to this one
	UFUNCTION()
	bool StackAdd(UItemBase* other);

	//Returns new Item with desired amount in stack
	UFUNCTION()
	UItemBase* StackGetSplit(int32 ammount, UObject* new_outer);

	//Probably should not be used
	//Since it's pretty ambiguous what to consider equal
	friend bool operator== (const UItemBase& lhs, const UItemBase& rhs) = delete;

	//Checks if items are equal for stacking purposes
	//Tag filter: only check for this tag and it's descendants
	//by default tag_filter is Item
	virtual bool IsEqualForStack(UItemBase* other_item, const FName& tag_filter = "Item") const;

	//tag_filter - only consider this tag and it's decsendants
	//if tag_filter empty or none, all tags are compared
	virtual bool CompareTags(UItemBase* other_item, const FName& tag_filter) const;

	//ItemActors ask ItemObject what to do
	//This behaviour might be reduntant and may change later
	virtual bool Interact(AActor* caller, EInteractionActions action);

	//Interfaces

	UFUNCTION(BlueprintCallable)
	virtual bool OnUse(AActor* caller) override;
protected:
	virtual FItemGuid CreateItemGuid();
protected:

	//Values shared between instance and DataTable
    UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = true))
	FInventoryItemInfo inventory_item_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = true))
	TSubclassOf<AItemActor> ItemActorClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bRotated;
private:
	//Unique ID for each item instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, meta = (AllowPrivateAccess = true))
	FItemGuid item_guid;

	//position in grid inventory
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FIntPoint upper_left_cell; 

};

template<typename ItemType>
inline ItemType* UItemBase::StaticCreateObject(UObject* outer, TSubclassOf<UItemBase> item_base_class, ItemObjectCreationMethod item_object_creation_method, FDataTableRowHandle dt_item_properties)
{
	ItemType* item_object = nullptr;

	if(item_object_creation_method == ItemObjectCreationMethod::CreateItemObjectFromDataTable)
	{
		item_object = NewObject<ItemType>(outer, item_base_class);
		if(item_object == nullptr)
		{
			UE_LOG(LogTemp, Fatal, TEXT("Failed to create ItemObject. Maybe there is something wrong with assigned class."));
			return nullptr;
		}
		if(item_object->Initialize(dt_item_properties) == false)
		{
			UE_LOG(LogTemp, Fatal, TEXT("Failed to set item properties. Maybe there is something wrong with the data table."));
			return nullptr;
		}
		item_object->SetItemActorClass(outer->GetClass()); //what ItemActor to spawn when ItemBase is dropped to world
		//item_object->SetOuterItemActor(outer);
	}
	else if(item_object_creation_method == ItemObjectCreationMethod::CreateItemObjectFromItemBaseBP)
	{
		//TODO: might now work now
		item_object = NewObject<ItemType>(outer, item_base_class);
		item_object->SetItemActorClass(outer->GetClass()); //what ItemActor to spawn when ItemBase is dropped to world
		//item_object->SetOuterItemActor(outer);
	}

	return item_object;
}

template<typename DataTableType>
inline FItemThumbnailInfo UItemBase::GetItemThumbnailInfoFromDT_Impl()
{
	auto handle = ItemActorClass.GetDefaultObject()->GetItemProperites();

	auto* ptr_row = handle.GetRow<DataTableType>("UItemBase::GetItemThumbnailInfo");

	if(ptr_row != nullptr)
	{
		return ptr_row->item_thumbnail_info;
	}

	checkf(false, TEXT("Failed to get ItemThumbnailInfo"));
	return FItemThumbnailInfo{};
}