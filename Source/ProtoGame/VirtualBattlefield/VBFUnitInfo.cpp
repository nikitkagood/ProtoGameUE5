// Nikita Belov, All rights reserved


#include "VBFUnitInfo.h"

#include "VBFUnitBase.h"

FVBFUnitInfoTable::FVBFUnitInfoTable()
{
    UnitClass = StaticClass<UVBFUnitBase>();
    MeshType = EUnitMeshType::None;
}
