// Nikita Belov, All rights reserved


#include "Item/WeaponAttachmentSight.h"
#include "Item/ItemActor.h"

//UWeaponAttachmentSight::UWeaponAttachmentSight()
//{
//
//}

bool UWeaponAttachmentSight::Initialize(FDataTableRowHandle handle)
{
	auto* ptr_row = handle.GetRow<DataTableType>("UWeaponAttachmentSight::Initialize");

	if(ptr_row != nullptr)
	{
		item_info = ptr_row->item_info;
		attachment_info = ptr_row->attachment_info;
		attachment_sight_info = ptr_row->attachment_sight_info;

		return true;
	}

	return false;
}

FItemThumbnailInfo UWeaponAttachmentSight::GetItemThumbnailInfoFromDT()
{
	return GetItemThumbnailInfoFromDT_Impl<DataTableType>();
}
