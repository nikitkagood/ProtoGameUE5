// Nikita Belov, All rights reserved


#include "VBFUnitLand.h"

bool UVBFUnitLand::TryMoveTo(FVector new_position)
{
    //TODO: Implement land-specific movement logic

    SetPosition(new_position);

    return true;
}
