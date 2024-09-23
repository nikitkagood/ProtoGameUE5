// Nikita Belov, All rights reserved

#include "VBFUnitLandInfo.h"

#include "VBFUnitLand.h"

FVBFUnitLandInfoTable::FVBFUnitLandInfoTable()
{
    unit_info_table.UnitClass = StaticClass<UVBFUnitLand>();
    unit_info_table.MeshType = EUnitMeshType::UseStaticMesh;

    unit_info_table.unit_info.MaxSpeed = 1;
}
