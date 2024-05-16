// Nikita Belov, All rights reserved

#include "VBFUnitBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

bool UVBFUnitBase::IsEverMovable() const
{
    if(vbf_unit_info.MaxSpeed > 0)
    {
        return true;
    }

    return false;
}

FVector UVBFUnitBase::GetDirection() const
{
    return UKismetMathLibrary::GetForwardVector(vbf_unit_info.Rotation);
}

bool UVBFUnitBase::TryMoveTo(FVector new_position)
{
    //vbf_unit_info.MaxSpeed
    ParallelFor(1, [](int32 idx){
        });
    //FMath::Lerp()
    //SetPosition(new_position);

    //ParallelFor(rows, [&](int32 idx)
    //    {
    //        TArray<int32> temp;
    //        temp.Init(EMPTY_SPACE, columns);
    //        InventoryGrid.Add(temp);
    //    });

    return false;
}

bool UVBFUnitBase::Stop()
{
    return false;
}
