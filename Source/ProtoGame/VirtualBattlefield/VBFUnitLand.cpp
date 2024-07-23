// Nikita Belov, All rights reserved


#include "VBFUnitLand.h"

bool UVBFUnitLand::Initialize(FDataTableRowHandle handle)
{
    auto* ptr_row = handle.GetRow<DataTableType>("UVBFUnitLand::Initialize");

    if (ptr_row == nullptr)
    {
        checkf(false, TEXT("UVBFUnitLand::Initialize failed to get row from DataTable"))
            return false;
    }

    vbf_unit_info = ptr_row->unit_info_table.unit_info;
    unit_land_info = ptr_row->unit_land_info;

    return true;
}

