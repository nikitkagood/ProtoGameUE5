// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Interfaces/InventoryInterface.h"

#include "InventoryManager.generated.h"

//What inventories are considered when making operations with items
UENUM(BlueprintType)
enum class EManagerInventoryType : uint8
{
	Any = 0						UMETA(DisplayName = "Any"),
	SpecialSlot					UMETA(DisplayName = "SpecialSlot"),
	InventoryComponent			UMETA(DisplayName = "InventoryComponent"), 
};

//Connects multiple inventories into an organized system
//For example, character might have pockets, backpack, vest and whatever - 
//we don't want manually go through every possible inventory, we just ask this Manager
//Not used when we already know all required inventories, Drag and Drop for example
UCLASS(ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class PROTOGAME_API UInventoryManager : public UActorComponent //, public IInventoryInterface
{
	GENERATED_BODY()

public:
	UInventoryManager();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Manager specific methods which follow IInventoryInterface names

	UFUNCTION(BlueprintCallable)
	bool MoveItemToInventory(UItemBase* item, EManagerInventoryType inventory_type);
	//UFUNCTION(BlueprintCallable)
	//bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination);
	UFUNCTION(BlueprintCallable)
	bool MoveItemToInventoryInGrid(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell);
	UFUNCTION(BlueprintCallable)
	bool AddItemFromWorld(UItemBase* item, EManagerInventoryType inventory_type = EManagerInventoryType::Any);
	UFUNCTION(BlueprintCallable)
	bool DropItemToWorld(UItemBase* item);
	//UFUNCTION(BlueprintCallable)
	//bool ReceiveItem(UItemBase* item);
	//UFUNCTION(BlueprintCallable)
	//bool ReceiveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell);
	//UFUNCTION(BlueprintCallable)
	//void UpdateStackDependencies(UItemBase* item, int32 new_stack_size)  {  };
	//UFUNCTION(BlueprintCallable)
	//void UpdateInventory();
	//end

	//void AddInventory(IInventoryInterface* inventory, uint8 priority);

	//C++: do not call it in Character constructor
	UFUNCTION(BlueprintCallable)
	void AddInventory(TScriptInterface<IInventoryInterface> inventory);

	//void SetInventories(TArray<IInventoryInterface*>&& new_inventories);

	UFUNCTION(BlueprintCallable)
	void RemoveInventory(TScriptInterface<IInventoryInterface> inventory);

protected:
	virtual void BeginPlay() override;

private:
	//Priority, inventory
	//Higher priority - first
	//TMultiMap<uint8, IInventoryInterface*> inventories;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Inventory)
	TArray<TScriptInterface<IInventoryInterface>> inventories;
};
