// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/EnumItemTypes.h"
#include "Interfaces/InventoryInterface.h"

#include "InventoryComponent.generated.h"

class UItemBase;
class ACharacter;
class UInvSpecialSlotComponent;

//Represents grid inventory
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew, ClassGroup = (Inventory), meta=(BlueprintSpawnableComponent, DisplayName = "Inventory Component"))
class PROTOGAME_API UInventoryComponent : public UActorComponent, public IInventoryInterface
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

	UFUNCTION(BlueprintCallable)
	bool Contains(UItemBase* item) const;

	//for debugging, print grid values to viewport 
	UFUNCTION(BlueprintCallable)
	void PrintInventory(); 

	UFUNCTION(BlueprintCallable)
	void PrintDebugInfo(); 

	UFUNCTION(BlueprintGetter)
	TMap<UItemBase*, int32> GetItems() const { return Items; };

	//IInventoryInterface
	//virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination) override;
	virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell) override;
	virtual bool AddItemFromWorld(UItemBase* item) override;
	virtual bool DropItemToWorld(UItemBase* item) override;
	//virtual bool ReceiveItem(UItemBase* item) override;
	virtual bool ReceiveItem(UItemBase* item, FIntPoint new_upper_left_cell) override;
	virtual void UpdateStackDependencies(UItemBase* item, int32 new_stack_size) override;
	virtual void UpdateInventory() override { OnInventoryUpdated.Broadcast(); };
	//Since InventoryComponent is ActorComponent, it can only have Actors as Owner/Outer
	//Thus you can't get hierarchy from InventoryComponent and there is no point it calling this method (at the moment at least)
	virtual TScriptInterface<IInventoryInterface> GetOuterUpstreamInventory() const override { return nullptr; };
	virtual UObject* GetInventoryOwner() override { return Cast<UObject>(GetOwner()); };
	bool SetInventoryOwner(UObject* new_owner);
	//IInventoryInterface end

	//Inventories outer can be Item
	//UItemBase* GetOuterItem() const;


	UPROPERTY(BlueprintAssignable)
	FOnInventoryUpdated OnInventoryUpdated;
protected:
	virtual void BeginPlay() override;

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	FName InventoryName;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = true, ClampMin = 1, ExposeOnSpawn = true))
	int32 Rows;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = true, ClampMin = 1, ExposeOnSpawn = true))
	int32 Columns;

    UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	int32 FreeSpaceLeft;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	float Mass;

	//TODO: may not be useful
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	float DropDistance;

private:
	void SetupDefaults();

	void InitializeInventoryGrid(int32 rows, int32 columns); 

	FIntPoint FindFreeSpaceInGrid(UItemBase* item) const;

	TPair<FIntPoint, FIntPoint> FindItemPosition(UItemBase* item) const;

	//Checks for free space only, i.e. -1
	bool CheckSpace(FIntPoint upper_left_cell, UItemBase* item) const;

	void FillSpaceInGrid(FIntPoint upper_left_cell, FIntPoint lower_right_cell, int32 item_idx);

	void UpdateFreeSpaceLeft(int32 space_change);

	int32 GenerateIndex();

	//True means recursion found
	//This function assumes InventoryComponent Outer = ItemObject with InventoryComponent
	//Implementation has to be changed if Outer is something else
	UFUNCTION(BlueprintCallable)
	virtual bool CheckSelfRecursion(UItemBase* item) const;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	TMap<UItemBase*, int32> Items; //item pointer + it's index in the grid

	TArray<int32> free_indices;
	
	TArray<TArray<int32>> InventoryGrid; //-1 is free space; 0 and so on - some item

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	TArray<ItemType> SupportedTypes; //TODO: not implemented yet; types this inventory can store, 0 means all


	mutable FCriticalSection InventoryMutex;
};
