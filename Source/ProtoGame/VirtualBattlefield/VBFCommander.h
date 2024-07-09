// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "VBFUnitBase.h"
#include "VBFFaction.h"

#include "VBFCommander.generated.h"

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced)
class PROTOGAME_API UVBFCommander : public UObject
{
	GENERATED_BODY()

    class UVBFMap;

public:
    UFUNCTION(BlueprintCallable)
    FText GetName() { return Name; };
    UFUNCTION(BlueprintCallable)
    int64 GetCommanderId() { return CommanderId; };

    auto GetAvailableUnits() { return AvailableUnits; };
    auto GetDeployedUnits() { return DeployedUnits; };

    UFUNCTION(BlueprintCallable)
    bool AddAvailableUnit(UVBFUnitBase* unit);
    UFUNCTION(BlueprintCallable)
    bool RemoveAvailableUnit(UVBFUnitBase* unit);

    UFUNCTION(BlueprintCallable)
    bool DeployUnit(UVBFUnitBase* unit);
    //UFUNCTION(BlueprintCallable)
    //bool AddDeployedUnit(UVBFUnitBase* unit);
    //UFUNCTION(BlueprintCallable)
    //bool RemoveDeployedUnit(UVBFUnitBase* unit);

protected:

    //GetManager()

    UVBFMap* GetAssociatedMap();

    //Unique values only
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSet<UVBFUnitBase*> AvailableUnits;

    //Unique values only
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TSet<UVBFUnitBase*> DeployedUnits;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ClampMin = -1))
    int64 CommanderId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, NoClear, meta = (AllowPrivateAccess = true))
    FText Name{ FText::FromString("DefaultCommanderName") };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, meta = (ExposeOnSpawn = true, AllowPrivateAccess = true))
    UVBFFaction* Faction;
};
