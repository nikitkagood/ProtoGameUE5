// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "ItemThumbnailInfo.generated.h"


USTRUCT(BlueprintType)
struct PROTOGAME_API FItemThumbnailInfo : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = 0.01))
	float CameraDistanceFromBoundsMultiplier = 0.75f;

    //Usually we need a bit of Z rotation to make a good thumbnail
    //-95 is for X-forward meshes, Z-up meshes
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    FRotator Rotation {0, 0, -95};

    //Whether use automatic adjustment based Component Bounds Y coordinate
    //If true Location will be added on top to what's calculated
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    bool AutoLocationAdjustment;

    //Usually we need to adjust location because after rotation thumbnail won't look centered
    //If AutoLocationAdjustment is true, will be added on top
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    FVector Location;

    //Thumbnail which consists of multiple meshes. Example is a gun and attachments 
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = 0))
    bool bMultipleMeshSKComponent;

    //Thumbnail that may change its appearance
    //UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = 0))
    //bool bNeedsDynamicThumbnail;
};