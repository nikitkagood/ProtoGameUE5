// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "ItemThumbnailInfo.generated.h"

//Thumbnail Info isn't to be included into ItemObject or ItemActor since we access this information infrequently
//Instead it is stored in Data Tables only. See FItemTable.
USTRUCT(BlueprintType)
struct PROTOGAME_API FItemThumbnailInfo : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = 0.01))
	float CameraDistanceFromBoundsMultiplier = 0.75f;

    //Usually we need a bit of Z (Yaw) rotation to make a good thumbnail
    //Default Z = -95 is for X-forward meshes, Z-up meshes
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    FRotator Rotation {0, -95, 0}; //Y, Z, X

    //Whether use automatic adjustment based Component Bounds Y coordinate
    //If true Location will be added on top to what's calculated
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    bool AutoLocationAdjustment = true;

    //Usually we need to adjust location because after rotation thumbnail won't look centered
    //If AutoLocationAdjustment is true, this value will be added on top
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    FVector Location;

    //Thumbnail which consists of multiple meshes. Example is a gun and attachments 
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = 0))
    bool bMultipleMeshSKComponent = false;
};