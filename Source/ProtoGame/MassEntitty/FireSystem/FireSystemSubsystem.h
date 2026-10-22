#pragma once

//#include "FireSystemFragments.h"

//#include "CoreMinimal.h"
#include "MassSubsystemBase.h"
#include "Subsystems/SubsystemCollection.h"
//#include "Engine/EngineBaseTypes.h"
//#include "MassProcessor.h"
#include "Mass/EntityFragments.h"
#include "MassMovementFragments.h"
#include "MassEntityManager.h"
#include "MassCommonTypes.h"
//#include "MassExecutionContext.h"

#include "FireSystemSubsystem.generated.h" 


UCLASS(BlueprintType, ClassGroup = FireSystem)
class PROTOGAME_API UFireSystemSubsystem : public UMassSubsystemBase
{
    GENERATED_BODY()

public:
    //UFireSystemSubsystem();

    FORCEINLINE static const UFireSystemSubsystem* Get() { return Instance; }

    //Fire system entity, a bullet, shell etc.
    void SpawnEntity(const FTransform& transform) const;

    //void SpawnEntityBatched();

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
    //Might be bugged if multiple worlds
    static UFireSystemSubsystem* Instance;

    FMassEntityManager* CachedEntityManager;

};

