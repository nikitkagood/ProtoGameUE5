// Nikita Belov, All rights reserved

#include "VBFUnitBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

//UVBFUnitBase* UVBFUnitBase::ConstructVBFUnit(UObject* outer, FDataTableRowHandle handle)
//{
//    auto* ptr_row = handle.GetRow<DataTableType>("UVBFUnitBase::Initialize");
//
//    if (ptr_row == nullptr)
//    {
//        checkf(false, TEXT("UVBFUnitBase::ConstructVBFUnit failed to get row from DataTable"))
//        return nullptr;
//    }
//
//    UVBFUnitBase* obj;
//
//    if (outer == nullptr)
//    {
//        //TODO:
//        //not tested but probably has to work
//        obj = NewObject<UVBFUnitBase>(GetTransientPackage(), ptr_row->UnitClass);
//    }
//    else
//    {
//        obj = NewObject<UVBFUnitBase>(outer, ptr_row->UnitClass);
//    }
//
//    obj->Initialize(handle);
//
//    return obj;
//}

bool UVBFUnitBase::Initialize(FDataTableRowHandle handle)
{
    auto* ptr_row = handle.GetRow<DataTableType>("UVBFUnitBase::Initialize");

    if (ptr_row == nullptr)
    {
        checkf(false, TEXT("UVBFUnitBase::Initialize failed to get row from DataTable"))
        return false;
    }

    vbf_unit_info = ptr_row->unit_info;

    return true;
}

void UVBFUnitBase::DestroyUnit_Implementation()
{
    DespawnUnitActor();

    this->MarkAsGarbage();
}

TSubclassOf<AActor> UVBFUnitBase::GetUnitActorClass_Implementation()
{
    return AVBFActorBase::StaticClass();
}

bool UVBFUnitBase::SpawnUnitActor_Implementation(UWorld* world, const FVector& location, const FRotator& rotation)
{
    if (IsValid(vbf_unit_actor) == true)
    {
        UE_LOG(LogTemp, Warning, TEXT("UVBFUnitBase::SpawnUnitActor_Implementation: unit actor has been spawned already"))
        return false;
    }

    //*Cast<TSubclassOf<AVBFActorBase>>(GetUnitActorClass())
    auto* actor_ptr = AVBFActorBase::StaticCreateObject(world, GetUnitActorClass(), this, location, rotation);

    if (actor_ptr == nullptr)
    {
        return false;
    }

    vbf_unit_actor = actor_ptr;

    return false;
}

void UVBFUnitBase::DespawnUnitActor_Implementation()
{

}

bool UVBFUnitBase::IsEverMovable_Implementation() const
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

//TSubclassOf<AVBFActorBase> UVBFUnitBase::GetUnitActorClassFromDT()
//{
//    auto* ptr_row = handle.GetRow<DataTableType>("UVBFUnitBase::Initialize");
//
//    if (ptr_row == nullptr)
//    {
//        checkf(false, TEXT("UVBFUnitBase::Initialize failed to get row from DataTable"))
//            return false;
//    }
//
//    vbf_unit_info = ptr_row->unit_info;
//
//    return TSubclassOf<AVBFActorBase>();
//}
