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
UCLASS(BlueprintType, DefaultToInstanced, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class PROTOGAME_API UInventoryManager : public UActorComponent //, public IInventoryInterface
{
	GENERATED_BODY()

public:
	UInventoryManager();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Manager specific methods which follow IInventoryInterface names for simplicity

	//Move item to an inventory. Filter by class. Exclude_types reverses the filter.
	//Empty inventory_types - any inventory 
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "inventory_types"))
	bool MoveItemToInventory(UItemBase* item, TArray<UClass*> inventory_types, bool exclude_types = false);
	//new_upper_left_cell is optional
	UFUNCTION(BlueprintCallable, meta = (new_upper_left_cell = "(-1, -1)"))
	bool MoveItemToInventoryDestination(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell);
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

	//UObject* GetInventories();

	//C++: do not call it in Character constructor, better call in BeginPlay
	//Add reference to an inventory, that has been already created
	UFUNCTION(BlueprintCallable)
	void AddExistingInventory(TScriptInterface<IInventoryInterface> inventory);

	//void SetInventories(TArray<IInventoryInterface*>&& new_inventories);

	UFUNCTION(BlueprintCallable)
	void RemoveInventory(TScriptInterface<IInventoryInterface> inventory);

protected:
	virtual void BeginPlay() override;

	//Create inventory object and add it
	UFUNCTION(BlueprintCallable)
	void AddInventory(const UClass* inventory_class, const FName& name);

private:
	//List of managed inventories
	//Must implement IInventoryInterface and be derived from UObject
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Inventory)
	TArray<TScriptInterface<IInventoryInterface>> inventories;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", MustImplement = "InventoryInterface"), Category = Inventory)
	//TArray<TSubclassOf<UObject>> inventories_test_subclassof_mustimplement;

	//Must implement InventoryInterface
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, meta = (AllowPrivateAccess = "true", MustImplement = "InventoryInterface"), Category = Inventory)
	TArray<UObject*> inventories_test_uobject_mustimplement;
};
