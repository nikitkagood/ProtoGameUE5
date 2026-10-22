#include "BallisticProcessor.h"

#include "Engine/DamageEvents.h"

#include "Mass/EntityFragments.h"
#include "MassMovementFragments.h"

UBallisticProcessor::UBallisticProcessor() : BallisticQuery(*this)
{
    //Execution order matters WITHIN one frame
    ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Movement;
    //ExecutionOrder.ExecuteBefore.Add(TEXT("UBallisticProcessor"));

    bAutoRegisterWithProcessingPhases = true;
    SetProcessingPhase(EMassProcessingPhase::PrePhysics);
    bRequiresGameThreadExecution = false; //async
}

void UBallisticProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    Super::ConfigureQueries(EntityManager);

    //BallisticQuery.AddRequirement<FBallisticEntityFragment>(EMassFragmentAccess::ReadWrite);
    //BallisticQuery.AddSharedRequirement<FBallisticSharedFragment>(EMassFragmentAccess::ReadOnly);

    BallisticQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    BallisticQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
}

/*
void UBallisticProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    const float DeltaTime = Context.GetDeltaTimeSeconds();
    auto CommandBuffer = Context.GetSharedDeferredCommandBuffer().Get();
    UWorld* World = EntityManager.GetWorld();

    if (World == nullptr)
    {
        return;
    }

    // Для оптимизации DOD-архитектуры здесь можно глобально игнорировать игроков, 
    // но в простом варианте мы итерируем чанками.
    FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(MassBulletTrace), true);


    BallisticQuery.ForEachEntityChunk(Context, [DeltaTime, World, &CommandBuffer, &TraceParams](FMassExecutionContext& ChunkContext)
        {
            TArrayView<FBallisticEntityFragment> BulletList = ChunkContext.GetMutableFragmentView<FBallisticEntityFragment>();
            const int32 NumEntities = ChunkContext.GetNumEntities();

            for (int32 i = 0; i < NumEntities; ++i)
            {
                FBallisticEntityFragment& Bullet = BulletList[i];
                FMassEntityHandle BulletEntity = ChunkContext.GetEntity(i);

                FVector OldPosition = Bullet.Position;

                // Применяем гравитацию Земли (-980 см/с²) к вектору скорости
                Bullet.Velocity.Z += -980.f * DeltaTime;

                FVector NewPosition = OldPosition + (Bullet.Velocity * DeltaTime);


                FHitResult HitResult;

                bool bHit = World->LineTraceSingleByChannel(HitResult, OldPosition,NewPosition, ECC_EngineTraceChannel1, TraceParams);

                if (bHit)
                {
                    // Наносим урон (Вызов классического ООП из DOD-системы)
                    if (AActor* HitActor = HitResult.GetActor())
                    {
                        // Делаем легкий вызов нанесения урона. Логику тяжелых эффектов 
                        // лучше делегировать сторонним менеджерам
                        FDamageEvent DamageEvent;
                        HitActor->TakeDamage(10.f, DamageEvent, nullptr, nullptr);
                    }

                    //TODO: можно сгенерировать C++ структуру-ивент попадания для Niagara

                    //TODO: If bullet hits, destroy
                    //temporary, should be penetration
                    CommandBuffer->DestroyEntity(BulletEntity);
                }
                else
                {
                    // Если коллизии не было — окончательно утверждаем новую позицию для пули
                    Bullet.Position = NewPosition;
                }
            }
        });
}
*/

void UBallisticProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    const float DeltaTime = Context.GetDeltaTimeSeconds();
    auto CommandBuffer = Context.GetSharedDeferredCommandBuffer().Get();

    UWorld* World = EntityManager.GetWorld();

    if (World == nullptr)
    {
        return;
    }

    FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(MassBulletTrace), true);

    BallisticQuery.ForEachEntityChunk(Context, [DeltaTime, World, &CommandBuffer, &TraceParams](FMassExecutionContext& ChunkContext)
        {
            const int32 NumEntities = ChunkContext.GetNumEntities();

            TArrayView<FTransformFragment> TransformList = ChunkContext.GetMutableFragmentView<FTransformFragment>();
            TConstArrayView<FMassVelocityFragment> VelocityList = ChunkContext.GetFragmentView<FMassVelocityFragment>();

            for (int32 i = 0; i < NumEntities; ++i)
            {
                FTransform& Transform = TransformList[i].GetMutableTransform();
                const FVector& Velocity = VelocityList[i].Value;

                FVector NewLocation = Transform.GetLocation() + (Velocity * DeltaTime);
                Transform.SetLocation(NewLocation);
            }

            if (IsValid(ProjectileStateChannel))
            {
                const auto TransformFragments = Context.GetFragmentView<FTransformFragment>();
                const auto VelocityFragments = Context.GetFragmentView<FMassVelocityFragment>();

                FNiagaraDataChannelSearchParameters SearchParameters;
                SearchParameters.Location =
                    TransformFragments[0].GetTransform().GetLocation();

                UNiagaraDataChannelWriter* Writer =
                    UNiagaraDataChannelLibrary::WriteToNiagaraDataChannel(
                        GetWorld(),
                        ProjectileStateChannel,
                        SearchParameters,
                        NumEntities,
                        true,
                        true,
                        true);

                if (Writer != nullptr)
                {
                    for (int32 Index = 0; Index < NumEntities; ++Index)
                    {
                        const FMassEntityHandle Entity = Context.GetEntity(Index);
                        const FTransform& Transform =
                            TransformFragments[Index].GetTransform();

                        Writer->WriteInt(TEXT("EntityIndex"), Entity.GetIndex());
                        Writer->WriteInt(TEXT("EntitySerial"), Entity.GetSerialNumber());
                        Writer->WritePosition(TEXT("Position"), Transform.GetLocation());
                        Writer->WriteVector(TEXT("Velocity"), VelocityFragments[Index].Value);
                    }

                    Writer->FinishWrite();
                }
            }
        });
}
