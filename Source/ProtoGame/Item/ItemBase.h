// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Interfaces/InteractionInterface.h"
#include "Interfaces/UseInterface.h"
#include "EnumItemTypes.h"
#include "ItemInfo.h"
#include "ItemActor.h"

#include "ItemBase.generated.h"

class UStaticMesh;
class AGameCharacterBase;
class AItemActor;
class IInventoryInterface;
enum class ItemObjectCreationMethod : uint8;

//This class represents items without phyisical location, world-independent.
//It represents actual properties of an item, as opposed to ItemActor which is used to represent physical location in a world.
//Note that items attached to a character ARE NOT considered ItemActors. They are ItemBase + SkeletalMesh (although this might change).
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

	virtual bool SetProperties(FDataTableRowHandle handle);

	void SetItemActorClass(TSubclassOf<AItemActor> value) { ItemActorClass = value; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FItemInfo& GetItemInfo() { return item_info; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual FItemThumbnailInfo GetItemThumbnailInfoFromDT();
protected:
	//We use this function to avoid code duplication
	//But to reamain BP interface and class-specific implementation
	template<typename DataTableType>
	FItemThumbnailInfo GetItemThumbnailInfoFromDT_Impl();

public:

	UFUNCTION(BlueprintCallable)
	const FText& GetItemName() const { return item_info.Name; }
	UFUNCTION(BlueprintCallable)
	const FText& GetItemNameShort() const { return item_info.NameShort; }
	UFUNCTION(BlueprintCallable)
	const FText& GetDescription() const { return item_info.Description; };
	UFUNCTION(BlueprintCallable)
	const FText& GetUseActionText() const { return item_info.UseActionText; };
    UFUNCTION(BlueprintCallable)
	ItemType GetType() const { return item_info.Type; };
	UFUNCTION(BlueprintCallable)
	float GetMassOneUnit() const { return item_info.Mass; };
	UFUNCTION(BlueprintCallable)
	float GetMassTotal() const { return item_info.Mass * item_info.CurrentStackSize; };
	UFUNCTION(BlueprintCallable)
	FIntPoint GetDimensions() const { return item_info.Dimensions; };
	UFUNCTION(BlueprintCallable)
	int32 GetRows() const { return item_info.Dimensions.X; };
	UFUNCTION(BlueprintCallable)
	int32 GetColumns() const { return item_info.Dimensions.Y; };
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentStackSize() const { return item_info.CurrentStackSize; };
	UFUNCTION(BlueprintCallable)
	int32 GetMaxStackSize() const { return item_info.MaxStackSize; };
	UFUNCTION(BlueprintCallable)
	TSoftObjectPtr<UTexture2DDynamic> GetThumbnail() const { return item_info.Thumbnail; };
	UFUNCTION(BlueprintCallable)
	void SetThumbnail(UTexture2DDynamic* dynamic_texture) { item_info.Thumbnail = dynamic_texture; };

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
	UFUNCTION(BlueprintCallable)
	void SetOuterUpstreamInventory(TScriptInterface<IInventoryInterface> inventory);
	UFUNCTION(BlueprintCallable)
	void SetOuterItemActor(AItemActor* item_actor);

	UFUNCTION(BlueprintCallable)
	FIntPoint GetUpperLeftCell() const { return upper_left_cell; }
	UFUNCTION()
	void SetUpperLeftCell(FIntPoint value) { upper_left_cell = value; };
	UFUNCTION(BlueprintCallable)
	FIntPoint GetLowerRightCell() const { return { upper_left_cell.X + GetRows() - 1, upper_left_cell.Y + GetColumns() - 1 }; }
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
	AActor* GetOwner() const { return Cast<AActor>(GetOuter()); };

	//Spawn ItemActor and set it as ItemBase Outer
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
	UItemBase* StackGet(int32 ammount, UObject* new_outer);

	//ItemActors ask ItemObject what to do
	//This behaviour might be reduntant and may change later
	virtual bool Interact(AActor* caller);

	//Interfaces

	UFUNCTION(BlueprintCallable)
	virtual bool OnUse(AActor* caller) override;

protected:
	void SetCurrentStackSize(int32 new_size);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = true))
	FItemInfo item_info;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bRotated;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = true))
	TSubclassOf<AItemActor> ItemActorClass;
private:
	//position in inventory
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FIntPoint upper_left_cell; 
public:

	//ItemBase items are considered to be identical when:
	//They have the same Name Short (for performance reasons)
	//Mass
	//Base price
	//Dimensions
	//TODO: some items may have different dimensions, like weapons
	friend bool operator== (const UItemBase& lhs, const UItemBase& rhs);
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
		if(item_object->SetProperties(dt_item_properties) == false)
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