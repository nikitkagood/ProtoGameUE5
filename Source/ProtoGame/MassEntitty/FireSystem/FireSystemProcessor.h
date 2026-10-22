#pragma once

#include "FireSystemFragments.h"

//#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "MassProcessor.h"
#include "Mass/EntityFragments.h"
#include "MassEntityManager.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"

#include "FireSystemProcessor.generated.h" 


UCLASS(BlueprintType, ClassGroup = MassEntity)
class PROTOGAME_API UFireSystemProcessor : public UMassProcessor
{
    GENERATED_BODY()

public:
    UFireSystemProcessor();

    //void QueueFireCommand(const FFireCommand& Cmd);
    //Commands.PushCommand<FCreateEntityCommand>(
    //    [Cmd](FMassEntityManager& Manager)
    //    {
    //        FMassEntityHandle Entity = Manager.CreateEntity();

    //        Manager.AddComponent<FTransform>(Entity,
    //            FTransform(Cmd.MuzzleRotation, Cmd.MuzzleLocation));

    //        Manager.AddComponent<FVelocity>(Entity,
    //            FVelocity{ Cmd.MuzzleRotation.Vector() * Cmd.MuzzleVelocity });

    //        Manager.AddComponent<FOwnerData>(Entity,
    //            FOwnerData{ Cmd.Instigator });
    //    }
    //);

protected:
    virtual void InitializeInternal(UObject& obj, const TSharedRef<FMassEntityManager>& EntityManager) override;

    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

protected:
    bool CheckNetMode(FMassEntityManager& EntityManager);

private:
    FMassArchetypeHandle BulletArchetype;

    FMassEntityQuery WeaponQuery;

};

