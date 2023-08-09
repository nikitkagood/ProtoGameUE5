// Nikita Belov, All rights reserved


#include "Item/InventoryItem.h"

UInventoryItem::UInventoryItem()
{
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

bool UInventoryItem::Initialize(FDataTableRowHandle handle)
{
    auto* ptr_row = handle.GetRow<DataTableType>("UInventoryItem::Initialize");

    if (ptr_row != nullptr)
    {
        item_info = ptr_row->item_info;
        InventoryComponent->Initialize(ptr_row->InventoryDimensions, ptr_row->InventoryName);

        return true;
    }

    return false;
}

FItemThumbnailInfo UInventoryItem::GetItemThumbnailInfoFromDT()
{
    return GetItemThumbnailInfoFromDT_Impl<DataTableType>();
}

bool UInventoryItem::MoveItemToInventory(UItemBase* item, TScriptInterface<IInventoryInterface> destination)
{
    return InventoryComponent->MoveItemToInventory(item, destination);
}

bool UInventoryItem::MoveItemToInventoryInGrid(UItemBase* item, TScriptInterface<IInventoryInterface> destination, FIntPoint new_upper_left_cell)
{
    return InventoryComponent->MoveItemToInventoryInGrid(item, destination, new_upper_left_cell);
}

bool UInventoryItem::AddItemFromWorld(UItemBase* item)
{
    return InventoryComponent->AddItemFromWorld(item);
}

bool UInventoryItem::DropItemToWorld(UItemBase* item)
{
    return InventoryComponent->DropItemToWorld(item);
}

bool UInventoryItem::ReceiveItem(UItemBase* item)
{
    return InventoryComponent->ReceiveItem(item);
}

bool UInventoryItem::ReceiveItemInGrid(UItemBase* item, FIntPoint new_upper_left_cell)
{
    return InventoryComponent->ReceiveItemInGrid(item, new_upper_left_cell);
}

void UInventoryItem::UpdateStackDependencies(UItemBase* item, int32 new_stack_size)
{
    InventoryComponent->UpdateStackDependencies(item, new_stack_size);
}

void UInventoryItem::UpdateInventory()
{
    InventoryComponent->UpdateInventory();
}

TScriptInterface<IInventoryInterface> UInventoryItem::GetOuterUpstreamInventory() const
{
    TScriptInterface<IInventoryInterface> res;
    res.SetInterface(Cast<IInventoryInterface>(GetOuter()));
    return res;
}
