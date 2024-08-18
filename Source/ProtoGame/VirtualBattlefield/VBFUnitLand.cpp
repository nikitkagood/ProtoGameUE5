// Nikita Belov, All rights reserved


#include "VBFUnitLand.h"

bool UVBFUnitLand::Initialize(FDataTableRowHandle handle)
{
    auto* ptr_row = handle.GetRow<DataTableType>("UVBFUnitLand::Initialize");

    if (ptr_row == nullptr)
    {
        checkf(false, TEXT("UVBFUnitLand::Initialize failed to get row from DataTable"))
            return false;
    }

    vbf_unit_info = ptr_row->unit_info_table.unit_info;
    unit_land_info = ptr_row->unit_land_info;

    return true;
}

bool UVBFUnitLand::SpawnUnitActor_Implementation(const FTransform& transform, FDataTableRowHandle handle, UWorld* world_optional)
{
    if (IsValid(vbf_unit_actor) == true)
    {
        UE_LOG(LogTemp, Warning, TEXT("UVBFUnitLand::SpawnUnitActor_Implementation: unit actor has been spawned already"))
            return false;
    }

    UWorld* world_to_use;

    if (world_optional != nullptr)
    {
        world_to_use = world_optional;
    }
    else
    {
        world_to_use = GetWorld();
    }

    if (world_to_use == nullptr)
    {
        checkf(false, TEXT("UVBFUnitLand::SpawnUnitActor_Implementation: GetWorld == nullptr, cannot spawn Actor"))
            return false;
    }

    auto* actor_ptr = AVBFActorBase::StaticCreateObject(world_to_use, Execute_GetUnitActorClass(this), this, transform);

    if (actor_ptr == nullptr)
    {
        return false;
    }

    auto* ptr_row = handle.GetRow<DataTableType>("UVBFUnitLand::SpawnUnitActor_Implementation");

    if (ptr_row == nullptr)
    {
        checkf(false, TEXT("UVBFUnitLand::SpawnUnitActor_Implementation: ptr_row == nullptr, cannot get info from DT"))
            return false;
    }

    UStreamableRenderAsset* temp_render_asset = nullptr;

    switch (ptr_row->unit_info_table.MeshType)
    {
    case EUnitMeshType::UseStaticMesh:
        if (ptr_row->unit_info_table.StaticMesh == nullptr)
        {
            checkf(false, TEXT("UVBFUnitLand::SpawnUnitActor_Implementation: Cannot initialize mesh - it is not set in DataTable"))
        }

        temp_render_asset = ptr_row->unit_info_table.StaticMesh;
        break;
    case EUnitMeshType::UseSkeletalMesh:
        if (ptr_row->unit_info_table.SkeletalMesh == nullptr)
        {
            checkf(false, TEXT("UVBFUnitLand::SpawnUnitActor_Implementation: Cannot initialize mesh - it is not set in DataTable"))
        }

        temp_render_asset = ptr_row->unit_info_table.SkeletalMesh;
        break;
    case EUnitMeshType::UseExternalSettings:
        break;
    default:
        break;
    }

    bool result = actor_ptr->Initialize(temp_render_asset);

    if (result == false)
    {
        return false;
    }

    vbf_unit_actor = actor_ptr;

    return true;
    }

