// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "InventoryInterface.generated.h"

class UItemBase;

//UENUM(BlueprintType)
//enum class EInventoryOperation : uint8
//{
//	MoveTo = 0					UMETA(DisplayName = "MoveTo (another inv)"),
//	Receive						UMETA(DisplayName = "Receive (from another inv)"),
//	AddFromWorld				UMETA(DisplayName = "AddFromWorld"),
//	DropToWorld					UMETA(DisplayName = "AddFromWorld"),
//	DestroyItem					UMETA(DisplayName = "DestroyItem"),
//};


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
	//Inventory operations

   // UFUNCTION(BlueprintCallable)
	//virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination) = 0;

	//new_upper_left_cell is optional, default value -1, -1 - invalid cell
	//NOT SET as standard C++ default value due to UFUNCTION
	UFUNCTION(BlueprintCallable, meta = (new_upper_left_cell = "(-1, -1)"))
	virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell) = 0;

	//Add item from world, laying on the ground for example
	//ANYTHING world-related is expected to be handled by Item itself
	//Inventory is not to predict Item behavior when moved into inventory, 
	// not even just to just delete its world representation - since this easily could be done wrong
	UFUNCTION(BlueprintCallable)
	virtual bool AddItemFromWorld(UItemBase* item) = 0; 

	UFUNCTION(BlueprintCallable)
	virtual bool DropItemToWorld(UItemBase* item) = 0;

	//Used internally, so origin inventory can check whether destination inventory has received the item
	//NOTE: "Destructive" operation if returns true - i.e. origin inventory has to stop "containing" the item
	//non-destructive if false
	//
	//new_upper_left_cell is optional, default value -1, -1 - invalid cell
	virtual bool ReceiveItem(UItemBase* item, FIntPoint new_upper_left_cell = {-1, -1}) = 0;


	//"Service" functions

	UFUNCTION(BlueprintCallable)
	virtual TScriptInterface<IInventoryInterface> GetOuterUpstreamInventory() const = 0;

	//UFUNCTION(BlueprintCallable)
	//virtual FIntPoint GetInventoryDimensions() const = 0;

	//Update everything which depends on stacking system. 
	//Example: every Inventory keeps track of mass
	virtual void UpdateStackDependencies(UItemBase* item, int32 new_stack_size) { };

	//Force update visual representation of an inventory.
	//Inventories ARE REQUIRED to update when they change state.
	//But when they don't, they might not. 
	//Prime example is Drag and Drop: if it fails, inventory doesn't change but visuals are required to be updated.
	UFUNCTION(BlueprintCallable)
	virtual void UpdateInventory() = 0;

	//For example: Player, AI Pawn, TransientPackage, World etc.
	UFUNCTION(BlueprintCallable)
	virtual UObject* GetInventoryOwner() = 0;

	UFUNCTION(BlueprintCallable)
	virtual bool SetInventoryOwner(UObject* new_owner) = 0;


	//Whether component always stays where it was created or it can be moved (inventory within another inventory)
	UFUNCTION(BlueprintCallable)
	virtual bool CanOwnerEverChange() { return true; };


	//TODO: not yet implemented
	// 
	//What a user (player, AI) can do: 
	//not visible, visible, editable
	//move items in, out, edit items
	UFUNCTION(BlueprintCallable)
	virtual void GetUserAccessRules() { return; };

	//TODO: not yet implemented
	// 
	//What a world/game can do: 
	//not visible, visible, editable
	//move items in, out, edit items
	UFUNCTION(BlueprintCallable)
	virtual void GetWorldAccessRules() { return; };

	//Safety threading/multiple access feature, 
	//whether move items IN (or move internally in a grid) is currently allowed 
	//i.e. not processing another IN operation
	//
	//Prime example is to lock the grid so items don't get placed into one space
	UFUNCTION()
	virtual bool IsLockGuarded_In() { return false; };

	//Safety threading/multiple access feature, 
	//whether move items OUT is currently allowed
	//
	//OUT operations are more simple, they don't check the space
	//Might not actually need to lock anything
	UFUNCTION()
	virtual bool IsLockGuarded_Out() { return false; };

	//Safety threading/multiple access feature, 
	//whether currently can edit items inside this inventory
	//
	//Note: it's better to lock items individually, rather than whole inventory
	UFUNCTION()
	virtual bool IsLockGuarded_ItemEdit() { return false; };

};

