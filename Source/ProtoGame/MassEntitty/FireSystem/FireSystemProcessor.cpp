#include "FireSystemProcessor.h"

UFireSystemProcessor::UFireSystemProcessor() : WeaponQuery(*this)
{
    //Might be needed:
    //Executes only on Clients and Standalone
    //ExecutionFlags = (int32)(EProcessorExecutionFlags::Client | EProcessorExecutionFlags::Standalone);

    //Execution order matters WITHIN one frame
    ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::UpdateWorldFromMass; 
    ExecutionOrder.ExecuteBefore.Add(TEXT("UBallisticProcessor"));

    bAutoRegisterWithProcessingPhases = true;
    SetProcessingPhase(EMassProcessingPhase::PrePhysics);
    bRequiresGameThreadExecution = false; //async
}

bool UFireSystemProcessor::CheckNetMode(FMassEntityManager& EntityManager)
{
    //UWorld* World = EntityManager.GetWorld();
    //if (!World || World->GetNetMode() == NM_Client)
    //{
    //    return;
    //}

    return false;
}

void UFireSystemProcessor::InitializeInternal(UObject& obj, const TSharedRef<FMassEntityManager>& EntityManager)
{
    //Maybe CallInitialize()

    TArray<UScriptStruct*> BulletFragments;
    BulletFragments.Add(FTransformFragment::StaticStruct()); //transform
    //BulletFragments.Add(FProjectileVelocityFragment::StaticStruct()); //velocity
    //BulletFragments.Add(FBulletDamageFragment::StaticStruct()); //damage

    BulletArchetype = EntityManager.Get().CreateArchetype(BulletFragments);

}

void UFireSystemProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    Super::ConfigureQueries(EntityManager);

    WeaponQuery.AddTagRequirement<FMEFireTag>(EMassFragmentPresence::All);

    //WeaponQuery.AddRequirement<FFireTriggerFragment>(EMassFragmentAccess::ReadWrite);
    //WeaponQuery.AddRequirement<FWeaponStatsFragment>(EMassFragmentAccess::ReadWrite);
    //WeaponQuery.AddRequirement<FWeaponTransformFragment>(EMassFragmentAccess::ReadOnly);
}

void UFireSystemProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    const float DeltaTime = Context.GetDeltaTimeSeconds();

    WeaponQuery.ForEachEntityChunk(Context, [DeltaTime](FMassExecutionContext& ChunkContext)
        {
            // 1. Получаем ПЛОСКИЙ массив фрагментов из текущего чанка памяти
            TArrayView<FFireTriggerFragment> BulletList = ChunkContext.GetMutableFragmentView<FFireTriggerFragment>();
            const int32 NumEntities = ChunkContext.GetNumEntities();

            // 2. Линейный проход по массиву в кэше процессора (DOD в действии)
            for (int32 i = 0; i < NumEntities; ++i)
            {
                //FBulletFragment& Bullet = BulletList[i];

                //// Простая баллистика: добавляем гравитацию к скорости
                //Bullet.Velocity += FVector(0.f, 0.f, -980.f) * DeltaTime;

                //// Двигаем пулю вперед
                //Bullet.Position += Bullet.Velocity * DeltaTime;

                // ЗДЕСЬ ЖЕ можно делать быстрый Async Line Trace для коллизии
            }
        });

    // 3. После этого кадра массив координат готов. 
    // Отдельная система соберет эти BulletList[i].Position и одной пачкой скинет в Niagara.
}

/*
    WeaponQuery.ForEachEntityChunk(EntityManager, Context,

        [this, &Context](FMassExecutionContext& ChunkContext)
        {
            TArrayView<FFireTriggerFragment> TriggerList = ChunkContext.GetMutableFragmentView<FFireTriggerFragment>();
            TArrayView<FWeaponStatsFragment> StatsList = ChunkContext.GetMutableFragmentView<FWeaponStatsFragment>();
            TConstArrayView<FWeaponTransformFragment> TransformList = ChunkContext.GetFragmentView<FWeaponTransformFragment>();

            const int32 NumEntities = ChunkContext.GetNumEntities();
            const float DeltaTime = ChunkContext.GetDeltaTimeSeconds();

            FMassCommandBuffer& CommandBuffer = Context.Defer();

            for (int32 i = 0; i < NumEntities; ++i)
            {
                FFireTriggerFragment& Trigger = TriggerList[i];
                FWeaponStatsFragment& Stats = StatsList[i];
                const FWeaponTransformFragment& WeaponTransform = TransformList[i];

                Trigger.TimeSinceLastShot += DeltaTime;

                if (Trigger.bIsTriggered && Trigger.TimeSinceLastShot >= Stats.FireRate && Stats.AmmoCount > 0)
                {
                    Trigger.TimeSinceLastShot = 0.f;

                    //TODO: refactor
                    Stats.AmmoCount--;

                    //FMassEntityHandle NewBullet = CommandBuffer.CreateEntity(BulletArchetype);

                    //FTransform BulletTransform;
                    //BulletTransform.SetLocation(WeaponTransform.MuzzleLocation);
                    //BulletTransform.SetRotation(WeaponTransform.FireDirection.ToOrientationQuat());

                    //FTransformFragment BulletTransformFrag;
                    //BulletTransformFrag.SetTransform(BulletTransform);
                    //CommandBuffer.SetFragmentView(NewBullet, FConstStructView::Make(BulletTransformFrag));

                    //FProjectileVelocityFragment BulletVelFrag;
                    //BulletVelFrag.Velocity = WeaponTransform.FireDirection * Stats.MuzzleVelocity;
                    //BulletVelFrag.GravityScale = 1.0f;
                    //CommandBuffer.SetFragmentView(NewBullet, FConstStructView::Make(BulletVelFrag));

                    //FBulletDamageFragment BulletDamageFrag;
                    //BulletDamageFrag.Damage = Stats.BaseDamage;
                    //BulletDamageFrag.InstigatorID = ChunkContext.GetEntity(i).Index;
                    //CommandBuffer.SetFragmentView(NewBullet, FConstStructView::Make(BulletDamageFrag));
                }
            }
        });*/