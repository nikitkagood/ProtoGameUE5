// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Interfaces/InventoryInterface.h"

#include "InventoryManager.generated.h"

//Connects multiple inventories into an organized system
//For example, character might have pockets, backpack, vest and whatever - 
//we don't want manually go through every possible inventory, we just ask this Manager
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOGAME_API UInventoryManager : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()

public:	
	UInventoryManager();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//IInventoryInterface
	
	//Move item by priority or where simply enough free space
	virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination) override;
	virtual bool MoveItemToInventoryInGrid(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell) override;
	virtual bool DropItemToWorld(UItemBase* item) override;
	virtual bool ReceiveItem(UItemBase* item) override;
	virtual bool ReceiveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell) override;
	virtual void UpdateStackDependencies(UItemBase* item, int32 new_stack_size) override {};
	virtual void UpdateInventory() override;
	//Iventory manager isn't really meant to be within another inventory
	//But techically possibility remains
	virtual TScriptInterface<IInventoryInterface> GetOuterUpstreamInventory() const override;
	//IInventoryInterface end

	//void AddInventory(IInventoryInterface* inventory, uint8 priority);
	void AddInventory(IInventoryInterface* inventory);

protected:
	virtual void BeginPlay() override;

private:
	//Priority, inventory
	//Higher priority - first
	//TMultiMap<uint8, IInventoryInterface*> inventories;
	TArray<IInventoryInterface*> inventories;
};
