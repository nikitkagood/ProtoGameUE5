// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryManager.h"
#include "Item/EnumItemTypes.h"
#include "GameplayTagContainer.h"

#include "Interfaces/InventoryInterface.h"

#include "InventoryComponent.generated.h"

class UItemBase;
class ACharacter;
class UInvSpecialSlotComponent;

UENUM(BlueprintType)
enum class EItemStackResult : uint8
{
	NotStacked = 0,
	StackedPartially, //added item trasfered some amount to any inventory item
	StackedFully //added item transfered everything and has to be destroyed
};

//TODO: NOT IMPLEMENTED YET
//so the idea is to optimize out whole grid thing and only generate it on-demand
//
//MassEntity with custom data as TArray<byte> with multiple MassFragments: basic inventory, firearm-specific fragment, health fragment etc
//UENUM(BlueprintType)
//enum class EInventoryComponentMode : uint8
//{
//	StandardGrid,
//	LazyOptimized
//};

//NOTE: only Item pointer is considered for comparison
USTRUCT(BlueprintType)
struct FInventoryGridItemWrapper
{
	GENERATED_BODY()

	//DO NOT CHANGE ORDER, THIS HAS TO BE FIRST
	UPROPERTY(BlueprintReadOnly)
	UItemBase* Item = nullptr;

	UPROPERTY(BlueprintReadOnly)
	int32 LocalInvID = -1;

	FORCEINLINE bool operator == (const FInventoryGridItemWrapper& other) const
	{
		return Item == other.Item;
	}
};

//Represents grid inventory
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew, ClassGroup = (Inventory), meta=(BlueprintSpawnableComponent, DisplayName = "Inventory Component"))
class PROTOGAME_API UInventoryComponent : public UObject, public IInventoryInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	UFUNCTION(BlueprintCallable)
	void Initialize(FIntPoint dimensions, FName name = "InventoryComp_DefaultName");
	//void Initialize(FIntPoint dimensions, FName name = "", float drop_distance);

	bool IsGridInitialized() const;

	UFUNCTION(BlueprintCallable)
	FIntPoint GetDimensions() const { return { Rows, Columns }; };

	UFUNCTION(BlueprintCallable)
	void SetInventoryName(FName name) { InventoryName = name; }

	//Move item internally
	UFUNCTION(BlueprintCallable)
	bool MoveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell, bool widget_rotation);

	//Checks for empty cells and cells with item idx; used by UI;
	//Works for both items within tis inventory and another InventoryComponents
	UFUNCTION(BlueprintCallable)
	bool CheckSpaceMove(const FIntPoint upper_left_cell, UItemBase* item, FIntPoint dimensions); 

	UFUNCTION(BlueprintCallable)
	bool RotateItem(UItemBase* item);

	//local id in the grid
	//or INDEX_NONE if not found
	UFUNCTION(BlueprintCallable)
	int32 FindItemLocalID(UItemBase* item) const;

	UFUNCTION(BlueprintCallable)
	bool Contains(UItemBase* item) const;

	//for debugging, print grid values to viewport 
	UFUNCTION(BlueprintCallable)
	void PrintInventory(); 

	UFUNCTION(BlueprintCallable)
	void PrintDebugInfo(); 

	UFUNCTION(BlueprintGetter)
	TArray<FInventoryGridItemWrapper> GetItems() const { return Items; };

	//IInventoryInterface
	//virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination) override;
	virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell) override;
	virtual bool AddItemFromWorld(UItemBase* item) override;
	virtual bool DropItemToWorld(UItemBase* item) override;
	//virtual bool ReceiveItem(UItemBase* item) override;
	virtual bool ReceiveItem(UItemBase* item, FIntPoint new_upper_left_cell) override;
	virtual void UpdateStackDependencies(UItemBase* item, int32 new_stack_size) override;
	virtual void UpdateInventory() override { OnInventoryUpdated.Broadcast(this); };
	//Since InventoryComponent is ActorComponent, it can only have Actors as Owner/Outer
	//Thus you can't get hierarchy from InventoryComponent and there is no point it calling this method (at the moment at least)
	virtual TScriptInterface<IInventoryInterface> GetOuterUpstreamInventory() const override { return nullptr; };
	virtual UObject* GetInventoryOwner() override { return GetOuter(); };
	bool SetInventoryOwner(UObject* new_owner);
	virtual FGameplayTagContainer GetInventoryTags() const { return Tags; };
	//IInventoryInterface end

	//Inventories outer can be Item
	//UItemBase* GetOuterItem() const;


	UPROPERTY(BlueprintAssignable)
	FOnInventoryUpdated OnInventoryUpdated;
