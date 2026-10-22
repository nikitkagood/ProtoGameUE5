#include "FireSystemSubsystem.h"

#include "MassEntitySubsystem.h"
#include "MassCommandBuffer.h"
#include "Engine/World.h"

UFireSystemSubsystem* UFireSystemSubsystem::Instance = nullptr; 

void UFireSystemSubsystem::SpawnEntity(const FTransform& transform) const
{
    FTransformFragment TransformFragment = transform;

    const int32 MUZZLE_VELOCITY_PLACEHOLDER = 10;

    FMassVelocityFragment VelocityFragment;
    VelocityFragment.Value = transform.GetRotation().GetForwardVector() * MUZZLE_VELOCITY_PLACEHOLDER;

    // 3. Задаем начальную скорость полета снаряда
    //FMassVelocityFragment InitialVelocity;
    //InitialVelocity.Value = MuzzleVelocity;
    //CommandBuffer.PushCommand<FMassCommandAddFragmentsTransitional>(BulletEntity, FMassVelocityFragment::StaticStruct(), FInstancedStruct::Make(InitialVelocity));

     //Example:
    //FMassEntityHandle Entity = EntityManager->ReserveEntity();
    //Context.Defer().PushCommand<FMassCommandBuildEntity>(Entity, FMyFragmentA(42), FMyTag())

    FMassEntityHandle Entity = CachedEntityManager->ReserveEntity();
    CachedEntityManager->Defer().PushCommand<FMassCommandBuildEntity>(Entity, TransformFragment, VelocityFragment);

}

void UFireSystemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UWorld* World = GetWorld();

    if (!IsValid(World) || World->GetNetMode() == NM_Client)
    {
        return;
    }

    if (Instance != nullptr && World->WorldType == EWorldType::Game && World->WorldType == EWorldType::PIE)
    {
        checkf(false, TEXT("Double initialization isn't allowed")); //might occur when multiple worlds running
    }

    Instance = this;

    check(Instance);

    UMassEntitySubsystem* EntitySubsystem = Collection.InitializeDependency<UMassEntitySubsystem>();

    check(EntitySubsystem);

    CachedEntityManager = &EntitySubsystem->GetMutableEntityManager();
}
