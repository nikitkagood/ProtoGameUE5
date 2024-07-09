// Nikita Belov, All rights reserved


#include "VirtualBattlefield/VBFCommander.h"

#include "VBFMap.h"

bool UVBFCommander::AddAvailableUnit(UVBFUnitBase* unit)
{
    if (unit != nullptr)
    {
        return AvailableUnits.Add(unit).IsValidId();
    }

    UE_LOG(LogTemp, Warning, TEXT("AddAvailableUnit: Invalid ptr"));

    return false;
}

bool UVBFCommander::RemoveAvailableUnit(UVBFUnitBase* unit)
{
    if (unit != nullptr)
    {
        return AvailableUnits.Remove(unit) != 0;
    }

    UE_LOG(LogTemp, Warning, TEXT("RemoveAvailableUnit: Invalid ptr"));

    return false;
}

bool UVBFCommander::DeployUnit(UVBFUnitBase* unit)
{
    if (unit != nullptr)
    {
        if (AvailableUnits.Remove(unit) != 0)
        {

            DeployedUnits.Add(unit);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("DeployUnit: Invalid ptr"));

    return false;
}