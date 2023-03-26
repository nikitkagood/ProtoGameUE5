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
UCLASS(BlueprintType, DefaultToInstanced, meta=(BlueprintSpawnableComponent), ClassGroup=(Inventory))
class PROTOGAME_API UInventoryComponent : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	UFUNCTION(BlueprintCallable)
	bool AddItemFromWorld(UItemBase* item); //add item; currently items destroy themselves

	UFUNCTION(BlueprintCallable)
	bool MoveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell, bool widget_rotation);

	//checks for empty cells and cells with item idx; used by UI;
	//works for both items within the inventory and outside
	UFUNCTION(BlueprintCallable)
	bool CheckSpaceMove(FIntPoint upper_left_cell, UItemBase* item, FIntPoint dimensions); 

	UFUNCTION(BlueprintCallable)
	bool RotateItem(UItemBase* item);

	UFUNCTION(BlueprintCallable)
	bool Contains(UItemBase* item);

	//for debugging, print grid values to viewport 
	UFUNCTION(BlueprintCallable)
	void PrintInventory(); 

	UFUNCTION(BlueprintCallable)
	void PrintDebugInfo(); 

	UFUNCTION(BlueprintGetter)
	TMap<UItemBase*, int32> GetItems() const { return Items; };

	//Inventory interface
	virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination) override;
	virtual bool MoveItemToInventoryInGrid(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell) override;
	virtual bool DropItemToWorld(UItemBase* item) override;
	virtual bool ReceiveItem(UItemBase* item) override;
	virtual bool ReceiveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell) override;
	virtual void UpdateInventory() override { OnInventoryUpdated.Broadcast(); };

	UPROPERTY(BlueprintAssignable)
	FOnInventoryUpdated OnInventoryUpdated;
protected:
	virtual void BeginPlay() override;

	bool AddItem(UItemBase* item); //finds free space then adds; it doesn't destroy ItemActors, does minimal checks; Must set World and Outer
	bool AddItemAt(UItemBase* item, FIntPoint new_upper_left_cell); //adds at pre-determined place 
	bool RemoveItem(UItemBase* item); //finds then removes; does minimal checks; Doesn't clear World and Outer because there Add to another inventory already overrides these;
	bool RemoveItemAt(UItemBase* item, FIntPoint upper_left_cell, FIntPoint lower_right_cell); //removes from pre-determined place

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = true))
	FName InventoryName;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true, ClampMin = 1))
	int32 Rows;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true, ClampMin = 1))
	int32 Columns;

    UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	int32 FreeSpaceLeft;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	float Mass;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	float DropDistance;

private:
	void SetupDefaults();

	void InitializeInventoryGrid(int32 rows, int32 columns); 

	FIntPoint FindFreeSpaceInGrid(UItemBase* item);

	TPair<FIntPoint, FIntPoint> FindItemPosition(UItemBase* item) const;

	//ñhecks for free space only, i.e. -1
	bool CheckSpace(FIntPoint upper_left_cell, UItemBase* item);

	void FillSpaceInGrid(FIntPoint upper_left_cell, FIntPoint lower_right_cell, int32 item_idx);

	void UpdateFreeSpaceLeft(int32 space_change);

	int32 GenerateIndex();

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	TMap<UItemBase*, int32> Items; //item pointer + it's index in the grid

	TArray<int32> free_indecies;
	
	TArray<TArray<int32>> InventoryGrid; //-1 is free space; 0 and so on - some item

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	TArray<ItemType> SupportedTypes; //TODO: not implemented yet; types this inventory can store, 0 means all
};
