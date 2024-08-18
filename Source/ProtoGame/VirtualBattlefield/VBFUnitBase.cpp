// Nikita Belov, All rights reserved

#include "VBFUnitBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

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

UWorld* UVBFUnitBase::GetWorldFromOuter() const
{
    if (GIsEditor && !GIsPlayInEditorWorld)
    {
        return nullptr;
    }
    if (GetOuter())
    {
        return GetOuter()->GetWorld();
    }

    return nullptr;
}

AActor* UVBFUnitBase::GetOwner() const
{
    auto* actor_comp_owner = Cast<UActorComponent>(GetOuter());

    if (actor_comp_owner != nullptr)
    {
        return actor_comp_owner->GetOwner();
    }

    return Cast<AActor>(GetOuter());
}

void UVBFUnitBase::DestroyUnit_Implementation()
{
    Execute_DespawnUnitActor(this);

    this->MarkAsGarbage();
}

TSubclassOf<AActor> UVBFUnitBase::GetUnitActorClass_Implementation()
{
    return AVBFActorBase::StaticClass();
}

bool UVBFUnitBase::SpawnUnitActor_Implementation(const FTransform& transform, FDataTableRowHandle handle, UWorld* world_optional)
{
    if (IsValid(vbf_unit_actor) == true)
    {
        UE_LOG(LogTemp, Warning, TEXT("UVBFUnitBase::SpawnUnitActor_Implementation: unit actor has been spawned already"))
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
        checkf(false, TEXT("UVBFUnitBase::SpawnUnitActor_Implementation: GetWorld == nullptr, cannot spawn Actor"))
        return false;
    }

    auto* actor_ptr = AVBFActorBase::StaticCreateObject(world_to_use, Execute_GetUnitActorClass(this), this, transform);

    if (actor_ptr == nullptr)
    {
        return false;
    }

    auto* ptr_row = handle.GetRow<DataTableType>("UVBFUnitBase::SpawnUnitActor_Implementation");

    if (ptr_row == nullptr)
    {
        checkf(false, TEXT("UVBFUnitBase::SpawnUnitActor_Implementation: ptr_row == nullptr, cannot get info from DT"))
        return false;
    }

    UStreamableRenderAsset* temp_render_asset = nullptr;

    switch (ptr_row->MeshType)
	{
	case EUnitMeshType::UseStaticMesh:
		if (ptr_row->StaticMesh == nullptr)
		{
			checkf(false, TEXT("UVBFUnitBase::SpawnUnitActor_Implementation: Cannot initialize mesh - it is not set in DataTable"))
		}

        temp_render_asset = ptr_row->StaticMesh;
		break;
	case EUnitMeshType::UseSkeletalMesh:
		if (ptr_row->SkeletalMesh == nullptr)
		{
			checkf(false, TEXT("UVBFUnitBase::SpawnUnitActor_Implementation: Cannot initialize mesh - it is not set in DataTable"))
		}

        temp_render_asset = ptr_row->SkeletalMesh;
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

void UVBFUnitBase::DespawnUnitActor_Implementation()
{
    if (vbf_unit_actor == nullptr)
    {
        return;
    }

    vbf_unit_actor->Destroy();
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
