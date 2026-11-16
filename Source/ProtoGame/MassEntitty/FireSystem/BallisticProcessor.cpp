#include "BallisticProcessor.h"

#include "Engine/DamageEvents.h"

#include "Mass/EntityFragments.h"
#include "MassMovementFragments.h"

#include "NiagaraDataChannelFunctionLibrary.h"
#include "NiagaraDataChannelAccessor.h"
#include "MassEntitty/FireSystem/FireSystemSubsystem.h"

UBallisticProcessor::UBallisticProcessor() : BallisticQuery(*this)
{
    //Execution order matters WITHIN one frame
    ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Movement;
    //ExecutionOrder.ExecuteBefore.Add(TEXT("UBallisticProcessor"));

    bAutoRegisterWithProcessingPhases = true;
    SetProcessingPhase(EMassProcessingPhase::PrePhysics);
    bRequiresGameThreadExecution = true;
}

void UBallisticProcessor::InitializeInternal(UObject& obj, const TSharedRef<FMassEntityManager>& EntityManager)
{
    AccessContext = &UNiagaraDataChannelLibrary::GetUsableAccessContextFromNDC(UFireSystemSubsystem::Get()->GetProjectileNDC()->GetAsset());
}

void UBallisticProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    Super::ConfigureQueries(EntityManager);

    //BallisticQuery.AddRequirement<FBallisticEntityFragment>(EMassFragmentAccess::ReadWrite);
    //BallisticQuery.AddSharedRequirement<FBallisticSharedFragment>(EMassFragmentAccess::ReadOnly);

    BallisticQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    BallisticQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
}

void UBallisticProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& GlobalContext)
{
    DeltaTime = GlobalContext.GetDeltaTimeSeconds();

    auto CommandBuffer = GlobalContext.GetSharedDeferredCommandBuffer().Get();

    UWorld* World = EntityManager.GetWorld();

    if (World == nullptr)
    {
        return;
    }

    //FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(MassBulletTrace), true);

    //Note that Context is different for each Query
    BallisticQuery.ForEachEntityChunk(GlobalContext, [this, World, &CommandBuffer](FMassExecutionContext& ChunkContext)
        {
            const int32 NumEntities = ChunkContext.GetNumEntities();

            if (NumEntities <= 0)
            {
                return;
            }

            TArrayView<FTransformFragment> TransformList = ChunkContext.GetMutableFragmentView<FTransformFragment>();

            TArrayView<FMassVelocityFragment> VelocityList = ChunkContext.GetMutableFragmentView<FMassVelocityFragment>();

            for (int32 i = 0; i < NumEntities; ++i)
            {
                auto Entity = ChunkContext.GetEntity(i);

                FTransform& Transform = TransformList[i].GetMutableTransform();
                FVector& Velocity = VelocityList[i].Value;

                CalculateAirDrag(Velocity);

                FVector OldLocation = Transform.GetLocation();
                FVector NewLocation = OldLocation + (Velocity * DeltaTime);

                Transform.SetLocation(NewLocation);

                FHitResult HitResult;
                FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(BulletTrace), true);

                //TODO: different shape traces, async trace or GPU compute
                //World->LineTraceSingleByChannel(HitResult, OldLocation, NewLocation, ECollisionChannel::ECC_WorldDynamic, QueryParams);

                if (Velocity.IsNearlyZero())
                {
                    //HandleProjectileHit(HitResult, Entity);

                    GEngine->AddOnScreenDebugMessage(100, 2.0f, FColor::Yellow, TEXT("Removed projectile: speed nearly zero"));

                    CommandBuffer->DestroyEntity(Entity);

                }
            }


            //if(tracer) TODO
            WriteToNDC(World, UFireSystemSubsystem::Get()->GetProjectileNDC(), ChunkContext);

        });



}

void UBallisticProcessor::WriteToNDC(UWorld* World, UNiagaraDataChannel* ProjectileStateChannel, FMassExecutionContext& LocalContext)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UBallisticProcessor::WriteToNDC);

    if (!IsValid(ProjectileStateChannel))
    {
        return;
    }

    check(ProjectileStateChannel->GetAsset());

    auto NumEntities = LocalContext.GetNumEntities();

    if (NumEntities < 1)
    {
        //there is nothing to write
        return;
    }

    TArrayView<FTransformFragment> TransformList = LocalContext.GetMutableFragmentView<FTransformFragment>();
    TConstArrayView<FMassVelocityFragment> VelocityList = LocalContext.GetFragmentView<FMassVelocityFragment>();

    UNiagaraDataChannelWriter* Writer = UNiagaraDataChannelLibrary::WriteToNiagaraDataChannel_WithContext(World, ProjectileStateChannel->GetAsset(), *AccessContext,
            NumEntities, true, true, true, TEXT("MassProjectileUpdate"));

    checkf(Writer, TEXT("UNiagaraDataChannelWriter is null"));

    for (int32 Index = 0; Index < NumEntities; ++Index)
    {
        //const FMassEntityHandle Entity = LocalContext.GetEntity(Index);

        //volatile auto VelocityDbg = VelocityList[Index].Value;

        //Writer->WriteInt(TEXT("EntityIndex"), Index, Entity.Index);
        //Writer->WriteInt(TEXT("EntitySerial"), Index, Entity.SerialNumber);
        Writer->WritePosition(TEXT("Position"), Index, TransformList[Index].GetTransform().GetLocation());
        Writer->WriteVector(TEXT("Velocity"), Index, VelocityList[Index].Value);
    }
}

void UBallisticProcessor::HandleProjectileHit(const FHitResult& HitResult, const FMassEntityHandle& Entity)
{
    FVector HitLocation = HitResult.ImpactPoint;
    FVector HitNormal = HitResult.ImpactNormal;
    AActor* HitActor = HitResult.GetActor();

    //Damage
    //if (HitActor && HitActor->Implements<UDamageInterface>())
    //{
    //    IDAMageInterface::Execute_TakeDamage(HitActor, DamageAmount, HitLocation);
    //}

    //Effect on hit
    //UNiagaraFunctionLibrary::SpawnSystemAtLocation(
    //    World, 
    //    ImpactEffectNiagara,
    //    HitLocation,
    //    HitNormal.Rotation()
    //);

    //Sound
    //UGameplayStatics::PlaySoundAtLocation(World, ImpactSound, HitLocation);
}

void UBallisticProcessor::CalculateAirDrag(FVector& Velocity)
{
    //TODO hardcoded for now
    const float DragCoefficient = 0.0001f;
    float SpeedSq = Velocity.SizeSquared();

    if (SpeedSq > KINDA_SMALL_NUMBER)
    {
        FVector DragForce = -Velocity * (FMath::Sqrt(SpeedSq) * DragCoefficient);
        Velocity += DragForce * DeltaTime;
    }

}
