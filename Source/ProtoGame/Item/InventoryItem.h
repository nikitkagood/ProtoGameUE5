// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBase.h"
#include "Interfaces/InventoryInterface.h"
#include "Inventory/InventoryComponent.h"

#include "InventoryItem.generated.h"

USTRUCT(BlueprintType)
struct PROTOGAME_API FInventoryItemTable : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FItemInfo item_info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FIntPoint InventoryDimensions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FName InventoryName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FItemThumbnailInfo item_thumbnail_info;
};

UCLASS()
class PROTOGAME_API UInventoryItem : public UItemBase, public IInventoryInterface
{
	GENERATED_BODY()

private:
	using DataTableType = FInventoryItemTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Instanced, meta = (AllowPrivateAccess = "true"), Category = Inventory)
	TObjectPtr<UInventoryComponent> InventoryComponent;
	
public:
	UInventoryItem();

	virtual bool Initialize(FDataTableRowHandle handle);

	virtual FItemThumbnailInfo GetItemThumbnailInfoFromDT() override;

	UFUNCTION(BlueprintCallable)
	UInventoryComponent* GetInventoryComponent() { return InventoryComponent; }

	virtual bool Interact(AActor* caller, EInteractionActions action) override;

	//IInventoryInterface
	virtual bool MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination) override;
	virtual bool MoveItemToInventoryInGrid(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell) override;
	virtual bool AddItemFromWorld(UItemBase* item) override;
	virtual bool DropItemToWorld(UItemBase* item) override;
	virtual bool ReceiveItem(UItemBase* item) override;
	virtual bool ReceiveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell) override;
	virtual void UpdateStackDependencies(UItemBase* item, int32 new_stack_size) override;
	virtual void UpdateInventory() override;
	virtual TScriptInterface<IInventoryInterface> GetOuterUpstreamInventory() const override;
	//IInventoryInterface end
};
