// Nikita Belov, All rights reserved


#include "VBFUnitInfo.h"

FVBFUnitInfoTable::FVBFUnitInfoTable()
{
    UnitClass = StaticClass<UVBFUnitBase>();
    UnitActorClass = StaticClass<AVBFActorBase>();
    MeshType = EUnitMeshType::None;
}