protected:
	//virtual void BeginPlay() override;
	virtual void PostInitProperties() override;

	//Finds free space then adds
	//It doesn't destroy ItemActors, does minimal checks
	//Must set World and Outer
	bool AddItem(UItemBase* item); 
	//Adds item at pre-determined place 
	bool AddItemAt(UItemBase* item, FIntPoint new_upper_left_cell); 
	//Finds then removes
	//Not lock guarded, does minimal checks 
	//Doesn't clear World and Outer because there Add to another inventory already overrides these
	bool RemoveItem(UItemBase* item); 
	//Removes from pre-determined place
	bool RemoveItemAt(UItemBase* item, FIntPoint upper_left_cell, FIntPoint lower_right_cell);

	void ChangeMass(float value);

	//Try to find an item to stack with
	//Successful stacking is not recursive: if anything stacked, then it stops
	// even if we could find another item to stack with
	EItemStackResult TryToStack(UItemBase* item);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	FName InventoryName;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = true, ClampMin = 1, ClampMax = 1000000, ExposeOnSpawn = true))
	int32 Rows;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = true, ClampMin = 1, ClampMax = 1000000, ExposeOnSpawn = true))
	int32 Columns;

    UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	int32 FreeSpaceLeft;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	float Mass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = true))
	bool DropDistanceOverride = false;

	//By default InventoryManager's value is used
	//But if this comp is not managed or DropDistanceOverride == true
	// then this value is used
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = true))
	float DropDistance;

private:
	FORCEINLINE void SetupDefaults();

	void InitializeInventoryGrid(); 

	//INDEX_NONE if false
	FIntPoint FindFreeSpaceInGrid(UItemBase* item) const;

	//LeftUpperCell, RightLowerCell
	TPair<FIntPoint, FIntPoint> FindItemPosition(UItemBase* item) const;

	//Checks for free space only, i.e. -1
	bool CheckSpace(FIntPoint upper_left_cell, UItemBase* item) const;

	void FillSpaceInGrid(FIntPoint upper_left_cell, FIntPoint lower_right_cell, int32 item_idx);

	void UpdateFreeSpaceLeft(int32 space_change);

	//If free_indecies not empty then use it,
	//otherwise create a new one
	int32 GenerateLocalInvIndex();

	//Check whether we try to place item inside itself
	UFUNCTION(BlueprintCallable)
	virtual bool CheckSelfRecursion(UItemBase* item) const;

	int32 GridToArrayIndex(int32 row, int32 col) const;
	FIntPoint ArrayIndexToGrid(int32 idx) const;

//#if WITH_EDITOR
//	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
//#endif

	//Item pointer + it's index in the grid
	//TArray is now used instead of TMap; It's worse at individual operations, 
	// but should be better when iterating over everything (not tested)
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	TArray<FInventoryGridItemWrapper> Items;

	UPROPERTY()
	TArray<int32> free_indices;
	
	//Flattened 2D Array; Use GridToArrayIndex to acces it like 2D
	//-1 is free space; 0 and so on - some item
	UPROPERTY(VisibleAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = true))
	TArray<int32> InventoryGrid;

	//TODO: not implemented yet; 
	//Types this inventory can store, 0 means all
	//UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	//TArray<ItemType> SupportedTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (Categories = "Inventory", AllowPrivateAccess = true))
	FGameplayTagContainer Tags;

	mutable FCriticalSection InventoryMutex;

	static constexpr int GRID_EMPTY_SPACE = -1;
};
