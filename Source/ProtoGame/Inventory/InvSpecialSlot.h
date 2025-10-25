// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Item/EnumItemTypes.h"
#include "Interfaces/InventoryInterface.h"

#include "InvSpecialSlot.generated.h"

class UItemBase;

USTRUCT(BlueprintType)
struct PROTOGAME_API FInvSpecialSlotCompatibleClassSettings
{
	GENERATED_BODY()

	bool include_subclasses;
};

//Used when there is a need for one per slot item and whole InventoryComponent with a grid is too much
UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew, ClassGroup = (Inventory), meta=(BlueprintSpawnableComponent, DisplayName = "Inventory Special Slot"))
class PROTOGAME_API UInvSpecialSlotComponent : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()
	
public:
	UInvSpecialSlotComponent();

	//Accounts for situations when Item is in transition between inventories. 
	//I.e. new inventory has already received it, but prev. inventory hasn't cleared reference yet.
	UFUNCTION(BlueprintGetter)
	UItemBase* GetItem() const;
	//UFUNCTION(BlueprintGetter)
	//const TArray<ItemType>& GetCompatibleTypes() const { return CompatibleTypes; }
	//UFUNCTION(BlueprintGetter)
	//TArray<ItemType>& GetCompatibleTypesRef() { return CompatibleTypes; }

	//IInventoryInterface

	virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination) override;
	virtual bool MoveItemToInventoryDestination(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell) override;
	virtual bool AddItemFromWorld(UItemBase* item) override;
	virtual bool DropItemToWorld(UItemBase* item) override;
	virtual bool ReceiveItem(UItemBase* item) override;
	//Simply reroute, new_upper_left_cell is ignored
	virtual bool ReceiveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell) override { return ReceiveItem(item); };
	virtual void UpdateInventory() override { OnInventoryUpdated.Broadcast(); };
	virtual TScriptInterface<IInventoryInterface> GetOuterUpstreamInventory() const override;
	virtual UObject* GetInventoryOwner() override { return GetOwner(); };
	//TODO: Implement
	virtual bool SetInventoryOwner(UObject* new_owner) override;
	//IInventoryInterface end

	bool IsItemCompatible(UItemBase* item);

public:

	UFUNCTION(BlueprintCallable)
	bool IsOccupied() const;

	UPROPERTY(BlueprintAssignable)
	FOnInventoryUpdated OnInventoryUpdated;

protected:
	//Must set World and Outer
	virtual bool AddItem(UItemBase* item);

	//Doesn't clear World and Outer because there Add (another inventory) comes first, Remove second
	void Clear(); 

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UItemBase* Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	FName InventoryName;
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	//bool CompatibleClassesIncludeChildren;

	//Class-based filter
	//Emtpy means any, has to be subclass of UItemBase
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	TMap<TSubclassOf<UItemBase>, FInvSpecialSlotCompatibleClassSettings> CompatibleClasses;
};
