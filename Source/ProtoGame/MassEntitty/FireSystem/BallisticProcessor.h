#pragma once

#include "CoreMinimal.h"
#include "FireSystemFragments.h"

#include "MassProcessor.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "NiagaraDataChannel.h"

#include "BallisticProcessor.generated.h" 

UCLASS(BlueprintType, ClassGroup = MassEntity)
class PROTOGAME_API UBallisticProcessor : public UMassProcessor
{
    GENERATED_BODY()
public:
    UBallisticProcessor();

protected:
    virtual void InitializeInternal(UObject& obj, const TSharedRef<FMassEntityManager>& EntityManager) override;

    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

    void HandleProjectileHit(const FHitResult& HitResult, const FMassEntityHandle& Entity);
    
    FORCEINLINE void CalculateAirDrag(FVector& Velocity);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
    FVector Gravity = {0, 0, -1};

private:
    //Niagara Data Channel, for visualization
    void WriteToNDC(UWorld* World, UNiagaraDataChannel* ProjectileStateChannel, FMassExecutionContext& Context);

    FMassEntityQuery BallisticQuery;

    FNDCAccessContextInst* AccessContext;

    float DeltaTime = 0;
};

