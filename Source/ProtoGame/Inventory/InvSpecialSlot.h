// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Item/EnumItemTypes.h"
#include "Interfaces/InventoryInterface.h"

#include "InvSpecialSlot.generated.h"

class UItemBase;

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, meta=(BlueprintSpawnableComponent), ClassGroup=(Inventory))
class PROTOGAME_API UInvSpecialSlotComponent : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()
	
public:
	UInvSpecialSlotComponent();

	//Accounts for situations when Item is in transition between inventories. 
	//I.e. new inventory has already received it, but prev. inventory hasn't cleared reference yet.
	UFUNCTION(BlueprintGetter)
	UItemBase* GetItem() const;
	UFUNCTION(BlueprintGetter)
	const TArray<ItemType>& GetCompatibleTypes() const { return CompatibleTypes; }
	UFUNCTION(BlueprintGetter)
	TArray<ItemType>& GetCompatibleTypesRef() { return CompatibleTypes; }

	//IInventoryInterface

	virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination) override;
	virtual bool MoveItemToInventoryInGrid(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell) override;
	virtual bool DropItemToWorld(UItemBase* item) override;
	virtual bool ReceiveItem(UItemBase* item) override;
	virtual void UpdateInventory() override { OnInventoryUpdated.Broadcast(); };
	virtual TScriptInterface<IInventoryInterface> GetOuterUpstreamInventory() const override;
	//IInventoryInterface end
private:
	//Grid receive operation isn't supported because SpecialSlot class doesn't have any grid
	virtual bool ReceiveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell) override { check(false); return false; };
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UItemBase* Item;
	//TSubclassOf<UItemBase> Item = nullptr;

	//TODO: implement
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<ItemType> CompatibleTypes; //0 means any
};
