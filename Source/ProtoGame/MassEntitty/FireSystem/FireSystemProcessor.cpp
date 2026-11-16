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

void UFireSystemProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& GlobalContext)
{
    const float DeltaTime = GlobalContext.GetDeltaTimeSeconds();

    WeaponQuery.ForEachEntityChunk(GlobalContext, [DeltaTime](FMassExecutionContext& ChunkContext)
        {
            const int32 NumEntities = ChunkContext.GetNumEntities();

            if (NumEntities <= 0)
            {
                return;
            }

            //TArrayView<FFireTriggerFragment> ProjectileList = ChunkContext.GetMutableFragmentView<FFireTriggerFragment>();

            // 2. Линейный проход по массиву в кэше процессора (DOD в действии)
            //for (int32 i = 0; i < NumEntities; ++i)
            //{
                //FBulletFragment& Bullet = BulletList[i];

                //Bullet.Velocity += FVector(0.f, 0.f, -980.f) * DeltaTime;

                //Bullet.Position += Bullet.Velocity * DeltaTime;

                // ЗДЕСЬ ЖЕ можно делать быстрый Async Line Trace для коллизии
            //}
        });

    //3. После этого кадра массив координат готов. 
    // Отдельная система соберет эти BulletList[i].Position и одной пачкой скинет в Niagara.
}

