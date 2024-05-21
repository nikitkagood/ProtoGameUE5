// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "VBFUnitInfo.h"

#include "VBFUnitBase.generated.h"

//Virtual Battlefield unit base class
UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, ClassGroup = VirtualBattlefield)
class PROTOGAME_API UVBFUnitBase : public UObject
{
    GENERATED_BODY()
public:
    //UVBFUnitBase(const FObjectInitializer&);

    UFUNCTION(BlueprintCallable)
    void SetPosition(FVector new_position) { vbf_unit_info.Position = new_position; };

    UFUNCTION(BlueprintCallable)
    void SetRotation(FRotator new_rotation) { vbf_unit_info.Rotation = new_rotation; };

    UFUNCTION(BlueprintCallable)
    float GetSpeed() const { return vbf_unit_info.Speed; };

    UFUNCTION(BlueprintCallable)
    float GetSpeedKmh() const { return vbf_unit_info.Speed / 3.6; };

    UFUNCTION(BlueprintCallable)
    float GetMaxSpeed() const { return vbf_unit_info.MaxSpeed / 3.6; };

    UFUNCTION(BlueprintCallable)
    float GetMaxSpeedKmh() const { return vbf_unit_info.MaxSpeed / 3.6; };

    UFUNCTION(BlueprintCallable)
    FRotator GetRotation() const { return vbf_unit_info.Rotation; };

    UFUNCTION(BlueprintCallable)
    FVector GetDirection() const;

    UFUNCTION(BlueprintCallable)
    bool IsEverMovable() const;

    UFUNCTION(BlueprintCallable)
    bool IsEverCommandable() const { return vbf_unit_info.IsEverCommandable; };

    //Commands

    //If unit cannot reach a position it will try to get as close as possible
    virtual bool TryMoveTo(FVector new_position);

    UFUNCTION(BlueprintCallable)
    virtual bool Stop();

    UFUNCTION(BlueprintCallable)
    FVBFUnitInfo GetUnitInfo() { return vbf_unit_info; };
private:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = true))
    bool IsCurrentlyCommandable;

    //UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = true))
    //bool IsEverMovable;

    //Things needed for unit to function
    UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
    FVBFUnitInfo vbf_unit_info;
};
