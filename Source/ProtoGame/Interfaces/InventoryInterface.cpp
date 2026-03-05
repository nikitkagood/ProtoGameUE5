#include "InventoryInterface.h"
// Nikita Belov, All rights reserved

FInventoryGuid IInventoryInterface::CreateItemGuid()
{
	if (inventory_guid.guid.IsValid())
	{
		return inventory_guid;
	}

	auto object = Cast<UObject>(this);

	if (IsValid(object) && !object->HasAnyFlags(RF_ClassDefaultObject))
	{
		return FInventoryGuid(FGuid::NewGuid());
	}

	return {};
};