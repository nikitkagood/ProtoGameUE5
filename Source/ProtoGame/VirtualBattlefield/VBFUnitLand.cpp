// Nikita Belov, All rights reserved


#include "VBFUnitLand.h"

#include "Kismet/KismetMathLibrary.h"
#include "Async/Async.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

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

bool UVBFUnitLand::IsEverMovable_Implementation() const
{
    if (vbf_unit_info.MaxSpeed > 0)
    {
        return true;
    }

    return false;
}

bool UVBFUnitLand::CanMove_Implementation() const
{
    return Execute_IsEverMovable(this);
}

bool UVBFUnitLand::SpawnUnitActor_Implementation(const FTransform& transform, const FVector& normal, FDataTableRowHandle handle, UWorld* world_optional)
{
    if (IsValid(vbf_unit_actor) == true)
    {
        UE_LOG(LogTemp, Warning, TEXT("UVBFUnitLand::SpawnUnitActor_Implementation: unit actor has been spawned already"));
        return false;
    }

    if (transform.IsValid() == false)
    {
        checkf(false, TEXT("UVBFUnitLand::SpawnUnitActor_Implementation: transform is invalid"));
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
        checkf(false, TEXT("UVBFUnitLand::SpawnUnitActor_Implementation: GetWorld == nullptr, cannot spawn Actor"));
        return false;
    }

    auto* ptr_row = handle.GetRow<DataTableType>("UVBFUnitLand::SpawnUnitActor_Implementation");

    if (ptr_row == nullptr)
    {
        checkf(false, TEXT("UVBFUnitLand::SpawnUnitActor_Implementation: ptr_row == nullptr, cannot get info from DT"));
        return false;
    }

    //Actually spawn actor
    auto* actor_ptr = AVBFActorBase::StaticCreateObjectDeferred(world_to_use, ptr_row->unit_info_table.UnitActorClass, this, transform);

    if (actor_ptr == nullptr)
    {
        return false;
    }

    constexpr int SPAWN_ADJUSTMENT = 5;
    
    FTransform TransformAdjustedLocation { transform.GetRotation(), transform.GetLocation() + (normal.GetSafeNormal() * SPAWN_ADJUSTMENT), transform.GetScale3D() };

    UGameplayStatics::FinishSpawningActor(actor_ptr, TransformAdjustedLocation);

    vbf_unit_actor = actor_ptr;

    return true;
}

//UStreamableRenderAsset* temp_render_asset = nullptr;

//switch (ptr_row->unit_info_table.MeshType)
//{
//case EUnitMeshType::UseStaticMesh:
//    if (ptr_row->unit_info_table.StaticMesh == nullptr)
//    {
//        checkf(false, TEXT("UVBFUnitLand::SpawnUnitActor_Implementation: Cannot initialize mesh - it is not set in DataTable"))
//    }

//    temp_render_asset = ptr_row->unit_info_table.StaticMesh;
//    break;
//case EUnitMeshType::UseSkeletalMesh:
//    if (ptr_row->unit_info_table.SkeletalMesh == nullptr)
//    {
//        checkf(false, TEXT("UVBFUnitLand::SpawnUnitActor_Implementation: Cannot initialize mesh - it is not set in DataTable"))
//    }

//    temp_render_asset = ptr_row->unit_info_table.SkeletalMesh;
//    break;
//case EUnitMeshType::UseExternalSettings:
//    break;
//default:
//    break;
//}

//bool result = actor_ptr->Initialize(temp_render_asset);

//if (result == false)
//{
//    return false;
//}


//TODO: async TryMoveTo 
//bool UVBFUnitLand::TryMoveTo_Implementation(const FVector & new_location)
// {
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
// }

bool UVBFUnitLand::TryMoveTo_Implementation(const FVector& new_location)
{
    if (Execute_IsCommandable(this) == false || Execute_CanMove(this) == false)
    {
        return false;
    }

    bool result = false;

    //otherwise move without actor
    bool MoveWithActor = IsValid(vbf_unit_actor);

    if (MoveWithActor)
    {
        //We move Actor and update UnitObject accordingly
        const double ACCEPTANCE_RADIUS = vbf_unit_actor->GetRootComponent()->Bounds.SphereRadius * 0.5;

        AAIController* AI_controller = Cast<AAIController>(vbf_unit_actor->GetController());
        if (AI_controller)
        {
            constexpr bool STOP_ON_OVERLAP = false;
            constexpr bool USE_PATHFINDING = true; //Maybe disable pathfinding for very short distances
            constexpr bool PROJECT_DESTINATION = false;
            constexpr bool CAN_STRAFE = true;
            //filter class
            constexpr bool PARTIAL_PATH = true;

            EPathFollowingRequestResult::Type path_request_result = AI_controller->MoveToLocation(new_location, ACCEPTANCE_RADIUS, STOP_ON_OVERLAP, USE_PATHFINDING, PROJECT_DESTINATION, CAN_STRAFE, nullptr, PARTIAL_PATH);

            switch (path_request_result)
            {
            case EPathFollowingRequestResult::Type::Failed:
                UE_LOG(LogMoverPawn, Warning, TEXT("Path request result: FAILED"));
                result = false;
                break;
            case EPathFollowingRequestResult::Type::RequestSuccessful:
                UE_LOG(LogMoverPawn, Warning, TEXT("Path request result: RequestSuccessful"));
                result = true;
                break;
            case EPathFollowingRequestResult::Type::AlreadyAtGoal:
                UE_LOG(LogMoverPawn, Warning, TEXT("Path request result: AlreadyAtGoal"));
                result = false;
                break;
            default:
                break;
            }
        }
    }
    else
    {
        //We only move UnitObject
        //TODO: move along path anyway

        //auto new_rotation = UKismetMathLibrary::FindLookAtRotation(Execute_GetLocation(this), new_location);

        UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

        //TODO: Get unit class from DT, get it's nav properties
        FNavAgentProperties NavProperties = FNavAgentProperties::DefaultProperties;

        auto NavData = NavSys->GetNavDataForProps(NavProperties);

        auto NavPath = NavSys->FindPathToLocationSynchronously(GetWorld(), vbf_unit_info.Location, new_location, NavData);

        double PathCost = NavPath->GetPathCost();
        double PathLength = NavPath->GetPathLength();


        //result = true;
    }
    

    if (result && MoveWithActor)
    {
        //Sync UnitObject and UnitActor
        //TODO: should be separate most likely, especially when async will be implemented
        vbf_unit_info.Location = new_location;
    }
    else if (result && !MoveWithActor)
    {
        //Update 
        vbf_unit_info.Location = new_location;
    }

    return result;
}

