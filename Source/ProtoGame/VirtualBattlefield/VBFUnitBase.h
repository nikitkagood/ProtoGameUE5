// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "VBFUnitInfo.h"
#include "VBFActorBase.h"

#include "Interfaces/VBFUnitInterface.h"
#include "Interfaces/DisplayInfo.h"

#include "VBFUnitBase.generated.h"

//Virtual Battlefield unit base class
UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, Abstract, ClassGroup = VirtualBattlefield, Meta = (BlueprintSpawnableComponent = false))
class PROTOGAME_API UVBFUnitBase : public UObject, public IVBFUnitInterface, public IDisplayInfo
{
    GENERATED_BODY()
public:
    //What type data table this has, which stores required information
    //Selected DT type == DataTableType required for GetRow<DataTableType> to work
    using DataTableType = FVBFUnitInfoTable;

    //If outer is empty, GetTransientPackage() will be used
    //UFUNCTION(BlueprintCallable)
    //static UVBFUnitBase* ConstructVBFUnit(UObject* outer, FDataTableRowHandle handle);

    //Mostly for setting necessary data from DataTable
    //"override completely" is used pattern (opposed to Super::Initialize() ... *this class*)
    // - to avoid lots of method calls
    UFUNCTION(BlueprintCallable)
    virtual bool Initialize(FDataTableRowHandle handle);

    //TODO: think of efficient way of doing this
    //Update UnitObject with Actor data
    //UFUNCTION(BlueprintCallable)
    //virtual bool SyncWithActor();
public:

    UFUNCTION(BlueprintCallable)
    virtual class UWorld* GetWorldFromOuter() const;

    UFUNCTION(BlueprintPure)
    virtual AActor* GetOwner() const;

    AVBFActorBase* GetVBFActor() { return vbf_unit_actor; };

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

    //TSubclassOf<AActor> GetUnitActorClass_Implementation();

    //General implementation
    //Since I found no way to dynamically change DataTableType, 
    //this is purely for example and has to be overriden
    bool SpawnUnitActor_Implementation(const FTransform& transform, const FVector& normal, FDataTableRowHandle handle, UWorld* world_optional = nullptr);
    void DespawnUnitActor_Implementation();


    bool TryMoveTo_Implementation(const FVector& location) { return false; };
    bool AttackMove_Implementation(const FVector& location) { return false; };
    bool StopMovement_ResetPlayerTarget_Implementation() { return false; };
    bool Attack_Implementation(UObject* target) { return false; };


    bool IsEverMovable_Implementation() const { return false; };

    bool CanMove_Implementation() const { return false; };

    bool IsCommandable_Implementation() const;

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


    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
    UStreamableRenderAsset* StreamableAssetTest;


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, meta = (AllowPrivateAccess = true))
    AVBFActorBase* vbf_unit_actor;

    //Things needed for unit to function
    UPROPERTY(EditAnywhere, BlueprintReadOnly, NoClear, meta = (AllowPrivateAccess = true))
    FVBFUnitInfo vbf_unit_info;
};
