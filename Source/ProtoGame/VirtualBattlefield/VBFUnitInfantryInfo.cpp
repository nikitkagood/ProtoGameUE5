// Nikita Belov, All rights reserved


#include "VirtualBattlefield/VBFUnitInfantryInfo.h"
#include "VirtualBattlefield/VBFUnitInfantry.h"

FVBFUnitInfantryInfoTable::FVBFUnitInfantryInfoTable()
{
    unit_land_table.unit_info_table.UnitClass = StaticClass<UVBFUnitInfantry>();
}
