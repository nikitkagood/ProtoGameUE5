// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Resource.h"

#include "VBFFaction.generated.h"

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced)
class PROTOGAME_API UVBFFaction : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    int32 GetFactionId() const { return FactionId; };

    //UFUNCTION(BlueprintCallable)
    //int32 GetFactionId() const { return FactionId; };

    //UFUNCTION(BlueprintCallable)
    const TMap<EResourceType, int64> GetResources() const { return Resources; };

protected:
    //Do not reuse, they have to be always unique
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, NoClear, meta = (AllowPrivateAccess = true))
    int32 FactionId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, NoClear, meta = (AllowPrivateAccess = true))
    FText Name { FText::FromString("DefaultFactionName") };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TMap<EResourceType, int64> Resources;

    //TODO: maybe it will be needed
    //UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
    //TMap<UItemBase*, int64> AccessibleItems;
};
