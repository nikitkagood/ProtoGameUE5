// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "InventoryInterface.generated.h"

class UItemBase;

UENUM(BlueprintType)
enum class EInventoryOperation : uint8
{
	MoveTo = 0					UMETA(DisplayName = "MoveTo (another inv)"),
	Receive						UMETA(DisplayName = "Receive (from another inv)"),
	AddFromWorld				UMETA(DisplayName = "AddFromWorld"),
	DropToWorld					UMETA(DisplayName = "AddFromWorld"),
	DestroyItem					UMETA(DisplayName = "DestroyItem"),
};


//to be declared in each class individually with UPROPERTY(BlueprintAssignable) due to interface limitations
//note that any inventory is responsible for refreshing UI
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

class PROTOGAME_API IInventoryInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual TScriptInterface<IInventoryInterface> GetOuterUpstreamInventory() const = 0;

	//UFUNCTION(BlueprintCallable)
	//virtual FIntPoint GetInventoryDimensions() const = 0;

    UFUNCTION(BlueprintCallable)
	virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination) = 0;

	UFUNCTION(BlueprintCallable)
	virtual bool MoveItemToInventoryInGrid(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell) = 0;

	UFUNCTION(BlueprintCallable)
	virtual bool AddItemFromWorld(UItemBase* item) = 0; //add item; currently items destroy themselves

	UFUNCTION(BlueprintCallable)
	virtual bool DropItemToWorld(UItemBase* item) = 0;

	virtual bool ReceiveItem(UItemBase* item) = 0;

	virtual bool ReceiveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell) = 0;

	//Update everything which depends on stacking system. 
	//Example: every Inventory keeps track of mass
	virtual void UpdateStackDependencies(UItemBase* item, int32 new_stack_size) {};

	//Force update visual representation of an inventory.
	//Inventories ARE REQUIRED to update when they change state.
	//But when they don't, they might not. 
	//Prime example is Drag and Drop: if it fails, inventory doesn't change but visuals are required to be updated.
	UFUNCTION(BlueprintCallable)
	virtual void UpdateInventory() = 0;

	UFUNCTION(BlueprintCallable)
	virtual AActor* GetInventoryOwner() = 0;

	//Whether component always stays where it was created or it can be moved
	UFUNCTION(BlueprintCallable)
	virtual bool CanOwnerEverChange() { return true; };

	//Whether inventory operations can be performed now
	UFUNCTION(BlueprintCallable)
	virtual bool IsLocked() { return false; };

	//UFUNCTION(BlueprintCallable)
	//TArray<EInventoryOperation> GetAvailableOperations();
};

