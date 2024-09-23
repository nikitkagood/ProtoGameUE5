// Nikita Belov, All rights reserved


#include "VBFUnitLand.h"

#include "Kismet/KismetMathLibrary.h"
#include "Async/Async.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

//#include "Perception/AIPerceptionComponent.h"

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

    auto* ptr_row = handle.GetRow<DataTableType>("UVBFUnitLand::SpawnUnitActor_Implementation");

    if (ptr_row == nullptr)
    {
        checkf(false, TEXT("UVBFUnitLand::SpawnUnitActor_Implementation: ptr_row == nullptr, cannot get info from DT"))
            return false;
    }

    auto* actor_ptr = AVBFActorBase::StaticCreateObjectDeferred(world_to_use, ptr_row->unit_info_table.UnitActorClass, this, transform);

    if (actor_ptr == nullptr)
    {
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

    UGameplayStatics::FinishSpawningActor(actor_ptr, transform);

    vbf_unit_actor = actor_ptr;

    return true;
}

bool UVBFUnitLand::TryMoveTo_Implementation(const FVector& new_location)
{
    //TODO. Currently it's just a teleportation

    constexpr double LOCATION_TOLERANCE = 0.15;

    if (Execute_IsCommandable(this) == false || Execute_CanMove(this) == false)
    {
        return false;
    }

    bool result = false;

    auto new_rotation = UKismetMathLibrary::FindLookAtRotation(Execute_GetLocation(this), new_location);

    //TODO
    if (IsValid(vbf_unit_actor))
    {
        //result = vbf_unit_actor->SetActorLocation(new_location);
        //vbf_unit_actor->SetActorRotation(new_rotation);

        //AsyncTask(ENamedThreads::GameThread, [&result, new_location, new_rotation, this]() 
        //    {
        //    UE_LOG(LogTemp, Warning, TEXT("Entered AsyncTask"))

        //    //while (vbf_unit_actor->GetActorLocation().Equals(new_location, LOCATION_TOLERANCE) == false)
        //    for (size_t i = 0; i < 50; i++)
        //    {
        //        UE_LOG(LogTemp, Warning, TEXT("AsyncTask: while loop"))

        //        const FVector velocity_vector = UKismetMathLibrary::GetForwardVector(new_rotation) * 5;

        //        result = vbf_unit_actor->SetActorLocation(vbf_unit_actor->GetActorLocation() + velocity_vector);

        //        if (result == false)
        //        {
        //            break;
        //        }

        //        vbf_unit_info.Location = vbf_unit_actor->GetActorLocation();
        //    }
        //    UE_LOG(LogTemp, Warning, TEXT("Exited AsyncTask loop"))
        //    });

        AAIController* AI_controller = Cast<AAIController>(vbf_unit_actor->GetController());
        if (AI_controller)
        {
            EPathFollowingRequestResult::Type path_request_result = AI_controller->MoveToLocation(new_location);

            switch (path_request_result)
            {
            case EPathFollowingRequestResult::Type::Failed:
                UE_LOG(LogTemp, Warning, TEXT("Path request result: FAILED"));
                break;
            case EPathFollowingRequestResult::Type::RequestSuccessful:
                UE_LOG(LogTemp, Warning, TEXT("Path request result: RequestSuccessful"));
                break;
            case EPathFollowingRequestResult::Type::AlreadyAtGoal:
                UE_LOG(LogTemp, Warning, TEXT("Path request result: AlreadyAtGoal"));
                break;
            default:
                break;
            }
        }
    }
    
    if (result)
    {
        vbf_unit_info.Location = new_location;
        vbf_unit_info.Rotation = new_rotation;
    }

    //Location += direction vector 

    //vbf_unit_info.MaxSpeed

    //FAutoDeleteAsyncTask

    return result;
}

