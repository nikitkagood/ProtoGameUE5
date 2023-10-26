// Nikita Belov, All rights reserved


#include "AmmoBase.h"

//UAmmoBase::UAmmoBase()
//{
//
//}

bool UAmmoBase::Initialize(FDataTableRowHandle handle)
{
	auto* ptr_row = handle.GetRow<DataTableType>("SetProperties_AmmoBase");

	if(ptr_row != nullptr)
	{
		inventory_item_info = ptr_row->inventory_item_info;
		ammo_info = ptr_row->ammo_info;

		return true;
	}

	return false;
}

FItemThumbnailInfo UAmmoBase::GetItemThumbnailInfoFromDT()
{
	return GetItemThumbnailInfoFromDT_Impl<DataTableType>();
}
