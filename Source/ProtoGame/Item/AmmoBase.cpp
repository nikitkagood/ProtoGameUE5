// Nikita Belov, All rights reserved


#include "AmmoBase.h"

UAmmoBase::UAmmoBase()
{

}

bool UAmmoBase::SetProperties(FDataTableRowHandle handle)
{
	auto* ptr_row = handle.GetRow<FAmmoTable>("SetProperties_AmmoBase");

	if(ptr_row != nullptr)
	{
		item_info = ptr_row->item_info;
		ammo_info = ptr_row->ammo_info;

		return true;
	}

	return false;
}
