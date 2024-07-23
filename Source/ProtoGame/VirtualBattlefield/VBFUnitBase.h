// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "VBFUnitInfo.h"
#include "VBFActorBase.h"

#include "Interfaces/VBFUnitInterface.h"

#include "VBFUnitBase.generated.h"

//Virtual Battlefield unit base class
UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, Abstract, ClassGroup = VirtualBattlefield, Meta = (BlueprintSpawnableComponent = false))
class PROTOGAME_API UVBFUnitBase : public UObject, public IVBFUnitInterface
{
    GENERATED_BODY()
public:
    //What type data table has which stores information needed for this class
    using DataTableType = FVBFUnitInfoTable;

    //If outer is empty, GetTransientPackage() will be used
    //UFUNCTION(BlueprintCallable)
    //static UVBFUnitBase* ConstructVBFUnit(UObject* outer, FDataTableRowHandle handle);

protected:
    virtual bool Initialize(FDataTableRowHandle handle);
public:

    //IVBFUnitInterface

    FText GetName_Implementation() const { return vbf_unit_info.Name; }

    FVector GetLocation_Implementation() const { return vbf_unit_info.Location; };

    FRotator GetRotation_Implementation() const { return vbf_unit_info.Rotation; };

    double GetSpeed_Implementation() const { return vbf_unit_info.Speed; };

    double GetMaxSpeed_Implementation() const { return vbf_unit_info.MaxSpeed; };


    bool SetName_Implementation(const FText& name) { vbf_unit_info.Name = name; return true; };

    bool SetLocation_Implementation(const FVector& new_position) { vbf_unit_info.Location = new_position; return true; };

    bool SetRotation_Implementation(const FRotator& new_rotation) { vbf_unit_info.Rotation = new_rotation; return true; };

   
    void DestroyUnit_Implementation();

    TSubclassOf<AActor> GetUnitActorClass_Implementation();

    bool SpawnUnitActor_Implementation(UWorld* world, const FVector& location, const FRotator& rotation);

    void DespawnUnitActor_Implementation();


    bool IsEverMovable_Implementation() const;

    bool IsEverCommandable_Implementation() const { return vbf_unit_info.IsEverCommandable; };


    //IVBFUnitInterface end

    UFUNCTION(BlueprintCallable)
    double GetSpeedKmh() const { return vbf_unit_info.Speed / 3.6; };

    UFUNCTION(BlueprintCallable)
    double GetMaxSpeedKmh() const { return vbf_unit_info.MaxSpeed / 3.6; };

    UFUNCTION(BlueprintCallable)
    FVector GetDirection() const;

    UFUNCTION(BlueprintCallable)
    FVBFUnitInfo GetUnitInfo() { return vbf_unit_info; };

    //TSubclassOf<AVBFActorBase> GetUnitActorClassFromDT();
private:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = true))
    bool IsCurrentlyCommandable;

    //UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = true))
    //bool IsEverMovable;

    
protected:
    //UPROPERTY(EditInstanceOnly, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
    //FDataTableRowHandle DT_handle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, meta = (AllowPrivateAccess = true))
    AVBFActorBase* vbf_unit_actor;

    //Things needed for unit to function
    UPROPERTY(EditAnywhere, BlueprintReadOnly, NoClear, meta = (AllowPrivateAccess = true))
    FVBFUnitInfo vbf_unit_info;
};
