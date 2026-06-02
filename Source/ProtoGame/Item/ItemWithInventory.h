// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBase.h"
#include "Interfaces/InventoryInterface.h"

#include "ItemWithInventory.generated.h"

USTRUCT(BlueprintType)
struct PROTOGAME_API FItemWithInventoryTable : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FInventoryItemInfo inventory_item_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FIntPoint InventoryDimensions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FName InventoryName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FItemThumbnailInfo item_thumbnail_info;
};

//Base class for item that implements InventoryInterface
UCLASS()
class PROTOGAME_API UItemWithInventory : public UItemBase, public IInventoryInterface
{
	GENERATED_BODY()

private:
	using DataTableType = FItemWithInventoryTable;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Instanced, meta = (AllowPrivateAccess = "true"), Category = Inventory)
	//TObjectPtr<UInventoryComponent> InventoryComponent;
	
public:
	UItemWithInventory();

	virtual bool Initialize(FDataTableRowHandle handle);

	virtual FItemThumbnailInfo GetItemThumbnailInfoFromDT() override;

	//UFUNCTION(BlueprintCallable)
	//UInventoryComponent* GetInventoryComponent() { return InventoryComponent; }

	virtual bool SetOuterUpstreamInventory(TScriptInterface<IInventoryInterface> inventory) override;

	virtual bool SetOuterItemActor(AItemActor* item_actor) override;

	//IInventoryInterface
	virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell) override;
	virtual bool AddItemFromWorld(UItemBase* item) override;
	virtual bool DropItemToWorld(UItemBase* item) override;
	virtual bool ReceiveItem(UItemBase* item, FIntPoint new_upper_left_cell) override;
	virtual void UpdateStackDependencies(UItemBase* item, int32 new_stack_size) override;
	virtual void UpdateInventory() override;
	virtual TScriptInterface<IInventoryInterface> GetOuterUpstreamInventory() const override;
	virtual UObject* GetInventoryOwner() { check(false) return nullptr; };
	bool SetInventoryOwner(UObject* new_owner);
	virtual FGameplayTagContainer GetInventoryTags() const { check(false) return{}; };
	//IInventoryInterface end
};
